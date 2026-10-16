#ifndef SLCLI_UNIT19_SEAL_LINUX_H
#define SLCLI_UNIT19_SEAL_LINUX_H
#if defined(__linux__)
#include "unit19_native.h"
#include "verity_linux.h"
/* Actual preparation-to-persistent-seal join. Readonly acquisition, admitted
 * directory/route and source-owned alias classification precede this call.
 * Closing the creator is one descriptor fact, never all-alias/FREE authority. */
struct slcli_unit19_seal_linux {
  struct slcli_arena *arena;
  struct slcli_unit19_native *prepared;
  struct slcli_verity_linux *verity;
  struct slcli_linux_file_capture readonly_before,creator_close;
  struct slcli_native_clock readonly_entry,close_entry;
  int readonly_original;
  uint32_t started,failed,creator_closed,sealed;
};
struct slcli_unit19_seal_linux *slcli_unit19_seal_linux_allocate(struct slcli_arena *);
int slcli_unit19_seal_linux_original(struct slcli_unit19_seal_linux *,
  struct slcli_unit19_native *,int,uint32_t,
  const struct slcli_source_deadline_original *,const void *,
  slcli_store_capture_sink,void *);
#endif
#endif
