#ifndef SLCLI_INITIAL_CHILD_MAPS_LINUX_H
#define SLCLI_INITIAL_CHILD_MAPS_LINUX_H
#include "child_birth_linux.h"
#if defined(__linux__)
struct slcli_initial_child_maps_linux {
 const struct slcli_initial_child_birth_linux *birth_original;
 const struct slcli_initial_child_maps_linux *prior_cut_original;
 unsigned char *bytes_original;
 struct slcli_initial_custody_call_linux *calls_original;
 uint64_t capacity,row_capacity,used,bytes_used,expires,prior_tick,native_attempts;
 char path_original[sizeof("/proc/2147483647/maps")];
 struct stat stat_original;
 struct slcli_native_clock finish;
 long descriptor_original;
 unsigned char overflow_original;
 uint32_t started,failed,completed,eof_observed;
};
/* Retained native observations only. Proc/kernel/namespace and full held-loaded
 * source/reservation/currentness qualification remains separately required. */
int slcli_initial_child_maps_linux_capture_original(struct slcli_initial_child_maps_linux *,
 const struct slcli_initial_child_birth_linux *,const struct slcli_initial_child_maps_linux *,unsigned char *,uint64_t,
 struct slcli_initial_custody_call_linux *,uint64_t);
int slcli_initial_child_maps_linux_matches_original(const struct slcli_initial_child_maps_linux *);
#endif
#endif
