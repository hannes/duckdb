#include "duckdb/parser/transformer.hpp"
#include "duckdb/parser/expression/star_expression.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/parser/query_node/select_node.hpp"
#include "duckdb/parser/tableref/subqueryref.hpp"

namespace duckdb {

unique_ptr<TableRef> Transformer::TransformMatchRecognize(unique_ptr<TableRef> table,
                                                          optional_ptr<duckdb_libpgquery::PGNode> options) {
	D_ASSERT(options);
	auto select_node = make_uniq<SelectNode>();
	select_node->from_table = std::move(table);
	select_node->select_list.push_back(std::move(make_uniq<StarExpression>()));
	auto select_stmt = make_uniq<SelectStatement>();
	select_stmt->node = std::move(select_node);
	return make_uniq<SubqueryRef>(std::move(select_stmt));;
}

} // namespace duckdb
