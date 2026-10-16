#ifndef SLCLI_HA2_LARGE_MEMBERS_H
#define SLCLI_HA2_LARGE_MEMBERS_H
#include "ha2_derivation_contract.h"
/* Borrowed INACTIVE root view, not complete nested closure/envelope admission.
 * Independently retained native ranges and full source/value relation required. */
struct slcli_ha2_closure_root {
  const unsigned char *raw;size_t bytes;
  struct slcli_ha2_ref source_unit,compiler_api;
  struct slcli_json_span entry_set,regions;
};
struct slcli_ha2_envelope_root {
  const unsigned char *raw;size_t bytes;
  struct slcli_json_span phases,domain_totals;
};
enum slcli_ha2_region_field {
  SLCLI_H2R_ID,SLCLI_H2R_SOURCE_REFS,SLCLI_H2R_CALL_EDGES,SLCLI_H2R_DOMAINS,
  SLCLI_H2R_VALUE_CLASS,SLCLI_H2R_INPUT_BOUNDS,SLCLI_H2R_BRANCH_CASES,
  SLCLI_H2R_ALLOCATIONS,SLCLI_H2R_NATIVE_RIGHTS,SLCLI_H2R_LIFETIME_CUTS,
  SLCLI_H2R_EVIDENCE_REFS,SLCLI_H2R_FIELDS
};
struct slcli_ha2_region_view { struct slcli_json_span fields[SLCLI_H2R_FIELDS]; };
int slcli_ha2_region_parse(const unsigned char *,size_t,struct slcli_json_span,
  const struct slcli_json_ha2_plan *,uint32_t,struct slcli_ha2_region_view *);
struct slcli_ha2_phase_view { struct slcli_json_span fields[8]; };
int slcli_ha2_phase_parse(const unsigned char *,size_t,struct slcli_json_span,
  const struct slcli_json_ha2_plan *,uint32_t,struct slcli_ha2_phase_view *);
struct slcli_ha2_domain_total_view {
  struct slcli_json_span fields[6];uint64_t domain,maximum,source_peak,categories[3];
};
int slcli_ha2_domain_total_parse(const unsigned char *,size_t,struct slcli_json_span,
  const struct slcli_json_ha2_plan *,struct slcli_ha2_domain_total_view *);
struct slcli_ha2_large_members_view {
  struct slcli_ha2_closure_root closure;struct slcli_ha2_envelope_root envelope;
  size_t regions,domains;
};
/* Closed structural view only. Unresolved nested field/source semantics,
 * exact native membership and owning simultaneous inequalities stay gated. */
int slcli_ha2_large_members_view_decode(const unsigned char *,size_t,
  const unsigned char *,size_t,const struct slcli_json_ha2_plan *,
  struct slcli_ha2_large_members_view *);
int slcli_ha2_closure_root_parse(const unsigned char *,size_t,
  const struct slcli_json_ha2_plan *,struct slcli_ha2_closure_root *);
int slcli_ha2_envelope_root_parse(const unsigned char *,size_t,
  const struct slcli_json_ha2_plan *,struct slcli_ha2_envelope_root *);
#endif
