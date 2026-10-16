#ifndef SLCLI_PREENTRY_BACKING_WINDOWS_H
#define SLCLI_PREENTRY_BACKING_WINDOWS_H
#include "source_deadline.h"
#if defined(_WIN32)
#include <windows.h>
struct slcli_preentry_backing_windows {
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin,*predecessor;
  const void *caller,*descriptor_original;
  size_t caller_bytes,descriptor_bytes;
  uint64_t origin_tick,expires,predecessor_tick,bytes;
  struct slcli_memory_owner *physical_owner;
  uint32_t section_only;
  struct slcli_extent_request request;
  struct slcli_native_clock before[6],after[6];
  uint64_t args[6][6];
  uint32_t called[6],returned[6];
  int before_result[6],after_result[6];
  SYSTEM_INFO page_geometry;
  SECURITY_ATTRIBUTES security;
  void *descriptor,*mapping;
  HANDLE backing,expected_backing,duplicate;
  struct slcli_native_clock duplicate_before,duplicate_after;
  uint64_t duplicate_args[7];
  BOOL duplicate_status,handle_status[2],compare_status;
  DWORD duplicate_error,handle_flags[2],handle_error[2],compare_error;
  uint32_t operation,duplicate_attempted,duplicate_called,duplicate_returned,duplicate_qualified;
  DWORD create_error,map_error,geometry_error;
  SIZE_T geometry_status;
  MEMORY_BASIC_INFORMATION geometry;
  int prepare_result,backing_result,partial_result,acquired_result,retained_result;
  uint32_t prepare_returned,backing_returned,partial_returned,acquired_returned,retained_returned;
  uint32_t started,prepared,mapped,qualified;
};
/* Real unnamed paging-backed inherited section and parent view. Explicit
 * original private descriptor admission is independent; all raw partial HANDLE
 * and view outputs remain charged/retained on any later failure. */
int slcli_preentry_backing_windows_create(struct slcli_preentry_backing_windows *,
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,
  size_t,const struct slcli_native_clock *,uint64_t,const void *,size_t);
/* Target owner is the genuine runtime physical owner, distinct from capture arena.
 * Section-only creation never maps the whole runtime into the parent. */
int slcli_preentry_backing_windows_create_section(struct slcli_preentry_backing_windows *,
  struct slcli_arena *,struct slcli_memory_owner *,
  const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,uint64_t,const void *,size_t);
int slcli_preentry_backing_windows_duplicate(struct slcli_preentry_backing_windows *,
  struct slcli_arena *,const struct slcli_native_clock *);
/* The second held alias is independently transported native original custody,
 * not a number read from the mapped bytes. Native same-object comparison is
 * still not independent actor/source/private parent admission. */
int slcli_preentry_backing_windows_receive(struct slcli_preentry_backing_windows *,
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,
  size_t,const struct slcli_native_clock *,HANDLE,HANDLE,uint64_t,
  const struct slcli_extent_request *);
/* Reobserve an already acquired prepared root; no mapping/promotion occurs.
 * Native handle equality and geometry do not prove section/view/source identity. */
int slcli_preentry_backing_windows_recheck_mapped(struct slcli_preentry_backing_windows *,
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,
  size_t,const struct slcli_native_clock *,HANDLE,HANDLE,uint64_t,
  const struct slcli_extent_request *);
#endif
#endif
