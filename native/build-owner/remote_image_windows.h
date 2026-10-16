#ifndef SLCLI_BUILD_REMOTE_IMAGE_WINDOWS_H
#define SLCLI_BUILD_REMOTE_IMAGE_WINDOWS_H
#include "loaded_file_windows.h"
#include "../host-service/arena.h"
#if defined(_WIN32)
struct slcli_build_pe64_geometry_windows {
 size_t image_bytes,headers_bytes,section_table_offset,section_count,page_bytes,capacity;
};
/* Pure structural byte geometry only. Fresh disjoint output; no native image,
 * membership, loaded mapping, prepared owner or qualification can be created. */
int slcli_build_pe64_geometry_windows_original(struct slcli_build_pe64_geometry_windows *,
 const unsigned char *,size_t,size_t);
struct slcli_build_remote_image_windows_row {
 struct slcli_build_windows_call query,read;
 MEMORY_BASIC_INFORMATION geometry_original;
 SIZE_T received_original;
 uintptr_t address_original;
 size_t bytes_original;
 uint32_t read_observed;
};
struct slcli_build_remote_image_windows {
 const struct slcli_build_held_file_windows *file_original;
 const struct slcli_build_loaded_file_windows *event_file_original;
 struct slcli_build_remote_image_windows_row *rows_original;
 unsigned char *body_original;
 unsigned char *expected_image_original;
 unsigned char *relocation_seen_original;
 struct slcli_build_windows_call issuing_thread;
 struct slcli_native_clock finish;
 struct slcli_native_clock bytes_before,bytes_after;
 uintptr_t base_original,cursor_original;
 uint64_t prior_tick,expires;
 size_t image_bytes,headers_bytes,section_table_offset,section_count,page_bytes,capacity,used;
 size_t relocation_cursor,relocations_processed,comparison_offset;
 uint32_t prepared,started,failed,geometry_complete,body_complete,headers_correspond;
 uint32_t initial_bytes_correspond;
};
/* Checked source-derived storage only, before tool birth; no native call or
 * image/source authority. Reserved/unreadable body spans are never read/proven. */
int slcli_build_remote_image_windows_prepare_original(struct slcli_build_remote_image_windows *,
 struct slcli_arena *,const struct slcli_build_held_file_windows *,size_t);
/* Native capture over an actual stopped root. No relocation/IAT/loader/source
 * conjunction or ContinueDebugEvent is implemented by a capture completion. */
int slcli_build_remote_image_windows_observe_original(struct slcli_build_remote_image_windows *,
 const struct slcli_build_loaded_file_windows *);
int slcli_build_remote_image_windows_initial_bytes_original(struct slcli_build_remote_image_windows *);
#endif
#endif
