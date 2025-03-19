//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/planner/bound_tokens.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

namespace duckdb {

//===--------------------------------------------------------------------===//
// Query Node
//===--------------------------------------------------------------------===//
class BoundQueryNode;
class BoundSelectNode;
class BoundSetOperationNode;
class BoundRecursiveCTENode;
class BoundCTENode;

//===--------------------------------------------------------------------===//
// Expressions
//===--------------------------------------------------------------------===//
class Expression;

class BoundAggregateExpression;
class BoundBetweenExpression;
class BoundCaseExpression;
class BoundCastExpression;
class BoundColumnRefExpression;
class BoundComparisonExpression;
class BoundConjunctionExpression;
class BoundConstantExpression;
class BoundDefaultExpression;
class BoundFunctionExpression;
class BoundMatchRecognizeRef;
class BoundOperatorExpression;
class BoundParameterExpression;
class BoundReferenceExpression;
class BoundSubqueryExpression;
class BoundUnnestExpression;
class BoundWindowExpression;

//===--------------------------------------------------------------------===//
// TableRefs
//===--------------------------------------------------------------------===//
class BoundTableRef;

class BoundBaseTableRef;
class BoundCTERef;
class BoundColumnDataRef;
class BoundDelimGetRef;
class BoundEmptyTableRef;
class BoundExpressionListRef;
class BoundJoinRef;
class BoundPivotRef;
class BoundSubqueryRef;
class BoundTableFunction;

} // namespace duckdb
