#ifndef SLCLI_PLATFORM_MEMORY_H
#define SLCLI_PLATFORM_MEMORY_H
#include "owner_memory.h"

/* Original ABI structures and request/results are retained by value before
 * reuse. A producer must bind these raw originals into the admitted capture
 * graph; this structure and a successful API return are not admission proof. */
struct slcli_memory_capture {
  struct slcli_extent_request request;
  uint64_t related_sequence;
  uintptr_t address, result;
  uint64_t bytes, reserved, backed, tick, native_status;
  uint64_t native_error;
  uint32_t native_error_observed;
  uint32_t operation, observed, geometry_bytes, page_bytes, backed_observed;
  uint32_t flags, protection, page_geometry_bytes;
  uint64_t query_result;
  uint64_t tick_raw[2], tick_status[2];
  uint32_t tick_calls;
  uint64_t request_args[6], query_args[3];
  unsigned char geometry[128];
  unsigned char page_geometry[64];
};
int slcli_platform_acquire(struct slcli_memory_owner *, uint32_t, uint64_t,
                           struct slcli_memory_capture *);
int slcli_platform_release(struct slcli_memory_owner *, uint32_t,
                           struct slcli_memory_capture *);
#endif
