#ifndef SLCLI_VERITY_LINUX_H
#define SLCLI_VERITY_LINUX_H
#if defined(__linux__)
#include "store_body.h"
#include "source_deadline.h"
/* Storage and raw SAME-inode persistent content protection only. Every row
 * and failed native effect stays in the original charged owner. This does not
 * authenticate a provider, raw file digest, source grant or installed birth.
 * Namespace/metadata remain mutable despite verity; readonly is not a seal. */
struct slcli_verity_linux {
  struct slcli_arena *arena;
  struct slcli_linux_file_capture rows[6];
  struct slcli_native_clock entry[6];
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_linux_file_capture *expected;
  const struct slcli_linux_file_capture *predecessor;
  const void *caller_original;
  const struct slcli_native_clock *original_origin;
  slcli_store_capture_sink sink;
  void *capture_original;
  uint64_t expires,origin_tick,previous_tick,bytes;
  int original;
  uint32_t started,failed,row_count,seal_applied,verified;
};
struct slcli_verity_linux *slcli_verity_linux_allocate(struct slcli_arena *);
/* Mode0 verifies an already sealed SAME held readonly original; mode1 enables
 * once then verifies. block_size is required only in mode1 and must originate
 * in independently qualified actual page/filesystem geometry. Kernel enable
 * latency is not a finite userspace completion guarantee. Later expiry retains
 * applied sealing and all captures; it never reports successful verification. */
int slcli_verity_linux_original(struct slcli_verity_linux *,int,uint32_t,uint32_t,
  const struct slcli_linux_file_capture *,const struct slcli_linux_file_capture *,uint64_t,
  const struct slcli_source_deadline_original *,const void *,
  slcli_store_capture_sink,void *);
#endif
#endif
