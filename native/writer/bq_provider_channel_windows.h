/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_BQ_PROVIDER_CHANNEL_WINDOWS_H
#define SLCLI_BQ_PROVIDER_CHANNEL_WINDOWS_H
#include "../host-service/bootstrap_qualification_admission.h"
#include "../host-service/bq_provider_expectations.h"
#if defined(_WIN32)
/* Native custody only. All aliases, including the parent's client and child72
 * duplicate, remain live/charged; observed does not authenticate a child or
 * finance any prerequisite. No close/reset/retry is performed here. */
struct slcli_bq_provider_channel_windows {
 struct slcli_arena *arena;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_native_clock *prior,*origin;
 const struct slcli_bq_windows_identity_original *identity;
 const struct slcli_bq_expected_principal *principal;
 const WCHAR *name;size_t name_bytes;
 uint64_t prior_tick,origin_tick,expires,previous_tick;
 union { SECURITY_DESCRIPTOR_RELATIVE alignment;unsigned char bytes[256]; } security;
 size_t security_bytes;
 SECURITY_ATTRIBUTES attributes;
 HANDLE server,client,child_endpoint,event;
 OVERLAPPED connect;
 DWORD connect_count,mode,handle_flags[4];
 uintptr_t args[11][8];
 uint64_t status[11];DWORD error[11];
 uint32_t called[11],returned[11];
 struct slcli_native_clock before[11],after[11],finish;
 int before_result[11],after_result[11],finish_result;
 uint32_t before_returned[11],after_returned[11],finish_returned;
 uint32_t started,first_failure,connect_pending,connect_complete,observed;
};
int slcli_bq_provider_channel_windows_original(
 struct slcli_bq_provider_channel_windows *,struct slcli_arena *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,const struct slcli_bq_windows_identity_original *,
 const struct slcli_bq_expected_principal *,const WCHAR *,size_t);
int slcli_bq_provider_channel_windows_matches_original(
 const struct slcli_bq_provider_channel_windows *);
#endif
#endif
