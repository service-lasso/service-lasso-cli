#ifndef SLCLI_PRODUCT_ROUTE_IMAGE_WINDOWS_H
#define SLCLI_PRODUCT_ROUTE_IMAGE_WINDOWS_H
#include "product_birth_windows.h"
#if defined(_WIN32)
/* Pure held-file representation, not source/nativecase or loaded authority. */
struct slcli_product_windows_route_image {
 const struct slcli_build_held_file_windows *file_original;
 uint32_t pe_offset,optional_bytes,section_count,image_bytes,route_rva,route_raw;
 uint32_t export_rva,export_bytes;
};
int slcli_product_windows_route_image_original(
 struct slcli_product_windows_route_image *,const struct slcli_build_held_file_windows *);
#endif
#endif
