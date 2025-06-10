//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/function/match_recognize.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/function/function.hpp"
#include "duckdb/planner/expression.hpp"

namespace duckdb {

struct MatchRecognizeFunctionData : FunctionData {

	vector<unique_ptr<Expression>> defines_expression_list;
	unique_ptr<Expression> pattern;

	unique_ptr<FunctionData> Copy() const override {
		auto res = make_uniq<MatchRecognizeFunctionData>();
		for (auto &expr : defines_expression_list) {
			res->defines_expression_list.emplace_back(expr->Copy());
		}
		res->pattern = pattern->Copy();
		return res;
	}
	bool Equals(const FunctionData &other_p) const override {
		auto &other = other_p.Cast<MatchRecognizeFunctionData>();
		throw NotImplementedException("MatchRecognizeFunctionData::Equals not implemented");
	}
};

struct MatchRecognizeFun {
	static unique_ptr<AggregateFunction> GetFunction();
};

} // namespace duckdb
