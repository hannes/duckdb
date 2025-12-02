//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/function/window/window_nonoverlap_intervals.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/function/window/window_executor.hpp"

namespace duckdb {

class WindowNonOverlapIntervalsExecutor : public WindowExecutor {
public:
	WindowNonOverlapIntervalsExecutor(BoundWindowExpression &wexpr, WindowSharedExpressions &shared);

	unique_ptr<GlobalSinkState> GetGlobalState(ClientContext &client, const idx_t payload_count,
	                                           const ValidityMask &partition_mask,
	                                           const ValidityMask &order_mask) const override;

	column_t low_idx = DConstants::INVALID_INDEX;
	column_t high_idx = DConstants::INVALID_INDEX;
	column_t inclusive_idx = DConstants::INVALID_INDEX;

protected:
	void EvaluateInternal(ExecutionContext &context, DataChunk &eval_chunk, Vector &result, idx_t count, idx_t row_idx,
	                      OperatorSinkInput &sink) const override;
};

} // namespace duckdb
