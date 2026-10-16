#ifndef SLCLI_BQ_SOURCE_NAMESPACE_LINUX_H
#define SLCLI_BQ_SOURCE_NAMESPACE_LINUX_H
#include "source_deadline.h"
#include "file_linux.h"
#if defined(__linux__)
struct slcli_bq_source_namespace_component_linux {
 long args[6],status;
 int descriptor;
 size_t name_offset,name_bytes;
 struct slcli_linux_file_capture observation;
 struct slcli_native_clock before,after,finish;
 int before_result,after_result,observe_result,finish_result;
 uint32_t attempted,called,returned,before_returned,after_returned,observe_returned,finish_returned;
};
/* BQ-local O_PATH native custody, not a readable input descriptor or the initial
 * designation/admission constructor. Actual regular-file type is observed
 * before any future separately charged readable-alias acquisition. All path
 * references and partial outputs survive failure; no close/reset/retry. */
struct slcli_bq_source_namespace_linux {
 struct slcli_arena *arena;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_native_clock *prior,*origin;
 const char *path;char *names;
 size_t path_bytes,count,attempted,completed;
 struct slcli_bq_source_namespace_component_linux *components;
 uint64_t origin_tick,prior_tick,expires,previous_tick;
 struct slcli_native_clock finish;
 int finish_result;
 uint32_t started,failed,observed,finish_returned,leaf_kind;
};
/* names owns exactly path_bytes+2 bytes; rows own exactly count components.
 * Actual canonical path determines count: slash root plus every component. */
int slcli_bq_source_namespace_linux_acquire_original(struct slcli_bq_source_namespace_linux *,
 struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,const char *,size_t,char *,
 struct slcli_bq_source_namespace_component_linux *,size_t);
int slcli_bq_source_namespace_linux_acquire_directory_original(struct slcli_bq_source_namespace_linux *,
 struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,const char *,size_t,char *,
 struct slcli_bq_source_namespace_component_linux *,size_t);
int slcli_bq_source_namespace_linux_matches_original(const struct slcli_bq_source_namespace_linux *);
#endif
#endif
