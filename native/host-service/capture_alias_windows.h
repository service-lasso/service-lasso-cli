#ifndef SLCLI_CAPTURE_ALIAS_WINDOWS_H
#define SLCLI_CAPTURE_ALIAS_WINDOWS_H
#include "primary_engine.h"
#include "preentry_backing_windows.h"
#if defined(_WIN32)
/* Storage is in the parent's original paid arena. The view borrows the SAME
 * runtime section; it never creates/reserves a second physical extent. */
struct slcli_capture_alias_windows {
 const struct slcli_preentry_backing_windows *backing_original;
 const struct slcli_binding_originals *binding_original;
 struct slcli_native_reference *section_reference,*view_reference;
 const struct slcli_source_deadline_original *deadline;
 const struct slcli_native_clock *origin,*predecessor;
 uint64_t origin_tick,expires,predecessor_tick;
 HANDLE backing_handle_original;
 uint64_t backing_bytes_original,backing_acquisition_original;
 uint32_t backing_extent_original,section_ordinal_original;
 const void *caller;
 size_t caller_bytes,capture_bytes;
 struct slcli_native_clock before,after,query_before,query_after;
 uint64_t map_args[5],query_args[3];
 void *view;
 MEMORY_BASIC_INFORMATION geometry;
 SIZE_T query_status;
 DWORD map_error,query_error;
 DWORD access_original;
 enum slcli_native_original_class native_class_original;
 struct slcli_native_clock initialize_before,initialize_after;
 size_t zero_scanned,zero_nonzero_offset,headers_written;
 unsigned char zero_nonzero_value;
 uint32_t initialize_started,initialize_returned,initialized;
 struct slcli_native_clock validate_before,validate_after;
 size_t validate_scanned,validate_mismatch_offset;
 unsigned char validate_actual,validate_expected;
 uint32_t validate_started,validate_returned,validated;
 struct slcli_native_clock unmap_before,unmap_after;
 uint64_t unmap_args[1];
 BOOL unmap_status;DWORD unmap_error;
 uint32_t unmap_attempted,unmap_called,unmap_returned,unmapped;
 int unmap_before_result,unmap_after_result;
 uint32_t started,map_called,map_returned,query_called,query_returned,qualified;
};
/* Enroll the source-reserved section row before entering the real section
 * constructor. Its raw child-independent capture remains held even if the
 * constructor fails before returning to this outer owning body. */
int slcli_preentry_section_windows_create_enrolled_original(
 struct slcli_preentry_backing_windows *,struct slcli_arena *,
 const struct slcli_binding_originals *,struct slcli_native_reference *,
 struct slcli_memory_owner *,const struct slcli_source_deadline_original *,
 const void *,size_t,const struct slcli_native_clock *,uint64_t,const void *,size_t);
int slcli_capture_alias_windows_bind_original(struct slcli_capture_alias_windows *,
 struct slcli_arena *,const struct slcli_binding_originals *,
 struct slcli_native_reference *,struct slcli_native_reference *,
 const struct slcli_preentry_backing_windows *);
int slcli_capture_initializer_windows_bind_original(struct slcli_capture_alias_windows *,
 struct slcli_arena *,const struct slcli_binding_originals *,
 struct slcli_native_reference *,struct slcli_native_reference *,
 const struct slcli_preentry_backing_windows *);
int slcli_preentry_section_windows_duplicate_enrolled_original(
 struct slcli_preentry_backing_windows *,struct slcli_arena *,
 const struct slcli_binding_originals *,struct slcli_native_reference *,
 struct slcli_native_reference *,const struct slcli_native_clock *);
/* Only original typed, already enrolled rows are accepted. This does not
 * derive section identity from a HANDLE or manufacture source enrollment. */
int slcli_capture_alias_windows_map_original(struct slcli_capture_alias_windows *,
 struct slcli_arena *,const struct slcli_binding_originals *,
 struct slcli_native_reference *,struct slcli_native_reference *,
 const struct slcli_preentry_backing_windows *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,size_t);
/* Fixed parent prebirth initializer view only. This actual distinct view must
 * be independently unmapped before birth; it is never the retained RO alias. */
int slcli_capture_initializer_windows_map_original(struct slcli_capture_alias_windows *,
 struct slcli_arena *,const struct slcli_binding_originals *,
 struct slcli_native_reference *,struct slcli_native_reference *,
 const struct slcli_preentry_backing_windows *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,size_t);
/* Fixed initializer's writer has returned (or never began); no child borrower
 * is admitted by this helper. Once-only unmap does not gate on useful clocks.
 * Actual containing caller must order this before child birth. */
int slcli_capture_initializer_windows_unmap_original(struct slcli_capture_alias_windows *,
 struct slcli_arena *);
#endif
#endif
