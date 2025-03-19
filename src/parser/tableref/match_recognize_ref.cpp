#include "duckdb/parser/tableref/match_recognize_ref.hpp"

namespace duckdb {

string MatchRecognizeRef::ToString() const {
	return "";
}


unique_ptr<TableRef> MatchRecognizeRef::Copy() {
	D_ASSERT(0);
	return nullptr;
}

} // namespace duckdb
