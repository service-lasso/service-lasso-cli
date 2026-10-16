#ifndef SLCLI_BQ_SOURCE_NAMESPACE_WINDOWS_H
#define SLCLI_BQ_SOURCE_NAMESPACE_WINDOWS_H
#include "installed_namespace_windows.h"
#if defined(_WIN32)
struct slcli_bq_source_namespace_target_windows {
 struct slcli_installed_namespace_component_windows *component;
 HANDLE *output;
 size_t ordinal;
 uint32_t taken;
};
/* Domain0 source-object custody only. No Service book, admission flag, quota,
 * or serialized/native authority constructor. Required genuine private-entry
 * source/caller/Q/P admission is an independent prerequisite of the caller. */
struct slcli_bq_source_namespace_windows {
 struct slcli_arena *arena;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const WCHAR *source_path;size_t path_bytes;
 WCHAR *path,*root;
 struct slcli_installed_namespace_component_windows *components;
 struct slcli_bq_source_namespace_target_windows *targets;
 const struct slcli_native_clock *prior;
 const struct slcli_native_clock *origin;
 uint64_t origin_tick,expires,previous_tick;
 size_t count,attempted,completed;
 struct slcli_native_clock finish;
 int finish_result;uint32_t finish_returned;
 uint32_t started,observed,failed;
};
int slcli_bq_source_namespace_windows_acquire_original(
 struct slcli_bq_source_namespace_windows *,struct slcli_arena *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,const WCHAR *,size_t,WCHAR *,WCHAR *,
 struct slcli_installed_namespace_component_windows *,
 struct slcli_bq_source_namespace_target_windows *,size_t);
int slcli_bq_source_namespace_windows_matches_original(
 const struct slcli_bq_source_namespace_windows *);
#endif
#endif
