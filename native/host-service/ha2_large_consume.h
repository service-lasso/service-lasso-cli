#ifndef SLCLI_HA2_LARGE_CONSUME_H
#define SLCLI_HA2_LARGE_CONSUME_H
#include "ha2_large_members.h"
#include "unit19_range.h"
#if defined(_WIN32) || defined(__linux__)
/* Private original BQ source consumer, no budget, principal or source grant.
 * Constructor must independently admit BQ domain0/full own source/extent. */
struct slcli_ha2_large_consumer {
  struct slcli_arena *arena;struct slcli_unit19_range *ranges[2];
  const struct slcli_unit19_reader *unit;
  const struct slcli_source_deadline_original *deadline;
  const void *caller_original;size_t caller_bytes;
  struct slcli_json_ha2_plan plan;
  struct slcli_native_clock bridge,parse_before,parse_after;
  struct slcli_ha2_large_members_view view;
  uint32_t prepared,started,failed,structural_view_decoded;
};
struct slcli_ha2_large_consumer *slcli_ha2_large_consumer_allocate(struct slcli_arena *,
  const size_t[2],const struct slcli_json_ha2_plan *);
int slcli_ha2_large_consumer_bind(struct slcli_ha2_large_consumer *,
  const struct slcli_unit19_reader *,const struct slcli_source_deadline_original *,
  const void *,size_t,const struct slcli_native_clock *,slcli_store_capture_sink,void *,size_t);
int slcli_ha2_large_consume(struct slcli_ha2_large_consumer *,slcli_input_handle);
#endif
#endif
