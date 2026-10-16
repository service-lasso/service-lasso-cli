#ifndef SLCLI_BUILD_SEED_COMPILE_WINDOWS_H
#define SLCLI_BUILD_SEED_COMPILE_WINDOWS_H
#include "bootstrap_windows.h"
#include "command_windows.h"
#include "child_principal_windows.h"
#include "loaded_file_windows.h"
#if defined(_WIN32)
/* Private fixed first seed compile, not argv/callback/public registration.
 * The fixed caller's whole source/SDK/header/include closure is held before
 * invoking this body. Further compilation/link/load remain the SAME schedule. */
struct slcli_build_seed_compile_windows_source {
 const struct slcli_build_held_file_windows *translation_unit_original;
 const WCHAR *object_original,*environment_original,*cwd_original;
 size_t object_units,environment_units,cwd_units;
 const struct slcli_build_windows_path *includes_original;
 size_t include_count,command_capacity,attribute_capacity;
 DWORD pipe_capacity;
};
struct slcli_build_seed_compile_windows {
 const struct slcli_build_bootstrap_windows *bootstrap_original;
 const struct slcli_build_seed_compile_windows_source *source_original;
 struct slcli_build_windows_command_source command_source_original;
 struct slcli_build_windows_command command_original;
 struct slcli_build_child_principal_windows child_original;
 struct slcli_build_loaded_file_windows image_original;
 WCHAR *command_original_bytes,*command_mutable_original;
 void *attributes_original;
 struct slcli_windows_file_capture *image_reads_original;
 struct slcli_native_clock *image_clocks_original;
 unsigned char *image_body_original;
 uint32_t started,failed,stopped_inputs_joined;
};
/* Consumes the actual held compiler in bootstrap_original and issues its real
 * fixed compile birth. Actual child/event/token/image observations stay held
 * and STOPPED. This does not continue a partially joined image, claim compiler
 * completion or manufacture the missing complete event/source/stream owner. */
int slcli_build_seed_compile_windows_birth_original(struct slcli_build_seed_compile_windows *,
 struct slcli_build_bootstrap_windows *,const struct slcli_build_seed_compile_windows_source *);
#endif
#endif
