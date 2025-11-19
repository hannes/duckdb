#include "duckdb/function/window/window_nonoverlap_intervals.hpp"
#include "duckdb/planner/expression/bound_window_expression.hpp"
#include "duckdb/function/window/window_shared_expressions.hpp"

namespace duckdb {

enum class IntervalCompareMode : uint8_t {
	INCLUSIVE, // start >= prev_end
	EXCLUSIVE  // start > prev_end
};

class WindowNonOverlapIntervalsGlobalState : public WindowExecutorGlobalState {
public:
	WindowNonOverlapIntervalsGlobalState(ClientContext &client, const WindowExecutor &executor_p,
	                                     const idx_t payload_count_p, const ValidityMask &partition_mask_p,
	                                     const ValidityMask &order_mask_p)
	    : WindowExecutorGlobalState(client, executor_p, payload_count_p, partition_mask_p, order_mask_p),
	      compare_mode(IntervalCompareMode::EXCLUSIVE), result_vec(executor_p.wexpr.return_type, payload_count_p) {
		FlatVector::Validity(result_vec).SetAllInvalid(payload_count_p);
		D_ASSERT(result_vec.GetType().id() == LogicalTypeId::STRUCT);
		auto &struct_entries = StructVector::GetEntries(result_vec);
		// Initialize validity for all struct entries
		for (idx_t i = 0; i < struct_entries.size(); i++) {
			// If this is a list vector, properly initialize it
			if (struct_entries[i]->GetType().id() == LogicalTypeId::LIST) {
				// Initialize list entries to empty lists
				auto list_data = FlatVector::GetData<list_entry_t>(*struct_entries[i]);
				for (idx_t j = 0; j < payload_count_p; j++) {
					list_data[j].offset = 0;
					list_data[j].length = 0;
				}
				// Initialize child vector with size 0
				ListVector::SetListSize(*struct_entries[i], 0);
			}
			FlatVector::Validity(*struct_entries[i]).SetAllInvalid(payload_count_p);
		}

		//		// first entry is list of classifiers
		//		FlatVector::Validity(*struct_entries[0]).SetAllInvalid(payload_count_p);
		//		FlatVector::Validity(*struct_entries[1]).SetAllInvalid(payload_count_p);
		//		FlatVector::Validity(*struct_entries[2]).SetAllInvalid(payload_count_p);
		//		FlatVector::Validity(*struct_entries[3]).SetAllInvalid(payload_count_p);
	};

	IntervalCompareMode compare_mode;
	unordered_map<idx_t, uint64_t> last_end;
	Vector result_vec;
};

class WindowNonOverlapIntervalsLocalState : public WindowExecutorBoundsLocalState {
public:
	explicit WindowNonOverlapIntervalsLocalState(ExecutionContext &context,
	                                             const WindowNonOverlapIntervalsGlobalState &grstate)
	    : WindowExecutorBoundsLocalState(context, grstate) { // grstate(grstate)
	}

	// Store last interval in global state
	void Sink(ExecutionContext &context, DataChunk &sink_chunk, DataChunk &coll_chunk, idx_t input_idx,
	          OperatorSinkInput &sink) override {

	};

	//! Finish the sinking and prepare to scan
	void Finalize(ExecutionContext &context, CollectionPtr collection, OperatorSinkInput &sink) override {};

	//	//! The corresponding global peer state
	//	const WindowNonOverlapIntervalsGlobalState &grstate;
};

WindowNonOverlapIntervalsExecutor::WindowNonOverlapIntervalsExecutor(BoundWindowExpression &wexpr,
                                                                     WindowSharedExpressions &shared)
    : WindowExecutor(wexpr, shared) {
	// Register children for evaluation
	start_idx = shared.RegisterEvaluate(wexpr.children[0]); // match_start
	end_idx = shared.RegisterEvaluate(wexpr.children[1]);   // match_end
}

unique_ptr<GlobalSinkState> WindowNonOverlapIntervalsExecutor::GetGlobalState(ClientContext &client,
                                                                              const idx_t payload_count,
                                                                              const ValidityMask &partition_mask,
                                                                              const ValidityMask &order_mask) const {
	return make_uniq<WindowNonOverlapIntervalsGlobalState>(client, *this, payload_count, partition_mask, order_mask);
}

void WindowNonOverlapIntervalsExecutor::EvaluateInternal(ExecutionContext &context, DataChunk &eval_chunk,
                                                         Vector &result, idx_t count, idx_t row_idx,
                                                         OperatorSinkInput &sink) const {
	auto &gstate = sink.global_state.Cast<WindowNonOverlapIntervalsGlobalState>();
	auto &lstate = sink.local_state.Cast<WindowNonOverlapIntervalsLocalState>();

	auto start_data = FlatVector::GetData<uint64_t>(eval_chunk.data[start_idx]);
	auto end_data = FlatVector::GetData<uint64_t>(eval_chunk.data[end_idx]);
	//    auto rdata= FlatVector::GetData<bool>(result);

	// Bounds for current partition
	auto partition_begin = FlatVector::GetData<const idx_t>(lstate.bounds.data[PARTITION_BEGIN]);
	auto partition_end = FlatVector::GetData<const idx_t>(lstate.bounds.data[PARTITION_END]);

	for (idx_t partition_idx = 0; partition_idx < count; ++partition_idx, ++row_idx) {
		auto pbegin = partition_begin[partition_idx];
		auto pend = partition_end[partition_idx];

		FlatVector::Validity(gstate.result_vec).SetValid(row_idx);
		auto &struct_entries = StructVector::GetEntries(gstate.result_vec);
		struct_entries[0]->SetValue(partition_idx, Value::LIST(LogicalType::VARCHAR, {}));
		struct_entries[2]->SetValue(partition_idx, Value::INTEGER(NumericCast<int32_t>(partition_idx)));
		struct_entries[3]->SetValue(partition_idx, Value::INTEGER(NumericCast<int32_t>(partition_idx < 4 ? 3 : pend)));

		// pick first interval in partition
		bool keep = false;
		if (row_idx == pbegin) {
			keep = true;
			gstate.last_end[pbegin] = end_data[partition_idx];
		} else {
			// pick next interval that does not overlap
			auto prev_end = gstate.last_end[pbegin];
			switch (gstate.compare_mode) {
			case IntervalCompareMode::INCLUSIVE:
				if (start_data[row_idx] >= prev_end) {
					keep = true;
					gstate.last_end[pbegin] = end_data[partition_idx];
				}
				break;
			case IntervalCompareMode::EXCLUSIVE:
				if (start_data[row_idx] > prev_end) {
					keep = true;
					gstate.last_end[pbegin] = end_data[partition_idx];
				}
				break;
			}
		}

		struct_entries[1]->SetValue(partition_idx, Value::BOOLEAN(keep));
		//        rdata[partition_idx] = keep; // mark interval to keep
	}
	result.Reference(gstate.result_vec);
}

void WindowNonOverlapIntervalsExecutor::Finalize(ExecutionContext &context, CollectionPtr collection,
                                                 OperatorSinkInput &sink) const {
	// No merge needed: all data already appended to global collection
}

} // namespace duckdb
