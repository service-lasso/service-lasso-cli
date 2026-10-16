#ifndef SLCLI_NODE_PREFIX_WINDOWS_H
#define SLCLI_NODE_PREFIX_WINDOWS_H
#include "receive_windows.h"
#if defined(_WIN32)
struct slcli_node_prefix_windows_region {
  struct slcli_native_clock before,after;
  MEMORY_BASIC_INFORMATION geometry;
  uint64_t args[3],result;
  DWORD error;
  uint32_t called,returned,error_observed,before_result,after_result;
};
struct slcli_node_prefix_windows {
  const struct slcli_node_receive_windows *receiver_original;
  const struct slcli_native_clock *prior_original;
  struct slcli_node_prefix_windows_region *rows;
  size_t capacity,used,allocated_regions,free_regions;
  const void *entry_original,*image_original,*stack_original;
  const struct slcli_node_prefix_windows_region *image_region_original;
  const struct slcli_node_prefix_windows_region *runtime_region_original;
  SYSTEM_INFO system;
  struct slcli_native_clock system_before,system_after;
  uint64_t system_args[1],expires,previous_tick;
  uintptr_t cursor,minimum,maximum;
  uint32_t started,system_called,system_returned,terminal,complete_scan;
  uint32_t image_observed,stack_observed,ingress_observed,runtime_observed;
};
/* Complete finite raw walk, not a size/order-based module/TLS admission.
 * Exact c/rows already belong to the genuine paid native-entry image prefix.
 * Full source association and original-owner promotion remain separate. */
int slcli_node_prefix_windows_observe(struct slcli_node_prefix_windows *,
  struct slcli_node_prefix_windows_region *,size_t,
  const struct slcli_node_receive_windows *,const void *,const void *);
#endif
#endif
