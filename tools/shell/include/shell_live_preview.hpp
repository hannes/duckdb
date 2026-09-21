//===----------------------------------------------------------------------===//
//                         DuckDB
//
// shell_live_preview.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/mutex.hpp"
#include "duckdb/common/types/column/column_data_collection.hpp"
#include "duckdb/common/time_point.hpp"
#include "duckdb/common/box_renderer.hpp"
#include "duckdb/common/error_data.hpp"
#include "duckdb/common/optional_idx.hpp"
#include "live_preview.hpp"
#include "shell_highlight.hpp"

#include <atomic>
#include <thread>

namespace duckdb_shell {

enum class LivePreviewStatus : uint8_t {
	//! Nothing typed, or nothing we are willing to preview
	EMPTY,
	//! The statement did not parse or did not bind
	ERROR,
	//! The statement would modify the database - we never run those on our own
	WRITE_STATEMENT,
	//! Live preview is suspended (an explicit transaction is open)
	SUSPENDED,
	//! The query is in flight
	RUNNING,
	//! We have (the first rows of) a result
	RESULT
};

//! Runs the statement being typed in the background and renders the first rows of the result below
//! the prompt. Only ever executes statements that cannot modify the database.
class ShellLivePreview : public duckdb::LivePreviewProvider {
public:
	explicit ShellLivePreview(ShellState &state);
	~ShellLivePreview() override;

public:
	void Start(const char *buf, idx_t len) override;
	void Cancel() override;
	void ClearPanel() override;
	bool IsRunning() override;
	bool HasPanel() override;
	string Render(idx_t max_rows, idx_t max_cols, const duckdb::LivePreviewAnchor &anchor) override;
	bool TryGetErrorLocation(idx_t &offset, idx_t &length) override;

	//! Stop the worker and wait for it to finish - must be called before the connection is used for
	//! anything else
	void Join();
	//! Stop the worker and throw the panel away - the database it describes is going away
	void Invalidate();

private:
	//! The worker body: prepare the statement, and execute it if it is read-only
	void Run(const string &sql);
	//! Record what the preview found, unless the buffer has moved on in the meantime
	void SetStatus(LivePreviewStatus new_status, const string &new_message);
	//! Record an error together with the source range it points at
	void SetError(const duckdb::ErrorData &error);
	//! Throw away what the panel holds, keeping panel_sql. The caller must hold panel_lock.
	void ClearPanelContents();
	string RenderResult(idx_t max_rows, idx_t max_cols);
	//! Render the box holding at most value_rows values, as a list of lines
	duckdb::vector<string> RenderBox(const duckdb::BoxRendererConfig &config, idx_t value_rows);
	string RenderMessage(const string &text, HighlightElementType element_type, idx_t max_rows);
	//! The engine's progress bar for the running query, once its own startup delay has passed
	string RenderProgressBar(double elapsed_seconds, idx_t available_width);
	//! Render an error, underlining the token it points at when the editor gave us an anchor
	string RenderError(const duckdb::LivePreviewAnchor &anchor, idx_t max_rows, idx_t max_cols);

private:
	ShellState &state;
	std::thread worker;
	//! Set while the worker is alive, cleared by the worker itself when it finishes
	std::atomic<bool> running;
	//! Set when the result of the in-flight preview is no longer wanted
	std::atomic<bool> discarded;

	//! Guards everything below - written by the worker, read by the editor
	duckdb::mutex panel_lock;
	LivePreviewStatus status = LivePreviewStatus::EMPTY;
	//! The SQL the panel describes
	string panel_sql;
	//! The parse tree of the statement the panel describes, as canonical SQL
	string panel_tree;
	string message;
	//! Offset of the error within panel_sql, when the error points at one
	duckdb::optional_idx error_offset;
	idx_t error_length = 1;
	//! How far panel_sql starts into the editor's buffer (leading whitespace we trimmed)
	idx_t sql_offset = 0;
	unique_ptr<duckdb::ColumnDataCollection> preview_rows;
	vector<string> column_names;
	double elapsed_seconds = 0;
	//! When the in-flight query started, used to render the elapsed time
	duckdb::TimePoint start_time;
	//! Advances every time the spinner is rendered
	idx_t spinner_index = 0;
};

} // namespace duckdb_shell
