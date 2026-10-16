#ifndef SLCLI_HA2_C22_CONSUME_H
#define SLCLI_HA2_C22_CONSUME_H
#include "unit19_range.h"
#include "calibration.h"
#include "profile.h"
#if defined(_WIN32) || defined(__linux__)
/* SAME independently admitted whole C22 native original, two distinct C13 raw
 * ranges, own service P23 member0 and independent original BQ P117. Consumption performs
 * real retained reads; it never creates a certificate, qualification, seal,
 * source grant, BQ/service owner or installation authority. The service range
 * keeps consumed2/whole-unit lineage; BQ's standalone P keeps consumed1. */
struct slcli_ha2_c22_consumer {
  struct slcli_arena *arena;
  struct slcli_unit19_range *service,*bq;
  struct slcli_calibration service_c,bq_c;
  struct slcli_profile service_profile;
  uint32_t started,failed,complete;
};
struct slcli_ha2_c22_consumer *slcli_ha2_c22_allocate(struct slcli_arena *,size_t,size_t);
int slcli_ha2_c22_consume(struct slcli_ha2_c22_consumer *,
  const struct slcli_unit19_reader *,slcli_input_handle,
  const struct slcli_unit19_range *,const struct slcli_input_body *,
  const unsigned char[32],const unsigned char[32]);
#endif
#endif
