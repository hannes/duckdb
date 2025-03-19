#include "duckdb/parser/tableref/match_recognize_ref.hpp"
#include "duckdb/planner/binder.hpp"

#include "duckdb/planner/operator/logical_window.hpp"
#include "duckdb/function/match_recognize.hpp"
#include "duckdb/planner/expression/bound_window_expression.hpp"
#include "duckdb/planner/tableref/bound_match_recognize.hpp"

namespace duckdb {

unique_ptr<BoundTableRef> Binder::Bind(MatchRecognizeRef &ref) {

	auto child_node = Bind(*ref.input);
	auto child_node_plan = CreatePlan(*child_node);

	// window
	// filter
	// window
	// filter

	// new window expression type

	// re turn boundMatchrecognize ref with logical plan inside
	//
	auto return_type = LogicalType::STRUCT({{"classifiers", LogicalType::LIST(LogicalType::VARCHAR)},
	                                        {"complete", LogicalType::BOOLEAN},
	                                        {"match_start", LogicalType::UBIGINT},
	                                        {"match_end", LogicalType::UBIGINT}});

	auto match_recognize_function_data = make_uniq<MatchRecognizeFunctionData>();
	// TODO bind and put stuff from ref->config into the function data

	auto window_expression =
	    make_uniq<BoundWindowExpression>(ExpressionType::WINDOW_MATCH_RECOGNIZE, return_type,
	                                     MatchRecognizeFun::GetFunction(), std::move(match_recognize_function_data));
	window_expression->start = WindowBoundary::UNBOUNDED_PRECEDING;
	window_expression->end = WindowBoundary::UNBOUNDED_FOLLOWING;

	auto window_operator = make_uniq<LogicalWindow>(GenerateTableIndex());
	window_operator->expressions.push_back(std::move(window_expression));
	window_operator->children.push_back(std::move(child_node_plan));
	window_operator->ResolveOperatorTypes();

	// TODO we need to stack more stuff on top of this operator
	return make_uniq<BoundMatchRecognizeRef>(std::move(window_operator));
}

} // namespace duckdb
