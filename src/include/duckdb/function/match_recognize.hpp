//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/function/match_recognize.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/function/function.hpp"

namespace duckdb {

struct MatchRecognizeFunctionData : FunctionData {
	unique_ptr<FunctionData> Copy() const override {
		return make_uniq<MatchRecognizeFunctionData>();
	}
	bool Equals(const FunctionData &other_p) const override {
		return true;
	}
};

struct MatchRecognizeFun {
	static unique_ptr<AggregateFunction> GetFunction();
};

} // namespace duckdb
