#ifndef SLCLI_INPUT_DEADLINE_H
#define SLCLI_INPUT_DEADLINE_H
#include "input_body.h"
#include "source_deadline.h"
#include "store_body.h"
#if defined(_WIN32) || defined(__linux__)
struct slcli_bq_source_control_sink_windows;
struct slcli_bq_serialized_factory_windows;
struct slcli_bq_factory_block_windows;
/* Private retained source caller boundary. Binding authenticates neither the
 * caller, expected digest, provider nor readonly/writer/map/alias closure.
 * Full body/readback/capture and all pre-clock rows are charged before IO. */
struct slcli_input_deadline_owner {
  struct slcli_arena *arena;
  struct slcli_input_body *body;
  size_t body_storage_bytes;
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin,*predecessor;
  const void *caller_original;
  size_t caller_bytes;
  slcli_store_capture_sink sink;
  void *capture_original;
  size_t capture_original_bytes;
#if defined(_WIN32)
  /* Assigned only by the actual private monotonic factory. These operands
   * grant no IO or source authority without fresh exact native/header joins. */
  struct slcli_bq_serialized_factory_windows *source_fixed_caller;
  const struct slcli_bq_factory_block_windows *source_body_block,*source_deadline_block;
  /* Fixed source-created per-body lane. The original sink stays capture_original;
   * addresses/counts select no authority without the SAME body allocation. */
  struct slcli_bq_source_control_sink_windows *source_sink;
  slcli_input_capture *source_captures;
  size_t source_capture_count,source_first_target;
#endif
  struct slcli_native_clock *before;
  size_t row_capacity,next;
  slcli_input_capture *pending;
  uint64_t expires,origin_tick,predecessor_tick,previous_tick;
  uint32_t prepared,predecessor_advanced,in_flight,failed;
};
struct slcli_input_deadline_owner *slcli_input_deadline_bind(struct slcli_arena *,
  struct slcli_input_body *,const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,slcli_store_capture_sink,void *,size_t);
/* One private completed-native predecessor transfer BEFORE first body IO.
 * Original deadline/caller/origin/expiry are retained without renewal. */
int slcli_input_deadline_precede(struct slcli_input_deadline_owner *,const struct slcli_native_clock *);
int slcli_input_deadline_ready(const struct slcli_input_body *);
int slcli_input_deadline_before(struct slcli_input_body *,slcli_input_capture *);
int slcli_input_deadline_after(struct slcli_input_body *,slcli_input_capture *,
  const unsigned char *,size_t,int);
#endif
#endif
