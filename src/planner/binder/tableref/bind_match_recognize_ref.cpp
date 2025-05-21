#include "duckdb/parser/tableref/match_recognize_ref.hpp"
#include "duckdb/planner/binder.hpp"

#include "duckdb/planner/operator/logical_window.hpp"
#include "duckdb/function/match_recognize.hpp"
#include "duckdb/planner/expression/bound_window_expression.hpp"
#include "duckdb/planner/tableref/bound_match_recognize.hpp"
#include "duckdb/planner/expression/bound_reference_expression.hpp"

#include "duckdb/planner/expression_iterator.hpp"
#include "duckdb/planner/expression/bound_constant_expression.hpp"
#include "duckdb/function/function_list.hpp"
#include "duckdb/function/scalar/nested_functions.hpp"
#include "duckdb/planner/expression/bound_function_expression.hpp"
#include "duckdb/function/cast/cast_function_set.hpp"
#include "duckdb/planner/expression/bound_cast_expression.hpp"

#include "duckdb/parser/expression/pattern_expression.hpp"

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

unique_ptr<Expression> CreateBoundStructExtract2(ClientContext &context, unique_ptr<Expression> expr, string key) {
	vector<unique_ptr<Expression>> arguments;
	arguments.push_back(std::move(expr));
	arguments.push_back(make_uniq<BoundConstantExpression>(Value(key)));
	auto extract_function = GetKeyExtractFunction();
	auto bind_info = extract_function.bind(context, extract_function, arguments);
	auto return_type = extract_function.return_type;
	auto result = make_uniq<BoundFunctionExpression>(return_type, std::move(extract_function), std::move(arguments),
	                                                 std::move(bind_info));
	result->SetAlias(std::move(key));
	return std::move(result);
}

unique_ptr<BoundTableRef> Binder::Bind(MatchRecognizeRef &ref) {

	// we bind everything regarding match recognize in a child binder such that these bindings (e.g. of the input table)
	// are hidden, and we can add a new binding for the input columns and the measures columns to the match recognize
	// alias.
	auto child_binder = Binder::CreateBinder(context, this);
	auto child_node = child_binder->Bind(*ref.input);

	auto &config = DBConfig::GetConfig(context);
	// the expression_binder needs the child_binder as its parent to reference to columns of the input table
	ExpressionBinder expression_binder(*child_binder, context);
	auto child_node_plan = child_binder->CreatePlan(*child_node);

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

	auto window_expression =
	    make_uniq<BoundWindowExpression>(ExpressionType::WINDOW_MATCH_RECOGNIZE, return_type,
	                                     MatchRecognizeFun::GetFunction(), make_uniq<MatchRecognizeFunctionData>());
	window_expression->start = WindowBoundary::UNBOUNDED_PRECEDING;
	window_expression->end = WindowBoundary::UNBOUNDED_FOLLOWING;
	for (auto &expr : ref.config->partition_expressions) {
		window_expression->partitions.push_back(expression_binder.Bind(expr));
	}

	for (auto &order : ref.config->order_by_expressions) {
		auto type = config.ResolveOrder(order.type);
		auto null_order = config.ResolveNullOrder(type, order.null_order);
		window_expression->orders.emplace_back(type, null_order, expression_binder.Bind(order.expression));
	}

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

	auto table_index = GenerateTableIndex();
	auto window_operator = make_uniq<LogicalWindow>(table_index);
	window_operator->expressions.push_back(std::move(window_expression));
	window_operator->children.push_back(std::move(child_node_plan));
	window_operator->ResolveOperatorTypes();

	// add the measures columns or whatever we need to our internal match recognize table
	child_binder->bind_context.AddGenericBinding(table_index, "__match_recognize_ref", {"__match_recognize_fun"},
	                                             {return_type});

	// TODO we need to stack more stuff on top of this operator

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
	unique_ptr<BoundSelectNode> subquery = make_uniq<BoundSelectNode>();
	auto bindings = window_operator->GetColumnBindings();

	for (idx_t i = 0; i < bindings.size(); i++) {
		auto col = make_uniq<BoundColumnRefExpression>(window_operator->types[i], bindings[i]);
		subquery->select_list.push_back(std::move(col));
	}
	subquery->from_table = make_uniq<BoundMatchRecognizeRef>(std::move(window_operator));

	// filter: add WHERE t.__match_recognize_fun.complete to only extract complete matches
	// t.__match_recognize_fun
	auto col_ref = make_uniq<BoundColumnRefExpression>(return_type, bindings[bindings.size()-1]);
    auto col_ref_be = make_uniq<BoundExpression>(std::move(col_ref));
	unique_ptr<Expression> col_ref_expr = std::move(col_ref_be->expr);

	// Struct extract: struct_extract(t.__match_recognize_fun, "complete")
	auto struct_extract_expr = CreateBoundStructExtract2(context, std::move(col_ref_expr), "complete");

	// CAST(struct_extract(t.__match_recognize_fun, "complete") AS boolean)
	// this feels very unnecessary as we know complete has type boolean but duckdb adds this cast as well if I actually add WHERE t.__match_recognize_fun.complete
	GetCastFunctionInput get_input(context);
	BoundCastInfo cast_function = CastFunctionSet::Get(context).GetCastFunction(struct_extract_expr->return_type, LogicalType::BOOLEAN, get_input);
	unique_ptr<BoundCastExpression> cast_expr = make_uniq<BoundCastExpression>(std::move(struct_extract_expr), LogicalType::BOOLEAN, std::move(cast_function));
	subquery->where_clause = std::move(cast_expr);
	subquery->where_clause = std::move(struct_extract_expr);

	// add the binding for the subquery
	idx_t bind_index = GenerateTableIndex();
	subquery->projection_index = bind_index;
	vector<string> names;
	vector<LogicalType> types;
	child_binder->bind_context.GetTypesAndNames(names, types);
	bind_context.AddGenericBinding(bind_index, mr_alias, names, types);

	return make_uniq<BoundSubqueryRef>(std::move(child_binder), std::move(subquery));
}

} // namespace duckdb
