#ifndef SLCLI_PREENTRY_BACKING_LINUX_H
#define SLCLI_PREENTRY_BACKING_LINUX_H
#include "source_deadline.h"
#include "platform_memory.h"
#if defined(__linux__)
#include <asm/stat.h>
struct slcli_preentry_backing_call {
  uint64_t args[6],result;
  uint32_t called,returned;
  int before_result,after_result;
  struct slcli_native_clock before,after;
};
struct slcli_preentry_backing_linux {
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin,*predecessor;
  const void *caller;
  size_t caller_bytes;
  uint64_t expires,origin_tick,predecessor_tick,bytes;
  struct slcli_memory_owner *physical_owner;
  uint32_t section_only;
  struct slcli_extent_request request;
  struct slcli_preentry_backing_call calls[6];
  struct stat backing_original,expected_backing_original;
  uint32_t operation;
  int descriptor;
  uintptr_t mapping;
  uint32_t started,prepared,mapped,zero_address_retained,qualified;
  int prepare_result,backing_result,partial_result,acquired_result,retained_result;
  uint32_t prepare_returned,backing_returned,partial_returned,acquired_returned,retained_returned;
};
/* Actual inherited backing constructor, not an admission/issuer interface.
 * The source caller reserves descriptor/map/capture/lifetime rows before entry;
 * all partial native outputs and prepared physical charge stay retained. */
int slcli_preentry_backing_linux_create(struct slcli_preentry_backing_linux *,
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,
  size_t,const struct slcli_native_clock *,uint64_t);
/* Genuine target physical owner is separate from the parent's control arena.
 * Backing-only creation records ftruncate before diagnostics and never maps the
 * whole runtime into the parent; original FD/size/seal/stat captures survive. */
int slcli_preentry_backing_linux_create_section(struct slcli_preentry_backing_linux *,
  struct slcli_arena *,struct slcli_memory_owner *,
  const struct slcli_source_deadline_original *,const void *,
  size_t,const struct slcli_native_clock *,uint64_t);
/* Receiver performs actual inherited-FD observations/map. Expected identity is
 * the independently transported original parent capture, never read from the
 * same mapping as self-authentication. Constructor/caller admission is separate. */
int slcli_preentry_backing_linux_receive(struct slcli_preentry_backing_linux *,
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,
  size_t,const struct slcli_native_clock *,int,uint64_t,const struct stat *,
  const struct slcli_extent_request *);
#endif
#endif
