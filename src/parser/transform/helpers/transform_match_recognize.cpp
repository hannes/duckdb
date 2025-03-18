#include "duckdb/parser/transformer.hpp"
#include "duckdb/parser/expression/star_expression.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/parser/query_node/select_node.hpp"
#include "duckdb/parser/tableref/subqueryref.hpp"

namespace duckdb {

unique_ptr<TableRef> Transformer::TransformMatchRecognize(unique_ptr<TableRef> table,
                                                          optional_ptr<duckdb_libpgquery::PGNode> options) {
	D_ASSERT(options);
	auto match_recognize_options = PGPointerCast<duckdb_libpgquery::PGMatchRecognizeStmt>(options.get());

	// optional partition
	vector<unique_ptr<ParsedExpression>> partition_expressions;
	if (match_recognize_options->partition_clause) {
		TransformExpressionList(*match_recognize_options->partition_clause, partition_expressions);
	}

	// optional ordering
	vector<OrderByNode> order_by_expressions;
	if (match_recognize_options->order_clause) {
		D_ASSERT(TransformOrderBy(match_recognize_options->order_clause, order_by_expressions));
	}

	// measures, this is required?
	vector<unique_ptr<ParsedExpression>> measures_expression_list;
	D_ASSERT (match_recognize_options->measures_clause);
	TransformExpressionList(*match_recognize_options->measures_clause, measures_expression_list);

	auto measures_string =StringUtil::Join(measures_expression_list, measures_expression_list.size(),",", [](const unique_ptr<ParsedExpression> &s) { return s->ToString(); });
	printf("measures_expression_list = %s\n", measures_string.c_str());
	//
	// // TODO ?
	// vector<unique_ptr<ParsedExpression>> pattern_expression_list;
	// D_ASSERT (match_recognize_options->pattern_clause);
	// TransformExpressionList(*match_recognize_options->pattern_clause, pattern_expression_list);

	// defines, this is required?
	vector<unique_ptr<ParsedExpression>> defines_expression_list;
	D_ASSERT (match_recognize_options->defines_clause);
	TransformExpressionList(*match_recognize_options->defines_clause, defines_expression_list);

	auto defines_string =StringUtil::Join(defines_expression_list, defines_expression_list.size(),",", [](const unique_ptr<ParsedExpression> &s) { return s->ToString(); });
	printf("defines_string = %s\n", defines_string.c_str());


	auto select_node = make_uniq<SelectNode>();
	select_node->from_table = std::move(table);
	select_node->select_list.push_back(std::move(make_uniq<StarExpression>()));
	auto select_stmt = make_uniq<SelectStatement>();
	select_stmt->node = std::move(select_node);
	return make_uniq<SubqueryRef>(std::move(select_stmt));;
}

} // namespace duckdb
