#ifndef SLCLI_BUILD_CHILD_PRINCIPAL_WINDOWS_H
#define SLCLI_BUILD_CHILD_PRINCIPAL_WINDOWS_H
#include "command_windows.h"
#if defined(_WIN32)
enum { SLCLI_BUILD_CHILD_PROCESS_ID=0,SLCLI_BUILD_CHILD_EVENT_PROCESS_ID=1,
 SLCLI_BUILD_CHILD_THREAD_ID=2,SLCLI_BUILD_CHILD_EVENT_THREAD_ID=3,
 SLCLI_BUILD_CHILD_THREAD_PROCESS_ID=4,SLCLI_BUILD_CHILD_EVENT_THREAD_PROCESS_ID=5,
 SLCLI_BUILD_CHILD_PROCESS_TIMES=6,SLCLI_BUILD_CHILD_EVENT_PROCESS_TIMES=7,
 SLCLI_BUILD_CHILD_TOKEN=8,SLCLI_BUILD_CHILD_USER=9,SLCLI_BUILD_CHILD_STATISTICS=10,
 SLCLI_BUILD_CHILD_SID_VALID=11,SLCLI_BUILD_CHILD_SID_LENGTH=12,
 SLCLI_BUILD_CHILD_TOKEN_RECHECK=13,SLCLI_BUILD_CHILD_PRINCIPAL_ROWS=14 };
struct slcli_build_child_principal_windows {
 const struct slcli_build_windows_command *birth_original;
 struct slcli_build_windows_call calls[SLCLI_BUILD_CHILD_PRINCIPAL_ROWS];
 FILETIME creation[2],exit[2],kernel[2],user_time[2];
 HANDLE token_original;
 struct { TOKEN_USER header;unsigned char sid[SECURITY_MAX_SID_SIZE]; } user_original;
 TOKEN_STATISTICS statistics_original,recheck_original;
 DWORD user_bytes_original,statistics_bytes_original,recheck_bytes_original;
 struct slcli_native_clock finish;
 uint64_t prior_tick,expires;
 uint32_t started,failed,completed;
};
int slcli_build_child_principal_windows_capture_original(struct slcli_build_child_principal_windows *,
 const struct slcli_build_windows_command *);
int slcli_build_child_principal_windows_output_away_original(
 const struct slcli_build_child_principal_windows *,const void *,size_t);
int slcli_build_child_principal_windows_matches_original(const struct slcli_build_child_principal_windows *);
#endif
#endif
