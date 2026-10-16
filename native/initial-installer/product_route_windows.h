#ifndef SLCLI_PRODUCT_ROUTE_WINDOWS_H
#define SLCLI_PRODUCT_ROUTE_WINDOWS_H
#include "product_deadline_windows.h"
#if defined(_WIN32)
#define SLCLI_PRODUCT_ROUTE_BYTES 32u
struct slcli_product_windows_entry_route {HANDLE readonly_section,parent_process;};
/* Distinct fixed product locator cell, following the existing node-owner
 * stopped-child route-cell mechanism. Never a clock/caller/profile grant.
 * Whole actual PE/export/source/native write/readback and own-writer exclusion
 * must qualify before release. Parent/child C pointers never cross this cell. */
extern volatile unsigned char slcli_product_route_cell_v1[SLCLI_PRODUCT_ROUTE_BYTES];
int slcli_product_windows_route_encode(unsigned char *,size_t,
 const struct slcli_product_windows_entry_route *);
int slcli_product_windows_route_decode(struct slcli_product_windows_entry_route *,
 const volatile unsigned char *,size_t);
#endif
#endif
