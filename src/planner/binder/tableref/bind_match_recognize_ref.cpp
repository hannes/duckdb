
#include "duckdb/function/match_recognize.hpp"

#include "duckdb/parser/expression/function_expression.hpp"
#include "duckdb/parser/expression/pattern_expression.hpp"
#include "duckdb/parser/parsed_expression_iterator.hpp"
#include "duckdb/parser/query_node/select_node.hpp"
#include "duckdb/parser/tableref/match_recognize_ref.hpp"
#include "duckdb/parser/tableref/subqueryref.hpp"

#include "duckdb/planner/binder.hpp"
#include "duckdb/planner/expression/bound_constant_expression.hpp"
#include "duckdb/planner/expression/bound_reference_expression.hpp"
#include "duckdb/planner/expression/bound_window_expression.hpp"
#include "duckdb/planner/expression_binder.hpp"
#include "duckdb/planner/expression_iterator.hpp"
#include "duckdb/planner/operator/logical_filter.hpp"

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

static void CheckAndZapDefineQualifiers(ParsedExpression &root_expr, const string &define_name) {
	ParsedExpressionIterator::VisitExpressionMutable<ColumnRefExpression>(root_expr, [&](ColumnRefExpression &colref) {
		if (colref.IsQualified() && colref.GetTableName() != define_name) {
			throw NotImplementedException("Define references cannot refer to other defines just yet");
		}
		colref.column_names = {colref.GetColumnName()};
	});
}

