#ifndef SLCLI_INITIAL_LAUNCHER_LINUX_H
#define SLCLI_INITIAL_LAUNCHER_LINUX_H
#include "preclaim_linux.h"
/* External exact launch owns complete fresh storage and native startup costs.
 * None of these current-process observations designate an admitted principal. */
enum { SLCLI_INITIAL_IDENTITY_CALLS=8,
 SLCLI_INITIAL_PREBIRTH_NATIVE_ATTEMPTS=SLCLI_INITIAL_IDENTITY_CALLS+
 2*SLCLI_INITIAL_IDENTITY_CALLS+1+4+1 };
struct slcli_initial_launcher_identity_linux {
 const struct slcli_native_clock *origin_original,*prior_original;
 uint64_t origin_tick,prior_tick,expires;
 long number[SLCLI_INITIAL_IDENTITY_CALLS];
 long args[SLCLI_INITIAL_IDENTITY_CALLS][6],raw[SLCLI_INITIAL_IDENTITY_CALLS];
 struct slcli_native_clock before[SLCLI_INITIAL_IDENTITY_CALLS],after[SLCLI_INITIAL_IDENTITY_CALLS],finish;
 uint32_t called[SLCLI_INITIAL_IDENTITY_CALLS],returned[SLCLI_INITIAL_IDENTITY_CALLS],started,completed,failed;
};
struct slcli_initial_launcher_linux {
 struct slcli_initial_preclaim_plan_linux plan;
 struct slcli_initial_launcher_identity_linux identity;
 struct slcli_initial_preclaim_linux preclaim;
 struct slcli_native_clock finish;
 uint32_t started,identity_returned,preclaim_returned,failed;
 int identity_result,preclaim_result;
};
int slcli_initial_launcher_linux_capture_prebirth_original(
 struct slcli_initial_launcher_linux *,
 const struct slcli_initial_preclaim_request *,uint64_t,uint64_t,uint64_t,
 const struct slcli_native_clock *,const struct slcli_native_clock *,uint64_t);
int slcli_initial_launcher_linux_prebirth_matches_original(
 const struct slcli_initial_launcher_linux *);
/* No child birth/resume, admission constructor, teardown or release here. */
#endif
