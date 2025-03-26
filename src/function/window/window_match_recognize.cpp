#include "duckdb/function/window/window_match_recognize.hpp"

#include "duckdb/function/match_recognize.hpp"

namespace duckdb {

struct WindowMatchRecognizeGlobalState : WindowExecutorGlobalState {
	WindowMatchRecognizeGlobalState(const WindowExecutor &executor_p, const idx_t payload_count_p,
	                                const ValidityMask &partition_mask_p, const ValidityMask &order_mask_p)
	    : WindowExecutorGlobalState(executor_p, payload_count_p, partition_mask_p, order_mask_p) {

	      };
};

//===--------------------------------------------------------------------===//
// WindowMatchRecognizeExecutor
//===--------------------------------------------------------------------===//
WindowMatchRecognizeExecutor::WindowMatchRecognizeExecutor(BoundWindowExpression &wexpr, ClientContext &context,
                                                           WindowSharedExpressions &shared)
    : WindowExecutor(wexpr, context, shared) {

	for (const auto &child : wexpr.children) {
		// TODO remember this index, we need it for the defines evaluation
		D_ASSERT(child->GetExpressionType() == ExpressionType::BOUND_REF);
		shared.RegisterCollection(child, false);
	}
}

unique_ptr<WindowExecutorGlobalState>
WindowMatchRecognizeExecutor::GetGlobalState(const idx_t payload_count, const ValidityMask &partition_mask,
                                             const ValidityMask &order_mask) const {
	return make_uniq<WindowMatchRecognizeGlobalState>(*this, payload_count, partition_mask, order_mask);
}

// this gets called per partition
void WindowMatchRecognizeExecutor::Finalize(WindowExecutorGlobalState &gstate, WindowExecutorLocalState &lstate,
                                            CollectionPtr collection) const {
	for (idx_t i = 0; i < gstate.payload_count; i++) {
		printf("%i\t%d\t%d\n", i, gstate.partition_mask.GetValidityEntry(i), gstate.order_mask.GetValidityEntry(i));
	}

	// we still need to resolve the mapping to inputs here
	auto &defines_expression_list = wexpr.bind_info->Cast<MatchRecognizeFunctionData>().defines_expression_list;

	// the sorted per-partition is in here:
	collection->inputs->Print();

	// we do our thing here and then dump the result in the global state for scanning later
	// how do we get back the context below?? which rows belonged to this?
	// ranges?? WindowCursor ??
}

void WindowMatchRecognizeExecutor::EvaluateInternal(WindowExecutorGlobalState &gstate, WindowExecutorLocalState &lstate,
                                                    DataChunk &eval_chunk, Vector &result, idx_t count,
                                                    idx_t row_idx) const {

	// here we fill the result, but with what? this looks like a scan in principle.

	// we know we are within a single hash group

	// row index points into partition mask
	// at the beginning of each partition a bit is set to mark new partition partition_mask_p

	// all those functions only operate on a single hash group at a time but there may be multiple partitions in it
	// partition boundaries dont' change

	// void WindowConstantAggregatorLocalState::Sink(DataChunk &sink_chunk, DataChunk &coll_chunk, idx_t row,

	// row windows into the hash group

	FlatVector::Validity(result).SetAllInvalid(count);
	D_ASSERT(result.GetType().id() == LogicalTypeId::STRUCT);
	auto &struct_entries = StructVector::GetEntries(result);
	// first entry is list of classifiers
	FlatVector::Validity(*struct_entries[0]).SetAllInvalid(count);
	FlatVector::Validity(*struct_entries[1]).SetAllInvalid(count);
	FlatVector::Validity(*struct_entries[2]).SetAllInvalid(count);
	FlatVector::Validity(*struct_entries[3]).SetAllInvalid(count);

	for (idx_t i = 0; i < count; i++) {
	}
}

} // namespace duckdb
