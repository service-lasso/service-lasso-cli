#ifndef SLCLI_HA2_JOINS_CONSUME_H
#define SLCLI_HA2_JOINS_CONSUME_H
#include "ha2_joins.h"
#include "ha2_target_controls.h"
#if defined(_WIN32) || defined(__linux__)
struct slcli_ha2_joins_consumer {
  struct slcli_arena *arena;
  struct slcli_unit19_range *range;
  struct slcli_ha2_joins joins;
  uint32_t started,failed,joins_decoded;
};
struct slcli_ha2_joins_consumer *slcli_ha2_joins_consumer_allocate(struct slcli_arena *,size_t);
/* Actual twice-held native member consumption and exact represented joins.
 * Native source/entry/caller/birth resolution and full secret-sensitive
 * allowlist/closure/envelope are independent prerequisites, never inferred. */
int slcli_ha2_joins_consume(struct slcli_ha2_joins_consumer *,
  const struct slcli_ha2_target_controls *,const struct slcli_unit19_reader *,
  slcli_input_handle,const struct slcli_unit19_reader *,const struct slcli_unit19_reader *);
int slcli_ha2_joins_positive_original(struct slcli_ha2_joins_consumer *,
  const struct slcli_ha2_target_controls *,const struct slcli_unit19_reader *,
  slcli_input_handle,const struct slcli_unit19_reader *,const struct slcli_unit19_reader *);
#endif
#endif
