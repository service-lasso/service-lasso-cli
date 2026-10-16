#ifndef SLCLI_NODE_ENTRY_WINDOWS_H
#define SLCLI_NODE_ENTRY_WINDOWS_H
#include "route_windows.h"
#include "capture_layout_windows.h"
#if defined(_WIN32)
struct slcli_node_entry_windows_capture {
 struct slcli_node_route_windows route;
 struct slcli_node_capture_layout_windows placement;
 const void *entry_original,*stack_original;
 struct slcli_node_receive_windows *receiver;
 struct slcli_node_prefix_windows *prefix;
 struct slcli_node_loader_windows *loader;
 struct slcli_node_bootstrap *bootstrap_original;
 size_t bootstrap_zero_scanned,bootstrap_nonzero_offset;
 unsigned char bootstrap_nonzero_value;
 int bootstrap_storage_result;
 uint32_t bootstrap_storage_attempted,bootstrap_storage_returned;
 int receive_result,prefix_result,loader_result;
 uint32_t started,receive_returned,prefix_returned,loader_returned;
 uint32_t native_geometry_matches;
};
int __cdecl slcli_node_entry_windows(void);
#endif
#endif
