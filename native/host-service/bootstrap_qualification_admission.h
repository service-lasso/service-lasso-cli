#ifndef SLCLI_BOOTSTRAP_QUALIFICATION_ADMISSION_H
#define SLCLI_BOOTSTRAP_QUALIFICATION_ADMISSION_H
#include "source_deadline.h"
#if defined(_WIN32)
#include <windows.h>
/* Native identity custody inside the private BQ entry, NOT designation or
 * admission. No public/serialized capability constructor is supplied. */
struct slcli_bq_windows_identity_original {
 struct slcli_arena *arena;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_native_clock *origin_original;
 uint64_t origin_tick,expires,previous_tick;
 HANDLE process_pseudo,thread_pseudo,thread_token,process_token,selected_token;
 DWORD process_id,thread_id;
 uint32_t pseudo_called[4],pseudo_returned[4];
 union {TOKEN_USER alignment;unsigned char bytes[sizeof(TOKEN_USER)+SECURITY_MAX_SID_SIZE];} user;
 TOKEN_STATISTICS statistics;
 DWORD user_bytes,statistics_bytes;
 FILETIME creation,exit,kernel,user_time;
 uintptr_t args[7][5];
 BOOL status[7];DWORD errors[7];
 struct slcli_native_clock clocks[8][2],finish;
 int clock_results[8][2],finish_result;
 uint32_t clock_returned[8][2],called[7],returned[7],error_observed[7],finish_returned;
 size_t sid_offset,sid_bytes;
 uint32_t started,observed,first_failure;
};
/* Same charged one-use original; failure once-closes actual token outputs
 * independently of useful clocks. Success keeps the token held/charged for
 * the still-missing admitted entry/last borrower. No retirement API supplied.
 * A true result means identity observation only, never BQ/root. */
int slcli_bootstrap_qualification_windows_identity_original(
 struct slcli_bq_windows_identity_original *,struct slcli_arena *,
 const struct slcli_source_deadline_original *,const void *,size_t);
/* Raw returning observation, also used before constructor publication.
 * Does not grant principal designation, admission, or token retirement. */
int slcli_bootstrap_qualification_windows_identity_matches_original(
 const struct slcli_bq_windows_identity_original *);
#endif
#endif
