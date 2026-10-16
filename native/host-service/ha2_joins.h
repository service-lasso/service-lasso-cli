#ifndef SLCLI_HA2_JOINS_H
#define SLCLI_HA2_JOINS_H
#include "ha2_derivation_contract.h"
enum slcli_ha2_join_relation {SLCLI_H2_SAME=1,SLCLI_H2_DISTINCT,
  SLCLI_H2_DISTINCT_ENTRY,SLCLI_H2_TRIAL_RETAINED};
struct slcli_ha2_join {
  struct slcli_ha2_ref trial,target;
  uint32_t relation;
};
struct slcli_ha2_joins {
  const unsigned char *raw;
  size_t bytes;
  struct slcli_ha2_join entries[27];
  unsigned char raw_digest[32];
};
/* Exact ordered representation only. SAME Ref equality is not native identity
 * admission. Every selected full native/source/entry/caller/birth/allowlist
 * resolver remains necessary; no relation label certifies secret behavior. */
int slcli_ha2_joins_parse(const unsigned char *,size_t,struct slcli_ha2_joins *);
int slcli_ha2_ref_same(const struct slcli_ha2_ref *,const struct slcli_ha2_ref *);
int slcli_ha2_joins_unit_members(const struct slcli_ha2_joins *,
  const struct slcli_unit19_reader *,const struct slcli_unit19_reader *,
  const struct slcli_unit19_reader *);
#endif
