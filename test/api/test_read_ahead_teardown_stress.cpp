#include "catch.hpp"
#include "test_helpers.hpp"

#include "duckdb/parallel/task.hpp"
#include "duckdb/parallel/task_scheduler.hpp"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <random>
#include <thread>

using namespace duckdb;

// Stress the multi-file read-ahead teardown paths that a client/server workload exercises:
// streaming results abandoned mid-scan, concurrent scans, and Interrupt() racing execution.
// Regression test for worker threads wedging forever in ~MultiFileLocalState / WaitForJob.

static void CreateFiles(Connection &con, const string &prefix, idx_t file_count, idx_t rows_per_file) {
	for (idx_t i = 0; i < file_count; i++) {
		auto file = TestCreatePath(prefix + std::to_string(i) + ".parquet");
		REQUIRE_NO_FAIL(con.Query("COPY (SELECT range AS i FROM range(" + std::to_string(rows_per_file) + ")) TO '" +
		                          file + "' (FORMAT parquet, ROW_GROUP_SIZE 2048)"));
	}
}

TEST_CASE("Stress read-ahead teardown: concurrent streaming abandonment", "[api]") {
	if (std::getenv("FORCE_ASYNC_SINK_SOURCE") != nullptr) {
		SKIP_TEST("not supported with forced async sink/source task injection");
		return;
	}
	DuckDB db(nullptr);
	Connection setup(db);
	CreateFiles(setup, "ra_stress_", 8, 100000);
	auto glob = TestCreatePath("ra_stress_*.parquet");
	REQUIRE_NO_FAIL(setup.Query("SET threads=8"));
	REQUIRE_NO_FAIL(setup.Query("SET read_ahead_depth=1"));

	constexpr idx_t NUM_WORKERS = 8;
	constexpr idx_t ITERATIONS = 100;
	std::atomic<bool> failed {false};

	duckdb::vector<std::thread> workers;
	for (idx_t w = 0; w < NUM_WORKERS; w++) {
		workers.emplace_back([&db, &glob, &failed, w]() {
			Connection con(db);
			std::mt19937 rng(static_cast<unsigned>(w));
			for (idx_t i = 0; i < ITERATIONS && !failed; i++) {
				auto stream = con.SendQuery("SELECT i FROM '" + glob + "'");
				if (stream->HasError()) {
					failed = true;
					break;
				}
				// fetch 0..2 chunks, then abandon mid-scan
				idx_t fetches = rng() % 3;
				for (idx_t f = 0; f < fetches; f++) {
					auto chunk = stream->Fetch();
					if (!chunk) {
						break;
					}
				}
				stream.reset();
			}
		});
	}
	for (auto &t : workers) {
		t.join();
	}
	REQUIRE(!failed);
	REQUIRE_NO_FAIL(setup.Query("SELECT 42"));
}

TEST_CASE("Stress read-ahead teardown: Interrupt() racing streaming scans", "[api]") {
	if (std::getenv("FORCE_ASYNC_SINK_SOURCE") != nullptr) {
		SKIP_TEST("not supported with forced async sink/source task injection");
		return;
	}
	DuckDB db(nullptr);
	Connection setup(db);
	CreateFiles(setup, "ra_intr_", 8, 100000);
	auto glob = TestCreatePath("ra_intr_*.parquet");
	REQUIRE_NO_FAIL(setup.Query("SET threads=8"));
	REQUIRE_NO_FAIL(setup.Query("SET read_ahead_depth=1"));

	constexpr idx_t NUM_WORKERS = 4;
	constexpr idx_t ITERATIONS = 100;
	std::atomic<bool> failed {false};

	duckdb::vector<std::thread> workers;
	for (idx_t w = 0; w < NUM_WORKERS; w++) {
		workers.emplace_back([&db, &glob, &failed, w]() {
			Connection con(db);
			std::mt19937 rng(static_cast<unsigned>(1000 + w));
			for (idx_t i = 0; i < ITERATIONS && !failed; i++) {
				std::atomic<bool> done {false};
				// interrupter mimics a remote CANCEL landing at a random point during execution
				std::thread interrupter([&con, &done, &rng]() {
					std::this_thread::sleep_for(std::chrono::microseconds(rng() % 5000));
					if (!done) {
						con.Interrupt();
					}
				});
				auto stream = con.SendQuery("SELECT i FROM '" + glob + "'");
				if (!stream->HasError()) {
					// drain a little, tolerate interrupt errors
					auto chunk = stream->Fetch();
					(void)chunk;
				}
				stream.reset();
				done = true;
				interrupter.join();
			}
		});
	}
	for (auto &t : workers) {
		t.join();
	}
	REQUIRE_NO_FAIL(setup.Query("SELECT 42"));
}

namespace {
//! Occupies one pool thread until released; used to simulate a scheduler whose pool threads
//! cannot pick up newly scheduled tasks (saturated pools / lost wake-up).
class PoolBlockerTask : public Task {
public:
	PoolBlockerTask(std::shared_ptr<std::atomic<bool>> release_p, std::shared_ptr<std::atomic<idx_t>> started_p)
	    : release(std::move(release_p)), started(std::move(started_p)) {
	}

