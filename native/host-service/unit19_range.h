#ifndef SLCLI_UNIT19_RANGE_H
#define SLCLI_UNIT19_RANGE_H
#include "input_body.h"
#include "input_deadline.h"
#include "unit19.h"
#if defined(_WIN32) || defined(__linux__)
/* An actual selected original member range in an independently admitted
 * whole immutable unit. One SAME held unit handle is borrowed, no extra physical
 * input original is born. Raw/native/source/seal/whole-unit authority remains a
 * prerequisite; decoder success is never admission. Both full member copies,
 * all fresh captures and whole-file EOF observations remain charged/retained. */
struct slcli_unit19_range {
  struct slcli_arena *arena;
  struct slcli_input_body *copies;
  const struct slcli_unit19_reader *unit;
  struct slcli_unit19_record selected;
  uint64_t whole_file_bytes;
  uint32_t ordinal,started,failed,complete,derivation_geometry,designated_ordinal;
};
struct slcli_unit19_range *slcli_unit19_range_allocate(struct slcli_arena *,uint64_t,size_t);
/* Full selected unit26 member, including large sourceClosure/envelope ranges,
 * borrowed from SAME whole admitted native unit. Fixed per-member geometry,
 * both full copies and all native rows must fit its actual charged BQ extent.
 * It neither reads the whole unit at once nor adds a budget/storage pool. */
struct slcli_unit19_range *slcli_unit19_derivation_range_allocate(struct slcli_arena *,
  uint32_t,size_t);
/* Private fresh-range original deadline binding before ANY native effect;
 * full copies/native captures/preclock rows remain in SAME charged owner.
 * Unbound mechanics are transitional, not installed original admission. */
struct slcli_input_deadline_owner *slcli_unit19_range_deadline_bind(struct slcli_unit19_range *,
  const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,slcli_store_capture_sink,void *,size_t);
int slcli_unit19_range_read(struct slcli_unit19_range *,const struct slcli_unit19_reader *,
  uint32_t,slcli_input_handle);
#endif
#endif
