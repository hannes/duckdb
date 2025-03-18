#include "duckdb/parser/transformer.hpp"
#include "duckdb/parser/parsed_data/sample_options.hpp"
#include "duckdb/parser/expression/constant_expression.hpp"
#include "duckdb/common/string_util.hpp"

namespace duckdb {

unique_ptr<TableRef> Transformer::TransformMatchRecognize(unique_ptr<TableRef> table,
                                                          optional_ptr<duckdb_libpgquery::PGNode> options) {
	D_ASSERT(options);

	// push a subquery with a window expression

	printf("moo2");
	return table;
}

} // namespace duckdb