	TaskExecutionResult Execute(TaskExecutionMode mode) override {
		++(*started);
		while (!release->load()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return TaskExecutionResult::TASK_FINISHED;
	}

private:
	std::shared_ptr<std::atomic<bool>> release;
	std::shared_ptr<std::atomic<idx_t>> started;
};
} // namespace

// Regression test: read-ahead waiters (WaitForJob and ~MultiFileLocalState) must make progress by
// draining their own scheduled I/O inline. If they merely yield until a pool thread runs the I/O,
// a saturated pool wedges the scan — and, on abandonment, wedges query teardown forever.
TEST_CASE("Read-ahead makes progress when no pool thread can run its I/O", "[api]") {
	if (std::getenv("FORCE_ASYNC_SINK_SOURCE") != nullptr) {
		SKIP_TEST("not supported with forced async sink/source task injection");
		return;
	}
	DuckDB db(nullptr);
	Connection setup(db);
	CreateFiles(setup, "ra_block_", 4, 200000);
	auto glob = TestCreatePath("ra_block_*.parquet");
	REQUIRE_NO_FAIL(setup.Query("SET threads=2"));
	REQUIRE_NO_FAIL(setup.Query("SET async_threads=2"));
	REQUIRE_NO_FAIL(setup.Query("SET read_ahead_depth=2"));
	// force the synchronous scan strategy so the scan waits in WaitForJob instead of parking
	REQUIRE_NO_FAIL(setup.Query("SET debug_physical_table_scan_execution_strategy='SYNCHRONOUS'"));

	auto &scheduler = TaskScheduler::GetScheduler(*db.instance);
	auto token = scheduler.CreateProducer();
	auto release = std::make_shared<std::atomic<bool>>(false);
	auto started = std::make_shared<std::atomic<idx_t>>(0);
	// Occupy every pool thread (regular + async) so scheduled read-ahead I/O tasks are stranded.
	// Deliberately over-schedule: surplus blockers sit queued, so any pool thread that ever frees
	// up (or is launched later) immediately blocks too — the blockade needs no exact thread count.
	constexpr idx_t BLOCKERS_PER_POOL = 64;
	for (idx_t i = 0; i < BLOCKERS_PER_POOL; i++) {
		scheduler.ScheduleTask(*token, make_shared_ptr<PoolBlockerTask>(release, started), TaskSchedulerType::REGULAR);
		scheduler.ScheduleTask(*token, make_shared_ptr<PoolBlockerTask>(release, started), TaskSchedulerType::ASYNC);
	}
	// wait until at least one pool thread is pinned, then let the rest settle
	for (idx_t waited_ms = 0; waited_ms < 5000 && started->load() == 0; waited_ms++) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	REQUIRE(started->load() > 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	std::atomic<bool> done {false};
	std::thread runner([&db, &glob, &done]() {
		Connection con(db);
		auto stream = con.SendQuery("SELECT i FROM '" + glob + "'");
		if (!stream->HasError()) {
			// fetching requires the read-ahead I/O to run: the waiter must drain it inline
			auto chunk = stream->Fetch();
			(void)chunk;
		}
		// abandoning mid-scan destroys local states with I/O still scheduled: same requirement
		stream.reset();
		done = true;
	});

	// generous bound; the scan must complete while every pool thread is still occupied
	for (idx_t waited_ms = 0; waited_ms < 30000 && !done; waited_ms++) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	bool completed_while_pools_blocked = done.load();
	// release the pools so an unfixed build can still unwind and join before the assertion fires
	*release = true;
	runner.join();
	REQUIRE(completed_while_pools_blocked);
	REQUIRE_NO_FAIL(setup.Query("SELECT 42"));
}

TEST_CASE("Stress read-ahead teardown: constrained async pool", "[api]") {
	if (std::getenv("FORCE_ASYNC_SINK_SOURCE") != nullptr) {
		SKIP_TEST("not supported with forced async sink/source task injection");
		return;
	}
	DuckDB db(nullptr);
	Connection setup(db);
	CreateFiles(setup, "ra_small_", 8, 100000);
	auto glob = TestCreatePath("ra_small_*.parquet");
	REQUIRE_NO_FAIL(setup.Query("SET threads=2"));
	REQUIRE_NO_FAIL(setup.Query("SET async_threads=1"));
	REQUIRE_NO_FAIL(setup.Query("SET read_ahead_depth=4"));

	constexpr idx_t NUM_WORKERS = 8;
	constexpr idx_t ITERATIONS = 50;
	std::atomic<bool> failed {false};

	duckdb::vector<std::thread> workers;
	for (idx_t w = 0; w < NUM_WORKERS; w++) {
		workers.emplace_back([&db, &glob, &failed, w]() {
			Connection con(db);
			std::mt19937 rng(static_cast<unsigned>(2000 + w));
			for (idx_t i = 0; i < ITERATIONS && !failed; i++) {
				auto stream = con.SendQuery("SELECT i FROM '" + glob + "'");
				if (stream->HasError()) {
					failed = true;
					break;
				}
				if (rng() % 2) {
					auto chunk = stream->Fetch();
					(void)chunk;
				}
				stream.reset();
			}
		});
	}
	for (auto &t : workers) {
		t.join();
	}
	REQUIRE(!failed);
	REQUIRE_NO_FAIL(setup.Query("SELECT 42"));
}
