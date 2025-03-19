add_library_unity(
  duckdb_parser_tableref
  OBJECT
  basetableref.cpp
  column_data_ref.cpp
  delimgetref.cpp
  emptytableref.cpp
  expressionlistref.cpp
  joinref.cpp
  match_recognize_ref.cpp
  pivotref.cpp
  showref.cpp
  subqueryref.cpp
  table_function.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:duckdb_parser_tableref>
    PARENT_SCOPE)
