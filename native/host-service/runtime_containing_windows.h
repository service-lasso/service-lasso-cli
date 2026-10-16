#ifndef SLCLI_RUNTIME_CONTAINING_WINDOWS_H
#define SLCLI_RUNTIME_CONTAINING_WINDOWS_H
#include "capture_alias_windows.h"
#include "child_mapping_windows.h"
#include "route_write_windows.h"
#include "source_stream_windows.h"
#include "../node-owner/capture_layout_windows.h"
#if defined(_WIN32)
struct slcli_runtime_containing_windows_inputs {
 size_t threads,events,regions,modules,name_units;
 uint64_t runtime_bytes;
 const void *descriptor;size_t descriptor_bytes;
 uint32_t section_ordinal,duplicate_ordinal,initializer_ordinal,alias_ordinal,child_view_ordinal;
 const struct slcli_source_file_windows *source_file;
 const struct slcli_source_stream_windows *source_stream;
 const uint32_t *route_buffer_ordinals;size_t route_buffer_count;
};
struct slcli_runtime_containing_windows_claim_inputs {
 struct slcli_binding_storage_original *storage;
 const struct slcli_native_reference *plans,*route_plans;
 const uint32_t *ids,*route_ids;
};
/* All control allocations are actual parent/Service-arena blocks. The section
 * belongs to its original Runtime physical owner; no binding book is moved or
 * copied into it. Partial pointers/raw outcomes are never discarded. */
struct slcli_runtime_containing_windows {
 const struct slcli_runtime_containing_windows_claim_inputs *claims_original;
 struct slcli_runtime_containing_windows_inputs original;
 const struct slcli_runtime_containing_windows_inputs *inputs_original;
 const struct slcli_binding_originals *binding_original;
 struct slcli_memory_owner *runtime_owner_original;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_native_clock *prior_original;
 struct slcli_node_capture_layout_windows *layout;
 struct slcli_preentry_backing_windows *backing;
 struct slcli_capture_alias_windows *initializer,*alias;
 struct slcli_child_mapping_windows child;
  struct slcli_route_write_windows route_writer;
 int route_prepare_result;uint32_t route_prepare_returned;
 uint32_t started,first_failure,capture_prepared;
 int layout_result,section_result,duplicate_result,initializer_bind_result;
 int initializer_map_result,initialize_result,unmap_result,alias_bind_result,alias_map_result;
 int validate_result;uint32_t validate_returned;
 struct slcli_native_clock constructor_after;
 int constructor_after_result;uint32_t constructor_after_returned;
 uint32_t layout_returned,section_returned,duplicate_returned,initializer_bind_returned;
 uint32_t initializer_map_returned,initialize_returned,unmap_returned,alias_bind_returned,alias_map_returned;
};
/* Actual fixed containing prefix constructor. It neither launches a child nor
 * supplies profile/source/native authority. Complete installed entry/caller,
 * classification/digests/borrowers and finite profile qualification remain
 * required before its output may be used for birth or guarded import. */
int slcli_runtime_containing_windows_prepare_original(
 struct slcli_runtime_containing_windows *,struct slcli_arena *,
 const struct slcli_binding_originals *,struct slcli_memory_owner *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,const struct slcli_runtime_containing_windows_inputs *);
int slcli_runtime_containing_windows_prepare_claimed_original(
 struct slcli_runtime_containing_windows *,struct slcli_arena *,
 const struct slcli_binding_originals *,struct slcli_memory_owner *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,const struct slcli_runtime_containing_windows_inputs *,
 const struct slcli_runtime_containing_windows_claim_inputs *);
struct slcli_runtime_source_windows_inputs {
 const WCHAR *path;size_t path_bytes;
 const uint32_t *namespace_ordinals;size_t namespace_count;
 const uint32_t *file_ordinals;
 const uint32_t *stream_ordinals;size_t stream_count;
 uint64_t image_file_bytes;size_t workspace_bytes;
 struct slcli_runtime_containing_windows_inputs containing;
};
struct slcli_runtime_source_windows_claim_inputs {
 struct slcli_binding_storage_original *storage;
 const struct slcli_native_reference *namespace_plans,*file_plans,*stream_plans;
 const uint32_t *namespace_ids,*file_ids,*stream_ids;
 const struct slcli_runtime_containing_windows_claim_inputs *containing;
};
/* Exact constructor schedule only: no source/member/actor admission, table
 * allocation or selected capacity follows from these checked counts. */
struct slcli_runtime_source_schedule_windows {
 struct slcli_source_stream_geometry_windows stream;
 size_t namespace_rows,file_rows,stream_rows,containing_rows,route_rows,total_rows;
 size_t namespace_offset,file_offset,stream_offset,containing_offset,route_offset;
 size_t plan_bytes,id_bytes,ordinal_bytes;
};
int slcli_runtime_source_schedule_windows_derive(size_t,uint64_t,size_t,size_t,
 struct slcli_runtime_source_schedule_windows *);
struct slcli_runtime_source_windows {
 const struct slcli_runtime_source_windows_claim_inputs *claims_original;
 const struct slcli_runtime_source_windows_inputs *inputs_original;
 const struct slcli_binding_originals *book;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 struct slcli_installed_namespace_windows *namespace_capture;
 struct slcli_source_file_windows *file;
 struct slcli_source_stream_windows *stream;
 struct slcli_runtime_containing_windows *containing;
 struct slcli_runtime_containing_windows_inputs *containing_inputs;
 struct slcli_source_stream_geometry_windows stream_geometry;
 struct slcli_runtime_source_schedule_windows schedule;
 struct slcli_native_clock before,allocated_after;
 int before_result,allocated_after_result,namespace_result,file_result,stream_result,containing_result;
 uint32_t started,first_failure,namespace_returned,file_returned,stream_returned,containing_returned;
};
/* Concrete fixed caller of the actual source acquisition and containing
 * constructors. It does not establish the independent installed host premise
 * or select a capacity; all source/numeric/native admission remains required. */
int slcli_runtime_source_windows_acquire_original(struct slcli_runtime_source_windows *,
 struct slcli_arena *,const struct slcli_binding_originals *,struct slcli_memory_owner *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,const struct slcli_runtime_source_windows_inputs *);
int slcli_runtime_source_windows_acquire_claimed_original(struct slcli_runtime_source_windows *,
 struct slcli_arena *,const struct slcli_binding_originals *,struct slcli_memory_owner *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,const struct slcli_runtime_source_windows_inputs *,
 const struct slcli_runtime_source_windows_claim_inputs *);
#endif
#endif
