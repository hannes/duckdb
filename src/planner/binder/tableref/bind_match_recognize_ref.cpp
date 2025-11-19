#include "duckdb/parser/tableref/match_recognize_ref.hpp"
#include "duckdb/parser/expression/pattern_expression.hpp"

#include "duckdb/function/cast/cast_function_set.hpp"
#include "duckdb/function/function_list.hpp"
#include "duckdb/function/match_recognize.hpp"

#include "duckdb/planner/binder.hpp"
#include "duckdb/planner/expression_binder.hpp"
#include "duckdb/planner/expression_iterator.hpp"
#include "duckdb/planner/expression/bound_cast_expression.hpp"
#include "duckdb/planner/expression/bound_reference_expression.hpp"
#include "duckdb/planner/expression/bound_window_expression.hpp"
#include "duckdb/planner/operator/logical_filter.hpp"
#include "duckdb/planner/operator/logical_projection.hpp"
#include "duckdb/planner/operator/logical_window.hpp"

namespace duckdb {

// TODO make abstract superclass
// class BoundPatternExpression : public Expression {
//
// 	vector<unique_ptr<Expression>> children;
//
// };

class BoundAlternationExpression : public Expression {
public:
	BoundAlternationExpression(unique_ptr<Expression> child_left_p, unique_ptr<Expression> child_right_p)
	    : Expression(ExpressionType::ALTERNATION, ExpressionClass::PATTERN, LogicalType::INVALID),
	      child_left(std::move(child_left_p)), child_right(std::move(child_right_p)) {
	}

	unique_ptr<Expression> child_left;
	unique_ptr<Expression> child_right;

	string ToString() const {
		return StringUtil::Format("(%s|%s)", child_left->ToString(), child_right->ToString());
	}

	unique_ptr<Expression> Copy() const {
		auto child_left_copy = child_left->Copy();
		auto child_right_copy = child_right->Copy();
		return make_uniq<BoundAlternationExpression>(std::move(child_left_copy), std::move(child_right_copy));
	}
};

class BoundConcatenationExpression : public Expression {
public:
	BoundConcatenationExpression(vector<unique_ptr<Expression>> children_p)
	    : Expression(ExpressionType::CONCATENATION, ExpressionClass::PATTERN, LogicalType::INVALID),
	      children(std::move(children_p)) {
	}

	vector<unique_ptr<Expression>> children;

	string ToString() const {
		return StringUtil::Join(children, children.size(), ", ",
		                        [](const unique_ptr<Expression> &expr) { return expr->ToString(); });
	}

	unique_ptr<Expression> Copy() const {
		vector<unique_ptr<Expression>> children_copy;
		for (auto &child : children) {
			children_copy.push_back(child->Copy());
		}
		return make_uniq<BoundConcatenationExpression>(std::move(children_copy));
	}
};

class BoundQuantifierExpression : public Expression {
public:
	BoundQuantifierExpression(unique_ptr<Expression> child_p, optional_idx min_count_p, optional_idx max_count_p)
	    : Expression(ExpressionType::QUANTIFIER, ExpressionClass::PATTERN, LogicalType::INVALID),
	      child(std::move(child_p)), min_count(min_count_p), max_count(max_count_p) {
	}

	unique_ptr<Expression> child;

	optional_idx min_count;
	optional_idx max_count;

	static string QuantifierToString(optional_idx min_count, optional_idx max_count) {
		return StringUtil::Format("{%s,%s}", min_count.IsValid() ? to_string(min_count.GetIndex()) : "",
		                          max_count.IsValid() ? to_string(max_count.GetIndex()) : "");
	}

	string ToString() const {
		return child->ToString() + QuantifierToString(min_count, max_count);
	}

