#ifndef SLCLI_MANAGEMENT_ED25519_ORIGINAL_H
#define SLCLI_MANAGEMENT_ED25519_ORIGINAL_H
#include "../host-service/arena.h"
#include "../host-service/ha2_journal_codec.h"
/* PRIVATE management target only. The service/public consumer must not link
 * this body. Original credential native retention/admission, secret-value
 * derivation cuts and SAME BQ pending operation remain mandatory callers. */
struct slcli_management_signature {
  struct slcli_arena *arena;
  const unsigned char *credential_original,*anchor_original;
  const unsigned char *prefix_original;
  unsigned char temporary_seed[32],secret_key[64],public_key[32];
  unsigned char preimage[SLCLI_HA2_JOURNAL_PREIMAGE_BYTES],signature[64];
  size_t preimage_bytes;
  uint32_t started,key_pair_complete,signed_complete,failed;
};
struct slcli_management_signature *slcli_management_signature_allocate(struct slcli_arena *);
/* One-use source-owned raw credential borrow, exact independent public anchor,
 * and before-effect first708 bytes. Matching an anchor/signature grants no
 * realm authority, native source, enrollment or journal-pair publication. */
int slcli_management_signature_original(struct slcli_management_signature *,
  const unsigned char[32],const unsigned char[32],const unsigned char *,size_t);
#endif
