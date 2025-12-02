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

// TODO this needs to live somewhere else!!
struct MatchRecognizeFunctionData : FunctionData {
	unique_ptr<Expression> pattern;

	unique_ptr<FunctionData> Copy() const override {
		auto res = make_uniq<MatchRecognizeFunctionData>();

		res->pattern = pattern->Copy();
		return res;
	}
	bool Equals(const FunctionData &other_p) const override {
		auto &other = other_p.Cast<MatchRecognizeFunctionData>();
		return other.pattern->Equals(*pattern);
	}
};

} // namespace duckdb
