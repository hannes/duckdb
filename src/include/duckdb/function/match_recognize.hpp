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
	// TODO pattern

	unique_ptr<FunctionData> Copy() const override {
		auto res = make_uniq<MatchRecognizeFunctionData>();
		for (auto &expr : defines_expression_list) {
			res->defines_expression_list.emplace_back(expr->Copy());
		}
		// TODO pattern
		return res;
	}
	bool Equals(const FunctionData &other_p) const override {
		auto &other = other_p.Cast<MatchRecognizeFunctionData>();
		// TODO pattern
		// return defines_expression_list == other.defines_expression_list;
		return true;
	}
};

struct MatchRecognizeFun {
	static unique_ptr<AggregateFunction> GetFunction();
};

} // namespace duckdb
