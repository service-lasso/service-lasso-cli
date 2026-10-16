#ifndef SLCLI_HA2_TARGET_CONTROLS_H
#define SLCLI_HA2_TARGET_CONTROLS_H
#include "ha2_derivation_contract.h"
#include "unit19_range.h"
#include "profile.h"
#include "grant.h"
#include "calibration.h"
#if defined(_WIN32) || defined(__linux__)
/* Private target byte consumption only. Full twelve-member relation, actual
 * C22 qualifications, production6/7 preparation and whole immutable U4 native
 * equality/admission are separate required source operations. No active field,
 * enrollment grant or certificate installation is available from this view. */
struct slcli_ha2_target_controls {
  struct slcli_arena *arena;
  struct slcli_unit19_range *ranges[5]; /* contract0, P6, G7, HQ1-8, C1-11 */
  struct slcli_ha2_derivation_contract contract;
  struct slcli_profile profile;
  struct slcli_grant grant;
  struct slcli_qualification qualification;
  struct slcli_calibration candidate;
  uint32_t started,failed,controls_decoded;
};
struct slcli_ha2_target_controls *slcli_ha2_target_controls_allocate(struct slcli_arena *,
  const size_t[5]);
int slcli_ha2_target_controls_consume(struct slcli_ha2_target_controls *,
  const struct slcli_unit19_reader *,slcli_input_handle,
  const struct slcli_unit19_reader *,const struct slcli_unit19_reader *,
  const unsigned char[32]);
#endif
#endif
