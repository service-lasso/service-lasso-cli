#ifndef SLCLI_MANAGEMENT_ENROLLMENT_SIGNATURE_H
#define SLCLI_MANAGEMENT_ENROLLMENT_SIGNATURE_H
#include "../host-service/arena.h"
#include "../host-service/ha2_enrollment_codec.h"
/* PRIVATE management target only. Service/public consumer must not link this
 * body or receive the realm seed. Original native custody/BQ authority and
 * full secret-value/source/stack/envelope qualification remain prerequisites. */
struct slcli_management_enrollment_signature {
  struct slcli_arena *arena;
  const unsigned char *credential_original,*anchor_original,*prefix_original;
  unsigned char temporary_seed[32],secret_key[64],public_key[32],signature[64];
  unsigned char preimage[SLCLI_HA2_ENROLLMENT_PREIMAGE_LIMIT];
  size_t preimage_bytes;
  uint32_t started,failed,key_pair_complete,signed_complete;
};
struct slcli_management_enrollment_signature *slcli_management_enrollment_signature_allocate(struct slcli_arena *);
int slcli_management_enrollment_signature_original(struct slcli_management_enrollment_signature *,
  const unsigned char[32],const unsigned char[32],const unsigned char *,size_t);
#endif
