// This file was generated by tools/jdbc/generator.py

#pragma once

#include "duckdb/common/assert.hpp"
#include "org_duckdb_DuckDBNative.h"
#include <exception>

void ThrowJNI(JNIEnv *env, const char *message);

jobject _duckdb_jdbc_startup(JNIEnv *env, jclass param0, jbyteArray param1, jboolean param2, jobject param3);

JNIEXPORT jobject JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1startup(JNIEnv *env, jclass param0,
                                                                             jbyteArray param1, jboolean param2,
                                                                             jobject param3);

jobject _duckdb_jdbc_connect(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT jobject JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1connect(JNIEnv *env, jclass param0,
                                                                             jobject param1);

void _duckdb_jdbc_set_auto_commit(JNIEnv *env, jclass param0, jobject param1, jboolean param2);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1set_1auto_1commit(JNIEnv *env, jclass param0,
                                                                                    jobject param1, jboolean param2);

jboolean _duckdb_jdbc_get_auto_commit(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT jboolean JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1get_1auto_1commit(JNIEnv *env, jclass param0,
                                                                                        jobject param1);

void _duckdb_jdbc_disconnect(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1disconnect(JNIEnv *env, jclass param0,
                                                                             jobject param1);

void _duckdb_jdbc_set_schema(JNIEnv *env, jclass param0, jobject param1, jstring param2);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1set_1schema(JNIEnv *env, jclass param0,
                                                                              jobject param1, jstring param2);

void _duckdb_jdbc_set_catalog(JNIEnv *env, jclass param0, jobject param1, jstring param2);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1set_1catalog(JNIEnv *env, jclass param0,
                                                                               jobject param1, jstring param2);

jstring _duckdb_jdbc_get_schema(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT jstring JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1get_1schema(JNIEnv *env, jclass param0,
                                                                                 jobject param1);

jstring _duckdb_jdbc_get_catalog(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT jstring JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1get_1catalog(JNIEnv *env, jclass param0,
                                                                                  jobject param1);

jobject _duckdb_jdbc_prepare(JNIEnv *env, jclass param0, jobject param1, jbyteArray param2);

JNIEXPORT jobject JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1prepare(JNIEnv *env, jclass param0, jobject param1,
                                                                             jbyteArray param2);

void _duckdb_jdbc_release(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1release(JNIEnv *env, jclass param0, jobject param1);

jobject _duckdb_jdbc_query_result_meta(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT jobject JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1query_1result_1meta(JNIEnv *env, jclass param0,
                                                                                         jobject param1);

jobject _duckdb_jdbc_prepared_statement_meta(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT jobject JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1prepared_1statement_1meta(JNIEnv *env,
                                                                                               jclass param0,
                                                                                               jobject param1);

jobject _duckdb_jdbc_execute(JNIEnv *env, jclass param0, jobject param1, jobjectArray param2);

JNIEXPORT jobject JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1execute(JNIEnv *env, jclass param0, jobject param1,
                                                                             jobjectArray param2);

void _duckdb_jdbc_free_result(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1free_1result(JNIEnv *env, jclass param0,
                                                                               jobject param1);

jobjectArray _duckdb_jdbc_fetch(JNIEnv *env, jclass param0, jobject param1, jobject param2);

JNIEXPORT jobjectArray JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1fetch(JNIEnv *env, jclass param0,
                                                                                jobject param1, jobject param2);

jint _duckdb_jdbc_fetch_size(JNIEnv *env, jclass param0);

JNIEXPORT jint JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1fetch_1size(JNIEnv *env, jclass param0);

jlong _duckdb_jdbc_arrow_stream(JNIEnv *env, jclass param0, jobject param1, jlong param2);

JNIEXPORT jlong JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1arrow_1stream(JNIEnv *env, jclass param0,
                                                                                 jobject param1, jlong param2);

void _duckdb_jdbc_arrow_register(JNIEnv *env, jclass param0, jobject param1, jlong param2, jbyteArray param3);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1arrow_1register(JNIEnv *env, jclass param0,
                                                                                  jobject param1, jlong param2,
                                                                                  jbyteArray param3);

jobject _duckdb_jdbc_create_appender(JNIEnv *env, jclass param0, jobject param1, jbyteArray param2, jbyteArray param3);

JNIEXPORT jobject JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1create_1appender(JNIEnv *env, jclass param0,
                                                                                      jobject param1, jbyteArray param2,
                                                                                      jbyteArray param3);

void _duckdb_jdbc_appender_begin_row(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1appender_1begin_1row(JNIEnv *env, jclass param0,
                                                                                       jobject param1);

void _duckdb_jdbc_appender_end_row(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1appender_1end_1row(JNIEnv *env, jclass param0,
                                                                                     jobject param1);

void _duckdb_jdbc_appender_flush(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1appender_1flush(JNIEnv *env, jclass param0,
                                                                                  jobject param1);

void _duckdb_jdbc_interrupt(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1interrupt(JNIEnv *env, jclass param0, jobject param1);

void _duckdb_jdbc_appender_close(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1appender_1close(JNIEnv *env, jclass param0,
                                                                                  jobject param1);

void _duckdb_jdbc_appender_append_boolean(JNIEnv *env, jclass param0, jobject param1, jboolean param2);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1appender_1append_1boolean(JNIEnv *env, jclass param0,
                                                                                            jobject param1,
                                                                                            jboolean param2);

void _duckdb_jdbc_appender_append_byte(JNIEnv *env, jclass param0, jobject param1, jbyte param2);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1appender_1append_1byte(JNIEnv *env, jclass param0,
                                                                                         jobject param1, jbyte param2);

void _duckdb_jdbc_appender_append_short(JNIEnv *env, jclass param0, jobject param1, jshort param2);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1appender_1append_1short(JNIEnv *env, jclass param0,
                                                                                          jobject param1,
                                                                                          jshort param2);

void _duckdb_jdbc_appender_append_int(JNIEnv *env, jclass param0, jobject param1, jint param2);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1appender_1append_1int(JNIEnv *env, jclass param0,
                                                                                        jobject param1, jint param2);

void _duckdb_jdbc_appender_append_long(JNIEnv *env, jclass param0, jobject param1, jlong param2);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1appender_1append_1long(JNIEnv *env, jclass param0,
                                                                                         jobject param1, jlong param2);

void _duckdb_jdbc_appender_append_float(JNIEnv *env, jclass param0, jobject param1, jfloat param2);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1appender_1append_1float(JNIEnv *env, jclass param0,
                                                                                          jobject param1,
                                                                                          jfloat param2);

void _duckdb_jdbc_appender_append_double(JNIEnv *env, jclass param0, jobject param1, jdouble param2);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1appender_1append_1double(JNIEnv *env, jclass param0,
                                                                                           jobject param1,
                                                                                           jdouble param2);

void _duckdb_jdbc_appender_append_string(JNIEnv *env, jclass param0, jobject param1, jbyteArray param2);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1appender_1append_1string(JNIEnv *env, jclass param0,
                                                                                           jobject param1,
                                                                                           jbyteArray param2);

void _duckdb_jdbc_appender_append_null(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1appender_1append_1null(JNIEnv *env, jclass param0,
                                                                                         jobject param1);

void _duckdb_jdbc_create_extension_type(JNIEnv *env, jclass param0, jobject param1);

JNIEXPORT void JNICALL Java_org_duckdb_DuckDBNative_duckdb_1jdbc_1create_1extension_1type(JNIEnv *env, jclass param0,
                                                                                          jobject param1);