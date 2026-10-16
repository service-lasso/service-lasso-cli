#ifndef SLCLI_NODE_BOOTSTRAP_H
#define SLCLI_NODE_BOOTSTRAP_H
#include "preentry.h"
#include "bootstrap_transport.h"
#include "../host-service/platform_memory.h"
/* Windows owns this object in the exact preplanned shared capture stage;
 * Linux still uses image BSS pending its containing-stage integration. The
 * image routing cell/code and full stack/TLS/loader prefix remain charged
 * originals. Getter/storage association is never source/native admission. */
struct slcli_node_bootstrap_caller {
  const void *entry_original;
  const struct slcli_native_clock *transported_origin_original;
  uint64_t transported_expires_original;
};
struct slcli_node_bootstrap {
#if defined(_WIN32)
  const struct slcli_node_entry_windows_capture *storage_entry_original;
  struct slcli_node_bootstrap_decode *transport_original;
  uint32_t storage_bound;
  uint32_t tail_adopt_started,tail_header_attempted,tail_header_written,tail_adopted;
  size_t tail_zero_scanned,tail_nonzero_offset;
  unsigned char tail_nonzero_value;
  struct slcli_native_clock tail_before,tail_scanned,tail_after;
  int tail_before_result,tail_scanned_result,tail_after_result;
  uint32_t control_stage,control_nonzero_stage;
  size_t control_zero_scanned,control_nonzero_offset;
  unsigned char control_nonzero_value;
  struct slcli_native_clock bind_storage_before,bind_storage_after;
  int bind_storage_before_result,bind_storage_after_result;
#endif
  struct slcli_memory_owner owner;
  struct slcli_arena arena;
  struct slcli_memory_capture acquisition;
  struct slcli_native_clock before,after;
  struct slcli_native_clock origin_original;
  uint64_t expires_original,bootstrap_bytes_original;
  const void *entry_original;
  struct slcli_node_bootstrap_caller *caller;
  struct slcli_native_clock *local_origin,*local_predecessor;
  struct slcli_source_deadline_original *deadline;
  struct slcli_extent_request *local_runtime_request;
  uint32_t started,acquire_returned,arena_returned,constructed;
  int acquire_result,arena_result,postclock_result,retained_result;
  uint32_t retained_returned;
#if defined(_WIN32)
  struct slcli_node_preentry_windows *node;
  FILETIME creation_original,*local_creation;
#elif defined(__linux__)
  struct slcli_node_preentry_linux *node;
  struct stat backing_original,*local_backing;
#endif
  uint32_t bind_attempted,bind_returned;
  int bind_result;
  uint32_t transport_attempted,transport_returned;
  int transport_result,transport_acquire_result,transport_bind_result,transport_identity_result;
  uint32_t transport_acquire_returned,transport_bind_returned;
};
struct slcli_node_bootstrap *slcli_node_bootstrap_image_original(void);
const struct slcli_node_bootstrap_decode *slcli_node_bootstrap_decode_original(const unsigned char *,size_t);
/* After independent native entry/source/transport admission, consume the exact
 * retained decoded transport against the existing original owner prefix. */
int slcli_node_bootstrap_from_transport_original(const void *);
/* Actual serialized private native entry only, after independently retained
 * image/stack/TLS/loader observations and original transport/source admission.
 * Origin is full raw native original, not a renewed clock or frontend grant. */
int slcli_node_bootstrap_acquire_original(const void *,const struct slcli_native_clock *,
  uint64_t,uint64_t);
#if defined(_WIN32)
struct slcli_node_entry_windows_capture;
/* Fixed entry storage association only, not owner/source/native promotion. */
int slcli_node_bootstrap_route_storage_windows_original(struct slcli_node_entry_windows_capture *);
/* Structural prepared-tail adoption only, after genuine native/source root
 * acquisition. Never reconstructs or promotes an owner from transport bytes. */
int slcli_node_bootstrap_adopt_tail_windows_original(void);
int slcli_node_bootstrap_windows_bind_original(HANDLE,HANDLE,uint64_t,const FILETIME *,size_t,size_t,const struct slcli_extent_request *);
#elif defined(__linux__)
int slcli_node_bootstrap_linux_bind_original(int,uint64_t,const struct stat *,size_t,const struct slcli_extent_request *);
#endif
#endif
