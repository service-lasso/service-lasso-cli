#ifndef SLCLI_STORE_DEADLINE_H
#define SLCLI_STORE_DEADLINE_H
#include "store_body.h"
#include "source_deadline.h"
#if defined(_WIN32) || defined(__linux__)
/* Private original source boundary, not actor/realm/source/seal authority.
 * Six is the existing native pair stream schedule, not a physical budget.
 * Every clock row/context/control remains charged in the original arena. */
struct slcli_store_deadline_owner {
  struct slcli_arena *arena;
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin,*predecessor;
  const void *caller_original;
  size_t caller_bytes;
  slcli_store_capture_sink sink;
  void *capture_original;
  size_t capture_original_bytes;
  struct slcli_store_stream *streams[6],*pending_stream;
  slcli_input_capture *pending_capture;
  const void *external_original;
  size_t external_bytes;
  struct slcli_native_clock *external_before;
  struct slcli_native_clock *before;
  size_t count,row_capacity,offsets[6],capacities[6],used[6],storage_bytes[6];
  uint64_t expires,origin_tick,predecessor_tick,previous_tick;
  uint32_t prepared,predecessor_advanced,in_flight,failed;
};
struct slcli_store_deadline_owner *slcli_store_deadline_bind(struct slcli_arena *,
  struct slcli_store_stream *const *,size_t,const struct slcli_source_deadline_original *,
  const void *,size_t,const struct slcli_native_clock *,slcli_store_capture_sink,void *,size_t);
/* Private typed namespace owner uses fresh embedded native clock rows;
 * no effect admission comes from a pointer, clock or source helper. */
int slcli_store_deadline_external_begin(struct slcli_store_deadline_owner *,
  void *,size_t,struct slcli_native_clock *);
int slcli_store_deadline_external_end(struct slcli_store_deadline_owner *,
  void *,size_t,struct slcli_native_clock *,int);
int slcli_store_deadline_precede(struct slcli_store_deadline_owner *,const struct slcli_native_clock *);
int slcli_store_deadline_ready(const struct slcli_store_deadline_owner *);
/* Original immutable source/member bytes cannot overlap any new mutable owner
 * or the original caller/deadline/capture/predecessor source tuple. */
int slcli_store_deadline_away(const struct slcli_store_deadline_owner *,const void *,size_t);
#endif
#endif