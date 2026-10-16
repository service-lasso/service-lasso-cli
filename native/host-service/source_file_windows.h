#ifndef SLCLI_SOURCE_FILE_WINDOWS_H
#define SLCLI_SOURCE_FILE_WINDOWS_H
#include "primary_engine.h"
#include "installed_namespace_windows.h"
#if defined(_WIN32)
#include <windows.h>
struct slcli_source_stream_windows;
struct slcli_source_file_read_windows {
 uint64_t offset;DWORD bytes;unsigned char *body;
 LARGE_INTEGER requested,position;
 struct slcli_native_clock seek_before,seek_after,read_before,read_after;
 uint64_t seek_args[4],read_args[5];
 BOOL seek_status,read_status;DWORD seek_error,read_error,received;
 uint32_t allocated,seek_called,seek_returned,read_called,read_returned,complete;
};
struct slcli_source_file_windows {
 const struct slcli_binding_originals *book;
 struct slcli_binding_storage_original *storage_original;
 const struct slcli_native_reference *plans_original;
 const uint32_t *ids_original;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const WCHAR *path_original;WCHAR *path;size_t path_bytes;
 uint32_t ordinals[11];uint64_t versions[11];
 HANDLE file;LARGE_INTEGER bytes;
 BY_HANDLE_FILE_INFORMATION information[2];
 struct slcli_native_clock before[5],after[5];
 uint64_t args[5][7],raw[5];DWORD errors[5];
 uint32_t called[5],returned[5];
 struct slcli_source_file_read_windows reads[5];
 uint32_t pe_offset,section_count,optional_bytes,image_bytes,export_rva,export_bytes;
 uint32_t route_rva,route_raw_offset,route_section;
 uint32_t started,geometry_complete;
 const struct slcli_installed_namespace_windows *namespace_original;
 BOOL duplicate_status;
 /* Sole full-file synchronous borrower, installed once even on failure. */
 const struct slcli_source_stream_windows *stream_original;
};
/* Eleven distinct source rows: file4, then five (buffer6, IO5) pairs.
 * This retains bounded file metadata only, never a full PE memory mapping. */
int slcli_source_file_windows_probe_original(struct slcli_source_file_windows *,
 struct slcli_arena *,const struct slcli_binding_originals *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,const WCHAR *,size_t,const uint32_t *);
/* Distinct acquired handle alias of the actual typed namespace leaf. This
 * performs no pathname reopen and supplies no original host/source authority. */
int slcli_source_file_windows_probe_held_original(struct slcli_source_file_windows *,
 struct slcli_arena *,const struct slcli_binding_originals *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,struct slcli_installed_namespace_windows *,
 const uint32_t *);
int slcli_source_file_windows_matches_original(const struct slcli_source_file_windows *,struct slcli_arena *);
int slcli_source_file_windows_probe_held_claimed_original(struct slcli_source_file_windows *,
 struct slcli_arena *,struct slcli_binding_storage_original *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,struct slcli_installed_namespace_windows *,
 const uint32_t *,const struct slcli_native_reference *,const uint32_t *);
#endif
#endif
