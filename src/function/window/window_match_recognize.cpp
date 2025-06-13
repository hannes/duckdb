#include "duckdb/function/window/window_match_recognize.hpp"

#include "duckdb/function/match_recognize.hpp"
#include "duckdb/planner/expression_iterator.hpp"

namespace duckdb {

struct WindowMatchRecognizeGlobalState : WindowExecutorGlobalState {
	WindowMatchRecognizeGlobalState(const WindowExecutor &executor_p, const idx_t payload_count_p,
	                                const ValidityMask &partition_mask_p, const ValidityMask &order_mask_p)
	    : WindowExecutorGlobalState(executor_p, payload_count_p, partition_mask_p, order_mask_p),
	      result_vec(executor_p.wexpr.return_type, payload_count_p) {

		FlatVector::Validity(result_vec).SetAllInvalid(payload_count_p);
		D_ASSERT(result_vec.GetType().id() == LogicalTypeId::STRUCT);
		auto &struct_entries = StructVector::GetEntries(result_vec);
		// first entry is list of classifiers
		FlatVector::Validity(*struct_entries[0]).SetAllInvalid(payload_count_p);
		FlatVector::Validity(*struct_entries[1]).SetAllInvalid(payload_count_p);
		FlatVector::Validity(*struct_entries[2]).SetAllInvalid(payload_count_p);
		FlatVector::Validity(*struct_entries[3]).SetAllInvalid(payload_count_p);
	};

	// TODO can we get away with putting this into the local state?
	mutex state_lock;

