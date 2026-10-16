#ifndef SLCLI_BUILD_NODE_LOADED_WINDOWS_H
#define SLCLI_BUILD_NODE_LOADED_WINDOWS_H
#include "node_abi_windows.h"
#if defined(_WIN32)
struct slcli_build_node_loaded_windows {
 const struct slcli_build_node_abi_windows *tuple_original;
 struct slcli_build_windows_call process,thread,process_handle,module,path,headers;
 struct slcli_build_windows_call queries[SLCLI_BUILD_NODE_API_IMPORTS];
 MEMORY_BASIC_INFORMATION geometry_original[SLCLI_BUILD_NODE_API_IMPORTS];
 uintptr_t addresses_original[SLCLI_BUILD_NODE_API_IMPORTS];
 HMODULE module_original;
 WCHAR *path_original;
 unsigned char *headers_original;
 SIZE_T header_bytes_original;
 struct slcli_native_clock finish;
 uint64_t prior_tick,expires;
 size_t symbols_observed;
 uint32_t started,failed,addresses_correspond;
};
/* Pure address arithmetic; never dereferences/calls or creates native evidence. */
int slcli_build_node_windows_rva_correspond_original(uintptr_t,uintptr_t,uint32_t,size_t);
/* Fixed compiled ordinary imports, actual module/path/header/page observations.
 * No typed ABI call/loader/source/ROOT grant or reference retirement. */
int slcli_build_node_loaded_windows_capture_original(struct slcli_build_node_loaded_windows *,
 struct slcli_build_bootstrap_windows *,const struct slcli_build_node_abi_windows *);
#endif
#endif
