#ifndef SLCLI_WRITER_CAPTURE_BACKING_WINDOWS_H
#define SLCLI_WRITER_CAPTURE_BACKING_WINDOWS_H
#include "capture_plan_windows.h"
#include "../host-service/primary_engine.h"
#if defined(_WIN32)
struct slcli_bq_capture_schedule_windows;
/* One actual full-file physical reservation and section pair. No view or
 * allocator LIVE state is produced by this constructor. */
struct slcli_writer_capture_backing_windows {
 struct slcli_arena *arena;
 struct slcli_binding_storage_original *storage_original;
 struct slcli_bq_capture_schedule_windows *bq_schedule_original;
 const struct slcli_native_reference *reference_plans;
 const uint32_t *reference_ids;
 uint32_t first_ordinal,claimed;
 size_t reference_count;
 struct slcli_writer_windows_readback_original *readback;
 struct slcli_memory_owner *owner;
 struct slcli_extent_request request;
 const struct slcli_writer_capture_view_plan_windows *plan;
 size_t count;uint64_t file_bytes,mapped_bytes,previous_tick;
 SYSTEM_INFO system;uintptr_t system_output;
 HANDLE sections[2];uint64_t section_args[2][6];DWORD section_error[2];
 struct slcli_windows_file_capture file_after;int file_after_result;
 struct slcli_native_clock clocks[4][2];
 int clock_result[4][2];uint32_t clock_returned[4][2],called[4],returned[4];
 size_t view_next;
 struct slcli_native_clock views_finish;
 int views_finish_result;uint32_t views_finish_returned;
 uint32_t views_started,views_failed,views_complete;
 uint32_t started,failed,complete,backing_registered;
};
struct slcli_writer_capture_ranges_windows;
/* One retained composing caller, not an admission/root or RANGE receipt. */
struct slcli_writer_capture_factory_windows {
 struct slcli_arena *arena;
 struct slcli_writer_windows_path *creator;
 struct slcli_writer_windows_output *output;
 const struct slcli_writer_windows_evidence_originals *inputs;
 struct slcli_writer_windows_readback_original *readback;
 struct slcli_writer_capture_backing_windows *backing;
 struct slcli_writer_capture_ranges_windows *ranges;
 struct slcli_writer_capture_ranges_windows *output_ranges;
 struct slcli_writer_capture_ranges_windows *control_ranges;
 struct slcli_bq_capture_schedule_windows *bq_schedule_original;
 struct slcli_native_clock finish;
 const struct slcli_native_clock *origin_original;
 uint64_t origin_tick_original,expires_original;
 int finish_result;uint32_t finish_returned;
 size_t range_index,range_count;
 size_t output_range_index,output_range_count;
 size_t control_range_index,control_range_count;
 int results[7];uint32_t called[7],returned[7];
 uint32_t started,failed,complete,first_failure;
};
int slcli_writer_capture_factory_windows_create_original(
 struct slcli_writer_capture_factory_windows *,struct slcli_arena *,
 struct slcli_writer_windows_path *,struct slcli_writer_windows_output *,
 const struct slcli_writer_windows_evidence_originals *,struct slcli_writer_windows_readback_original *,
 struct slcli_writer_capture_backing_windows *,struct slcli_writer_capture_ranges_windows *,size_t,size_t,
 struct slcli_memory_owner *,const struct slcli_extent_request *,uint64_t,
 const struct slcli_writer_capture_view_plan_windows *,size_t,
 struct slcli_binding_storage_original *,const struct slcli_native_reference *,const uint32_t *);
int slcli_writer_capture_backing_windows_prepare_original(
 struct slcli_writer_capture_backing_windows *,struct slcli_arena *,
 struct slcli_writer_windows_readback_original *,struct slcli_memory_owner *,
 const struct slcli_extent_request *,uint64_t,
 const struct slcli_writer_capture_view_plan_windows *,size_t,
 struct slcli_binding_storage_original *,const struct slcli_native_reference *,const uint32_t *);
int slcli_writer_capture_backing_windows_matches_original(
 const struct slcli_writer_capture_backing_windows *);
/* Distinct BQ-local raw constructor. The genuine independently admitted BQ
 * entry must own the schedule and prior full native reservation before calling.
 * This function does not acquire qualification/source/caller authority. */
int slcli_writer_capture_backing_windows_prepare_bq_original(
 struct slcli_writer_capture_backing_windows *,struct slcli_arena *,
 struct slcli_writer_windows_readback_original *,struct slcli_memory_owner *,
 const struct slcli_extent_request *,uint64_t,
 const struct slcli_writer_capture_view_plan_windows *,size_t,
 struct slcli_bq_capture_schedule_windows *);
int slcli_writer_capture_factory_windows_create_bq_original(
 struct slcli_writer_capture_factory_windows *,struct slcli_arena *,
 struct slcli_writer_windows_path *,struct slcli_writer_windows_output *,
 const struct slcli_writer_windows_evidence_originals *,struct slcli_writer_windows_readback_original *,
 struct slcli_writer_capture_backing_windows *,struct slcli_writer_capture_ranges_windows *,size_t,size_t,
 struct slcli_writer_capture_ranges_windows *,size_t,size_t,
 struct slcli_writer_capture_ranges_windows *,size_t,size_t,
 struct slcli_bq_capture_schedule_windows *);
/* Allocation/native-call-free preflight; success grants no entry authority. */
int slcli_writer_capture_factory_windows_bq_preflight_original(
 struct slcli_writer_capture_factory_windows *,struct slcli_arena *,
 struct slcli_writer_windows_path *,struct slcli_writer_windows_output *,
 const struct slcli_writer_windows_evidence_originals *,struct slcli_writer_windows_readback_original *,
 struct slcli_writer_capture_backing_windows *,struct slcli_writer_capture_ranges_windows *,size_t,size_t,
 struct slcli_writer_capture_ranges_windows *,size_t,size_t,
 struct slcli_writer_capture_ranges_windows *,size_t,size_t,
 struct slcli_bq_capture_schedule_windows *);
int slcli_writer_capture_factory_windows_bq_matches_original(
 const struct slcli_writer_capture_factory_windows *);
#endif
#endif
