#include "duckdb/parser/tableref/match_recognize_ref.hpp"
#include "duckdb/planner/binder.hpp"

namespace duckdb {

unique_ptr<BoundTableRef> Binder::Bind(MatchRecognizeRef &ref) {
	D_ASSERT(0);

	// window
	// filter
	// window
	// filter

	return nullptr;
}

} // namespace duckdb
