/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_BQ_BOOTSTRAP_ACK_LINUX_H
#define SLCLI_BQ_BOOTSTRAP_ACK_LINUX_H
#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include "bq_channel_linux.h"
#include "bq_readiness_linux.h"
#include "../host-service/bq_subject_transfer.h"
#if defined(__linux__)
/* Producer-local native originals. Structural ACK correspondence is neither
 * destination archive authority nor retirement/last-borrower settlement. */
enum { SLCLI_BQ_BOOTSTRAP_ACK_CONTROL_BYTES=CMSG_SPACE(sizeof(struct ucred))+CMSG_SPACE(sizeof(int)),
 SLCLI_BQ_BOOTSTRAP_ACK_FDS=(SLCLI_BQ_BOOTSTRAP_ACK_CONTROL_BYTES-CMSG_LEN(0))/sizeof(int) };
struct slcli_bq_bootstrap_ack_linux_original {
 struct slcli_arena *arena;
 const struct slcli_bq_channel_linux_original *channel_original;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_bq_subject_route *offer_original;
 struct slcli_bq_subject_route offer_snapshot,decoded;
 const struct slcli_native_clock *prior,*origin;
 uint64_t prior_tick,origin_tick,expires;
 struct slcli_bq_readiness_linux_original readiness;
 int readiness_result;uint32_t readiness_returned;
 struct iovec vector;struct msghdr message;
 unsigned char payload[SLCLI_BQ_SUBJECT_RECORD_BYTES];
 union {struct cmsghdr alignment;unsigned char bytes[SLCLI_BQ_BOOTSTRAP_ACK_CONTROL_BYTES];} control;
 int received_fds[SLCLI_BQ_BOOTSTRAP_ACK_FDS];
 uint32_t fd_count,credential_count,rights_count,ancillary_scanned;
 struct ucred credentials;
 long args[6],raw_return;
 struct slcli_native_clock before,after,validation_after;
 int before_result,after_result,validation_result,ancillary_result,decode_result;
 uint32_t before_returned,after_returned,validation_returned,decode_returned;
 uint32_t started,called,returned,failed,pending_borrow,complete;
};
/* Exact fixed bootstrap ordinal1..3 only, no allocation/retry or generic
 * receive authority. All malformed installed descriptor custody is retained. */
int slcli_bq_bootstrap_ack_linux_original(
 struct slcli_bq_bootstrap_ack_linux_original *,struct slcli_arena *,
 const struct slcli_bq_channel_linux_original *,const struct slcli_bq_subject_route *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *);
int slcli_bq_bootstrap_ack_linux_matches_original(
 const struct slcli_bq_bootstrap_ack_linux_original *);
#endif
#endif