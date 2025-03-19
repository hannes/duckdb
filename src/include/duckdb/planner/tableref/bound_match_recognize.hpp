//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/planner/tableref/bound_match_recognize.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/planner/bound_tableref.hpp"
#include "duckdb/planner/logical_operator.hpp"
#include "duckdb/planner/tableref/bound_subqueryref.hpp"

namespace duckdb {

//! Represents a reference to a table-producing function call
class BoundMatchRecognizeRef : public BoundTableRef {
public:
	static constexpr const TableReferenceType TYPE = TableReferenceType::MATCH_RECOGNIZE;

public:
	explicit BoundMatchRecognizeRef(unique_ptr<LogicalOperator> get)
	    : BoundTableRef(TableReferenceType::MATCH_RECOGNIZE), get(std::move(get)) {
	}

	unique_ptr<LogicalOperator> get;
};

} // namespace duckdb
