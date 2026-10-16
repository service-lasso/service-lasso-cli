#ifndef SLCLI_UNIT19_POSTSEAL_LINUX_H
#define SLCLI_UNIT19_POSTSEAL_LINUX_H
#if defined(__linux__)
#include "unit19_seal_linux.h"
/* A separately allocated SAME-arena complete streamed postseal comparison.
 * Both prior preparations, seal captures, actual held fd and raw source bodies
 * remain held. No repacking/retirement/new quota or whole-file read-all occurs. */
struct slcli_unit19_postseal_linux {
  struct slcli_arena *arena;
  struct slcli_store_stream *read;
  struct slcli_native_clock *clocks;
  size_t clock_capacity,clock_count;
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin;
  const void *caller_original;
  slcli_store_capture_sink sink;
  void *capture_original;
  const struct slcli_unit19_seal_linux *seal_original;
  const struct slcli_unit19_framing *framing_original;
  const unsigned char *const *members_original;
  uint64_t expires,origin_tick,previous_tick;
  uint32_t started,failed,complete;
};
struct slcli_unit19_postseal_linux *slcli_unit19_postseal_linux_allocate(struct slcli_arena *,size_t);
int slcli_unit19_postseal_linux_original(struct slcli_unit19_postseal_linux *,
  const struct slcli_unit19_seal_linux *,const struct slcli_unit19_framing *,
  const unsigned char *const *,const struct slcli_source_deadline_original *,
  const void *,slcli_store_capture_sink,void *);
#endif
#endif