static void ReplaceFunctions(unique_ptr<ParsedExpression> &expr, const WindowExpression &pattern_window) {
	if (expr->GetExpressionType() == ExpressionType::FUNCTION) {
		auto &function = expr->Cast<FunctionExpression>();
		auto function_name = StringUtil::Upper(function.function_name);

		auto new_type = ExpressionType::INVALID;
		if (function_name == "PREV") {
			new_type = ExpressionType::WINDOW_LAG;
		} else if (function_name == "NEXT") {
			new_type = ExpressionType::WINDOW_LEAD;
		} else if (function_name == "FIRST") {
			new_type = ExpressionType::WINDOW_FIRST_VALUE;
		} else if (function_name == "LAST") {
			new_type = ExpressionType::WINDOW_LAST_VALUE;
		} else if (function_name == "MATCH_NUMBER") {
			throw NotImplementedException("MATCH_NUMBER");
		} else if (function_name == "CLASSIFIER") {
			throw NotImplementedException("CLASSIFIER");
		}

		if (new_type != ExpressionType::INVALID) {
			auto new_expr =
			    pattern_window.Copy(); // we copy here because we need to keep all the partitioning and stuff
			auto &new_window = new_expr->Cast<WindowExpression>();
			new_window.type = new_type;
			new_window.function_name = ExpressionTypeToString(new_window.type);
			new_window.children = std::move(function.children);
			expr = std::move(new_expr); // make_uniq_base<ParsedExpression, ColumnRefExpression>(new_window->alias);
		}
		// we do nothing if it's something else
	}
	ParsedExpressionIterator::EnumerateChildren(
	    *expr, [&](unique_ptr<ParsedExpression> &child) { ReplaceFunctions(child, pattern_window); });
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

	auto subquery = std::move(ref.input);
	auto select_node = make_uniq<SelectNode>();
	select_node->from_table = std::move(subquery);
	select_node->select_list.push_back(make_uniq<StarExpression>());

	// Pattern Matching Window: placeholder window expression
	auto pattern_window =
	    make_uniq<WindowExpression>(ExpressionType::WINDOW_MATCH_RECOGNIZE, "", "", "match_recognize");

	pattern_window->start = WindowBoundary::UNBOUNDED_PRECEDING;
	pattern_window->end = WindowBoundary::UNBOUNDED_FOLLOWING;

	// copy partitions to bind them twice in different places
	vector<unique_ptr<ParsedExpression>> partitions;
	for (auto &expr : ref.config->partition_expressions) {
		partitions.push_back(expr->Copy());
	}
	pattern_window->partitions = std::move(partitions);
	pattern_window->orders = std::move(ref.config->order_by_expressions);

	// another select node
	// all the inputs for the defines go in their own select node

	auto define_select_node = make_uniq<SelectNode>();
	define_select_node->from_table = std::move(select_node->from_table);

	vector<unique_ptr<WindowExpression>> child_windows;
	define_select_node->select_list.push_back(make_uniq<StarExpression>());

	// we use this window function as a template for order, partition, and boundaries
	D_ASSERT(pattern_window->children.empty()); // for now
	auto window_template = pattern_window->Copy();

	for (auto &expr : ref.config->defines_expression_list) {
		CheckAndZapDefineQualifiers(*expr, expr->GetAlias());
		ReplaceFunctions(expr, window_template->Cast<WindowExpression>());
		pattern_window->children.push_back(make_uniq<ColumnRefExpression>(expr->alias));
		define_select_node->select_list.push_back(std::move(expr));
	}

	// we abuse the child list to push the pattern to the binder
	pattern_window->children.push_back(std::move(ref.config->pattern));

	auto define_select = make_uniq<SelectStatement>();
	define_select->node = std::move(define_select_node);
	select_node->from_table = make_uniq<SubqueryRef>(std::move(define_select));
	pattern_window->alias = "__pattern_window";

	vector<unique_ptr<ParsedExpression>> qualify_children;
	D_ASSERT(!pattern_window->alias.empty());
	qualify_children.push_back(make_uniq<ColumnRefExpression>(pattern_window->alias));
	qualify_children.push_back(make_uniq<ConstantExpression>("complete"));
	select_node->qualify =
	    make_uniq_base<ParsedExpression, FunctionExpression>("struct_extract", std::move(qualify_children));

	select_node->select_list.push_back(std::move(pattern_window));

	// Final Projection: collect names and types of all bindings present in our last operator
	// we add more to names, types and bindings if we have an after match window

	// After Match skip option
	bool is_skip_to_next_row =
	    ref.config->after_match == MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_NEXT_ROW;
	unique_ptr<LogicalFilter> last_filter = nullptr;

	if (!is_skip_to_next_row) {
		throw NotImplementedException("not implemented");
		// TODO
		// // After Match window
		// auto after_match_return_type = LogicalType::BOOLEAN;
		// auto after_match_window = make_uniq<BoundWindowExpression>(ExpressionType::WINDOW_NON_OVERLAP_INTERVALS,
		// 														   after_match_return_type, nullptr, nullptr);
		// after_match_window->start = WindowBoundary::UNBOUNDED_PRECEDING;
		// after_match_window->end = WindowBoundary::CURRENT_ROW_RANGE;
		//
		// // bind same partitions to second window as well
		// for (auto &expr : ref.config->partition_expressions) {
		// 	after_match_window->partitions.push_back(expression_binder.Bind(expr));
		// }
		//
		// // After Match Window: actualize the implicit ORDER BY
		// // TODO: we could spare us some of this if the intervals window took the struct as input
		// auto complete_filter_bindings = complete_filter->GetColumnBindings();
		// auto pattern_struct_bound2 = make_uniq<BoundColumnRefExpression>(
		// 	"pattern_struct_bound2", pattern_return_type, complete_filter_bindings[complete_filter_bindings.size() -
		// 1]); unique_ptr<Expression> pattern_struct_expr2 =
		// 	std::move(make_uniq<BoundExpression>(std::move(pattern_struct_bound2))->expr);
		// unique_ptr<Expression> pattern_struct_expr3 = pattern_struct_expr2->Copy();
		// unique_ptr<Expression> pattern_struct_expr4 = pattern_struct_expr2->Copy();
		//
		// // enforce ordering
		// auto low = CreateBoundStructExtract(context, std::move(pattern_struct_expr2), "match_start");
		// BoundOrderByNode order_node(OrderType::ASCENDING, OrderByNullType::NULLS_LAST, std::move(low));
		// after_match_window->orders.emplace_back(order_node.type, order_node.null_order,
		// std::move(order_node.expression));
		//
		// // lower interval bound: match_start
		// auto low2 = CreateBoundStructExtract(context, std::move(pattern_struct_expr3), "match_start");
		// after_match_window->children.push_back(std::move(low2));
		//
		// // upper interval bound: depends on the skip option
		// switch (ref.config->after_match) {
		// 	case MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_FIRST_VAR:
		// 	case MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_LAST_VAR: {
		// 		auto high = CreateBoundStructExtract(context, std::move(pattern_struct_expr4), "skip_to");
		// 	    after_match_window->children.push_back(std::move(high));
		// 		after_match_window->inclusive = make_uniq<BoundConstantExpression>(Value::BOOLEAN(true));
		// 		break;
		//     }
		// 	case MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_LAST_ROW:
		//     case MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_DEFAULT:
		// 	default: {
		// 		auto high = CreateBoundStructExtract(context, std::move(pattern_struct_expr4), "match_end");
		// 	    after_match_window->children.push_back(std::move(high));
		// 		after_match_window->inclusive = make_uniq<BoundConstantExpression>(Value::BOOLEAN(false));
		// 		break;
		//     }
		// }
		//
		// // Pattern Matching Window: Generate Table
		// auto after_match_table_index = GenerateTableIndex();
		// auto after_match_window_operator = make_uniq<LogicalWindow>(after_match_table_index);
		// after_match_window_operator->expressions.push_back(std::move(after_match_window));
		// after_match_window_operator->children.push_back(std::move(complete_filter));
		// after_match_window_operator->ResolveOperatorTypes();
		//
		// auto after_match_binder = Binder::CreateBinder(context, child_binder.get());
		// after_match_binder->bind_context.AddGenericBinding(after_match_table_index,
		// "__match_recognize_after_match_window",
		// 												   {"__after_match_window_bool"}, {after_match_return_type});
		//
		// // Filter for non-overlapping matches
		// auto after_match_window_bindings = after_match_window_operator->GetColumnBindings();
		// auto after_match_keep =
		// 	make_uniq<BoundColumnRefExpression>("after_match_keep", after_match_return_type,
		// 										after_match_window_bindings[after_match_window_bindings.size() - 1]);
		// auto no_overlap_filter = make_uniq<LogicalFilter>(std::move(after_match_keep));
		// no_overlap_filter->children.push_back(std::move(after_match_window_operator));
		// no_overlap_filter->ResolveOperatorTypes();
		//
		// // add
		// after_match_binder->bind_context.GetTypesAndNames(names, types);
		// bindings = no_overlap_filter->GetColumnBindings();
		// last_filter = std::move(no_overlap_filter);
	}

	// TODO we need to stack more stuff on top of this operator
	// add the measures columns and whatever else we need to our internal match recognize table

	// // Final Projection: collect names and types of all bindings we want to output
	// vector<string> final_names;
	// vector<LogicalType> final_types;
	// child_binder->bind_context.GetTypesAndNames(final_names, final_types);
	// // TODO: in the end, we do not want the struct here but the measures columns
	// pattern_window_binder->bind_context.GetTypesAndNames(final_names, final_types);
	//
	// // Final Projection: collect all bindings we want to output
	// vector<unique_ptr<Expression>> select_expressions;
	//
	// for (idx_t i = 0; i < bindings.size(); i++) {
	// 	// pattern window struct is the second last binding; skip it.
	// 	if (!is_skip_to_next_row && i == bindings.size() - 1){
	// 		continue;
	// 	}
	// 	auto col = make_uniq<BoundColumnRefExpression>(StringUtil::Format("select%d", i), types[i], bindings[i]);
	// 	select_expressions.push_back(std::move(col));
	// }
	//
	// // Final Projection: Generate Table
	// auto final_projection_table_index = GenerateTableIndex();
	// auto final_projection = make_uniq<LogicalProjection>(final_projection_table_index,
	// std::move(select_expressions)); final_projection->children.push_back(std::move(last_filter));
	// bind_context.AddGenericBinding(final_projection_table_index, mr_alias, final_names, final_types);

	auto child_binder = Binder::CreateBinder(context, this);
	auto result = child_binder->Bind(*select_node);
	bind_context.AddGenericBinding(result.plan->GetRootIndex(), "__match_recognize_table", result.names, result.types);
	return result;
}

} // namespace duckdb
