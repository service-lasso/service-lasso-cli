/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_BQ_SOURCE_BODY_LINUX_H
#define SLCLI_BQ_SOURCE_BODY_LINUX_H
#include "bq_source_readable_linux.h"
#include "crypto.h"
#if defined(__linux__)
struct slcli_bq_source_body_linux_row {
 struct slcli_linux_file_capture native;
 struct slcli_native_clock before,after;
 size_t offset,requested;
 uint32_t pass,kind,attempted,called,returned,before_returned,after_returned;
 int result,before_result,after_result;
};
/* BQ-local acquired byte observations, not source/member/provider admission.
 * Each output block owns expected+1 bytes: the last byte is its distinct EOF
 * output lane, never a payload byte. All rows/partial bytes remain borrowed
 * through the fixed installation verifier/archive/useful last borrower. */
struct slcli_bq_source_body_linux {
 struct slcli_arena *arena;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_bq_source_readable_linux *source;
 const struct slcli_native_clock *prior,*origin;
 uint64_t prior_tick,origin_tick,expires,previous_tick;
 const unsigned char *expected_digest_original;
 unsigned char *body,*readback,digest[2][32];
 size_t expected,capacity,attempted,pass_first[2],pass_end[2],offset[2];
 struct slcli_bq_source_body_linux_row *rows;
 struct slcli_sha256 hashes[2];
 int hash_update_result[2],hash_finish_result[2];
 struct slcli_native_clock finish;int finish_result;
 uint32_t started,failed,observed,finish_returned,pass_complete[2];
};
int slcli_bq_source_body_linux_acquire_original(struct slcli_bq_source_body_linux *,
 struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_bq_source_readable_linux *,const struct slcli_native_clock *,
 unsigned char *,unsigned char *,size_t,const unsigned char[32],
 struct slcli_bq_source_body_linux_row *,size_t);
int slcli_bq_source_body_linux_matches_original(const struct slcli_bq_source_body_linux *);
#endif
#endif
