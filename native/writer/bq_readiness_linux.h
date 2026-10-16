/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_BQ_READINESS_LINUX_H
#define SLCLI_BQ_READINESS_LINUX_H
#include "../host-service/source_deadline.h"
#if defined(__linux__)
#include <linux/poll.h>
#include <linux/time_types.h>
struct slcli_bq_readiness_linux_original {
 struct slcli_arena *arena;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_native_clock *prior,*origin;
 uint64_t prior_tick,origin_tick,expires;
 int endpoint;short requested_events;struct pollfd descriptor;
 struct __kernel_timespec timeout_input,timeout;long args[6],raw_return;
 struct slcli_native_clock before,after,validation_after;
 int before_result,after_result,validation_clock_result;
 uint32_t started,called,returned,before_returned,after_returned;
 uint32_t validation_returned,complete,failed;
};
int slcli_bq_readiness_linux_preowned_original(struct slcli_bq_readiness_linux_original *,
 struct slcli_arena *,int,short,const struct slcli_source_deadline_original *,
 const void *,size_t,const struct slcli_native_clock *);
int slcli_bq_readiness_linux_matches_original(const struct slcli_bq_readiness_linux_original *);
#endif
#endif
