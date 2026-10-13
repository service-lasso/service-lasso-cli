#ifndef SLCLI_INITIAL_HELD_FILE_LINUX_H
#define SLCLI_INITIAL_HELD_FILE_LINUX_H
#include "custody_memfd_linux.h"
#if defined(__linux__)
struct slcli_initial_held_file_linux {
 const struct slcli_initial_custody_memfd_linux *custody_original;
 const char *path_original;
 uint64_t path_bytes,prior_tick,expires;
 struct slcli_initial_custody_call_linux calls[6];
 struct stat opened_stat_original,stat_original;
 struct slcli_native_clock finish;
 long opened_descriptor_original,descriptor_original;
 uint32_t started,failed,completed;
};
/* Retains a native source observation; not source/root/reserve admission. */
int slcli_initial_held_file_linux_capture_original(
 struct slcli_initial_held_file_linux *,
 const struct slcli_initial_custody_memfd_linux *,const char *,uint64_t);
int slcli_initial_held_file_linux_matches_original(
 const struct slcli_initial_held_file_linux *);
#endif
#endif
