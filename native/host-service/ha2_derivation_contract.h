#ifndef SLCLI_HA2_DERIVATION_CONTRACT_H
#define SLCLI_HA2_DERIVATION_CONTRACT_H
#include "json.h"
#include "unit19.h"
/* Exact borrowed representation, never a seal/native source/credential/caller
 * capability. Decoded references require independent whole original custody,
 * native range/EOF/rights/birth/source/Git joins before any derivation effect. */
struct slcli_ha2_ref {
  uint32_t slot;
  uint64_t offset,bytes;
  unsigned char association[32],capture[32],digest[32],source[32],caller[32],birth[32];
};
enum slcli_ha2_contract_ref {
  SLCLI_H2_BQ=0,SLCLI_H2_SERVICE,SLCLI_H2_SOURCE,SLCLI_H2_ENTRY,SLCLI_H2_IMAGE,
  SLCLI_H2_COMPILER_API,SLCLI_H2_TRIAL_P,SLCLI_H2_TARGET_P,SLCLI_H2_TRIAL_C,
  SLCLI_H2_TARGET_C,SLCLI_H2_CASE_MAP,SLCLI_H2_ENVELOPE,SLCLI_H2_CLOSURE,
  SLCLI_H2_REF_COUNT
};
struct slcli_ha2_derivation_contract {
  const unsigned char *raw;
  size_t bytes;
  struct slcli_ha2_ref refs[SLCLI_H2_REF_COUNT];
  unsigned char source_commit[20],raw_digest[32];
};
int slcli_ha2_ref_parse(const unsigned char *,size_t,struct slcli_json_span,
  struct slcli_ha2_ref *);
int slcli_ha2_derivation_contract_parse(const unsigned char *,size_t,
  struct slcli_ha2_derivation_contract *);
/* Typed exact raw/table range comparison only. Caller/native/source/birth
 * fields are deliberately not inferred from a member digest or table match. */
int slcli_ha2_ref_member_matches(const struct slcli_ha2_ref *,
  const struct slcli_unit19_reader *,uint32_t,uint32_t);
#endif
