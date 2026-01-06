#include "duckdb/function/window/window_match_recognize.hpp"

#include "duckdb/function/match_recognize.hpp"
#include "duckdb/planner/expression_iterator.hpp"
#include "duckdb/parser/expression/pattern_expression.hpp"

namespace duckdb {

struct WindowMatchRecognizeGlobalState : WindowExecutorGlobalState {
	WindowMatchRecognizeGlobalState(ClientContext &client, const WindowExecutor &executor_p,
	                                const idx_t payload_count_p, const ValidityMask &partition_mask_p,
	                                const ValidityMask &order_mask_p)
	    : WindowExecutorGlobalState(client, executor_p, payload_count_p, partition_mask_p, order_mask_p),
	      result_vec(executor_p.wexpr.return_type, payload_count_p) {
		FlatVector::Validity(result_vec).SetAllInvalid(payload_count_p);
		D_ASSERT(result_vec.GetType().id() == LogicalTypeId::STRUCT);
		auto &struct_entries = StructVector::GetEntries(result_vec);
		// first entry is list of classifiers
		FlatVector::Validity(*struct_entries[0]).SetAllInvalid(payload_count_p);
		FlatVector::Validity(*struct_entries[1]).SetAllInvalid(payload_count_p);
		FlatVector::Validity(*struct_entries[2]).SetAllInvalid(payload_count_p);
		FlatVector::Validity(*struct_entries[3]).SetAllInvalid(payload_count_p);
		FlatVector::Validity(*struct_entries[4]).SetAllInvalid(payload_count_p);
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
WindowMatchRecognizeExecutor::WindowMatchRecognizeExecutor(BoundWindowExpression &wexpr,
                                                           WindowSharedExpressions &shared)
    : WindowExecutor(wexpr, shared) {
	// RegisterCollection deduplicates child expressions - we need to update the references in the defines to its return
	// value
	unordered_map<column_t, column_t> replacement_map;
	for (idx_t expr_idx = 0; expr_idx < wexpr.children.size(); expr_idx++) {
		const auto &child = wexpr.children[expr_idx];
		// D_ASSERT(child->GetExpressionType() == ExpressionType::BOUND_REF);
		auto new_index = shared.RegisterCollection(child, /* this seems to not matter for the partition_mask */ false);
		replacement_map[expr_idx] = new_index;
	}
	// TODO
	// auto &define_expressions = wexpr.bind_info->Cast<MatchRecognizeFunctionData>().defines_expression_list;
	// for (auto &define_expression : define_expressions) {
	// 	UpdateColumnBindings(define_expression, replacement_map);
	// }
}

unique_ptr<GlobalSinkState> WindowMatchRecognizeExecutor::GetGlobalState(ClientContext &client,
                                                                         const idx_t payload_count,
                                                                         const ValidityMask &partition_mask,
                                                                         const ValidityMask &order_mask) const {
	return make_uniq<WindowMatchRecognizeGlobalState>(client, *this, payload_count, partition_mask, order_mask);
}

inline bool RowIsVisible(ColumnDataScanState &scan, idx_t row_idx) {
	return (row_idx < scan.next_row_index && scan.current_row_index <= row_idx);
}

static void FetchPartition(ClientContext &context, ColumnDataCollection &input, DataChunk &result_chunk,
                           idx_t partition_start, idx_t partition_end) {
	ColumnDataScanState scan_state;
	DataChunk scan_chunk;

	// TODO cache those allocations
	input.InitializeScanChunk(scan_chunk);

	auto partition_size = partition_end - partition_start + 1;

	D_ASSERT(result_chunk.GetCapacity() <= partition_size);

	input.InitializeScan(scan_state);
	// we do one Scan() because Seek() does nothing if its already on the right chunk
	input.Scan(scan_state, scan_chunk);
	input.Seek(partition_start, scan_state, scan_chunk);

	// we may have to slice the first chunk because the partition may start somewhere halfway into the chunk
	auto chunk_offset = partition_start - scan_state.current_row_index;
	scan_chunk.Slice(chunk_offset, scan_chunk.size() - chunk_offset);

	result_chunk.Append(scan_chunk, false);

	while (partition_end <= scan_state.next_row_index) {
		if (!input.Scan(scan_state, scan_chunk)) {
			// we need to get out here because otherwise the check below goes wrong
			break;
		}
		// we may have too many rows, so slice again
		if (scan_state.next_row_index > partition_end) {
			scan_chunk.Slice(0, scan_state.next_row_index - partition_end); // TODO verify this very complex math
		}
		result_chunk.Append(scan_chunk, false);
	}
	D_ASSERT(result_chunk.size() == partition_size);
}

struct Match {
	Match(bool success_p, optional_idx end_idx_p = optional_idx::Invalid(), bool optional_p = false)
	    : success(success_p), end_idx(end_idx_p), optional(optional_p) {
	}
	optional_idx end_idx;
	bool success;
	bool optional;
};

// simplistic backtracking-based pattern executor
// FIXME pretty naive this, and an allocation-fest.
static vector<Match> MatchPattern(const Expression &pattern, const DataChunk &input, const idx_t offset,
                                  unordered_map<string, idx_t> &define_child_mapping) {
	switch (pattern.type) {
	case ExpressionType::CONCATENATION: {
		const auto &concatenation_expr = (BoundConcatenationExpression &)pattern;

		auto child_start_idx = offset;
		idx_t token_idx = 0;
		vector<Match> matches;

		while (token_idx < concatenation_expr.children.size()) {
			auto &child_pattern = *concatenation_expr.children[token_idx];
			auto res = MatchPattern(child_pattern, input, child_start_idx, define_child_mapping);
			if (res.back().success) {
				matches.insert(matches.end(), res.begin(), res.end());
				child_start_idx = res.back().end_idx.GetIndex();
				token_idx++;
			} else {
				auto token_is_optional = false;
				if (pattern.type == ExpressionType::QUANTIFIER) {
					const auto &quantifier_expr = (BoundQuantifierExpression &)child_pattern;
					if (quantifier_expr.min_count.IsValid() && quantifier_expr.min_count.GetIndex() > 0) {
						token_is_optional = true;
					}
				}
				if (token_is_optional) {
					token_idx++;
					continue;
				}
				if (!matches.empty() && matches.back().optional) {
					child_start_idx = matches.back().end_idx.GetIndex();
					matches.pop_back();
					continue;
				}
				break;
			}
		}
		return {Match(token_idx == concatenation_expr.children.size(), offset, child_start_idx)};
	}
	case ExpressionType::QUANTIFIER: {
		const auto &quantifier_expr = (BoundQuantifierExpression &)pattern;
		idx_t match_count = 0;
		auto child_start_idx = offset;
		auto max_offset = quantifier_expr.max_count.IsValid()
		                      ? MinValue(input.size(), quantifier_expr.max_count.GetIndex())
		                      : input.size();
		vector<Match> matches;
		while (match_count < max_offset) {
			auto res = MatchPattern(*quantifier_expr.child, input, child_start_idx, define_child_mapping);
			if (!res.back().success) {
				break;
			}
			child_start_idx = res.back().end_idx.GetIndex();
			match_count++;
			auto is_optional =
			    quantifier_expr.min_count.IsValid() ? match_count >= quantifier_expr.min_count.GetIndex() : true;
			matches.emplace_back(Match(true, child_start_idx, is_optional));
		}
		if (!matches.empty() &&
		    (!quantifier_expr.min_count.IsValid() || match_count >= quantifier_expr.min_count.GetIndex())) {
			return matches;
		}
		return {Match(false)};
	}
	case ExpressionType::BOUND_COLUMN_REF: {
		// TODO cache those pointers in the map instead of the vector
		D_ASSERT(define_child_mapping.find(pattern.alias) != define_child_mapping.end());
		auto &child_vector = StructVector::GetEntries(input.data[0])[define_child_mapping[pattern.alias]];
		D_ASSERT(child_vector->GetVectorType() == VectorType::FLAT_VECTOR);
		return {Match(FlatVector::GetData<uint8_t>(*child_vector)[offset], offset)};
	}
	default:
		throw InternalException("Unsupported pattern type");
	}
}

// this gets called per partition
void WindowMatchRecognizeExecutor::Finalize(ExecutionContext &context, CollectionPtr collection,
                                            OperatorSinkInput &sink) const {
	auto &gstate = sink.global_state.Cast<WindowMatchRecognizeGlobalState>();
	lock_guard<mutex> lock(gstate.state_lock);

	auto &config = wexpr.bind_info->Cast<MatchRecognizeFunctionData>();

	idx_t partition_start = 0;
	// we always start with a new partition
	D_ASSERT(gstate.partition_mask.RowIsValid(0));

	// TODO this can be cached higher up
	// figure out which define has which child offset
	unordered_map<string, idx_t> define_child_mapping;
	auto &defines_struct_child = collection->inputs->Types()[0];
	for (idx_t struct_child_idx = 0; struct_child_idx < StructType::GetChildCount(defines_struct_child);
	     struct_child_idx++) {
		define_child_mapping[StructType::GetChildName(defines_struct_child, struct_child_idx)] = struct_child_idx;
	}

	for (idx_t payload_idx = 1; payload_idx < gstate.payload_count; payload_idx++) {
		if (!gstate.partition_mask.RowIsValid(payload_idx) && payload_idx + 1 < gstate.payload_count) {
			continue;
		}
		// the partition end offset depends on whether we found a next partition or if we are at the end
		auto partition_end =
		    payload_idx + 1 == gstate.partition_mask.RowIsValid(payload_idx) ? payload_idx - 1 : payload_idx;

		// TODO we should not reallocate this all the time but whatever
		DataChunk partition_chunk;
		partition_chunk.Initialize(context.client, collection->inputs->Types(), partition_end - partition_start + 1);
		// FIXME
		FetchPartition(context.client, *collection->inputs, partition_chunk, partition_start, partition_end);

		// FIXME
		partition_chunk.Print();
		config.pattern->Print();

		for (idx_t partition_idx = partition_start; partition_idx <= partition_end; partition_idx++) {
			auto match = MatchPattern(*config.pattern, partition_chunk, partition_idx, define_child_mapping).back();

			FlatVector::Validity(gstate.result_vec).SetValid(partition_idx);
			auto &struct_entries = StructVector::GetEntries(gstate.result_vec);
			// first entry is list of classifiers, TODO
			struct_entries[1]->SetValue(partition_idx, Value::BOOLEAN(match.end_idx == partition_end));
			struct_entries[2]->SetValue(partition_idx, Value::INTEGER(NumericCast<int32_t>(partition_start)));
			struct_entries[3]->SetValue(partition_idx, Value::INTEGER(NumericCast<int32_t>(match.end_idx.GetIndex())));
			struct_entries[4]->SetValue(partition_idx, Value::INTEGER(NumericCast<int32_t>(match.end_idx.GetIndex())));

			gstate.result_vec.Print(partition_idx);
		}

		partition_start = payload_idx;
	}
}

// this should actually be it yay!
void WindowMatchRecognizeExecutor::EvaluateInternal(ExecutionContext &context, DataChunk &eval_chunk, Vector &result,
                                                    idx_t count, idx_t row_idx, OperatorSinkInput &sink) const {
	auto &gstate = sink.global_state.Cast<WindowMatchRecognizeGlobalState>();
	lock_guard<mutex> lock(gstate.state_lock);
	result.Slice(gstate.result_vec, row_idx, row_idx + count);
}

} // namespace duckdb
