/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_BQ_PROCESS_REFERENCE_LINUX_H
#define SLCLI_BQ_PROCESS_REFERENCE_LINUX_H
#include "bq_channel_linux.h"
#include "bq_readiness_linux.h"
#include "../host-service/bq_subject_transfer.h"
#if defined(__linux__)
/* Parent/BQ-local originals. These addresses are NEVER transport fields.
 * Acquisition observes this process only; it does not authenticate image,
 * principal designation, initial caller, Q/P reservation or a destination. */
struct slcli_bq_process_reference_linux_original {
 struct slcli_arena *arena;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_native_clock *prior,*origin;
 uint64_t prior_tick,origin_tick,expires;
 uint64_t args[8][6];long status[8];
 struct slcli_native_clock before[8],after[8],finish;
 int before_result[8],after_result[8],finish_result;
 uint32_t before_returned[8],after_returned[8],called[8],returned[8],finish_returned;
 int pidfd;uint32_t started,next,failed,observed;
};
int slcli_bq_process_reference_linux_acquire_original(
 struct slcli_bq_process_reference_linux_original *,struct slcli_arena *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *);
int slcli_bq_process_reference_linux_matches_original(
 const struct slcli_bq_process_reference_linux_original *);

/* Exact preowned separate blocks: owner, process, send, payload64, route.
 * Raw offer uses SAME pair endpoint71, one newly acquired self process ref.
 * Native process/image/entry/UID authentication and ACK/closure remain separate.
 * A true result means the actual send returned64 only, never remote admission. */
struct slcli_bq_process_reference_linux_offer {
 struct slcli_arena *arena;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_bq_channel_linux_original *channel;
 const struct slcli_native_clock *prior_original;uint64_t prior_tick;
 struct slcli_bq_process_reference_linux_original *process;
 struct slcli_bq_send_linux_original *send;
 const struct slcli_bq_subject_route *route_original;
 struct slcli_bq_subject_route route;
 unsigned char *payload;
 struct slcli_bq_readiness_linux_original readiness;
 int readiness_result;uint32_t readiness_returned;
 int acquire_result,encode_result,send_result;
 struct slcli_native_clock finish;
 int validation_result,finish_result;
 uint32_t validation_returned,finish_returned;
 uint32_t started,failed,acquire_returned,encode_returned,send_returned,offered;
};
int slcli_bq_process_reference_linux_offer_original(
 struct slcli_bq_process_reference_linux_offer *,struct slcli_arena *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_bq_channel_linux_original *,
 struct slcli_bq_process_reference_linux_original *,struct slcli_bq_send_linux_original *,
 unsigned char *,const struct slcli_bq_subject_route *,const struct slcli_native_clock *);
int slcli_bq_process_reference_linux_offer_matches_original(
 const struct slcli_bq_process_reference_linux_offer *);
#endif
#endif
