/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_BQ_BOOTSTRAP_STORAGE_LINUX_H
#define SLCLI_BQ_BOOTSTRAP_STORAGE_LINUX_H
#include "bq_bootstrap_ack_linux.h"
#include "bq_process_reference_linux.h"
#include "../host-service/bq_receipt_foundation_linux.h"
#if defined(__linux__)
struct slcli_bq_provider_linux_birth_original;
struct slcli_bq_bootstrap_linux_original;
/* Source-local prebirth output geometry. No future fd/result and no financing,
 * native principal, currentness, archive authority or transport pointer. */
struct slcli_bq_bootstrap_storage_linux_plan {
 struct slcli_arena *arena_original;
 struct slcli_memory_owner *owner_original;
 uint32_t extent_index_original;uint64_t acquisition_original;
 const void *caller_original;size_t caller_bytes;
 const struct slcli_source_deadline_original *deadline_original;
 const struct slcli_native_clock *origin_original;
 const struct slcli_bq_provider_roster *roster_original;
 struct slcli_bq_process_reference_linux_offer *process_offer;
 struct slcli_bq_process_reference_linux_original *process;
 struct slcli_bq_send_linux_original *process_send;
 unsigned char *process_payload;
 struct slcli_bq_subject_route *process_route;
 struct slcli_bq_receipt_foundation_linux_creation *creation;
 struct slcli_bq_receipt_readback_linux_bind *bind;
 struct slcli_bq_receipt_readback_linux_entry *readback_entry;
 const char *readback_source_path;size_t readback_source_path_bytes;
 struct slcli_bq_source_namespace_linux *readback_source;
 char *readback_names;
 struct slcli_bq_source_namespace_component_linux *readback_components;size_t readback_component_count;
 struct slcli_bq_source_readable_linux *readback;
 const struct slcli_bq_source_namespace_linux *foundation_parent_original,*readback_proc_original;
 const struct slcli_bq_retention_policy_linux_original *policy_original;
 const unsigned char *foundation_name_original;size_t foundation_name_bytes;
 const struct slcli_bq_provider_expectations *expectations_original;
 struct slcli_bq_readiness_linux_original *readiness[2];
 struct slcli_bq_send_linux_original *send[2];
 unsigned char *payload[2];struct slcli_bq_subject_route *route[2];
 struct slcli_bq_bootstrap_ack_linux_original *ack[3];
 struct slcli_bq_bootstrap_linux_original *composer;
};
struct slcli_bq_bootstrap_linux_original {
 struct slcli_bq_provider_linux_birth_original *birth_original;
 const struct slcli_bq_bootstrap_storage_linux_plan *plan_original;
 struct slcli_bq_bootstrap_storage_linux_plan plan_snapshot;
 const struct slcli_bq_provider_expectations *expectations_original;
 const struct slcli_native_clock *prior_original;
 uint64_t prior_tick;
 int route_results[2],readiness_results[2],send_results[2],ack_results[3];
 int enrollment_result,creation_result,acquisition_result,encode_results[2];
 uint32_t enrollment_returned,creation_returned,acquisition_returned,encode_returned[2];
 uint32_t route_returned[2],readiness_returned[2],send_returned[2],ack_returned[3];
 struct slcli_native_clock validation_after;
 int validation_result;uint32_t validation_returned;
 uint32_t started,next,failed,complete;
};
/* Fresh is only for actual prebirth; returning shape does not demand zero rows.
 * Neither pure checker grants reservation or source admission. */
int slcli_bq_bootstrap_storage_linux_fresh_original(
 const struct slcli_bq_bootstrap_storage_linux_plan *);
int slcli_bq_bootstrap_storage_linux_same_original(
 const struct slcli_bq_bootstrap_storage_linux_plan *,const struct slcli_bq_bootstrap_storage_linux_plan *);
int slcli_bq_bootstrap_storage_linux_away_original(
 const struct slcli_bq_bootstrap_storage_linux_plan *,const void *,size_t);
int slcli_bq_bootstrap_storage_linux_matches_original(
 const struct slcli_bq_bootstrap_storage_linux_plan *);
/* Raw fixed source-custody composition only. Genuine initial installed entry,
 * principal/root/reserve/currentness must precede invocation. No product entry
 * or positive qualification binding is supplied by this capture function. */
int slcli_bq_bootstrap_linux_capture_raw_original(struct slcli_bq_provider_linux_birth_original *);
int slcli_bq_bootstrap_linux_capture_matches_original(const struct slcli_bq_bootstrap_linux_original *);
#endif
#endif
