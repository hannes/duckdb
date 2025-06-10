#include "duckdb/parser/transformer.hpp"
#include "duckdb/parser/tableref/match_recognize_ref.hpp"

namespace duckdb {

class ConcatenationExpression : public ParsedExpression {
public:
	ConcatenationExpression(vector<unique_ptr<ParsedExpression>> children_p)
	    : ParsedExpression(ExpressionType::CONCATENATION, ExpressionClass::PATTERN), children(std::move(children_p)) {
	}

	string ToString() const {
		return "(" +
		       StringUtil::Join(children, children.size(), " ",
		                        [](const unique_ptr<ParsedExpression> &expr) { return expr->ToString(); }) +
		       ")";
	}

	unique_ptr<ParsedExpression> Copy() const {
		throw NotImplementedException("eeek");
	}

	vector<unique_ptr<ParsedExpression>> children;
};

class QuantifiedExpression : public ParsedExpression {
public:
	QuantifiedExpression(unique_ptr<ParsedExpression> child_p, optional_idx min_count_p, optional_idx max_count_p)
	    : ParsedExpression(ExpressionType::QUANTIFIER, ExpressionClass::PATTERN), child(std::move(child_p)),
	      min_count(min_count_p), max_count(max_count_p) {
		if (min_count.IsValid() && max_count.IsValid() && min_count.GetIndex() > max_count.GetIndex()) {
			throw ParserException("Min count cannot be larger than max count");
		}
	}

	string ToString() const {
		return StringUtil::Format("%s{%s,%s}", child->ToString().c_str(),
		                          min_count.IsValid() ? to_string(min_count.GetIndex()) : "",
		                          max_count.IsValid() ? to_string(max_count.GetIndex()) : "");
	}

	unique_ptr<ParsedExpression> Copy() const {
		throw NotImplementedException("eeek");
	}
	unique_ptr<ParsedExpression> child;

	optional_idx min_count;
	optional_idx max_count;
};

unique_ptr<ParsedExpression> Transformer::TransformPattern(duckdb_libpgquery::PGMatchRecognizePattern *pattern) {
	D_ASSERT(pattern);
	switch (pattern->type) {
	case duckdb_libpgquery::PGMatchRecognizePatternLabel: {
		auto name = PGPointerCast<duckdb_libpgquery::PGValue>(pattern->child);
		auto child = make_uniq_base<ParsedExpression, ColumnRefExpression>(name.get()->val.str);
		optional_idx min_count;
		optional_idx max_count;
		if (pattern->min_count >= 0) {
			min_count = NumericCast<idx_t>(pattern->min_count);
		}
		if (pattern->max_count >= 0) {
			max_count = NumericCast<idx_t>(pattern->max_count);
		}
		if (min_count.IsValid() && max_count.IsValid() && min_count.GetIndex() > max_count.GetIndex()) {
			throw ParserException("Min count cannot be larger than max count");
		}
		return make_uniq_base<ParsedExpression, QuantifiedExpression>(std::move(child), min_count, max_count);
	}
	case duckdb_libpgquery::PGMatchRecognizePatternGrouping: {
		auto child = TransformPatternList(PGPointerCast<duckdb_libpgquery::PGList>(pattern->child).get());
		if (pattern->min_count < 0 && pattern->max_count) {
			return child; // easy case
		}
		// TODO std::pair??
		optional_idx min_count;
		optional_idx max_count;
		if (pattern->min_count >= 0) {
			min_count = NumericCast<idx_t>(pattern->min_count);
		}
		if (pattern->max_count >= 0) {
			max_count = NumericCast<idx_t>(pattern->max_count);
		}
		if (min_count.IsValid() && max_count.IsValid() && min_count.GetIndex() > max_count.GetIndex()) {
			throw ParserException("Min count cannot be larger than max count");
		}
		return make_uniq_base<ParsedExpression, QuantifiedExpression>(std::move(child), min_count, max_count);
	}
	default:
		throw NotImplementedException("Unknown pattern type %d", pattern->type);
	}
}

unique_ptr<ParsedExpression> Transformer::TransformPatternList(duckdb_libpgquery::PGList *pattern_list) {
	D_ASSERT(pattern_list);
	D_ASSERT(pattern_list->length > 0);
	vector<unique_ptr<ParsedExpression>> children;
	for (auto node = pattern_list->head; node != nullptr; node = node->next) {
		auto child_pattern = PGPointerCast<duckdb_libpgquery::PGMatchRecognizePattern>(node->data.ptr_value);
		auto expr = TransformPattern(child_pattern.get());
		children.push_back(std::move(expr));
	}
	return make_uniq_base<ParsedExpression, ConcatenationExpression>(std::move(children));
}

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

	// after match skip option
	D_ASSERT(match_recognize_stmt->after_match_clause);
	if (match_recognize_stmt->after_match_clause->after_match == duckdb_libpgquery::PGMatchRecognizeAfterMatchDefault) {
#ifdef DUCKDB_ALTERNATIVE_VERIFY
		match_recognize_config->after_match = MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_LAST_ROW;
#else
		match_recognize_config->after_match = MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_DEFAULT;
#endif
	} else if (match_recognize_stmt->after_match_clause->after_match ==
	           duckdb_libpgquery::PGMatchRecognizeAfterMatchNextRow) {
		match_recognize_config->after_match = MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_NEXT_ROW;
	} else if (match_recognize_stmt->after_match_clause->after_match ==
	           duckdb_libpgquery::PGMatchRecognizeAfterMatchLastRow) {
		match_recognize_config->after_match = MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_LAST_ROW;
	} else if (match_recognize_stmt->after_match_clause->after_match ==
	           duckdb_libpgquery::PGMatchRecognizeAfterMatchFirstVar) {
		match_recognize_config->after_match = MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_FIRST_VAR;
		match_recognize_config->after_match_variable =
		    TransformValue(*match_recognize_stmt->after_match_clause->variable);
	} else if (match_recognize_stmt->after_match_clause->after_match ==
	           duckdb_libpgquery::PGMatchRecognizeAfterMatchLastVar) {
		match_recognize_config->after_match = MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_LAST_VAR;
		match_recognize_config->after_match_variable =
		    TransformValue(*match_recognize_stmt->after_match_clause->variable);
	}

	// pattern
	if (!match_recognize_stmt->pattern_clause || list_length(match_recognize_stmt->pattern_clause) < 1) {
		throw ParserException("MATCH_RECOGNIZE needs a non-empty pattern");
	}

	auto pattern = TransformPatternList(match_recognize_stmt->pattern_clause);

	printf("PATTERN: %s\n", pattern->ToString().c_str());

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
