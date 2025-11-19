//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/function/window/window_match_recognize.hpp
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

	//	unique_ptr<LocalSinkState> GetLocalState(ExecutionContext &context, const GlobalSinkState &gstate) const
	//override;

	void Finalize(ExecutionContext &context, CollectionPtr collection, OperatorSinkInput &sink) const override;

protected:
	void EvaluateInternal(ExecutionContext &context, DataChunk &eval_chunk, Vector &result, idx_t count, idx_t row_idx,
	                      OperatorSinkInput &sink) const override;

private:
	column_t start_idx;
	column_t end_idx;
};

} // namespace duckdb
