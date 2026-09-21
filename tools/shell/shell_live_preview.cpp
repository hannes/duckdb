#include "shell_live_preview.hpp"

#include "duckdb/common/box_renderer.hpp"
#include "duckdb/common/box_renderer_context.hpp"
#include "duckdb/common/column_data_collection_render_interface.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/common/progress_bar/display/terminal_progress_bar_display.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/parser/query_error_context.hpp"

namespace duckdb_shell {

using duckdb::ErrorData;
using duckdb::LivePreviewAnchor;
using duckdb::LogicalType;
using duckdb::StringUtil;
using duckdb::TimePoint;

//! How long a query has to run before the spinner appears - anything quicker renders its result
//! straight away, without a spinner flashing in front of it first
static constexpr double LIVE_PREVIEW_SPINNER_DELAY_SECONDS = 0.15;

//! Box-drawing marker that underlines the token an error points at, with a tick leading down to the
//! message: "\u2500\u2500\u252c\u2500\u2500" over the token, "\u2570\u2500\u2192 " in front of the message
static constexpr const char *MARKER_LINE = "\u2500";               // NOLINT: "\u2500"
static constexpr const char *MARKER_TICK = "\u252c";               // NOLINT: "\u252c"
static constexpr const char *MARKER_ELBOW = "\u2570\u2500\u2192 "; // NOLINT: "\u2570\u2500\u2192 "
//! Render width of MARKER_ELBOW
static constexpr idx_t MARKER_ELBOW_WIDTH = 4;
//! The message needs at least this much room next to the elbow, otherwise it starts at column 0
static constexpr idx_t MARKER_MIN_TEXT_WIDTH = 30;
//! Leads the one-line notes below the panel
static constexpr const char *NOTE_GLYPH = "\u21b3 "; // NOLINT: "\u21b3 "

static const char *SPINNER_FRAMES[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
static constexpr idx_t SPINNER_FRAME_COUNT = 10;

//! Renders a box into a string, colouring each element the way the shell colours a real result.
//! The shell's own DuckBoxRenderer writes to the terminal directly when highlighting is on, so the
//! panel - which has to be captured and placed below the prompt - needs its own.
class StringBoxRenderer : public duckdb::BaseResultRenderer {
public:
	explicit StringBoxRenderer(bool highlight) : highlight(highlight) {
	}

	void RenderLayout(const string &text) override {
		Append(text, HighlightElementType::LAYOUT);
	}
	void RenderColumnName(const string &text) override {
		Append(text, HighlightElementType::COLUMN_NAME);
	}
	void RenderType(const string &text) override {
		Append(text, HighlightElementType::COLUMN_TYPE);
	}
	void RenderValue(const string &text, const LogicalType &type) override {
		if (type.IsNumeric()) {
			Append(text, HighlightElementType::NUMERIC_VALUE);
		} else if (type.IsTemporal()) {
			Append(text, HighlightElementType::TEMPORAL_VALUE);
		} else {
			Append(text, HighlightElementType::STRING_VALUE);
		}
	}
	void RenderStringLiteral(const string &text, const LogicalType &type) override {
		Append(text, HighlightElementType::STRING_CONSTANT);
	}
	void RenderNull(const string &text, const LogicalType &type) override {
		Append(text, HighlightElementType::NULL_VALUE);
	}
	void RenderFooter(const string &text) override {
		Append(text, HighlightElementType::FOOTER);
	}

public:
	string result;

private:
	void Append(const string &text, HighlightElementType element_type) {
		auto &element = ShellHighlight::GetHighlightElement(element_type);
		auto color = highlight ? ShellHighlight::TerminalCode(element.color, element.intensity) : string();
		if (color.empty()) {
			result += text;
			return;
		}
		result += color + text + ShellHighlight::ResetTerminalCode();
	}

private:
	bool highlight;
};

ShellLivePreview::ShellLivePreview(ShellState &state) : state(state), running(false), discarded(false) {
}

ShellLivePreview::~ShellLivePreview() {
	// the editor must not be left holding a pointer to a provider that no longer exists
	if (duckdb::LivePreviewProvider::Get() == this) {
		duckdb::LivePreviewProvider::Set(nullptr);
	}
	Cancel();
	Join();
}

bool ShellLivePreview::IsRunning() {
	return running && !discarded;
}

bool ShellLivePreview::HasPanel() {
	duckdb::lock_guard<duckdb::mutex> guard(panel_lock);
	return status != LivePreviewStatus::EMPTY;
}

void ShellLivePreview::Cancel() {
	if (!running || !state.conn) {
		return;
	}
	discarded = true;
	{
		duckdb::lock_guard<duckdb::mutex> guard(panel_lock);
		if (status == LivePreviewStatus::RUNNING) {
			// the query is being abandoned, so the spinner must not freeze on screen. the panel no longer
			// describes the buffer either - clearing the sql makes the next Start run it again
			ClearPanelContents();
			panel_sql.clear();
		}
	}
	// the preview shares the connection with the shell, so interrupting it is the same interrupt the
	// Ctrl+C path uses
	state.conn->Interrupt();
}

void ShellLivePreview::ClearPanel() {
	Cancel();
	duckdb::lock_guard<duckdb::mutex> guard(panel_lock);
	panel_sql.clear();
	// the shell is about to run something of its own, which may change what this statement would
	// return - drop the cached parse tree so the next preview runs it again
	ClearPanelContents();
}

void ShellLivePreview::Join() {
	if (worker.joinable()) {
		worker.join();
	}
	if (!discarded) {
		return;
	}
	discarded = false;
	// the interrupt we raised to stop the worker must not leak into the next query
	if (state.conn) {
		state.conn->context->ClearInterrupt();
	}
}

void ShellLivePreview::Invalidate() {
	ClearPanel();
	Join();
}

void ShellLivePreview::ClearPanelContents() {
	status = LivePreviewStatus::EMPTY;
	message.clear();
	error_offset = duckdb::optional_idx();
	error_length = 1;
	preview_rows.reset();
	column_names.clear();
	elapsed_seconds = 0;
	panel_tree.clear();
}

void ShellLivePreview::Start(const char *buf, idx_t len) {
	string sql(buf, len);
	// error positions are offsets into the sql we hand the engine - remember how far that is from the
	// start of the buffer, so the editor can point at the right character
	idx_t leading_whitespace = 0;
	while (leading_whitespace < sql.size() && StringUtil::CharacterIsSpace(sql[leading_whitespace])) {
		leading_whitespace++;
	}
	StringUtil::Trim(sql);
	// Run() compares parse trees, but that only helps once a preview has finished - deciding whether to
	// abandon one that is still in flight happens here, before anything is parsed. Trailing semicolons
	// and whitespace are the edits people make to a query they have just finished typing, so strip them
	// rather than throw the running query away over them.
	while (!sql.empty() && (sql.back() == ';' || StringUtil::CharacterIsSpace(sql.back()))) {
		sql.pop_back();
	}
	{
		duckdb::lock_guard<duckdb::mutex> guard(panel_lock);
		if (sql == panel_sql) {
			// the panel already describes this buffer, or a query computing exactly it is still in flight -
			// either way there is nothing to redo
			return;
		}
	}
	// a previous preview may still be winding down - it holds the connection, so wait for it
	Cancel();
	Join();

	{
		duckdb::lock_guard<duckdb::mutex> guard(panel_lock);
		panel_sql = sql;
		sql_offset = leading_whitespace;
	}
	if (sql.empty() || sql[0] == '.') {
		// nothing to preview - dot commands are not SQL
		duckdb::lock_guard<duckdb::mutex> guard(panel_lock);
		ClearPanelContents();
		return;
	}
	if (!state.conn->context->transaction.IsAutoCommit()) {
		// inside an explicit transaction a failed preview would abort the user's transaction
		{
			duckdb::lock_guard<duckdb::mutex> guard(panel_lock);
			ClearPanelContents();
		}
		SetStatus(LivePreviewStatus::SUSPENDED, "live preview paused inside a transaction");
		return;
	}
	// the panel is left alone until Run has parsed the buffer - if it parses to the statement already
	// on screen there is nothing to redo, and clearing it here would make it flicker for no reason
	running = true;
	worker = std::thread([this, sql]() {
		try {
			Run(sql);
		} catch (std::exception &ex) {
			SetStatus(LivePreviewStatus::ERROR, ErrorData(ex).Message());
		}
		running = false;
	});
}

void ShellLivePreview::SetError(const ErrorData &error) {
	// an error that was never run through ClientContext::ProcessError still carries the source range it
	// refers to - "position" is the offset, "location" is "[offset,length]"
	duckdb::optional_idx offset;
	idx_t length = 1;
	auto &extra_info = error.ExtraInfo();
	auto position_entry = extra_info.find("position");
	if (position_entry != extra_info.end()) {
		try {
			offset = duckdb::optional_idx(std::stoull(position_entry->second));
		} catch (std::exception &) { // NOLINT: a malformed position simply means we cannot point at it
			offset = duckdb::optional_idx();
		}
	}
	auto location_entry = extra_info.find("location");
	if (offset.IsValid() && location_entry != extra_info.end()) {
		auto comma = location_entry->second.find(',');
		auto end = location_entry->second.find(']');
		if (comma != string::npos && end != string::npos && end > comma + 1) {
			try {
				length = std::stoull(location_entry->second.substr(comma + 1, end - comma - 1));
			} catch (std::exception &) { // NOLINT: fall back to a single caret
				length = 1;
			}
		}
	}
	if (discarded) {
		return;
	}
	duckdb::lock_guard<duckdb::mutex> guard(panel_lock);
	status = LivePreviewStatus::ERROR;
	// the final message carries the exception type ("Binder Error: ...") but not the source excerpt,
	// which has not been folded in yet - that is exactly what we want to render next to our own caret
	message = error.Message();
	error_offset = offset;
	error_length = duckdb::MaxValue<idx_t>(length, 1);
}

void ShellLivePreview::SetStatus(LivePreviewStatus new_status, const string &new_message) {
	if (discarded) {
		// the buffer moved on - whatever we found describes a query nobody is waiting for any more,
		// and the error we would report is our own interrupt
		return;
	}
	duckdb::lock_guard<duckdb::mutex> guard(panel_lock);
	status = new_status;
	message = new_message;
}

void ShellLivePreview::Run(const string &sql) {
	auto &con = *state.conn;
	// parse first: a buffer that is still being typed is usually an incomplete statement, and we only
	// preview a buffer that holds exactly one statement
	duckdb::vector<duckdb::unique_ptr<duckdb::SQLStatement>> statements;
	try {
		statements = con.ExtractStatements(sql);
	} catch (std::exception &ex) {
		{
			duckdb::lock_guard<duckdb::mutex> guard(panel_lock);
			ClearPanelContents();
		}
		SetError(ErrorData(ex));
		return;
	}
	if (statements.size() != 1) {
		SetStatus(LivePreviewStatus::EMPTY, string());
		return;
	}
	// text that parses to the statement already on screen produces the same answer, so finishing a line
	// off with a semicolon, a space or a comment must not send the query round again
	string tree;
	try {
		tree = statements[0]->ToString();
	} catch (std::exception &) { // NOLINT: a statement we cannot print simply never matches
		tree.clear();
	}
	{
		duckdb::lock_guard<duckdb::mutex> guard(panel_lock);
		if (!tree.empty() && tree == panel_tree && status != LivePreviewStatus::EMPTY) {
			// keep what is on screen
			return;
		}
		// from here on the panel describes this statement, whatever it turns out to be
		ClearPanelContents();
		panel_tree = tree;
		status = LivePreviewStatus::RUNNING;
		start_time = TimePoint::Tick();
	}
	// bind a copy of the statement - this reports parser and binder errors without running anything, and
	// unlike Prepare it hands us the error before the position is folded into the message
	duckdb::StatementSignature signature;
	try {
		signature = con.context->BindStatement(statements[0]->Copy());
	} catch (std::exception &ex) {
		SetError(ErrorData(ex));
		return;
	}
	auto &properties = signature.properties;
	if (!properties.IsReadOnly()) {
		SetStatus(LivePreviewStatus::WRITE_STATEMENT, "modifies the database \u2014 press Enter to run it");
		return;
	}
	if (properties.return_type != duckdb::StatementReturnType::QUERY_RESULT || properties.parameter_count > 0) {
		// nothing to render: the statement returns no rows, or it needs parameters we do not have
		SetStatus(LivePreviewStatus::EMPTY, string());
		return;
	}
	// stream the result so we only ever pay for the rows we show
	auto result = con.SendQuery(std::move(statements[0]));
	if (result->HasError()) {
		// an execution error has already been through ProcessError, so it carries its own excerpt
		SetStatus(LivePreviewStatus::ERROR, result->GetError());
		return;
	}
	auto collection =
	    duckdb::make_uniq<duckdb::ColumnDataCollection>(duckdb::Allocator::DefaultAllocator(), result->GetTypes());
	// read the whole result, the same way duckbox does for a submitted query, so the footer reports the
	// real row count. a preview that takes too long is cancelled by the next key press
	while (true) {
		auto chunk = result->Fetch();
		if (!chunk || chunk->size() == 0) {
			break;
		}
		collection->Append(*chunk);
	}
	duckdb::vector<string> names;
	for (idx_t c = 0; c < result->ColumnCount(); c++) {
		names.push_back(result->ColumnName(c).GetIdentifierName());
	}
	// the result is dropped here, which tears down the pipeline without running it to completion
	result.reset();
	if (discarded) {
		return;
	}
	duckdb::lock_guard<duckdb::mutex> guard(panel_lock);
	status = LivePreviewStatus::RESULT;
	preview_rows = std::move(collection);
	column_names = std::move(names);
	elapsed_seconds = start_time.ElapsedSeconds();
}

//! Break text into lines that fit within width, preferring to break on spaces. Counts bytes rather
//! than render width, which is close enough for error messages - the editor truncates as a backstop.
static duckdb::vector<string> WrapText(const string &text, idx_t width) {
	duckdb::vector<string> result;
	for (auto &line : StringUtil::Split(text, '\n')) {
		if (line.empty() || width < 2) {
			result.push_back(line);
			continue;
		}
		idx_t start = 0;
		while (start < line.size()) {
			if (line.size() - start <= width) {
				result.push_back(line.substr(start));
				break;
			}
			auto split = line.rfind(' ', start + width);
			if (split == string::npos || split <= start) {
				// a single word longer than the line - break it
				split = start + width;
			}
			result.push_back(line.substr(start, split - start));
			start = split;
			while (start < line.size() && line[start] == ' ') {
				start++;
			}
		}
	}
	return result;
}

//! Wrap text in the terminal codes for a highlight element
static string Colored(const string &text, HighlightElementType element_type) {
	if (!ShellHighlight::IsEnabled()) {
		return text;
	}
	auto &element = ShellHighlight::GetHighlightElement(element_type);
	auto color = ShellHighlight::TerminalCode(element.color, element.intensity);
	if (color.empty()) {
		return text;
	}
	return color + text + ShellHighlight::ResetTerminalCode();
}

//! Whether the line holds anything other than terminal escape sequences
static bool HasVisibleText(const string &line) {
	for (idx_t i = 0; i < line.size(); i++) {
		if (line[i] == '\x1b') {
			// skip over the escape sequence
			while (i < line.size() && line[i] != 'm') {
				i++;
			}
			continue;
		}
		if (!duckdb::StringUtil::CharacterIsSpace(line[i])) {
			return true;
		}
	}
	return false;
}

bool ShellLivePreview::TryGetErrorLocation(idx_t &offset, idx_t &length) {
	duckdb::lock_guard<duckdb::mutex> guard(panel_lock);
	if (status != LivePreviewStatus::ERROR || !error_offset.IsValid()) {
		return false;
	}
	offset = error_offset.GetIndex() + sql_offset;
	length = error_length;
	return true;
}

string ShellLivePreview::RenderProgressBar(double elapsed_seconds, idx_t available_width) {
	auto &config = duckdb::ClientConfig::GetConfig(*state.conn->context);
	if (!config.enable_progress_bar || config.wait_time < 0) {
		return string();
	}
	// the engine keeps its own startup delay before a progress bar is worth showing - wait it out too,
	// so a query that finishes just after the spinner appears does not also flash a bar
	if (elapsed_seconds * 1000 < static_cast<double>(config.wait_time)) {
		return string();
	}
	auto percentage = state.conn->context->GetQueryProgress().GetPercentage();
	if (percentage < 0) {
		// the plan cannot report progress
		return string();
	}
	duckdb::ProgressBarDisplayInfo display_info;
	// " " + bar + " 100%" - leave the bar out entirely rather than let it spill off the line
	idx_t overhead = 7;
	if (available_width < overhead + 8) {
		return string();
	}
	display_info.width = duckdb::MinValue<idx_t>(display_info.width, available_width - overhead);
	auto rounded = static_cast<int32_t>(percentage);
	return " " + duckdb::TerminalProgressBarDisplay::FormatProgressBar(display_info, rounded) +
	       StringUtil::Format(" %d%%", rounded);
}

string ShellLivePreview::RenderMessage(const string &text, HighlightElementType element_type, idx_t max_rows) {
	string result;
	idx_t rendered_rows = 0;
	for (auto &line : StringUtil::Split(text, '\n')) {
		if (rendered_rows >= max_rows) {
			break;
		}
		if (rendered_rows > 0) {
			result += "\n";
		}
		result += Colored(line, element_type);
		rendered_rows++;
	}
	return result;
}

string ShellLivePreview::RenderError(const LivePreviewAnchor &anchor, idx_t max_rows, idx_t max_cols) {
	if (!anchor.column.IsValid()) {
		// the editor cannot point at the offending token - fall back to the engine's own excerpt, which
		// repeats the line the error is on and underlines it there
		auto text = message;
		if (error_offset.IsValid()) {
			text = duckdb::QueryErrorContext::Format(panel_sql, message, error_offset.GetIndex(), error_length);
		}
		return RenderMessage(text, HighlightElementType::ERROR_TOKEN, max_rows);
	}
	auto column = anchor.column.GetIndex();
	auto width = duckdb::MaxValue<idx_t>(anchor.width, 1);
	// the tick sits in the middle of the underlined token and leads down to the message
	auto tick_offset = width / 2;

	string marker;
	for (idx_t i = 0; i < width; i++) {
		marker += i == tick_offset ? MARKER_TICK : MARKER_LINE;
	}

	duckdb::vector<string> lines;
	lines.push_back(string(column, ' ') + Colored(marker, HighlightElementType::ERROR_TOKEN));

	auto text_column = column + tick_offset;
	bool elbow_fits = text_column + MARKER_ELBOW_WIDTH + MARKER_MIN_TEXT_WIDTH <= max_cols;
	// hang the message off the tick when there is room for it, otherwise start it at the left edge
	auto indent = elbow_fits ? text_column + MARKER_ELBOW_WIDTH : 0;
	bool first = true;
	for (auto &line : WrapText(message, max_cols - indent)) {
		if (lines.size() >= max_rows) {
			break;
		}
		if (first && elbow_fits) {
			lines.push_back(string(text_column, ' ') +
			                Colored(string(MARKER_ELBOW) + line, HighlightElementType::ERROR_TOKEN));
		} else {
			lines.push_back(string(indent, ' ') + Colored(line, HighlightElementType::ERROR_TOKEN));
		}
		first = false;
	}
	return StringUtil::Join(lines, "\n");
}

duckdb::vector<string> ShellLivePreview::RenderBox(const duckdb::BoxRendererConfig &config_p, idx_t value_rows) {
	auto config = config_p;
	config.max_rows = value_rows;

	StringBoxRenderer renderer(ShellHighlight::IsEnabled());
	duckdb::ColumnDataCollectionWrapper wrapper(*preview_rows);
	duckdb::ClientBoxRendererContext context(*state.conn->context);
	duckdb::BoxRenderer box_renderer(config);
	box_renderer.Render(context, column_names, wrapper, renderer);

	// the box renderer ends with a newline, wrapped in colour codes - drop every trailing line that
	// holds no visible text, so the timing line lands directly below the box
	auto lines = StringUtil::Split(renderer.result, '\n');
	while (!lines.empty() && !HasVisibleText(lines.back())) {
		lines.pop_back();
	}
	return lines;
}

string ShellLivePreview::RenderResult(idx_t max_rows, idx_t max_cols) {
	duckdb::BoxRendererConfig config;
	config.max_width = max_cols;
	config.null_value = state.nullValue;

	// the box wraps the values in a header, a separator, a bottom border and a footer, and grows an
	// ellipsis when it hides rows - rather than predict how many lines that is, search for the largest
	// row count whose box still fits in the panel (+1 for the timing line we append below)
	duckdb::vector<string> lines;
	idx_t lower = 1;
	// the panel can never show more values than it has lines, so there is no point searching past that
	idx_t upper = duckdb::MaxValue<idx_t>(duckdb::MinValue<idx_t>(preview_rows->Count(), max_rows), 1);
	while (lower <= upper) {
		auto value_rows = (lower + upper) / 2;
		auto rendered = RenderBox(config, value_rows);
		if (rendered.size() + 1 <= max_rows) {
			lines = std::move(rendered);
			lower = value_rows + 1;
		} else {
			upper = value_rows - 1;
		}
	}
	if (lines.empty()) {
		// not even one row fits - render the smallest box there is and let the editor clip it
		lines = RenderBox(config, 1);
	}
	if (elapsed_seconds >= LIVE_PREVIEW_SPINNER_DELAY_SECONDS) {
		// only worth a line of its own when the query took long enough to notice - for everything else
		// the box alone is the answer
		auto timing = StringUtil::Format("%s%.0f ms", NOTE_GLYPH, elapsed_seconds * 1000);
		lines.push_back(RenderMessage(timing, HighlightElementType::FOOTER, 1));
	}
	return StringUtil::Join(lines, "\n");
}

string ShellLivePreview::Render(idx_t max_rows, idx_t max_cols, const LivePreviewAnchor &anchor) {
	duckdb::lock_guard<duckdb::mutex> guard(panel_lock);
	switch (status) {
	case LivePreviewStatus::RUNNING: {
		auto elapsed_seconds = start_time.ElapsedSeconds();
		if (elapsed_seconds < LIVE_PREVIEW_SPINNER_DELAY_SECONDS) {
			// too early to tell whether this query is slow - show nothing rather than a flash
			return string();
		}
		auto frame = SPINNER_FRAMES[spinner_index++ % SPINNER_FRAME_COUNT];
		auto line = StringUtil::Format("%s running\u2026 %.1fs", frame, elapsed_seconds);
		line += RenderProgressBar(elapsed_seconds, max_cols - line.size());
		return RenderMessage(line, HighlightElementType::FOOTER, max_rows);
	}
	case LivePreviewStatus::ERROR:
		return RenderError(anchor, max_rows, max_cols);
	case LivePreviewStatus::WRITE_STATEMENT:
	case LivePreviewStatus::SUSPENDED:
		return RenderMessage(string(NOTE_GLYPH) + message, HighlightElementType::FOOTER, max_rows);
	case LivePreviewStatus::RESULT:
		try {
			return RenderResult(max_rows, max_cols);
		} catch (std::exception &ex) {
			return RenderMessage(ErrorData(ex).Message(), HighlightElementType::ERROR_TOKEN, max_rows);
		}
	default:
		return string();
	}
}

} // namespace duckdb_shell
