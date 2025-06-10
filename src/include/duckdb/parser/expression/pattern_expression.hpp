//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/parser/expression/pattern_expression.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/parser/parsed_expression.hpp"

namespace duckdb {

// TODO move stuff to implementation file

class ConcatenationExpression : public ParsedExpression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::PATTERN;

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
	static constexpr const ExpressionClass TYPE = ExpressionClass::PATTERN;

public:
	QuantifiedExpression(unique_ptr<ParsedExpression> child_p, optional_idx min_count_p, optional_idx max_count_p)
	    : ParsedExpression(ExpressionType::QUANTIFIER, ExpressionClass::PATTERN), child(std::move(child_p)),
	      min_count(min_count_p), max_count(max_count_p) {
		if (min_count.IsValid() && max_count.IsValid() && min_count.GetIndex() > max_count.GetIndex()) {
			throw ParserException("Min count cannot be larger than max count");
		}
	}

	static string QuantifierString(optional_idx min_count, optional_idx max_count) {
		if (!min_count.IsValid() && !max_count.IsValid()) {
			return "";
		}
		if (min_count.IsValid() && min_count.GetIndex() == 0 && !max_count.IsValid()) {
			return "*";
		}
		if (min_count.IsValid() && min_count.GetIndex() == 1 && !max_count.IsValid()) {
			return "+";
		}
		return StringUtil::Format("{%s,%s}", min_count.IsValid() ? to_string(min_count.GetIndex()) : "",
		                          max_count.IsValid() ? to_string(max_count.GetIndex()) : "");
	}

	string ToString() const {
		return StringUtil::Format("%s%s", child->ToString(), QuantifierString(min_count, max_count));
	}

	unique_ptr<ParsedExpression> Copy() const {
		throw NotImplementedException("eeek");
	}
	unique_ptr<ParsedExpression> child;

	optional_idx min_count;
	optional_idx max_count;
};

class AlternationExpression : public ParsedExpression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::PATTERN;

public:
	AlternationExpression(unique_ptr<ParsedExpression> child_left_p, unique_ptr<ParsedExpression> child_right_p)
	    : ParsedExpression(ExpressionType::ALTERNATION, ExpressionClass::PATTERN), child_left(std::move(child_left_p)),
	      child_right(std::move(child_right_p)) {
	}

	string ToString() const {
		return StringUtil::Format("(%s)|(%s)", child_left->ToString(), child_right->ToString());
	}

	unique_ptr<ParsedExpression> Copy() const {
		throw NotImplementedException("eeek");
	}
	// TODO should this be a child list too?
	unique_ptr<ParsedExpression> child_left;
	unique_ptr<ParsedExpression> child_right;
};

static std::pair<optional_idx, optional_idx> ParseQuantifier(int min_count_in, int max_count_in) {
	optional_idx min_count;
	optional_idx max_count;

	if (min_count_in >= 0) {
		min_count = NumericCast<idx_t>(min_count_in);
	}
	if (max_count_in >= 0) {
		max_count = NumericCast<idx_t>(max_count_in);
	}
	if (min_count.IsValid() && max_count.IsValid() && min_count.GetIndex() > max_count.GetIndex()) {
		throw ParserException("Min count cannot be larger than max count");
	}
	return std::pair<optional_idx, optional_idx>(min_count, max_count);
}

} // namespace duckdb
