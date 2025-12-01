#include "duckdb/parser/tableref/match_recognize_ref.hpp"
#include "duckdb/parser/expression/pattern_expression.hpp"

#include "duckdb/function/cast/cast_function_set.hpp"
#include "duckdb/function/function_list.hpp"
#include "duckdb/function/match_recognize.hpp"

#include "duckdb/planner/binder.hpp"
#include "duckdb/planner/expression_binder.hpp"
#include "duckdb/planner/expression_iterator.hpp"
#include "duckdb/planner/expression/bound_cast_expression.hpp"
#include "duckdb/planner/expression/bound_constant_expression.hpp"
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
	// apply pattern matching window (todo: complete implementation)
	// filter out only complete matches
	// apply after match window
	// filter out only non-overlapping matches
	// todo: all rows per match, measures, final, ...
	// final projection that hides intermediate structures

	// we bind everything regarding match recognize in a child binder such that these bindings (e.g. of the input table)
	// are hidden, and we can add a new binding for the input columns and the measures columns to the match recognize
	// alias.
	auto child_binder = Binder::CreateBinder(context, this);
	auto child_node = child_binder->Bind(*ref.input);
	auto &config = DBConfig::GetConfig(context);

	// Pattern Matching Window: return type
	auto pattern_return_type = LogicalType::STRUCT({{"classifiers", LogicalType::LIST(LogicalType::VARCHAR)},
	                                                {"complete", LogicalType::BOOLEAN},
	                                                {"match_start", LogicalType::UBIGINT},
	                                                {"match_end", LogicalType::UBIGINT},
	                                                {"skip_to", LogicalType::UBIGINT}});

	// Pattern Matching Window: window expression
	auto pattern_window =
	    make_uniq<BoundWindowExpression>(ExpressionType::WINDOW_MATCH_RECOGNIZE, pattern_return_type,
	                                     MatchRecognizeFun::GetFunction(), make_uniq<MatchRecognizeFunctionData>());
	pattern_window->start = WindowBoundary::UNBOUNDED_PRECEDING;
	pattern_window->end = WindowBoundary::UNBOUNDED_FOLLOWING;

	// the expression_binder needs the child_binder as its parent to reference to columns of the input table
	ExpressionBinder expression_binder(*child_binder, context);

	// copy partitions to bind them twice in different places
	vector<unique_ptr<ParsedExpression>> partitions;
	for (auto &expr : ref.config->partition_expressions) {
		partitions.push_back(expr->Copy());
	}

	for (auto &expr : partitions) {
		pattern_window->partitions.push_back(expression_binder.Bind(expr));
	}

	for (auto &order : ref.config->order_by_expressions) {
		auto type = config.ResolveOrder(context, order.type);
		auto null_order = config.ResolveNullOrder(context, type, order.null_order);
		pattern_window->orders.emplace_back(type, null_order, expression_binder.Bind(order.expression));
	}

	// Pattern Matching Window: Define List
	// we need to extract column refs, those that are used in defines become parameters of the window functions
	// we should hand over the **bound** expressions
	auto pattern_binder = Binder::CreateBinder(context);
	ExpressionBinder pattern_expr_binder(*pattern_binder, context);
	vector<string> define_names;
	for (auto &expr : ref.config->defines_expression_list) {
		auto bound_expr = expression_binder.Bind(expr);
		define_names.push_back(bound_expr->GetAlias());
		ExtractColumnBindings(bound_expr, pattern_window->children);
		pattern_window->bind_info->Cast<MatchRecognizeFunctionData>().defines_expression_list.emplace_back(
		    std::move(bound_expr));
	}
	pattern_binder->bind_context.AddGenericBinding(0, "__match_recognize_defines", define_names,
	                                               {ref.config->defines_expression_list.size(), LogicalType::BOOLEAN});

	auto bound_pattern = pattern_expr_binder.Bind(ref.config->pattern);
	pattern_window->bind_info->Cast<MatchRecognizeFunctionData>().pattern = std::move(bound_pattern);

	// Pattern Matching Window: Generate Table
	auto pattern_table_index = GenerateTableIndex();
	auto pattern_window_operator = make_uniq<LogicalWindow>(pattern_table_index);
	pattern_window_operator->expressions.push_back(std::move(pattern_window));
	pattern_window_operator->children.push_back(std::move(child_node.plan));
	pattern_window_operator->ResolveOperatorTypes();

	auto pattern_window_binder = Binder::CreateBinder(context, child_binder.get());
	pattern_window_binder->bind_context.AddGenericBinding(pattern_table_index, "__match_recognize_pattern_window",
	                                                      {"__pattern_window_struct"}, {pattern_return_type});

	// Filter for complete matches
	auto pattern_window_bindings = pattern_window_operator->GetColumnBindings();
	auto pattern_struct_bound = make_uniq<BoundColumnRefExpression>(
	    "pattern_struct_bound", pattern_return_type, pattern_window_bindings[pattern_window_bindings.size() - 1]);
	unique_ptr<Expression> pattern_struct_expr =
	    std::move(make_uniq<BoundExpression>(std::move(pattern_struct_bound))->expr);
	auto pattern_struct_complete = CreateBoundStructExtract(context, std::move(pattern_struct_expr), "complete");

	auto complete_filter = make_uniq<LogicalFilter>(std::move(pattern_struct_complete));
	complete_filter->children.push_back(std::move(pattern_window_operator));
	complete_filter->ResolveOperatorTypes();

	// Final Projection: collect names and types of all bindings present in our last operator
	// we add more to names, types and bindings if we have an after match window
	vector<string> names;
	vector<LogicalType> types;
	child_binder->bind_context.GetTypesAndNames(names, types);
	pattern_window_binder->bind_context.GetTypesAndNames(names, types);
	auto bindings = complete_filter->GetColumnBindings();

	// After Match skip option
	bool is_skip_to_next_row =
	    ref.config->after_match == MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_NEXT_ROW;
	unique_ptr<LogicalFilter> last_filter = nullptr;

	if (is_skip_to_next_row) {
		// No window and filter needed
		last_filter = std::move(complete_filter);
	} else {
		// After Match window
		auto after_match_return_type = LogicalType::BOOLEAN;
		auto after_match_window = make_uniq<BoundWindowExpression>(ExpressionType::WINDOW_NON_OVERLAP_INTERVALS,
		                                                           after_match_return_type, nullptr, nullptr);
		after_match_window->start = WindowBoundary::UNBOUNDED_PRECEDING;
		after_match_window->end = WindowBoundary::CURRENT_ROW_RANGE;

		// bind same partitions to second window as well
		for (auto &expr : ref.config->partition_expressions) {
			after_match_window->partitions.push_back(expression_binder.Bind(expr));
		}

		// After Match Window: actualize the implicit ORDER BY
		// TODO: we could spare us some of this if the intervals window took the struct as input
		auto complete_filter_bindings = complete_filter->GetColumnBindings();
		auto pattern_struct_bound2 =
		    make_uniq<BoundColumnRefExpression>("pattern_struct_bound2", pattern_return_type,
		                                        complete_filter_bindings[complete_filter_bindings.size() - 1]);
		unique_ptr<Expression> pattern_struct_expr2 =
		    std::move(make_uniq<BoundExpression>(std::move(pattern_struct_bound2))->expr);
		unique_ptr<Expression> pattern_struct_expr3 = pattern_struct_expr2->Copy();
		unique_ptr<Expression> pattern_struct_expr4 = pattern_struct_expr2->Copy();

		// enforce ordering
		auto low = CreateBoundStructExtract(context, std::move(pattern_struct_expr2), "match_start");
		BoundOrderByNode order_node(OrderType::ASCENDING, OrderByNullType::NULLS_LAST, std::move(low));
		after_match_window->orders.emplace_back(order_node.type, order_node.null_order,
		                                        std::move(order_node.expression));

		// lower interval bound: match_start
		auto low2 = CreateBoundStructExtract(context, std::move(pattern_struct_expr3), "match_start");
		after_match_window->children.push_back(std::move(low2));

		// upper interval bound: depends on the skip option
		switch (ref.config->after_match) {
		case MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_FIRST_VAR:
		case MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_LAST_VAR: {
			auto high = CreateBoundStructExtract(context, std::move(pattern_struct_expr4), "skip_to");
			after_match_window->children.push_back(std::move(high));
			after_match_window->inclusive = make_uniq<BoundConstantExpression>(Value::BOOLEAN(true));
			break;
		}
		case MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_LAST_ROW:
		case MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_DEFAULT:
		default: {
			auto high = CreateBoundStructExtract(context, std::move(pattern_struct_expr4), "match_end");
			after_match_window->children.push_back(std::move(high));
			after_match_window->inclusive = make_uniq<BoundConstantExpression>(Value::BOOLEAN(false));
			break;
		}
		}

		// Pattern Matching Window: Generate Table
		auto after_match_table_index = GenerateTableIndex();
		auto after_match_window_operator = make_uniq<LogicalWindow>(after_match_table_index);
		after_match_window_operator->expressions.push_back(std::move(after_match_window));
		after_match_window_operator->children.push_back(std::move(complete_filter));
		after_match_window_operator->ResolveOperatorTypes();

		auto after_match_binder = Binder::CreateBinder(context, child_binder.get());
		after_match_binder->bind_context.AddGenericBinding(after_match_table_index,
		                                                   "__match_recognize_after_match_window",
		                                                   {"__after_match_window_bool"}, {after_match_return_type});

		// Filter for non-overlapping matches
		auto after_match_window_bindings = after_match_window_operator->GetColumnBindings();
		auto after_match_keep =
		    make_uniq<BoundColumnRefExpression>("after_match_keep", after_match_return_type,
		                                        after_match_window_bindings[after_match_window_bindings.size() - 1]);
		auto no_overlap_filter = make_uniq<LogicalFilter>(std::move(after_match_keep));
		no_overlap_filter->children.push_back(std::move(after_match_window_operator));
		no_overlap_filter->ResolveOperatorTypes();

		// add
		after_match_binder->bind_context.GetTypesAndNames(names, types);
		bindings = no_overlap_filter->GetColumnBindings();
		last_filter = std::move(no_overlap_filter);
	}

	// TODO we need to stack more stuff on top of this operator
	// add the measures columns and whatever else we need to our internal match recognize table

	// Final Projection: retrieve the match recognize alias
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

	// Final Projection: collect names and types of all bindings we want to output
	vector<string> final_names;
	vector<LogicalType> final_types;
	child_binder->bind_context.GetTypesAndNames(final_names, final_types);
	// TODO: in the end, we do not want the struct here but the measures columns
	pattern_window_binder->bind_context.GetTypesAndNames(final_names, final_types);

	// Final Projection: collect all bindings we want to output
	vector<unique_ptr<Expression>> select_expressions;

	for (idx_t i = 0; i < bindings.size(); i++) {
		// pattern window struct is the second last binding; skip it.
		if (!is_skip_to_next_row && i == bindings.size() - 1) {
			continue;
		}
		auto col = make_uniq<BoundColumnRefExpression>(StringUtil::Format("select%d", i), types[i], bindings[i]);
		select_expressions.push_back(std::move(col));
	}

	// Final Projection: Generate Table
	auto final_projection_table_index = GenerateTableIndex();
	auto final_projection = make_uniq<LogicalProjection>(final_projection_table_index, std::move(select_expressions));
	final_projection->children.push_back(std::move(last_filter));
	bind_context.AddGenericBinding(final_projection_table_index, mr_alias, final_names, final_types);

	BoundStatement result_statement;
	result_statement.types = final_types;
	result_statement.names = final_names;
	result_statement.plan = std::move(final_projection);
	return result_statement;
}

} // namespace duckdb
