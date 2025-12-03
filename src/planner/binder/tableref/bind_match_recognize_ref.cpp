
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

static void CheckAndZapQualifiers(ParsedExpression &root_expr, const string &define_name) {
	ParsedExpressionIterator::VisitExpressionMutable<ColumnRefExpression>(root_expr, [&](ColumnRefExpression &colref) {
		if (colref.IsQualified() && colref.GetTableName() != define_name) {
			throw NotImplementedException("Define references cannot refer to other defines just yet %s <> %s",
			                              colref.GetTableName(), define_name.c_str());
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

static unique_ptr<ParsedExpression> CreateStructExtract(const string &column_name, const string &child_name) {
	vector<unique_ptr<ParsedExpression>> children;
	children.push_back(make_uniq<ColumnRefExpression>(column_name));
	children.push_back(make_uniq<ConstantExpression>(child_name));
	return make_uniq<FunctionExpression>("struct_extract", std::move(children));
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

	auto select_node = make_uniq<SelectNode>(std::move(ref.input));
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

	auto define_select_node = make_uniq<SelectNode>(std::move(select_node->from_table));

	vector<unique_ptr<WindowExpression>> child_windows;
	define_select_node->select_list.push_back(make_uniq<StarExpression>());

	// we use this window function as a template for order, partition, and boundaries
	D_ASSERT(pattern_window->children.empty()); // for now
	auto window_template = pattern_window->Copy();

	case_insensitive_set_t define_names;

	for (auto &expr : ref.config->defines_expression_list) {
		auto &define_name = expr->alias;
		// TODO can this happen?
		D_ASSERT(!define_name.empty());
		D_ASSERT(define_names.find(define_name) == define_names.end());

		CheckAndZapQualifiers(*expr, define_name);
		ReplaceFunctions(expr, window_template->Cast<WindowExpression>());
		pattern_window->children.push_back(make_uniq<ColumnRefExpression>(define_name));
		define_select_node->select_list.push_back(std::move(expr));
		define_names.insert(define_name);
	}

	// push computation of measures into the lowest window.
	// for (auto &expr : ref.config->measures_expression_list) {
	// 	D_ASSERT(!expr->alias.empty());
	// 	CheckAndZapQualifiers(*expr, expr->alias);
	// 	define_select_node->select_list.push_back(expr->Copy());
	// 	pattern_window->children.push_back(make_uniq<ColumnRefExpression>(expr->alias));
	// }

	// for any symbols that *are* in the pattern but are *not* in the defines,
	// we just push a dummy column so this can bind
	ParsedExpressionIterator::VisitExpression<ColumnRefExpression>(
	    *ref.config->pattern, [&](const ColumnRefExpression &colref) {
		    D_ASSERT(colref.column_names.size() == 1);
		    auto &symbol_name = colref.column_names[0];
		    if (define_names.find(symbol_name) == define_names.end()) { // TODO can those even occur multiple times?
			    // not in define list, implicitly created symbol yay
			    pattern_window->children.push_back(make_uniq<ColumnRefExpression>(symbol_name));
			    auto define_expression = make_uniq<ConstantExpression>(Value::BOOLEAN(true));
			    define_expression->alias = symbol_name;
			    define_select_node->select_list.push_back(std::move(define_expression));
			    define_names.insert(symbol_name);
		    }
	    });

	// TODO deal with measures, what on earth is in the output??
	// TODO likely measures have to be pushed into the way-down window, too

	// we abuse the child list to push the pattern to the binder
	pattern_window->children.push_back(std::move(ref.config->pattern));

	auto define_select = make_uniq<SelectStatement>(std::move(define_select_node));
	select_node->from_table = make_uniq<SubqueryRef>(std::move(define_select));
	pattern_window->alias = "__pattern_window";
	select_node->select_list.push_back(std::move(pattern_window));

	select_node->qualify = CreateStructExtract("__pattern_window", "complete");

	// After Match skip option
	bool is_skip_to_next_row =
	    ref.config->after_match == MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_NEXT_ROW;
	unique_ptr<LogicalFilter> last_filter = nullptr;

	if (!is_skip_to_next_row) {
		// // After Match window
		auto after_match_window = make_uniq<WindowExpression>(ExpressionType::WINDOW_NON_OVERLAP_INTERVALS, "", "",
		                                                      "window_non_overlap_intervals");
		after_match_window->start = WindowBoundary::UNBOUNDED_PRECEDING;
		after_match_window->end = WindowBoundary::CURRENT_ROW_RANGE;

		// bind same partitions to second window as well
		for (auto &expr : ref.config->partition_expressions) {
			after_match_window->partitions.push_back(expr->Copy());
		}

		// enforce ordering
		auto low = CreateStructExtract("__pattern_window", "match_start");
		after_match_window->children.push_back(std::move(low));

		after_match_window->alias = "__after_match_window";

		// upper interval bound: depends on the skip option
		switch (ref.config->after_match) {
		case MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_FIRST_VAR:
		case MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_LAST_VAR: {
			auto high = CreateStructExtract("__pattern_window", "skip_to");
			after_match_window->children.push_back(std::move(high));
			after_match_window->inclusive = make_uniq<ConstantExpression>(Value::BOOLEAN(true));
			break;
		}
		case MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_LAST_ROW:
		case MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_DEFAULT:
		default: {
			auto high = CreateStructExtract("__pattern_window", "match_end");
			after_match_window->children.push_back(std::move(high));
			after_match_window->inclusive = make_uniq<ConstantExpression>(Value::BOOLEAN(false));
			break;
		}
		}

		auto skip_select = make_uniq<SelectStatement>(std::move(select_node));
		auto skip_node = make_uniq<SelectNode>(make_uniq<SubqueryRef>(std::move(skip_select)));
		skip_node->select_list.push_back(make_uniq<StarExpression>()); // TODO ??
		skip_node->qualify = std::move(after_match_window);

		select_node = std::move(skip_node);
	}

	// auto measures_select = make_uniq<SelectStatement>(std::move(select_node));
	// auto measures_node = make_uniq<SelectNode>(make_uniq<SubqueryRef>(std::move(measures_select)));
	//
	// // it seems the partitions go first?
	// for (auto &expr : ref.config->partition_expressions) {
	// 	measures_node->select_list.push_back(expr->Copy());
	// }
	// // now all the measures that we have prepared earlier
	// for (auto &expr : ref.config->measures_expression_list) {
	// 	D_ASSERT(!expr->alias.empty());
	// 	measures_node->select_list.push_back(make_uniq<ColumnRefExpression>(expr->alias));
	// }
	// select_node = std::move(measures_node);

	printf("SQL\n%s\n", select_node->ToString().c_str());

	auto child_binder = Binder::CreateBinder(context, this);
	auto result = child_binder->Bind(*select_node);
	bind_context.AddGenericBinding(result.plan->GetRootIndex(),
	                               !ref.alias.empty() ? ref.alias : "__match_recognize_table", result.names,
	                               result.types);
	return result;
}

} // namespace duckdb
