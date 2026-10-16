#ifndef SLCLI_INSTALLED_NAMESPACE_WINDOWS_H
#define SLCLI_INSTALLED_NAMESPACE_WINDOWS_H
#include "primary_engine.h"
#if defined(_WIN32)
#include <windows.h>
#include <winternl.h>
struct slcli_installed_namespace_component_windows {
 UNICODE_STRING name;OBJECT_ATTRIBUTES attributes;IO_STATUS_BLOCK io;
 HANDLE handle;NTSTATUS status;FILE_ATTRIBUTE_TAG_INFO tag;
 uint64_t args[11],query_args[4],version;uint32_t ordinal;
 struct slcli_native_clock before,after,query_before,query_after;
 BOOL query_status;DWORD query_error;
 uint32_t called,returned,query_called,query_returned,complete;
};
struct slcli_installed_namespace_windows {
 const struct slcli_binding_originals *book;
 struct slcli_binding_storage_original *storage_original;
 const struct slcli_native_reference *plans_original;
 const uint32_t *ids_original;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const WCHAR *path_original;size_t path_bytes;
 const uint32_t *ordinals_original;
 const struct slcli_native_clock *prior_original;
 WCHAR *path,*root;
 struct slcli_installed_namespace_component_windows *components;
 size_t count,prepared,attempted,completed;
 const void *leaf_borrower_original;size_t leaf_borrower_bytes;
 uint32_t leaf_borrow_attempted;
 uint32_t started,acquired;
};
/* Actual source-object acquisition only. The independently qualified original
 * installation/actor premise is not established by this constructor. All raw
 * partial handles and ancestors remain retained; no close/retry is implicit. */
int slcli_installed_namespace_windows_acquire_original(
 struct slcli_installed_namespace_windows *,struct slcli_arena *,
 const struct slcli_binding_originals *,const struct slcli_source_deadline_original *,
 const void *,size_t,const struct slcli_native_clock *,const WCHAR *,size_t,
 const uint32_t *,size_t);
int slcli_installed_namespace_windows_matches_original(
 const struct slcli_installed_namespace_windows *,struct slcli_arena *);
/* Source descriptor plans remain claims. This variant consumes actual typed
 * component reservations before the first namespace native effect. */
int slcli_installed_namespace_windows_acquire_claimed_original(
 struct slcli_installed_namespace_windows *,struct slcli_arena *,
 struct slcli_binding_storage_original *,const struct slcli_source_deadline_original *,
 const void *,size_t,const struct slcli_native_clock *,const WCHAR *,size_t,
 const uint32_t *,size_t,const struct slcli_native_reference *,const uint32_t *);
#endif
#endif
