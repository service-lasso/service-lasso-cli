#ifndef SLCLI_CHILD_MAPPING_WINDOWS_H
#define SLCLI_CHILD_MAPPING_WINDOWS_H
#include "source_deadline.h"
#if defined(_WIN32)
#include <windows.h>
#include <winternl.h>
struct slcli_runtime_containing_windows;
struct slcli_launcher_windows_capture;
struct slcli_child_mapping_windows {
 const struct slcli_runtime_containing_windows *root_original;
 const struct slcli_launcher_windows_capture *birth_original;
 HANDLE process_original,section_original;
 DWORD process_id_original,thread_id_original;
 FILETIME creation_original;
 struct slcli_native_clock before,after,query_before,query_after;
 uint64_t map_args[10],query_args[4];
 void *base,*requested_base_original;SIZE_T bytes,bytes_before;
 LARGE_INTEGER offset;NTSTATUS status;
 uint64_t row_version_original,section_version_original;
 MEMORY_BASIC_INFORMATION geometry;SIZE_T query_status;DWORD query_error;
 uint32_t enrolled,attempted,map_called,map_returned,possibly_mapped;
 uint32_t query_called,query_returned,qualified;
};
/* Actual original parent-owned cells, no dereference of the child address.
 * Distinct native view row; SAME physical section, before any child resume. */
int slcli_child_mapping_windows_map_original(struct slcli_runtime_containing_windows *,
 struct slcli_arena *,const struct slcli_launcher_windows_capture *);
/* Read-only original correspondence before any resume attempt. No native
 * re-observation, lifetime exclusion, root or source authority is granted. */
int slcli_child_mapping_windows_matches_original(struct slcli_runtime_containing_windows *,
 struct slcli_arena *,const struct slcli_launcher_windows_capture *);
#endif
#endif
