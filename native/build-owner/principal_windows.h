#ifndef SLCLI_BUILD_PRINCIPAL_WINDOWS_H
#define SLCLI_BUILD_PRINCIPAL_WINDOWS_H
#include "../host-service/native_clock.h"
#include "../host-service/bq_provider_expectations.h"
#if defined(_WIN32)
enum { SLCLI_BUILD_SELF_PROCESS=0,SLCLI_BUILD_SELF_THREAD=1,
 SLCLI_BUILD_PROCESS_TIMES=2,SLCLI_BUILD_THREAD_TIMES=3,
 SLCLI_BUILD_THREAD_TOKEN=4,SLCLI_BUILD_PROCESS_TOKEN=5,
 SLCLI_BUILD_TOKEN_USER=6,SLCLI_BUILD_TOKEN_STATISTICS=7,
 SLCLI_BUILD_SID_VALID=8,SLCLI_BUILD_SID_LENGTH=9,
 SLCLI_BUILD_TOKEN_RECHECK=10,SLCLI_BUILD_PRINCIPAL_ROWS=11 };
struct slcli_build_windows_call {
 uintptr_t args[6],raw;
 struct slcli_native_clock before,after;
 DWORD error;
 uint32_t called,returned,error_observed;
};
struct slcli_build_principal_windows {
 const struct slcli_bq_expected_principal *expected_original;
 const struct slcli_native_clock *origin_original,*prior_original;
 uint64_t origin_tick,prior_tick,expires;
 struct slcli_build_windows_call calls[SLCLI_BUILD_PRINCIPAL_ROWS];
 DWORD process_original,thread_original;
 FILETIME process_creation,process_exit,process_kernel,process_user;
 FILETIME thread_creation,thread_exit,thread_kernel,thread_user;
 HANDLE thread_token_original,process_token_original,effective_token_original;
 struct { TOKEN_USER header; unsigned char sid[SECURITY_MAX_SID_SIZE]; } user_original;
 TOKEN_STATISTICS statistics_original,recheck_original;
 DWORD user_bytes_original,statistics_bytes_original,recheck_bytes_original;
 struct slcli_native_clock finish;
 uint32_t started,failed,completed,process_token_selected;
};
/* Actual effective-token/process/thread observations only. A matched expected
 * SID/LUID is correspondence, never a designated actor/source/reservation grant.
 * External exact launch owns all fresh storage/stack/native failure custody;
 * complete build factory must join source/loaded/prior reserve before tool use.
 * Every acquired real token handle remains held; no close is inferred here. */
int slcli_build_principal_windows_capture_original(struct slcli_build_principal_windows *,
 const struct slcli_bq_expected_principal *,const struct slcli_native_clock *,
 const struct slcli_native_clock *,uint64_t);
int slcli_build_principal_windows_matches_original(const struct slcli_build_principal_windows *);
#endif
#endif
