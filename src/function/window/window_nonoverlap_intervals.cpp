#include "duckdb/function/window/window_nonoverlap_intervals.hpp"
#include "duckdb/planner/expression/bound_window_expression.hpp"
#include "duckdb/function/window/window_shared_expressions.hpp"

namespace duckdb {

enum class IntervalCompareMode : uint8_t {
	INCLUSIVE, // start >= prev_end
	EXCLUSIVE  // start > prev_end
};

class WindowNonOverlapIntervalsGlobalState : public WindowExecutorGlobalState {
public:
	WindowNonOverlapIntervalsGlobalState(ClientContext &client, const WindowExecutor &executor_p,
	                                     const idx_t payload_count_p, const ValidityMask &partition_mask_p,
	                                     const ValidityMask &order_mask_p)
	    : WindowExecutorGlobalState(client, executor_p, payload_count_p, partition_mask_p, order_mask_p),
	      compare_mode(IntervalCompareMode::EXCLUSIVE) {
	};

	IntervalCompareMode compare_mode;
	unordered_map<idx_t, uint64_t> last_end;
};

class WindowNonOverlapIntervalsLocalState : public WindowExecutorBoundsLocalState {
public:
	explicit WindowNonOverlapIntervalsLocalState(ExecutionContext &context,
	                                             const WindowNonOverlapIntervalsGlobalState &grstate)
	    : WindowExecutorBoundsLocalState(context, grstate) {}

	void Sink(ExecutionContext &context, DataChunk &sink_chunk, DataChunk &coll_chunk, idx_t input_idx,
	          OperatorSinkInput &sink) override {};

	void Finalize(ExecutionContext &context, CollectionPtr collection, OperatorSinkInput &sink) override {};

};

//===--------------------------------------------------------------------===//
// WindowNonOverlapIntervalsExecutor
//===--------------------------------------------------------------------===//
WindowNonOverlapIntervalsExecutor::WindowNonOverlapIntervalsExecutor(BoundWindowExpression &wexpr,
                                                                     WindowSharedExpressions &shared)
    : WindowExecutor(wexpr, shared) {
	// Register children for evaluation
	low_idx = shared.RegisterEvaluate(wexpr.children[0]);
	high_idx = shared.RegisterEvaluate(wexpr.children[1]);
	inclusive_idx = shared.RegisterEvaluate(wexpr.inclusive);
}

unique_ptr<GlobalSinkState> WindowNonOverlapIntervalsExecutor::GetGlobalState(ClientContext &client,
                                                                              const idx_t payload_count,
                                                                              const ValidityMask &partition_mask,
                                                                              const ValidityMask &order_mask) const {
	return make_uniq<WindowNonOverlapIntervalsGlobalState>(client, *this, payload_count, partition_mask, order_mask);
}

void WindowNonOverlapIntervalsExecutor::EvaluateInternal(ExecutionContext &context, DataChunk &eval_chunk,
                                                         Vector &result, idx_t count, idx_t row_idx,
                                                         OperatorSinkInput &sink) const {
	auto &gstate = sink.global_state.Cast<WindowNonOverlapIntervalsGlobalState>();
	auto &lstate = sink.local_state.Cast<WindowNonOverlapIntervalsLocalState>();
	auto rdata= FlatVector::GetData<bool>(result);

	// Bounds for current partition
	auto partition_begin = FlatVector::GetData<const idx_t>(lstate.bounds.data[PARTITION_BEGIN]);

	// Data from our arguments: intervals(low, high)
	auto low  = FlatVector::GetData<uint64_t>(eval_chunk.data[low_idx]);
	auto high = FlatVector::GetData<uint64_t>(eval_chunk.data[high_idx]);

	auto incl = false;
	if (inclusive_idx < eval_chunk.data.size()){
		WindowInputExpression incl_expr(eval_chunk, inclusive_idx);
		incl = incl_expr.GetCell<bool>(0);
	}
	if (incl || gstate.compare_mode == IntervalCompareMode::INCLUSIVE){
		gstate.compare_mode = IntervalCompareMode::INCLUSIVE;
	} else {
		gstate.compare_mode = IntervalCompareMode::EXCLUSIVE;
	}

	// i = index within chunk, row_idx = global index/offset
	for (idx_t i = 0; i < count; ++i) {
		auto pbegin = partition_begin[i];

		// pick first interval in partition
		bool keep = false;
		if (row_idx + i == pbegin) {
			keep = true;
			gstate.last_end[pbegin] = high[i];
		} else {
			// pick next interval that does not overlap
			auto prev_end = gstate.last_end[pbegin];
			switch (gstate.compare_mode) {
			case IntervalCompareMode::INCLUSIVE:
				if (low[i] >= prev_end) {
					keep = true;
					gstate.last_end[pbegin] = high[i];
				}
				break;
			case IntervalCompareMode::EXCLUSIVE:
				if (low[i] > prev_end) {
					keep = true;
					gstate.last_end[pbegin] = high[i];
				}
				break;
			}
		}
		// write result into result vector
		rdata[i] = keep;
	}
}

} // namespace duckdb
