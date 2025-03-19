//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/parser/tableref/match_recognize_ref.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/parser/tableref.hpp"
#include "duckdb/parser/parsed_expression.hpp"
#include "duckdb/common/types.hpp"
#include "duckdb/common/vector.hpp"
#include "duckdb/parser/query_node.hpp"

namespace duckdb {

struct MatchRecognizeConfig {
	vector<unique_ptr<ParsedExpression>> partition_expressions;
	vector<OrderByNode> order_by_expressions;
	vector<unique_ptr<ParsedExpression>> measures_expression_list;
	vector<unique_ptr<ParsedExpression>> defines_expression_list;
	// TODO pattern

	string ToString() {
		auto measures_string = StringUtil::Join(measures_expression_list, measures_expression_list.size(), ",",
		                                        [](const unique_ptr<ParsedExpression> &s) { return s->ToString(); });
		printf("measures_expression_list = %s\n", measures_string.c_str());

		auto defines_string = StringUtil::Join(defines_expression_list, defines_expression_list.size(), ",",
		                                       [](const unique_ptr<ParsedExpression> &s) { return s->ToString(); });
		printf("defines_string = %s\n", defines_string.c_str());

		return "";
	}
};

//! Represents a SHOW/DESCRIBE/SUMMARIZE statement
class MatchRecognizeRef : public TableRef {
public:
	static constexpr const TableReferenceType TYPE = TableReferenceType::MATCH_RECOGNIZE;

	unique_ptr<TableRef> input;
	unique_ptr<MatchRecognizeConfig> config;

public:
	MatchRecognizeRef(unique_ptr<TableRef> input_p, unique_ptr<MatchRecognizeConfig> config_p) : TableRef(TableReferenceType::MATCH_RECOGNIZE), input(std::move(input_p)), config(std::move(config_p)) {}

public:
	string ToString() const override;
	// bool Equals(const TableRef &other_p) const override;
	//
	unique_ptr<TableRef> Copy() override;
	//
	// //! Deserializes a blob back into a ExpressionListRef
	// void Serialize(Serializer &serializer) const override;
	// static unique_ptr<TableRef> Deserialize(Deserializer &source);
};

} // namespace duckdb
