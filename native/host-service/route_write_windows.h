#ifndef SLCLI_ROUTE_WRITE_WINDOWS_H
#define SLCLI_ROUTE_WRITE_WINDOWS_H
#include "source_file_windows.h"
#if defined(_WIN32)
struct slcli_runtime_containing_windows;
struct slcli_launcher_windows_capture;
struct slcli_route_read_windows {
 unsigned char *body;size_t bytes;uint32_t ordinal;uint64_t version;
 const void *address;SIZE_T received;BOOL status;DWORD error;
 struct slcli_native_clock before,after;uint64_t args[5];
 uint32_t allocated,called,returned,complete;
};
struct slcli_route_image_windows {
 MEMORY_BASIC_INFORMATION geometry;
 struct slcli_native_clock before,after;
 uint64_t args[4];SIZE_T status;DWORD error;
 uint32_t called,returned,headers_match;
 struct slcli_route_read_windows reads[3];
};
struct slcli_route_write_windows {
 struct slcli_binding_storage_original *storage_original;
 const struct slcli_native_reference *plans_original;
 const uint32_t *ids_original,*ordinals_original;
 const struct slcli_runtime_containing_windows *root;
 const struct slcli_source_file_windows *source;
 const struct slcli_launcher_windows_capture *birth;
 struct slcli_route_image_windows *rows;
 size_t capacity,used;uintptr_t minimum,maximum,cursor,image_base;
 struct slcli_route_read_windows initial,readback;
 MEMORY_BASIC_INFORMATION geometry;
 struct slcli_native_clock query_before,query_after,write_before,write_after;
 uint64_t query_args[4],write_args[5];SIZE_T query_status,written;
 BOOL write_status;DWORD query_error,write_error;
 unsigned char route_bytes_original[128];
 uint32_t started,prepared,scan_complete,matches,query_called,query_returned;
 uint32_t write_called,write_returned,possibly_written,readback_matches;
};
int slcli_route_write_windows_prepare_original(struct slcli_route_write_windows *,
 struct slcli_runtime_containing_windows *,struct slcli_arena *,
 const struct slcli_source_file_windows *,const uint32_t *,size_t);
int slcli_route_write_windows_write_original(struct slcli_runtime_containing_windows *,
 struct slcli_arena *,const struct slcli_launcher_windows_capture *);
int slcli_route_write_windows_prepare_claimed_original(struct slcli_route_write_windows *,
 struct slcli_runtime_containing_windows *,struct slcli_arena *,
 const struct slcli_source_file_windows *,const uint32_t *,size_t,
 struct slcli_binding_storage_original *,const struct slcli_native_reference *,const uint32_t *);
#endif
#endif
