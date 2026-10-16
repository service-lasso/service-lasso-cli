#ifndef SLCLI_NODE_ROUTE_WINDOWS_H
#define SLCLI_NODE_ROUTE_WINDOWS_H
#include <stddef.h>
#include <stdint.h>
#if defined(_WIN32)
#include <windows.h>
#define SLCLI_NODE_ROUTE_CELL_BYTES 128u
struct slcli_node_route_windows {
 HANDLE transport,parent;
 void *base;
 uint64_t bytes,capture_bytes,threads,events,regions,modules,name_units,page_bytes,granularity;
};
/* A locator only. No native object, source identity or allocation authority. */
extern volatile unsigned char slcli_node_route_cell_v1[SLCLI_NODE_ROUTE_CELL_BYTES];
int slcli_node_route_windows_encode(unsigned char *,size_t,const struct slcli_node_route_windows *);
int slcli_node_route_windows_decode(const volatile unsigned char *,size_t,struct slcli_node_route_windows *);
int slcli_node_route_windows_read_original(struct slcli_node_route_windows *);
#endif
#endif
