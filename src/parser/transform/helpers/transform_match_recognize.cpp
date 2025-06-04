#include "duckdb/parser/transformer.hpp"
#include "duckdb/parser/tableref/match_recognize_ref.hpp"

namespace duckdb {


class ConcatenationExpression : public ParsedExpression {
public:



	ConcatenationExpression(vector<unique_ptr<ParsedExpression>> children_p) : ParsedExpression(ExpressionType::CONCATENATION, ExpressionClass::PATTERN), children(std::move(children_p)) {
	}


	string ToString() const {
		return "(" + StringUtil::Join(children, children.size(), " ", [](const unique_ptr<ParsedExpression> &expr) {
			return expr->ToString();
		}) + ")";
	}

	unique_ptr<ParsedExpression> Copy() const {

	}

	vector<unique_ptr<ParsedExpression>> children;

};


class QuantifiedExpression : public ParsedExpression {
public:
	QuantifiedExpression(unique_ptr<ParsedExpression> child_p, optional_idx min_count_p, optional_idx max_count_p) : ParsedExpression(ExpressionType::QUANTIFIER, ExpressionClass::PATTERN), child(std::move(child_p)), min_count(min_count_p), max_count(max_count_p) {
		if (min_count.IsValid() && max_count.IsValid() && min_count.GetIndex() > max_count.GetIndex()) {
			throw ParserException("Min count cannot be larger than max count");
		}
	}

	string ToString() const {
		return StringUtil::Format("%s{%s,%s}", child->ToString().c_str(), min_count.IsValid()? to_string(min_count.GetIndex()) : "" , max_count.IsValid()? to_string(max_count.GetIndex()) : "");
	}

	unique_ptr<ParsedExpression> Copy() const {

	}
	unique_ptr<ParsedExpression> child;

	optional_idx min_count;
	optional_idx max_count;

};

// TODO not liking this list throwing a whole lot..
unique_ptr<ParsedExpression> Transformer::TransformPattern(duckdb_libpgquery::PGList *pattern) {
	D_ASSERT(pattern);
	D_ASSERT(pattern->length > 0);
	auto type_string = StringUtil::Lower(PGPointerCast<duckdb_libpgquery::PGValue>(duckdb_libpgquery::list_nth(pattern, 0))->val.str);
	if (type_string == "variable") {
		D_ASSERT(pattern->length == 3); // type name quantifier
		auto define_col_name = StringUtil::Lower(PGPointerCast<duckdb_libpgquery::PGValue>(duckdb_libpgquery::list_nth(pattern, 1))->val.str);

		optional_idx min_count, max_count;

		auto quantifier_list = PGPointerCast<duckdb_libpgquery::PGList>(duckdb_libpgquery::list_nth(pattern, 2));
		if (quantifier_list) {

			D_ASSERT(quantifier_list.get()->length == 2); // min_count max_count (currently)
				auto min_count_ptr = PGPointerCast<duckdb_libpgquery::PGValue>(duckdb_libpgquery::list_nth(quantifier_list.get(), 0));
				auto max_count_ptr = PGPointerCast<duckdb_libpgquery::PGValue>(duckdb_libpgquery::list_nth(quantifier_list.get(), 1));
			if (min_count_ptr) {
				min_count =  NumericCast<idx_t>(min_count_ptr->val.ival) ;
			}
			if (max_count_ptr) {
				max_count =  NumericCast<idx_t>(max_count_ptr->val.ival) ;
			}
		}
		auto child = make_uniq<ColumnRefExpression>(define_col_name);
		return make_uniq_base<ParsedExpression, QuantifiedExpression>(std::move(child), min_count, max_count);
	}

	if (type_string == "concatenation") {
		D_ASSERT(pattern->length > 1); // type child1 child2 child3...
		vector<unique_ptr<ParsedExpression>> children;
		// we start iterating from entry two onwards
		for (auto node = pattern->head->next; node != nullptr; node = node->next) {
			auto target = PGPointerCast<duckdb_libpgquery::PGList>(node->data.ptr_value);
			auto expr = TransformPattern(target.get());
			children.push_back(std::move(expr));
		}

		return make_uniq_base<ParsedExpression, ConcatenationExpression>(std::move(children));
	}

	if (type_string == "grouping") {
		D_ASSERT(pattern->length == 3); // type children quantifier
		vector<unique_ptr<ParsedExpression>> children;
		// we start iterating from entry two onwards
		auto child_ptr = PGPointerCast<duckdb_libpgquery::PGList>(duckdb_libpgquery::list_nth(pattern, 1));
		auto child = Transformer::TransformPattern(child_ptr.get());

// FIXME copy-paste
		optional_idx min_count, max_count;

		auto quantifier_list = PGPointerCast<duckdb_libpgquery::PGList>(duckdb_libpgquery::list_nth(pattern, 2));
		if (quantifier_list) {

			D_ASSERT(quantifier_list.get()->length == 2); // min_count max_count (currently)
			auto min_count_ptr = PGPointerCast<duckdb_libpgquery::PGValue>(duckdb_libpgquery::list_nth(quantifier_list.get(), 0));
			auto max_count_ptr = PGPointerCast<duckdb_libpgquery::PGValue>(duckdb_libpgquery::list_nth(quantifier_list.get(), 1));
			if (min_count_ptr) {
				min_count =  NumericCast<idx_t>(min_count_ptr->val.ival) ;
			}
			if (max_count_ptr) {
				max_count =  NumericCast<idx_t>(max_count_ptr->val.ival) ;
			}
		}

		return make_uniq_base<ParsedExpression, QuantifiedExpression>(std::move(child), min_count, max_count);
	}


	throw ParserException("Unknown pattern type: %s", type_string.c_str());
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
	} else if (match_recognize_stmt->after_match_clause->after_match == duckdb_libpgquery::PGMatchRecognizeAfterMatchNextRow) {
		match_recognize_config->after_match = MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_NEXT_ROW;
	} else if (match_recognize_stmt->after_match_clause->after_match == duckdb_libpgquery::PGMatchRecognizeAfterMatchLastRow) {
		match_recognize_config->after_match = MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_LAST_ROW;
	} else if (match_recognize_stmt->after_match_clause->after_match == duckdb_libpgquery::PGMatchRecognizeAfterMatchFirstVar) {
		match_recognize_config->after_match = MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_FIRST_VAR;
		match_recognize_config->after_match_variable = TransformValue(*match_recognize_stmt->after_match_clause->variable);
	} else if (match_recognize_stmt->after_match_clause->after_match == duckdb_libpgquery::PGMatchRecognizeAfterMatchLastVar) {
		match_recognize_config->after_match = MatchRecognizeAfterMatch::MATCH_RECOGNIZE_AFTER_MATCH_LAST_VAR;
		match_recognize_config->after_match_variable = TransformValue(*match_recognize_stmt->after_match_clause->variable);
	}

	// pattern
	if (!match_recognize_stmt->pattern_clause || list_length(match_recognize_stmt->pattern_clause) < 1) {
		throw ParserException("MATCH_RECOGNIZE needs a non-empty pattern");
	}

	auto pattern = TransformPattern(match_recognize_stmt->pattern_clause);

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
