//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/function/lambda_functions.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/function/function.hpp"

namespace duckdb {
struct MatchRecognizeFun {
	static AggregateFunction GetFunction();
};

} // namespace duckdb