	unique_ptr<Expression> Copy() const {
		auto child_copy = child->Copy();
		return make_uniq<BoundQuantifierExpression>(std::move(child_copy), min_count, max_count);
	}
};

BindResult ExpressionBinder::BindPatternExpression(unique_ptr<ParsedExpression> &expr, idx_t depth) {
	switch (expr->GetExpressionType()) {
	case ExpressionType::ALTERNATION: {
		auto &alternation = expr->Cast<AlternationExpression>();
		auto bound_left = BindExpression(alternation.child_left, depth);
		if (bound_left.HasError()) {
			return BindResult(bound_left.error);
		}
		auto bound_right = BindExpression(alternation.child_right, depth);
		if (bound_right.HasError()) {
			return BindResult(bound_right.error);
		}
		return BindResult(make_uniq_base<Expression, BoundAlternationExpression>(std::move(bound_left.expression),
		                                                                         std::move(bound_right.expression)));
	}
	case ExpressionType::CONCATENATION: {
		auto &concatenation = expr->Cast<ConcatenationExpression>();
		vector<unique_ptr<Expression>> bound_children;
		for (auto &child : concatenation.children) {
			auto child_bind_result = BindExpression(child, depth);
			if (child_bind_result.HasError()) {
				return BindResult(child_bind_result.error);
			}
			bound_children.push_back(std::move(child_bind_result.expression));
		}
		return BindResult(make_uniq_base<Expression, BoundConcatenationExpression>(std::move(bound_children)));
	}
	case ExpressionType::QUANTIFIER: {
		auto &quantifier = expr->Cast<QuantifiedExpression>();
		auto bound_child = BindExpression(quantifier.child, depth);
		if (bound_child.HasError()) {
			return BindResult(bound_child.error);
		}
		return BindResult(make_uniq_base<Expression, BoundQuantifierExpression>(
		    std::move(bound_child.expression), quantifier.min_count, quantifier.max_count));
	}
	default:
		throw NotImplementedException("Unimplemented pattern expression %s", ExpressionTypeToString(expr->type));
	}
}

void ExtractColumnBindings(unique_ptr<Expression> &expr, vector<unique_ptr<Expression>> &bindings) {
	if (expr->GetExpressionType() == ExpressionType::BOUND_COLUMN_REF) {
		auto &bound_ref = expr->Cast<BoundColumnRefExpression>();
		bindings.push_back(bound_ref.Copy());
		expr = make_uniq_base<Expression, BoundReferenceExpression>(bound_ref.return_type, bindings.size() - 1);
	}
	ExpressionIterator::EnumerateChildren(
	    *expr, [&](unique_ptr<Expression> &child) { ExtractColumnBindings(child, bindings); });
}

BoundStatement Binder::Bind(MatchRecognizeRef &ref) {
	// Steps:
	// binding+planning input table
	// apply pattern matching window
	// filter out only complete matches
	// apply after match window
	// filter out only non-overlapping matches
	// all rows per match, measures, final, ...

	// we bind everything regarding match recognize in a child binder such that these bindings (e.g. of the input table)
	// are hidden, and we can add a new binding for the input columns and the measures columns to the match recognize
	// alias.
	auto child_binder = Binder::CreateBinder(context, this);
	auto child_node = child_binder->Bind(*ref.input);
	auto &config = DBConfig::GetConfig(context);

	// Pattern Matching Window: return type
	auto return_type = LogicalType::STRUCT({{"classifiers", LogicalType::LIST(LogicalType::VARCHAR)},
	                                        {"complete", LogicalType::BOOLEAN},
	                                        {"match_start", LogicalType::UBIGINT},
	                                        {"match_end", LogicalType::UBIGINT}});

	// Pattern Matching Window: window expression
	auto window_expression =
	    make_uniq<BoundWindowExpression>(ExpressionType::WINDOW_MATCH_RECOGNIZE, return_type,
	                                     MatchRecognizeFun::GetFunction(), make_uniq<MatchRecognizeFunctionData>());
	window_expression->start = WindowBoundary::UNBOUNDED_PRECEDING;
	window_expression->end = WindowBoundary::UNBOUNDED_FOLLOWING;

	// the expression_binder needs the child_binder as its parent to reference to columns of the input table
	ExpressionBinder expression_binder(*child_binder, context);

	vector<unique_ptr<ParsedExpression>> partitions;
	for (auto &expr : ref.config->partition_expressions) {
		partitions.push_back(expr->Copy());
	}

	for (auto &expr : partitions) {
		window_expression->partitions.push_back(expression_binder.Bind(expr));
	}

	for (auto &order : ref.config->order_by_expressions) {
		auto type = config.ResolveOrder(context, order.type);
		auto null_order = config.ResolveNullOrder(context, type, order.null_order);
		window_expression->orders.emplace_back(type, null_order, expression_binder.Bind(order.expression));
	}

	// Pattern Matching Window: Define List
	// we need to extract column refs, those that are used in defines become parameters of the window functions
	// we should hand over the **bound** expressions
	auto pattern_binder = Binder::CreateBinder(context);
	ExpressionBinder pattern_binder_expr(*pattern_binder, context);
	vector<string> define_names;
	for (auto &expr : ref.config->defines_expression_list) {
		auto bound_expr = expression_binder.Bind(expr);
		define_names.push_back(bound_expr->GetAlias());
		ExtractColumnBindings(bound_expr, window_expression->children);
		window_expression->bind_info->Cast<MatchRecognizeFunctionData>().defines_expression_list.emplace_back(
		    std::move(bound_expr));
	}
	pattern_binder->bind_context.AddGenericBinding(0, "__match_recognize_defines", define_names,
	                                               {ref.config->defines_expression_list.size(), LogicalType::BOOLEAN});

	auto bound_pattern = pattern_binder_expr.Bind(ref.config->pattern);
	window_expression->bind_info->Cast<MatchRecognizeFunctionData>().pattern = std::move(bound_pattern);

	// Pattern Matching Window: Generate Table
	auto table_index1 = GenerateTableIndex();
	auto window_operator = make_uniq<LogicalWindow>(table_index1);
	window_operator->expressions.push_back(std::move(window_expression));
	window_operator->children.push_back(std::move(child_node.plan));
	window_operator->ResolveOperatorTypes();

	// Filter for complete matches
	auto first_window_bindings = window_operator->GetColumnBindings();
	auto bound_col_ref = make_uniq<BoundColumnRefExpression>("bound_col_ref", return_type,
	                                                         first_window_bindings[first_window_bindings.size() - 1]);
	unique_ptr<Expression> col_ref_expr = std::move(make_uniq<BoundExpression>(std::move(bound_col_ref))->expr);
	auto bound_condition = CreateBoundStructExtract(context, std::move(col_ref_expr), "complete");

	auto filter_complete_matches = make_uniq<LogicalFilter>(std::move(bound_condition));
	filter_complete_matches->children.push_back(std::move(window_operator));
	filter_complete_matches->ResolveOperatorTypes();

	auto child_binder2 = Binder::CreateBinder(context, child_binder.get());
	child_binder2->bind_context.AddGenericBinding(table_index1, "__match_recognize_w1", {"__w1_struct"}, {return_type});

	// After Match Window
	auto after_match_return_type = LogicalType::STRUCT({{"classifiers", LogicalType::LIST(LogicalType::VARCHAR)},
	                                                    {"keep", LogicalType::BOOLEAN},
	                                                    {"match_start", LogicalType::UBIGINT},
	                                                    {"match_end", LogicalType::UBIGINT}});

	auto after_match_window = make_uniq<BoundWindowExpression>(ExpressionType::WINDOW_NON_OVERLAP_INTERVALS,
	                                                           after_match_return_type, nullptr, nullptr);
	after_match_window->start = WindowBoundary::UNBOUNDED_PRECEDING;
	after_match_window->end = WindowBoundary::CURRENT_ROW_RANGE;

	// TODO: Monday! continue debugging here: resulttype for expression binding is null and that seems to be a problem.
	// It isn't earlier though. check what happens there. Maybe something else is wrong.
	for (auto &expr : ref.config->partition_expressions) {
		after_match_window->partitions.push_back(expression_binder.Bind(expr));
	}

	// After Match Window: actualize the implicit order by
	auto first_filter_bindings = filter_complete_matches->GetColumnBindings();
	auto bound_col_ref2 = make_uniq<BoundColumnRefExpression>("bound_col_ref2", return_type,
	                                                          first_filter_bindings[first_filter_bindings.size() - 1]);
	unique_ptr<Expression> col_ref_expr2 = std::move(make_uniq<BoundExpression>(std::move(bound_col_ref2))->expr);
	unique_ptr<Expression> col_ref_expr3 = col_ref_expr2->Copy();
	unique_ptr<Expression> col_ref_expr4 = col_ref_expr2->Copy();
	auto low = CreateBoundStructExtract(context, std::move(col_ref_expr2), "match_start");
	auto low2 = CreateBoundStructExtract(context, std::move(col_ref_expr3), "match_start");
	auto high = CreateBoundStructExtract(context, std::move(col_ref_expr4), "match_end");
	BoundOrderByNode order_node(OrderType::ASCENDING, OrderByNullType::NULLS_LAST, std::move(low));
	auto type = config.ResolveOrder(context, order_node.type);
	auto null_order = config.ResolveNullOrder(context, type, order_node.null_order);
	after_match_window->orders.emplace_back(type, null_order, std::move(order_node.expression));
	after_match_window->children.push_back(std::move(low2));
	after_match_window->children.push_back(std::move(high));

	// Pattern Matching Window: Generate Table
	auto table_index2 = GenerateTableIndex();
	auto window_operator2 = make_uniq<LogicalWindow>(table_index2);
	window_operator2->expressions.push_back(std::move(after_match_window));
	window_operator2->children.push_back(std::move(filter_complete_matches));
	window_operator2->ResolveOperatorTypes();

	// Filter for non-overlapping matches
	auto second_window_bindings = window_operator2->GetColumnBindings();
	auto bound_col_ref3 = make_uniq<BoundColumnRefExpression>(
	    "bound_col_ref3", after_match_return_type, second_window_bindings[second_window_bindings.size() - 1]);
	unique_ptr<Expression> col_ref_expr5 = std::move(make_uniq<BoundExpression>(std::move(bound_col_ref3))->expr);
	auto bound_condition2 = CreateBoundStructExtract(context, std::move(col_ref_expr5), "keep");

	auto filter_non_overlapping_matches = make_uniq<LogicalFilter>(std::move(bound_condition2));
	filter_non_overlapping_matches->children.push_back(std::move(window_operator2));
	filter_non_overlapping_matches->ResolveOperatorTypes();

	// add the measures columns or whatever we need to our internal match recognize table
	auto child_binder3 = Binder::CreateBinder(context, child_binder.get());
	child_binder3->bind_context.AddGenericBinding(table_index2, "__match_recognize_ref", {"__match_recognize_fun"},
	                                              {after_match_return_type});
	vector<string> names;
	vector<LogicalType> types;
	child_binder->bind_context.GetTypesAndNames(names, types);
	child_binder2->bind_context.GetTypesAndNames(names, types);
	child_binder3->bind_context.GetTypesAndNames(names, types);

	// TODO we need to stack more stuff on top of this operator

	// Bind the match recognize alias in a subquery
	string mr_alias;
	if (ref.alias.empty()) {
		auto index = unnamed_subquery_index++;
		mr_alias = "unnamed_mr"; //"__match_recognize_ref"
		if (index > 1) {
			mr_alias += to_string(index);
		}
	} else {
		mr_alias = ref.alias;
	}

	// put the match recognize tables into a subquery to hide the original data
	// SELECT * FROM (input_table, window_operator) AS mr_alias
	auto bindings = filter_non_overlapping_matches->GetColumnBindings();
	vector<string> final_names;
	vector<LogicalType> final_types;
	child_binder->bind_context.GetTypesAndNames(final_names, final_types);
	child_binder3->bind_context.GetTypesAndNames(final_names, final_types);
	vector<unique_ptr<Expression>> select_expressions;

	for (idx_t i = 0; i < bindings.size(); i++) {
		if (i == 4)
			continue;
		auto col = make_uniq<BoundColumnRefExpression>(StringUtil::Format("select%d", i), types[i], bindings[i]);
		select_expressions.push_back(std::move(col));
	}

	auto table_index3 = GenerateTableIndex();
	auto log_projection = make_uniq<LogicalProjection>(table_index3, std::move(select_expressions));
	log_projection->children.push_back(std::move(filter_non_overlapping_matches));
	bind_context.AddGenericBinding(table_index3, mr_alias, final_names, final_types);

	BoundStatement result_statement;
	result_statement.types = final_types;
	result_statement.names = final_names;
	result_statement.plan = std::move(log_projection);
	return result_statement;
}

} // namespace duckdb
