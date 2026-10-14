#ifndef SLCLI_INITIAL_CUSTODY_MEMFD_LINUX_H
#define SLCLI_INITIAL_CUSTODY_MEMFD_LINUX_H
#include "launcher_linux.h"
#if defined(__linux__)
#include <asm/stat.h>
struct slcli_initial_custody_call_linux {
 long number,args[6],raw;
 struct slcli_native_clock before,after;
 uint32_t called,returned;
};
struct slcli_initial_custody_memfd_linux {
 const struct slcli_initial_launcher_linux *launcher_original;
 const unsigned char *ledger_original;
 unsigned char *readback_original;
 struct slcli_initial_custody_call_linux *calls_original;
 uint64_t ledger_bytes,capacity,used,expires,prior_tick;
 long descriptor_original;
 struct stat stat_original;
 unsigned char eof_original;
 struct slcli_native_clock finish;
 uint32_t started,failed,completed;
};
/* Retained storage construction, never source/root/principal/reserve authority.
 * The external launch owns fresh output/stack and every failure/pending cost. */
int slcli_initial_custody_memfd_linux_capture_original(
 struct slcli_initial_custody_memfd_linux *,
 const struct slcli_initial_launcher_linux *,const unsigned char *,uint64_t,
 unsigned char *,struct slcli_initial_custody_call_linux *,uint64_t);
int slcli_initial_custody_memfd_linux_matches_original(
 const struct slcli_initial_custody_memfd_linux *);
#endif
#endif
