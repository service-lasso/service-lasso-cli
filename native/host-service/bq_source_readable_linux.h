/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_BQ_SOURCE_READABLE_LINUX_H
#define SLCLI_BQ_SOURCE_READABLE_LINUX_H
#include "bq_source_namespace_linux.h"
#include "../writer/bq_process_reference_linux.h"
#if defined(__linux__)
#include <asm/statfs.h>
/* Source-local native observations only. No namespace/provider/principal or
 * own-writer admission is conferred by procfs magic or matching metadata.
 * Both namespace rosters and process original remain borrowed and live; no
 * participant may close/reuse the O_PATH descriptor during this occurrence.
 * Native pending-call supervision/selected SDK/kernel eligibility is separate.
 * Failure retains the actual returned descriptor, even after a failed clock. */
struct slcli_bq_source_readable_linux {
 struct slcli_arena *arena;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_bq_source_namespace_linux *source,*proc_directory;
 const struct slcli_bq_process_reference_linux_original *process;
 const struct slcli_native_clock *prior,*origin;
 uint64_t prior_tick,origin_tick,expires,previous_tick;
 char proc_path[32],descriptor_name[11];
 long fs_args[6],fs_status,open_args[6],open_status;
 struct statfs filesystem;
 struct slcli_linux_file_capture source_before,readable,source_after;
 struct slcli_native_clock before[5],after[5],finish;
 int before_result[5],after_result[5],finish_result,observe_result[3];
 uint32_t before_returned[5],after_returned[5],called[5],returned[5],finish_returned;
 uint32_t started,next,failed,observed;
 int descriptor;
};
int slcli_bq_source_readable_linux_acquire_original(struct slcli_bq_source_readable_linux *,
 struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_bq_source_namespace_linux *,const struct slcli_bq_source_namespace_linux *,
 const struct slcli_bq_process_reference_linux_original *,const struct slcli_native_clock *);
int slcli_bq_source_readable_linux_matches_original(const struct slcli_bq_source_readable_linux *);
#endif
#endif
