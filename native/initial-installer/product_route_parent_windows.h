#ifndef SLCLI_PRODUCT_ROUTE_PARENT_WINDOWS_H
#define SLCLI_PRODUCT_ROUTE_PARENT_WINDOWS_H
#include "product_route_image_windows.h"
#include "product_route_windows.h"
#if defined(_WIN32)
/* The genuine initial/nativecase constructor must construct and finance these
 * exact private source operands. A span/count/cost pointer is not that issuer. */
struct slcli_product_windows_route_source {
 const struct slcli_product_windows_birth *birth_original;
 const struct slcli_product_initial_windows_source_span *scan_source_original;
 const struct slcli_bq_resource_cost *cost_original,*failed_overlap_original;
 size_t scan_capacity;
};
struct slcli_product_windows_route_scan {
 MEMORY_BASIC_INFORMATION geometry;
 struct slcli_build_windows_call query,reads[3];
 SIZE_T received[3];unsigned char *headers_original;
};
struct slcli_product_windows_route_parent {
 struct slcli_product_storage_windows *storage_original;
 const struct slcli_product_windows_route_source *source_original;
 struct slcli_product_windows_route_source source_snapshot;
 const struct slcli_product_windows_birth *birth_original;
 struct slcli_product_windows_route_image image_original;
 struct slcli_product_windows_route_scan *scans_original;
 unsigned char *headers_original;
 size_t header_stride,scan_capacity,used;
 uintptr_t minimum,maximum,cursor,image_base,route_address;
 struct slcli_native_clock entry,finish;
 uint64_t prior_tick,expires;
 struct slcli_build_windows_call route_query,initial_read,write,readback;
 MEMORY_BASIC_INFORMATION route_geometry;
 unsigned char encoded[32],initial[32],observed[32];SIZE_T received[2],written;
 struct slcli_product_windows_entry_route route_original;
 /* Actual aliases duplicate BACK out of the stopped child's own handle table.
  * Never confused with the inherited/original handles; retained on all paths. */
 HANDLE aliases_original[2];DWORD alias_flags[2];
 struct slcli_build_windows_call duplicates[2],comparisons[2],flags[2];
 uint32_t started,failed,matches,scan_completed,possibly_written,route_captured;
};
int slcli_product_windows_route_parent_original(struct slcli_product_storage_windows *,
 const struct slcli_product_windows_route_source *);
int slcli_product_windows_route_parent_output_away_original(
 const struct slcli_product_windows_route_parent *,const void *,size_t);
#endif
#endif
