#include "duckdb/parser/transformer.hpp"
#include "duckdb/parser/tableref/match_recognize_ref.hpp"

namespace duckdb {

unique_ptr<TableRef> Transformer::TransformMatchRecognize(unique_ptr<TableRef> table,
                                                          optional_ptr<duckdb_libpgquery::PGNode> options) {
	D_ASSERT(options);
	auto match_recognize_stmt = PGPointerCast<duckdb_libpgquery::PGMatchRecognizeStmt>(options.get());
	auto match_recognize_config = make_uniq<MatchRecognizeConfig>();
	// optional partition
	if (match_recognize_stmt->partition_clause) {
		TransformExpressionList(*match_recognize_stmt->partition_clause, match_recognize_config->partition_expressions);
	}

	// optional ordering
	if (match_recognize_stmt->order_clause) {
		TransformOrderBy(match_recognize_stmt->order_clause, match_recognize_config->order_by_expressions);
	}

	// measures, I guess this is required?
	D_ASSERT(match_recognize_stmt->measures_clause);
	TransformExpressionList(*match_recognize_stmt->measures_clause, match_recognize_config->measures_expression_list);

	// rows per match
	if (match_recognize_stmt->rows_per_match == duckdb_libpgquery::PGMatchRecognizeRowsPerMatchDefault) {
#ifdef DUCKDB_ALTERNATIVE_VERIFY
			match_recognize_config->rows_per_match = MatchRecognizeRows::MATCH_RECOGNIZE_ROWS_ONE;
#else
			match_recognize_config->rows_per_match = MatchRecognizeRows::MATCH_RECOGNIZE_ROWS_DEFAULT;
#endif
	} else if (match_recognize_stmt->rows_per_match == duckdb_libpgquery::PGMatchRecognizeRowsPerMatchOneRow) {
		match_recognize_config->rows_per_match = MatchRecognizeRows::MATCH_RECOGNIZE_ROWS_ONE;
	} else if (match_recognize_stmt->rows_per_match == duckdb_libpgquery::PGMatchRecognizeRowsPerMatchAllRows) {
		match_recognize_config->rows_per_match = MatchRecognizeRows::MATCH_RECOGNIZE_ROWS_ALL;
	}

	// TODO pattern?!

	// defines, this is required?
	D_ASSERT(match_recognize_stmt->defines_clause);
	TransformExpressionList(*match_recognize_stmt->defines_clause, match_recognize_config->defines_expression_list);

	auto result = make_uniq<MatchRecognizeRef>(std::move(table), std::move(match_recognize_config));

	if (match_recognize_stmt->alias) {
		result->alias = TransformAlias(match_recognize_stmt->alias, result->column_name_alias);
	}

	return std::move(result);
}

} // namespace duckdb
