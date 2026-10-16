/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_BQ_RECEIPT_FOUNDATION_LINUX_H
#define SLCLI_BQ_RECEIPT_FOUNDATION_LINUX_H
#include "bq_source_readable_linux.h"
#include "input_body.h"
#include "bq_provider_expectations.h"
#if defined(__linux__)
/* Private native custody, never transport, source admission or financing. */
struct slcli_bq_retention_policy_linux_original {
 const struct slcli_input_body *installed_policy_body_original;
 const struct slcli_bq_source_namespace_linux *installed_policy_namespace_original;
 const struct slcli_bq_source_readable_linux *installed_policy_readable_original;
 uint64_t maximum_bytes,maximum_record_bytes,partial_attempts,failure_tail_bytes;
 uint64_t whole_lineage_bytes,whole_lineage_attempts,whole_lineage_nodes;
};
struct slcli_bq_retention_native_linux_original {
 long number,args[6],raw_return;uint32_t started,called,returned;
 struct slcli_native_clock before,after;
 int before_result,after_result;uint32_t before_returned,after_returned;
};
struct slcli_bq_receipt_foundation_linux_creation {
 struct slcli_arena *prebirth_arena_original;
 const struct slcli_source_deadline_original *deadline_original;
 const struct slcli_native_clock *prior_original;
 const struct slcli_bq_source_namespace_linux *parent_original;
 const struct slcli_bq_process_reference_linux_original *producer_process_original;
 const struct slcli_bq_retention_policy_linux_original *policy_original;
 struct slcli_bq_retention_policy_linux_original policy_value;
 const struct slcli_native_clock *origin_original;uint64_t origin_tick,prior_tick,expires;
 const unsigned char *name_original;size_t name_bytes;
 int writer108; /* genuine constructor sets -1 before effects, fd0 is valid */
 struct slcli_bq_retention_native_linux_original create108,allocate108;
 struct slcli_linux_file_capture parent_before,parent_after,writer_before,writer_after;
 struct slcli_native_clock observation_clocks[8];
 int observation_clock_results[8],observation_results[4];
 uint32_t observation_clock_returned[8],observation_returned[4];
 uint64_t previous_tick;
 uint32_t started,create_returned,allocation_returned,failed;
};
struct slcli_bq_receipt_index_linux_original {
 uint64_t sequence,predecessor_sequence;
 uint64_t control_offset,control_bytes,input_offset,input_bytes,output_offset,output_bytes;
 uint32_t operation,abi_version;
};
struct slcli_bq_receipt_foundation_linux_plan {
 const struct slcli_bq_receipt_foundation_linux_creation *creation_original;
 const struct slcli_bq_retention_policy_linux_original *policy_original;
 struct slcli_bq_retention_policy_linux_original policy_value;
 uint64_t sequence,records_offset,records_bytes,index_offset,index_bytes;
 uint64_t terminal_offset,terminal_bytes,attempt_capacity,node_capacity;
 uint64_t native_cell_bytes,source_window_bytes,write_window_bytes,readback_window_bytes;
 uint64_t pair_window_bytes,active_index_bytes,partial_tail_bytes;
};
struct slcli_bq_receipt_readback_linux_bind {
 struct slcli_arena *arena_original;
 const struct slcli_native_clock *origin_original;uint64_t origin_tick,prior_tick,expires;
 const struct slcli_bq_receipt_foundation_linux_creation *creation_original;
 const struct slcli_bq_source_readable_linux *readback_acquisition_original;
 const struct slcli_source_deadline_original *deadline_original;
 const struct slcli_native_clock *prior_original;
 struct slcli_linux_file_capture writer_before,readback_before,writer_after,readback_after;
 struct slcli_native_clock clocks[5];int observe_results[4],clock_results[5];
 uint32_t observe_returned[4],clock_returned[5];
 int writer_descriptor_original,readback_descriptor_original;
 uint32_t started,failed,returned;
};
/* Distinct fixed independent-entry custody, enrolled before108 birth. Neither
 * fields nor a structural result designate the entry or finance acquisition. */
