#include "duckdb/common/pair.hpp"
#include "duckdb/function/match_recognize.hpp"
#include "duckdb/planner/expression/bound_aggregate_expression.hpp"
#include "duckdb/function/aggregate_function.hpp"
#include "duckdb/logging/logger.hpp"

namespace duckdb {

static idx_t MatchRecognizeStateSize(const AggregateFunction &function) {
	return 0;
}

static void MatchRecognizeStateInitialize(const AggregateFunction &function, data_ptr_t state) {
}

static void MatchRecognizeUpdate(Vector inputs[], AggregateInputData &aggr_input_data, idx_t input_count, Vector &state,
                                 idx_t count) {
}

static void MatchRecognizeFinalize(Vector &state, AggregateInputData &aggr_input_data, Vector &result, idx_t count,
                                   idx_t offset) {
	// DUCKDB_LOG_INFO(context, "duckdb.MatchRecognize", "MatchRecognizeFinalize");
}

static unique_ptr<FunctionData> MatchRecognizeBind(ClientContext &context, AggregateFunction &function,
                                                   vector<unique_ptr<Expression>> &arguments) {
	DUCKDB_LOG_INFO(context, "duckdb.MatchRecognize", "MatchRecognizeBind");

	return nullptr;
}

static void MatchRecognizeWindow(AggregateInputData &aggr_input_data, const WindowPartitionInput &partition,
                                 const_data_ptr_t g_state, data_ptr_t l_state, const SubFrames &subframes,
                                 Vector &result, idx_t rid) {
}

AggregateFunction MatchRecognizeFun::GetFunction() {
	auto func = AggregateFunction({LogicalType::ANY}, LogicalTypeId::ANY, MatchRecognizeStateSize,
	                              MatchRecognizeStateInitialize, MatchRecognizeUpdate, nullptr, MatchRecognizeFinalize,
	                              nullptr, MatchRecognizeBind, nullptr, nullptr, nullptr);

	return func;
}

} // namespace duckdb

// combine will  have to re-sort and partition??
// TODO now to trigger this
// will have to happen in transformer?
