#include "duckdb/function/window/window_match_recognize.hpp"

namespace duckdb {

//===--------------------------------------------------------------------===//
// WindowMatchRecognizeExecutor
//===--------------------------------------------------------------------===//
WindowMatchRecognizeExecutor::WindowMatchRecognizeExecutor(BoundWindowExpression &wexpr, ClientContext &context,
                                                           WindowSharedExpressions &shared)
    : WindowExecutor(wexpr, context, shared) {
	D_ASSERT(0);
}

unique_ptr<WindowExecutorGlobalState>
WindowMatchRecognizeExecutor::GetGlobalState(const idx_t payload_count, const ValidityMask &partition_mask,
                                             const ValidityMask &order_mask) const {
	D_ASSERT(0);
	return nullptr;
}

unique_ptr<WindowExecutorLocalState>
WindowMatchRecognizeExecutor::GetLocalState(const WindowExecutorGlobalState &gstate) const {
	D_ASSERT(0);
	return nullptr;
}

void WindowMatchRecognizeExecutor::EvaluateInternal(WindowExecutorGlobalState &gstate, WindowExecutorLocalState &lstate,
                                                    DataChunk &eval_chunk, Vector &result, idx_t count,
                                                    idx_t row_idx) const {
	D_ASSERT(0);
}

} // namespace duckdb
