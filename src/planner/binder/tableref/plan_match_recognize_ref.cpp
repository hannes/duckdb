#include "duckdb/planner/binder.hpp"
#include "duckdb/planner/operator/logical_get.hpp"
#include "duckdb/planner/tableref/bound_match_recognize.hpp"
namespace duckdb {

unique_ptr<LogicalOperator> Binder::CreatePlan(BoundMatchRecognizeRef &ref) {
	return std::move(ref.get);
}

} // namespace duckdb
