#ifndef SLCLI_NODE_SELECTOR_WINDOWS_H
#define SLCLI_NODE_SELECTOR_WINDOWS_H
#if defined(_WIN32)
#include <windows.h>
#include <stddef.h>
#include <stdint.h>
#define SLCLI_NODE_SELECTOR_NAME L"SLCLI_BOOTSTRAP_ROUTE_V6"
#define SLCLI_NODE_SELECTOR_VALUE_CHARS 67u
#define SLCLI_NODE_SELECTOR_ENTRY_CHARS (sizeof(SLCLI_NODE_SELECTOR_NAME)/sizeof(WCHAR)+1u+SLCLI_NODE_SELECTOR_VALUE_CHARS)
/* Canonical routing only. Source/parent/private object authority is never a
 * consequence of this syntax or of an environment value being present. */
int slcli_node_selector_windows_encode(WCHAR *,size_t,HANDLE,HANDLE,const void *,uint64_t);
int slcli_node_selector_windows_decode(const WCHAR *,size_t,HANDLE *,HANDLE *,void **,uint64_t *);
#endif
#endif
