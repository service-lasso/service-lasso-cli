#ifndef SLCLI_INITIAL_CHILD_CONTROL_LINUX_H
#define SLCLI_INITIAL_CHILD_CONTROL_LINUX_H
#include "held_file_linux.h"
#if defined(__linux__)
struct slcli_initial_child_string_linux { const char *original;uint64_t bytes; };
struct slcli_initial_child_input_linux {
 const char *const *argv_original,*const *env_original;
 const struct slcli_initial_child_string_linux *argv_spans,*env_spans;
 uint64_t argc,envc;
};
enum { SLCLI_INITIAL_CHILD_TRACEME=0,SLCLI_INITIAL_CHILD_GETPID=1,
 SLCLI_INITIAL_CHILD_GETTID=2,SLCLI_INITIAL_CHILD_STOP=3,
 SLCLI_INITIAL_CHILD_GETFD3=4,SLCLI_INITIAL_CHILD_CLOSE3=5,
 SLCLI_INITIAL_CHILD_DUP3=6,SLCLI_INITIAL_CHILD_SETFD3=7,
 SLCLI_INITIAL_CHILD_EXEC=8,SLCLI_INITIAL_CHILD_EXIT=9,
 SLCLI_INITIAL_CHILD_CALLS=10 };
struct slcli_initial_child_control_linux {
 struct slcli_initial_custody_call_linux child_calls[SLCLI_INITIAL_CHILD_CALLS];
 long clone_child_raw;
 uint32_t clone_child_returned,failed,exit_called,exit_returned,trap_attempted;
};
struct slcli_initial_child_control_map_linux {
 const struct slcli_initial_held_file_linux *executable_original;
 const struct slcli_initial_child_input_linux *input_original;
 struct slcli_initial_custody_call_linux mapping;
 struct slcli_native_clock finish;
 struct slcli_initial_child_control_linux *control_original;
 uint64_t page_bytes,mapping_bytes,prior_tick,expires;
 uint32_t started,failed,prepared;
};
/* Native shared storage only; no clone, ptrace, resume or admission. */
int slcli_initial_child_control_map_linux_capture_original(
 struct slcli_initial_child_control_map_linux *,const struct slcli_initial_held_file_linux *,
 const struct slcli_initial_child_input_linux *,uint64_t);
int slcli_initial_child_control_map_linux_matches_original(
 const struct slcli_initial_child_control_map_linux *);
/* Pure exclusion against the actual retained mapping and its borrowed graph. */
int slcli_initial_child_control_map_linux_output_away_original(
 const struct slcli_initial_child_control_map_linux *,const void *,uint64_t);
#endif
#endif
