#ifndef SLCLI_INITIAL_CHILD_BIRTH_LINUX_H
#define SLCLI_INITIAL_CHILD_BIRTH_LINUX_H
#include "child_control_linux.h"
#if defined(__linux__)
#include <linux/sched.h>
#include <signal.h>
enum { SLCLI_INITIAL_WAIT_NO_EVENT=1,SLCLI_INITIAL_WAIT_STOP=2,
 SLCLI_INITIAL_WAIT_EXEC=3,SLCLI_INITIAL_WAIT_EXIT=4,SLCLI_INITIAL_WAIT_UNEXPECTED=5 };
enum { SLCLI_INITIAL_SLOT3_ABSENT=1,SLCLI_INITIAL_SLOT3_CUSTODY=2,
 SLCLI_INITIAL_SLOT3_SELF_PIDFD=3,SLCLI_INITIAL_SLOT3_OPENED_FILE=4 };
struct slcli_initial_child_wait_linux {
 struct slcli_initial_custody_call_linux call;
 siginfo_t info;
 uint32_t classification;
};
struct slcli_initial_child_wait_stage_linux {
 struct slcli_initial_child_wait_linux *rows_original;
 uint64_t capacity,used;
 struct slcli_native_clock finish;
 uint32_t started,completed,pending,failed;
};
/* Actual birth/ptrace producer must retain these native originals. This type
 * alone cannot authenticate a clone, issue finance or designate a principal. */
struct slcli_initial_child_birth_linux {
 const struct slcli_initial_child_control_map_linux *map_original;
 struct clone_args clone_input;
 struct slcli_initial_custody_call_linux slot3_call,clone_call,options_call,continue_call;
 uint32_t slot3_started,slot3_completed,slot3_failed,slot3_pending,slot3_kind;
 int pidfd_original;
 uint64_t wait_row_bytes,wait_native_attempts,parent_native_attempts;
 uint32_t parent_started,parent_completed,parent_failed,parent_pending;
 struct slcli_initial_child_wait_stage_linux stages[2];
};
/* Fresh prebirth enrollment; native/pure/source finance remains external. */
int slcli_initial_child_birth_linux_prepare_wait_roster_original(
 struct slcli_initial_child_birth_linux *,const struct slcli_initial_child_control_map_linux *,
 struct slcli_initial_child_wait_linux *,uint64_t,struct slcli_initial_child_wait_linux *,uint64_t);
/* Actual prebirth observation only; no foreign descriptor close or admission. */
int slcli_initial_child_birth_linux_probe_slot3_original(struct slcli_initial_child_birth_linux *);
int slcli_initial_child_birth_linux_slot3_matches_original(const struct slcli_initial_child_birth_linux *);
/* Finite direct wait observations of this SAME birth; no clone/CONT/cleanup. */
int slcli_initial_child_birth_linux_wait_stage_original(
 struct slcli_initial_child_birth_linux *,uint32_t);
int slcli_initial_child_birth_linux_wait_stage_matches_original(
 const struct slcli_initial_child_birth_linux *,uint32_t);
/* Actual fixed own-child stop/options/CONT/exec observer; no clone or admission. */
int slcli_initial_child_birth_linux_parent_observe_original(struct slcli_initial_child_birth_linux *);
int slcli_initial_child_birth_linux_parent_matches_original(const struct slcli_initial_child_birth_linux *);
#endif
#endif
