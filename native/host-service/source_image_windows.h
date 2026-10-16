#ifndef SLCLI_SOURCE_IMAGE_WINDOWS_H
#define SLCLI_SOURCE_IMAGE_WINDOWS_H
#include "primary_engine.h"
#if defined(_WIN32)
#include <windows.h>
struct slcli_source_image_windows {
 const struct slcli_binding_originals *book;
 struct slcli_memory_owner *owner;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const WCHAR *path_original;WCHAR *path;size_t path_bytes;
 uint32_t ordinals[3],physical_index_original;uint64_t versions[3];
 struct slcli_extent_request request;
 struct slcli_native_clock before[8],after[8];
 uint64_t args[8][7],raw[8];DWORD errors[8];
 uint32_t called[8],returned[8],error_observed[8];
 HANDLE file,section;void *view;
 LARGE_INTEGER file_bytes;SYSTEM_INFO system;
 BY_HANDLE_FILE_INFORMATION information[2];
 MEMORY_BASIC_INFORMATION geometry;
 uint64_t rounded_bytes;
 uint32_t started,reserved,backing_recorded,view_recorded,mapped;
  int acquired_result;
  uint32_t acquired_returned,acquired_recorded;
};
/* Original raw source storage, not a loaded-image/source/namespace grant.
 * Three distinct fresh SAME-book rows precede all effects. Full file pages
 * belong to the real containing runtime owner, never a Service byte copy. */
int slcli_source_image_windows_acquire_original(struct slcli_source_image_windows *,
 struct slcli_arena *,const struct slcli_binding_originals *,struct slcli_memory_owner *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,const WCHAR *,size_t,uint32_t,uint32_t,uint32_t);
#endif
#endif
