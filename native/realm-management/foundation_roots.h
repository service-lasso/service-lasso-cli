#ifndef SLCLI_MANAGEMENT_FOUNDATION_ROOTS_H
#define SLCLI_MANAGEMENT_FOUNDATION_ROOTS_H
#include "../host-service/source_deadline.h"
#include "../host-service/store_body.h"
#if defined(_WIN32) || defined(__linux__)
/* Existing original input roles, in this fixed order: source97, retention98,
 * store21, qualification realm15, production realm6. Handles route actual held
 * originals; neither the numeric value nor observed identity designates them.
 * Authentic caller68/101..103 producer/ancestor/protection and full source/native
 * admission must precede effects. No extra original is created by these rows. */
struct slcli_management_foundation_roots {
  struct slcli_arena *arena;
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin;
  const void *caller_original;
  size_t caller_bytes;
  slcli_store_capture_sink sink;
  void *capture_original;
  size_t capture_original_bytes;
  slcli_input_handle originals[5];
  slcli_input_handle *birth_cells[5];
  const void *birth_producers[5];
  size_t birth_producer_bytes[5];
  slcli_input_capture captures[10];
  struct slcli_native_clock clocks[20];
  uint64_t expires,origin_tick,previous_tick;
  uint32_t prepared,started,rows_attempted,rows_preserved,complete,unresolved;
  uint32_t unborn,joined_mask;
};
struct slcli_management_foundation_roots *slcli_management_foundation_roots_allocate(
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
  const slcli_input_handle[5],slcli_store_capture_sink,void *,size_t);
struct slcli_management_foundation_roots *slcli_management_foundation_roots_allocate_unborn(
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
  slcli_input_handle *const[5],const void *const[5],const size_t[5],
  slcli_store_capture_sink,void *,size_t);
/* One original native output-cell join, including failed postbirth diagnostics.
 * This supplies custody only; source designation and native authority preexist. */
int slcli_management_foundation_roots_born(struct slcli_management_foundation_roots *,
  uint32_t,const struct slcli_native_clock *);
/* Twice observe SAME held directory originals and preserve all failed rows.
 * This returns custody comparison, never ROOT_FOUNDATION authority or grant. */
int slcli_management_foundation_roots_observe(struct slcli_management_foundation_roots *);
#endif
#endif
