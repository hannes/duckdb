#include "duckdb/planner/operator/logical_show.hpp"

namespace duckdb {

void LogicalShow::Serialize(FieldWriter &writer) const {
	writer.WriteRegularSerializableList(types_select);
	writer.WriteList<string>(aliases);
}

unique_ptr<LogicalOperator> LogicalShow::Deserialize(ClientContext &context, LogicalOperatorType type,
                                                     FieldReader &reader) {
	auto types_select = reader.ReadRequiredSerializableList<LogicalType, LogicalType>();
	auto aliases = reader.ReadRequiredList<string>();

	// TODO(stephwang): review if we need to pass unique_ptr<LogicalOperator> plan
	auto result = make_unique<LogicalShow>(nullptr);
	result->types_select = types_select;
	result->aliases = aliases;
	return result;
}
} // namespace duckdb
