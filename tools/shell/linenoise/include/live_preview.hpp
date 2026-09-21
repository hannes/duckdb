//===----------------------------------------------------------------------===//
//                         DuckDB
//
// live_preview.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/common.hpp"
#include "duckdb/common/optional_idx.hpp"

namespace duckdb {

//! Where the panel may draw a marker pointing back at the line being edited: the screen column the
//! error starts at and how wide it is. Only set when the editor can line a marker up with the input.
struct LivePreviewAnchor {
	optional_idx column;
	idx_t width = 1;
};

//! Interface implemented by the shell. The editor drives the preview through it without knowing
//! anything about queries or result rendering - it only starts, cancels and renders.
class LivePreviewProvider {
public:
	virtual ~LivePreviewProvider() = default;

public:
	//! Start a preview for the given buffer - returns immediately, the work happens in the background
	virtual void Start(const char *buf, idx_t len) = 0;
	//! Ask an in-flight preview to stop - returns without waiting for it, so that typing stays
	//! responsive while a slow query winds down
	virtual void Cancel() = 0;
	//! Cancel any in-flight preview and forget the panel, so nothing is rendered until the buffer is
	//! previewed again - used when the editor is about to hand the screen back to the shell
	virtual void ClearPanel() = 0;
	//! Whether a preview is currently in flight
	virtual bool IsRunning() = 0;
	//! Whether there is anything to render for the current buffer
	virtual bool HasPanel() = 0;
	//! Render the panel into at most max_rows rows of at most max_cols columns. A valid anchor lets an
	//! error underline the token it refers to, instead of repeating the line the error is on.
	virtual string Render(idx_t max_rows, idx_t max_cols, const LivePreviewAnchor &anchor) = 0;
	//! The range in the edit buffer that the panel's error points at, if there is one
	virtual bool TryGetErrorLocation(idx_t &offset, idx_t &length) = 0;

	//! The provider used by the editor, or nullptr if live mode is off
	static void Set(LivePreviewProvider *provider);
	static LivePreviewProvider *Get();
};

} // namespace duckdb
