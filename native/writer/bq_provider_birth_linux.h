/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_BQ_PROVIDER_BIRTH_LINUX_H
#define SLCLI_BQ_PROVIDER_BIRTH_LINUX_H
#include "bq_channel_linux.h"
#include "../host-service/platform_memory.h"
#include "../host-service/bq_provider_entry.h"
#include "../host-service/bq_subject_transfer.h"
#include "bq_process_reference_linux.h"
#include "bq_bootstrap_storage_linux.h"
#if defined(__linux__)
#include <stdatomic.h>
#include <linux/sched.h>
#include <linux/time_types.h>
enum { SLCLI_BQ_PROVIDER_KEEP=1,SLCLI_BQ_PROVIDER_EXEC_ONLY=2 };
struct slcli_bq_provider_inherited_fd {int fd;uint32_t action;};
struct slcli_bq_provider_native_row {
 long number,args[6],status;
 struct slcli_native_clock before,after;
 int before_result,after_result;
 uint32_t started,called,returned,before_returned,after_returned;
};
struct slcli_bq_provider_fd_row {
 struct slcli_bq_provider_inherited_fd original;
 struct slcli_bq_provider_native_row flags,set;
};
/* All child writes are in a genuinely preowned shared mapped extent; parent
 * control and original input pointers remain local COW inputs, never a wire ABI.
 * birth_publication releases the six immutable birth rows; publication2 releases
 * pre-exec rows and publication3 adds failure/exit intent before exit_group;
 * only publication4 records an unexpected return from that exit call. */
struct slcli_bq_provider_linux_child {
 atomic_uint birth_publication,publication,gate;
 long clone_status;uint32_t clone_returned;
 struct slcli_bq_provider_native_row birth[6],wait,directory,exec,exit;
 struct slcli_bq_provider_fd_row *fds;
 struct slcli_bq_provider_native_row *gaps;
 size_t fd_count,gap_count;
 struct __kernel_timespec timeout;
 uint64_t previous_tick;
 uint32_t started,failed;
};
struct slcli_bq_provider_linux_birth_inputs {
 const struct slcli_bq_channel_linux_original *channel;
 const struct slcli_bq_provider_inherited_fd *allowlist;size_t allowlist_count;
 int executable,directory;
 const char *const *argv,*const *environment;size_t argc,environment_count;
 char *endpoint_text; /* exact separate preowned11B block, initially zero */
 char *count_text; /* exact separate preowned21B block, initially zero */
 const struct slcli_bq_provider_roster *roster;
 const struct slcli_bq_bootstrap_storage_linux_plan *bootstrap_storage;
 struct slcli_arena *shared_arena;
 const struct slcli_memory_capture *shared_original;
 struct slcli_bq_provider_linux_child *child;
 struct slcli_bq_provider_fd_row *fds;
 struct slcli_bq_provider_native_row *gaps;
};
struct slcli_bq_provider_linux_birth_original {
 struct slcli_arena *arena;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_bq_provider_linux_birth_inputs *inputs;
 struct slcli_bq_provider_linux_birth_inputs original;
 struct slcli_bq_provider_roster roster_original;
 struct slcli_bq_bootstrap_storage_linux_plan bootstrap_storage_original;
 const struct slcli_native_clock *prior,*origin;
 const struct slcli_native_clock *release_prior;
 uint64_t release_prior_tick;
 uint64_t prior_tick,origin_tick,expires;
 struct clone_args clone;
 struct slcli_native_clock before,after,release_before,release_after;
 int before_result,after_result,release_before_result,release_after_result;
 long clone_args[6],clone_status,wake_args[6],wake_status;
 int pidfd;size_t endpoint_bytes,count_bytes,offer_count;
 uint32_t started,possible_child_effects,clone_called,clone_returned,born;
 uint32_t before_returned,after_returned,release_started,wake_called,wake_returned;
 uint32_t release_before_returned,release_after_returned,gate_possible,released,failed;
 struct slcli_bq_process_reference_linux_offer *first_offer;
 struct slcli_bq_subject_route *first_route;
 struct slcli_native_clock first_offer_prior;
 int first_offer_clock_result,first_offer_result,first_release_result;
 uint32_t first_offer_started,first_offer_clock_returned,first_offer_returned,first_release_returned;
};
/* Allocation-free native qualification-tool birth, not product Node launch.
 * Independent authentic installer/source/Q/P/finite budget must precede calls.
 * Inputs own a genuinely shared live arena and ALL fixed child rows before
 * clone. Parent retains pidfd/shared failure prefix; no close/kill/retry here.
 * True means raw child birth only, not UID/image/entry/source admission. */
int slcli_bq_provider_linux_birth_preowned_original(
 struct slcli_bq_provider_linux_birth_original *,struct slcli_arena *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,const struct slcli_bq_provider_linux_birth_inputs *);
int slcli_bq_provider_linux_release_original(struct slcli_bq_provider_linux_birth_original *,
 const struct slcli_native_clock *);
int slcli_bq_provider_linux_birth_matches_original(const struct slcli_bq_provider_linux_birth_original *);
int slcli_bq_provider_linux_release_matches_original(const struct slcli_bq_provider_linux_birth_original *);
int slcli_bq_provider_linux_bootstrap_borrowed_away_original(const struct slcli_bq_provider_linux_birth_original *);
int slcli_bq_provider_linux_child_birth_matches_original(const struct slcli_bq_provider_linux_birth_original *);
/* Raw composing caller: queue exact roster0 self reference before gate release.
 * Independent authentic supervisor admission remains required before invocation;
 * successful queue publication/release cannot create it. No retry/cleanup here. */
int slcli_bq_provider_linux_offer_then_release_original(
 struct slcli_bq_provider_linux_birth_original *,struct slcli_bq_process_reference_linux_offer *,
 struct slcli_bq_process_reference_linux_original *,struct slcli_bq_send_linux_original *,
 unsigned char *,struct slcli_bq_subject_route *);
#endif
#endif
