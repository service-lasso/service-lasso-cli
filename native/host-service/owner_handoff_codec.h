#ifndef SLCLI_OWNER_HANDOFF_CODEC_H
#define SLCLI_OWNER_HANDOFF_CODEC_H
#include "owner_memory.h"
/* Explicit little-endian local-source transport, never native struct memcpy.
 * Header16 + owner244 + all256 original64-byte physical rows. This private
 * transport size is independent of public frame16384 and runtime row count. */
#define SLCLI_OWNER_HANDOFF_BYTES (16u+244u+256u*64u)
struct slcli_owner_handoff_decode {
  const unsigned char *raw_original;
  size_t bytes_original;
  struct slcli_memory_owner owner;
  uint32_t started,returned,qualified;
};
/* Genuine native constructor precharges and exclusively owns all these spans.
 * These pure byte methods supply no private channel/source/actor authority. */
int slcli_owner_handoff_encode(unsigned char *,size_t,
                                const struct slcli_memory_owner *);
int slcli_owner_handoff_decode(struct slcli_owner_handoff_decode *,
                                const unsigned char *,size_t);
/* Exact encoded-byte equality with the retained frozen predecessor. No large
 * scratch copy, mutation, IO, writer exclusion or source authority is implied. */
int slcli_owner_handoff_matches(const unsigned char *,size_t,
                                 const struct slcli_memory_owner *);
#endif
