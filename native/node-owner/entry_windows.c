#include "entry_windows.h"
#if defined(_WIN32)
/* The source path has no explicit CRT/environment/allocation call before its
 * native observations. Compiler-generated helpers, stack probes and entry ABI
 * still require whole source/build qualification before selecting /ENTRY.
 * The parent must have joined this exact export/image and mapped prefix before
 * resume. Malformed pointers can still cause natural loss; outside preeffect
 * uncertainty is retained and no such loss counts as resource disposition. */
int __cdecl slcli_node_entry_windows(void) {
 struct slcli_node_route_windows route;
 struct slcli_node_capture_layout_windows placement;
 struct slcli_node_entry_windows_capture *c;unsigned char stack_original;
 unsigned char *base;size_t name_capacity;
 if(!slcli_node_route_windows_read_original(&route)||
  !slcli_node_capture_layout_windows_route_headers_original(&route,&placement))return 1;
 base=route.base;c=(struct slcli_node_entry_windows_capture *)(base+placement.stages[SLCLI_CAPTURE_ENTRY].offset);
 c->started=1;c->route=route;c->placement=placement;
 c->entry_original=(const void *)&slcli_node_entry_windows;c->stack_original=&stack_original;
 c->receiver=(struct slcli_node_receive_windows *)(base+placement.stages[SLCLI_CAPTURE_RECEIVE].offset);
 c->prefix=(struct slcli_node_prefix_windows *)(base+placement.stages[SLCLI_CAPTURE_PREFIX].offset);
 c->loader=(struct slcli_node_loader_windows *)(base+placement.stages[SLCLI_CAPTURE_LOADER].offset);
 c->receive_result=slcli_node_receive_windows_original(c->receiver,route.transport,route.parent,c->entry_original);
 c->receive_returned=1;if(!c->receive_result)return 1;
 if(c->receiver->decode.transport.mapped_view.base!=(uintptr_t)route.base||
  c->receiver->decode.transport.mapped_view.bytes!=route.bytes)return 1;
 c->bootstrap_storage_result=slcli_node_bootstrap_route_storage_windows_original(c);
 c->bootstrap_storage_returned=1;if(!c->bootstrap_storage_result)return 1;
 c->prefix_result=slcli_node_prefix_windows_observe(c->prefix,
  (struct slcli_node_prefix_windows_region *)(base+placement.stages[SLCLI_CAPTURE_REGIONS].offset),
  placement.regions,c->receiver,c->entry_original,c->stack_original);
 c->prefix_returned=1;if(!c->prefix_result)return 1;
 c->native_geometry_matches=c->prefix->system.dwPageSize==route.page_bytes&&
  c->prefix->system.dwAllocationGranularity==route.granularity;
 if(!c->native_geometry_matches)return 1;
 name_capacity=placement.modules*placement.name_units; /* checked by derivation */
 c->loader_result=slcli_node_loader_windows_observe(c->loader,
  (struct slcli_node_loader_windows_module *)(base+placement.stages[SLCLI_CAPTURE_MODULES].offset),
  placement.modules,(WCHAR *)(base+placement.stages[SLCLI_CAPTURE_NAMES].offset),
  name_capacity,placement.name_units,c->prefix);
 c->loader_returned=1;
 /* Whole module/TLS/source-original promotion is still absent. Observations
  * cannot call original CRT startup or install an acquired process root. */
 return 1;
}
#endif
