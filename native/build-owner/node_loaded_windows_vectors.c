/* Structural arithmetic fixtures only. No native module, imported function,
 * genuine owner, loaded/source authority or qualification is manufactured. */
#include "node_loaded_windows.h"
#if defined(_WIN32)
int slcli_build_node_loaded_windows_vectors(void) {
 if(!slcli_build_node_windows_rva_correspond_original(0x100000,0x101234,0x1234,0x3000))return 0;
 if(slcli_build_node_windows_rva_correspond_original(0x100000,0x101235,0x1234,0x3000))return 0;
 if(slcli_build_node_windows_rva_correspond_original(0x100000,0x100000,0,0x3000))return 0;
 if(slcli_build_node_windows_rva_correspond_original(0x100000,0x103000,0x3000,0x3000))return 0;
 if(slcli_build_node_windows_rva_correspond_original(0,0x1234,0x1234,0x3000))return 0;
 if(slcli_build_node_windows_rva_correspond_original(UINTPTR_MAX-0x1000,UINTPTR_MAX,0x1000,0x3000))return 0;
 if(slcli_build_node_windows_rva_correspond_original(0x100000,0x101234,0x1234,0))return 0;
 return 1;
}
#endif
