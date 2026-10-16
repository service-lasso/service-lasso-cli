#ifndef SLCLI_BQ_CAPTURE_SCHEDULE_WINDOWS_H
#define SLCLI_BQ_CAPTURE_SCHEDULE_WINDOWS_H
#include "capture_backing_windows.h"
#if defined(_WIN32)
/* BQ-local control routing only. These slots contain no native resource IDs,
 * quota or admission flag. Genuine Q125/capacity127/P117/source/caller admission
 * and prior native reservations are independent prerequisites of effect callers. */
struct slcli_bq_capture_target_windows {
 const void *containing;
 void *output;
 size_t containing_bytes,output_offset;
 uint64_t file_offset,bytes;
 uint32_t kind,taken;
};
struct slcli_bq_capture_schedule_windows {
 struct slcli_arena *arena;
 struct slcli_writer_capture_backing_windows *backing;
 const struct slcli_writer_capture_view_plan_windows *plan;
 struct slcli_bq_capture_target_windows *targets;
 struct slcli_memory_owner *owner;
 struct slcli_extent_request request;
 size_t count,target_count,next;
 uint64_t file_bytes;
 uint32_t started,prepared,failed;
};
int slcli_bq_capture_schedule_windows_prepare_original(
 struct slcli_bq_capture_schedule_windows *,struct slcli_arena *,
 struct slcli_writer_capture_backing_windows *,
 const struct slcli_writer_capture_view_plan_windows *,size_t,
 struct slcli_bq_capture_target_windows *,size_t,
 struct slcli_memory_owner *,const struct slcli_extent_request *,uint64_t);
int slcli_bq_capture_schedule_windows_matches_original(
 const struct slcli_bq_capture_schedule_windows *);
/* Consume before the actual effect; no failed target can be replayed. */
int slcli_bq_capture_schedule_windows_take_original(
 struct slcli_bq_capture_schedule_windows *,size_t,const void *,size_t,void *);
#endif
#endif
