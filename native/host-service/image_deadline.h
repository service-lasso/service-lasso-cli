#ifndef SLCLI_IMAGE_DEADLINE_H
#define SLCLI_IMAGE_DEADLINE_H
#include "image_reader.h"
#if defined(_WIN32) || defined(__linux__)
struct slcli_image_deadline_owner {
  struct slcli_arena *arena;
  struct slcli_image_reader *reader;
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin;
  const void *caller;
  void *capture_owner;
  struct slcli_shared_workspace *workspace;
  slcli_input_capture *captures;
  slcli_store_capture_sink sink;
  struct slcli_native_clock *clocks;
  size_t caller_bytes,capture_owner_bytes,capacity,used,active;
  uint64_t expires,origin_tick,previous_tick;
  slcli_input_handle original;
  slcli_input_handle *birth_cell;
  const void *birth_producer;
  size_t birth_producer_bytes;
  uint32_t prepared,inflight,failed,unborn,birth_joined;
};
int slcli_image_deadline_before(struct slcli_image_reader *,slcli_input_capture *);
int slcli_image_deadline_after(struct slcli_image_reader *,slcli_input_capture *,int);
int slcli_image_deadline_prepare_unborn(struct slcli_arena *,struct slcli_image_reader *,
  const struct slcli_source_deadline_original *,const void *,size_t,void *,size_t,
  uint64_t,slcli_input_handle *,const void *,size_t);
/* Caller is the actual private native creation body, after its retained raw
 * successful operation checks. This binds that original output, not authority. */
int slcli_image_deadline_born(struct slcli_image_reader *,slcli_input_handle *,
  const struct slcli_native_clock *);
int slcli_image_deadline_precede(struct slcli_image_reader *,const struct slcli_native_clock *);
#endif
#endif
