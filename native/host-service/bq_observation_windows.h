#ifndef SLCLI_BQ_OBSERVATION_WINDOWS_H
#define SLCLI_BQ_OBSERVATION_WINDOWS_H
#include "primary_engine.h"
#include "file_windows.h"
#if defined(_WIN32)
/* Local Service targets only. No remote address, transfer receipt or authority
 * is constructed here. The actual native receiver must join original birth,
 * prior BQ reservation and reduced-rights native reference before observing. */
struct slcli_bq_file_observation_slot {
 struct slcli_windows_file_capture raw;
 HANDLE sender_process,sender_file,received;
 uint64_t duplicate_args[7];
 BOOL duplicate_status;DWORD duplicate_error;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_native_clock *prior_original;uint64_t prior_tick;
 const struct slcli_native_clock *origin;uint64_t origin_tick,expires;
 struct slcli_native_clock before,after,observe_before,observe_after,validation_after;
 uint64_t previous_tick;
 int before_result,after_result,observe_before_result,observe_result,observe_after_result;
 uint32_t receiver_started,receiver_failed,duplicate_called,duplicate_returned;
 uint32_t before_returned,after_returned,observe_before_returned,observe_returned,observe_after_returned;
 uint32_t observation_complete;
 int validation_result,validation_clock_result;
 uint32_t validation_returned,validation_clock_returned;
 uint32_t ordinal,id,claim_attempted,claim_returned;
 int claim_result;
};
struct slcli_bq_observation_preparation {
 struct slcli_binding_storage_original *storage;
 struct slcli_bq_file_observation_slot *slots;
 const struct slcli_native_reference *plans;
 const uint32_t *ids;
 uint32_t count,first_ordinal,next,started,failed,prepared;
};
/* Allocation-free; caller brackets scans/claims with SAME retained deadline
 * and owns serialization. Native receiver/enrollment is deliberately absent. */
int slcli_bq_observation_prepare_preowned_original(
 struct slcli_bq_observation_preparation *,struct slcli_binding_storage_original *,
 struct slcli_bq_file_observation_slot *,const struct slcli_native_reference *,
 const uint32_t *,uint32_t);
/* Actual raw cross-process duplication/returning observation only. The genuine
 * supervisor must independently join sender birth/file/prior reservation;
 * success here is not source authority or native enrollment. */
int slcli_bq_file_observe_duplicate_original(
 struct slcli_bq_observation_preparation *,uint32_t,HANDLE,HANDLE,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *);
int slcli_bq_file_observation_matches_original(
 const struct slcli_bq_observation_preparation *,uint32_t);
#endif
#endif
