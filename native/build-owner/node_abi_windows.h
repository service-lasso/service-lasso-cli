#ifndef SLCLI_BUILD_NODE_ABI_WINDOWS_H
#define SLCLI_BUILD_NODE_ABI_WINDOWS_H
#include "bootstrap_windows.h"
#include "remote_image_windows.h"
#if defined(_WIN32)
enum { SLCLI_BUILD_NODE_IMAGE=0,SLCLI_BUILD_NODE_LIBRARY=1,SLCLI_BUILD_NODE_API_HEADER=2,
 SLCLI_BUILD_JS_NATIVE_HEADER=3,SLCLI_BUILD_JS_NATIVE_TYPES=4,SLCLI_BUILD_NODE_API_TYPES=5,
 SLCLI_BUILD_NODE_VERSION_HEADER=6,SLCLI_BUILD_NODE_INPUTS=7,SLCLI_BUILD_NODE_API_IMPORTS=11 };
struct slcli_build_node_abi_windows_source {
 struct slcli_build_windows_member members[SLCLI_BUILD_NODE_INPUTS];
};
struct slcli_build_node_input_windows {
 struct slcli_build_held_file_windows held_original;
 struct slcli_build_windows_open *ancestors_original;
 struct slcli_windows_file_capture *reads_original;
 unsigned char *body_original;
};
struct slcli_build_node_api_symbol_windows {
 size_t import_header_offset,export_name_offset;
 uint32_t export_rva;
 uint32_t imports_found,exports_found;
};
struct slcli_build_node_archive_windows {
 struct slcli_build_node_api_symbol_windows symbols[SLCLI_BUILD_NODE_API_IMPORTS];
 size_t cursor,members,short_imports;
};
/* Pure bounded structural parser, also used by labelled byte fixtures. It
 * cannot construct held/loaded/native authority, a callback or callable ABI. */
int slcli_build_node_archive_windows_parse_original(struct slcli_build_node_archive_windows *,const unsigned char *,size_t);
int slcli_build_node_abi_windows_source_output_away_original(const struct slcli_build_node_abi_windows_source *,const void *,size_t);
struct slcli_build_node_abi_windows {
 const struct slcli_build_node_abi_windows_source *source_original;
 const struct slcli_build_bootstrap_windows *bootstrap_original;
 struct slcli_build_node_input_windows inputs[SLCLI_BUILD_NODE_INPUTS];
 struct slcli_build_node_archive_windows archive_original;
 struct slcli_build_pe64_geometry_windows image_geometry;
 struct slcli_native_clock before_parse,after_parse;
 size_t inputs_acquired,export_names_checked;
 uint32_t started,failed,byte_tuple_corresponds;
};
/* Actual held seven-file acquisition + complete bounded import/export byte
 * correspondence for the selected Node22.23.2/default Node-API8 seed surface.
 * No loaded module, callable function, reservation, signature or ROOT authority
 * can be minted by this result. All native load/call gates remain independent. */
int slcli_build_node_abi_windows_capture_original(struct slcli_build_node_abi_windows *,
 struct slcli_build_bootstrap_windows *,const struct slcli_build_node_abi_windows_source *);
#endif
#endif
