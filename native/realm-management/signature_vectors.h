#ifndef SLCLI_MANAGEMENT_SIGNATURE_VECTORS_H
#define SLCLI_MANAGEMENT_SIGNATURE_VECTORS_H
#include "ed25519_original.h"
/* Actual independently admitted original credential/anchor/prefix storage is
 * supplied by the PRIVATE native fixture owner; never synthetic enrollment or
 * a caller-selected public signing route. All originals/work remain retained. */
int slcli_management_signature_positive_vector(struct slcli_management_signature *,
  const unsigned char[32],const unsigned char[32],const unsigned char *,size_t);
struct slcli_management_record_vector {
  struct slcli_management_signature *signer;
  unsigned char prefix[SLCLI_HA2_JOURNAL_SIGNED_BYTES];
  unsigned char raw[SLCLI_HA2_JOURNAL_BYTES];
  uint32_t started,complete,failed;
};
struct slcli_management_record_vector *slcli_management_record_vector_allocate(struct slcli_arena *);
/* Full representation/signature positive caller, retaining SAME charged new
 * prefix and record. It is not journal IO, C22 qualification or native adoption. */
int slcli_management_record_positive_vector(struct slcli_management_record_vector *,
  const struct slcli_ha2_journal_prefix_originals *,const unsigned char[32],const unsigned char[32]);
#endif