struct slcli_bq_receipt_readback_linux_entry_operands {
 struct slcli_arena *arena_original;
 const struct slcli_source_deadline_original *deadline_original;
 const void *caller_original;size_t caller_bytes;
 const struct slcli_bq_receipt_foundation_linux_creation *creation_original;
 const struct slcli_bq_source_namespace_linux *proc_directory_original;
 const struct slcli_bq_process_reference_linux_original *process_original;
 const char *source_path_original;size_t source_path_bytes;
 struct slcli_bq_source_namespace_linux *source_output_original;
 char *names_output_original;
 struct slcli_bq_source_namespace_component_linux *components_output_original;size_t component_count;
 struct slcli_bq_source_readable_linux *readable_output_original;
 struct slcli_bq_receipt_readback_linux_bind *bind_output_original;
 const struct slcli_native_clock *enrollment_prior_original;
 uint64_t enrollment_prior_tick,origin_tick,expires;
};
struct slcli_bq_receipt_readback_linux_entry {
 struct slcli_bq_receipt_readback_linux_entry_operands operands,retained_operands;
 struct slcli_native_clock acquired_before;
 int acquired_before_result,source_result,readable_result,bind_result;
 uint32_t enrolled,started,source_returned,readable_returned,bind_returned,failed;
};
int slcli_bq_receipt_readback_linux_entry_enroll_original(
 struct slcli_bq_receipt_readback_linux_entry *,struct slcli_arena *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_bq_receipt_foundation_linux_creation *,
 const struct slcli_bq_source_namespace_linux *,const struct slcli_bq_process_reference_linux_original *,
 const char *,size_t,struct slcli_bq_source_namespace_linux *,char *,
 struct slcli_bq_source_namespace_component_linux *,size_t,
 struct slcli_bq_source_readable_linux *,struct slcli_bq_receipt_readback_linux_bind *,
 const struct slcli_native_clock *);
int slcli_bq_receipt_readback_linux_entry_acquire_original(struct slcli_bq_receipt_readback_linux_entry *);
int slcli_bq_receipt_readback_linux_entry_matches_original(const struct slcli_bq_receipt_readback_linux_entry *);
/* Pure actual installed-policy producer/consumer, no file effects or authority.
 * Complete source/output disjointness; refusal leaves output unchanged. */
int slcli_bq_retention_policy_linux_body_matches_original(const struct slcli_bq_retention_policy_linux_original *);
int slcli_bq_retention_policy_linux_values_shape(const struct slcli_bq_retention_policy_values *);
int slcli_bq_retention_policy_linux_encode(unsigned char *,size_t,const struct slcli_bq_retention_policy_values *);
int slcli_bq_retention_policy_linux_decode(struct slcli_bq_retention_policy_values *,const unsigned char *,size_t);
int slcli_bq_retention_policy_linux_encode_plan_original(unsigned char *,size_t,
 const struct slcli_bq_receipt_foundation_linux_plan *);
/* Parameterized native construction only. Genuine root/finance/principal/source
 * currentness precede useful invocation. Every failure retains born outputs. */
int slcli_bq_receipt_foundation_linux_create_original(
 struct slcli_bq_receipt_foundation_linux_creation *,struct slcli_arena *,
 const struct slcli_source_deadline_original *,const struct slcli_native_clock *,
 const struct slcli_bq_source_namespace_linux *,
 const struct slcli_bq_process_reference_linux_original *,
 const struct slcli_bq_retention_policy_linux_original *,const unsigned char *,size_t);
int slcli_bq_receipt_foundation_linux_creation_matches_original(
 const struct slcli_bq_receipt_foundation_linux_creation *);
int slcli_bq_receipt_readback_linux_bind_original(struct slcli_bq_receipt_readback_linux_bind *,
 const struct slcli_bq_receipt_foundation_linux_creation *,
 const struct slcli_bq_source_readable_linux *,const struct slcli_source_deadline_original *,
 const struct slcli_native_clock *);
int slcli_bq_receipt_readback_linux_bind_matches_original(
 const struct slcli_bq_receipt_readback_linux_bind *);
/* Pure preeffect plan. Native creation/readback-ready/currentness remain distinct;
 * no native constructor or positive stub is supplied by this plan source. */
int slcli_bq_receipt_foundation_linux_plan_original(
 struct slcli_bq_receipt_foundation_linux_plan *,
 const struct slcli_bq_receipt_foundation_linux_creation *,
 const struct slcli_bq_retention_policy_linux_original *);
int slcli_bq_receipt_foundation_linux_plan_matches_original(
 const struct slcli_bq_receipt_foundation_linux_plan *);
#endif
#endif