	Vector result_vec;
};

void UpdateColumnBindings(unique_ptr<Expression> &expr, unordered_map<column_t, column_t> &replacement_map) {
	if (expr->GetExpressionType() == ExpressionType::BOUND_REF) {
		auto &bound_ref = expr->Cast<BoundReferenceExpression>();
		bound_ref.index = replacement_map[bound_ref.index];
	}
	ExpressionIterator::EnumerateChildren(
	    *expr, [&](unique_ptr<Expression> &child) { UpdateColumnBindings(child, replacement_map); });
}

//===--------------------------------------------------------------------===//
// WindowMatchRecognizeExecutor
//===--------------------------------------------------------------------===//
WindowMatchRecognizeExecutor::WindowMatchRecognizeExecutor(BoundWindowExpression &wexpr, ClientContext &context,
                                                           WindowSharedExpressions &shared)
    : WindowExecutor(wexpr, context, shared) {

	// RegisterCollection deduplicates child expressions - we need to update the references in the defines to its return
	// value
	unordered_map<column_t, column_t> replacement_map;
	for (idx_t expr_idx = 0; expr_idx < wexpr.children.size(); expr_idx++) {
		const auto &child = wexpr.children[expr_idx];
		D_ASSERT(child->GetExpressionType() == ExpressionType::BOUND_REF);
		auto new_index = shared.RegisterCollection(child, /* this seems to not matter for the partition_mask */ false);
		replacement_map[expr_idx] = new_index;
	}
	auto &define_expressions = wexpr.bind_info->Cast<MatchRecognizeFunctionData>().defines_expression_list;
	for (auto &define_expression : define_expressions) {
		UpdateColumnBindings(define_expression, replacement_map);
	}
}

unique_ptr<WindowExecutorGlobalState>
WindowMatchRecognizeExecutor::GetGlobalState(const idx_t payload_count, const ValidityMask &partition_mask,
                                             const ValidityMask &order_mask) const {
	return make_uniq<WindowMatchRecognizeGlobalState>(*this, payload_count, partition_mask, order_mask);
}

inline bool RowIsVisible(ColumnDataScanState &scan, idx_t row_idx) {
	return (row_idx < scan.next_row_index && scan.current_row_index <= row_idx);
}

static void FetchPartitionAndExecute(ClientContext &context, ColumnDataCollection &input, ExpressionExecutor &executor,
                                     DataChunk &result_chunk, idx_t partition_start, idx_t partition_end) {
	ColumnDataScanState scan_state;
	DataChunk scan_chunk;
	DataChunk execute_result_chunk;

	// TODO cache those allocations
	input.InitializeScanChunk(scan_chunk);
	execute_result_chunk.Initialize(context, result_chunk.GetTypes());

	auto partition_size = partition_end - partition_start + 1;

	D_ASSERT(result_chunk.GetCapacity() <= partition_size);

	input.InitializeScan(scan_state);
	// we do one Scan() because Seek() does nothing if its already on the right chunk
	input.Scan(scan_state, scan_chunk);
	input.Seek(partition_start, scan_state, scan_chunk);

	// we may have to slice the first chunk because the partition may start somewhere halfway into the chunk
	auto chunk_offset = partition_start - scan_state.current_row_index;
	scan_chunk.Slice(chunk_offset, scan_chunk.size() - chunk_offset);

	executor.Execute(scan_chunk, execute_result_chunk);
	result_chunk.Append(execute_result_chunk, false);

	while (partition_end <= scan_state.next_row_index) {
		if (!input.Scan(scan_state, scan_chunk)) {
			// we need to get out here because otherwise the check below goes wrong
			break;
		}
		// we may have too many rows, so slice again
		if (scan_state.next_row_index > partition_end) {
			scan_chunk.Slice(0, scan_state.next_row_index - partition_end); // TODO verify this very complex math
		}

		executor.Execute(scan_chunk, execute_result_chunk);
		result_chunk.Append(execute_result_chunk, false);
	}
	D_ASSERT(result_chunk.size() == partition_size);
}

// this gets called per partition
void WindowMatchRecognizeExecutor::Finalize(WindowExecutorGlobalState &gstate_p, WindowExecutorLocalState &lstate_p,
                                            CollectionPtr collection_p) const {
	auto &gstate = gstate_p.Cast<WindowMatchRecognizeGlobalState>();
	lock_guard<mutex> lock(gstate.state_lock);

	auto &config = wexpr.bind_info->Cast<MatchRecognizeFunctionData>();

	ExpressionExecutor define_executor(context, config.defines_expression_list);

	vector<LogicalType> define_result_chunk_types;
	for (auto &def_expr : config.defines_expression_list) {
		define_result_chunk_types.push_back(def_expr->return_type);
	}

	idx_t partition_start = 0;
	// we always start with a new partition
	D_ASSERT(gstate.partition_mask.RowIsValid(0));

	// TODO figure out biggest partition size before, payload count is upper bound but overkill
	DataChunk define_result_chunk;
	define_result_chunk.Initialize(context, define_result_chunk_types, gstate.payload_count);

	for (idx_t payload_idx = 1; payload_idx < gstate.payload_count; payload_idx++) {
		if (!gstate.partition_mask.RowIsValid(payload_idx) && payload_idx + 1 < gstate.payload_count) {
			continue;
		}
		define_result_chunk.SetCardinality(0);

		// the partition end offset depends on whether we found a next partition or if we are at the end
		auto partition_end =
		    payload_idx + 1 == gstate.partition_mask.RowIsValid(payload_idx) ? payload_idx - 1 : payload_idx;
		FetchPartitionAndExecute(context, *collection_p->inputs, define_executor, define_result_chunk, partition_start,
		                         partition_end);

		config.pattern->Print();
		// define_result_chunk.Print();

		// TODO state machine stuff

		// printf("moo [%llu, %llu]\n", partition_start, partition_end);

		// set some dummy values to check result projection
		for (idx_t partition_idx = partition_start; partition_idx <= partition_end; partition_idx++) {
			FlatVector::Validity(gstate.result_vec).SetValid(partition_idx);
			auto &struct_entries = StructVector::GetEntries(gstate.result_vec);
			// first entry is list of classifiers, TODO
			struct_entries[1]->SetValue(partition_idx, Value::BOOLEAN(true));
			struct_entries[2]->SetValue(partition_idx, Value::INTEGER(NumericCast<int32_t>(partition_start)));
			struct_entries[3]->SetValue(partition_idx, Value::INTEGER(NumericCast<int32_t>(partition_idx)));
		}

		partition_start = payload_idx;
	}
}

// this should actually be it yay!
void WindowMatchRecognizeExecutor::EvaluateInternal(WindowExecutorGlobalState &gstate_p,
                                                    WindowExecutorLocalState &lstate_p, DataChunk &eval_chunk_p,
                                                    Vector &result_p, idx_t count_p, idx_t row_idx_p) const {
	auto &gstate = gstate_p.Cast<WindowMatchRecognizeGlobalState>();
	lock_guard<mutex> lock(gstate.state_lock);
	result_p.Slice(gstate.result_vec, row_idx_p, row_idx_p + count_p);
}

} // namespace duckdb
