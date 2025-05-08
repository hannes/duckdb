#include "duckdb/parser/tableref/match_recognize_ref.hpp"

#include "duckdb/common/serializer/serializer.hpp"
#include "duckdb/common/serializer/deserializer.hpp"

namespace duckdb {

string MatchRecognizeRef::ToString() const {
	return "";
}

MatchRecognizeRef::MatchRecognizeRef() : TableRef(TableReferenceType::MATCH_RECOGNIZE) {
}

unique_ptr<TableRef> MatchRecognizeRef::Copy() {
	D_ASSERT(0);
	return nullptr;
}

} // namespace duckdb
