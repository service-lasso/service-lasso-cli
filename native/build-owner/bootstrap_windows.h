#ifndef SLCLI_BUILD_BOOTSTRAP_WINDOWS_H
#define SLCLI_BUILD_BOOTSTRAP_WINDOWS_H
#include "held_file_windows.h"
#include "../host-service/platform_memory.h"
#include "../host-service/arena.h"
#if defined(_WIN32)
/* PRIVATE SAME-process seed caller operands, never wire/JS/external-pointer
 * authority. The independently admitted initial occurrence owns the complete
 * source/loaded/principal/profile join BEFORE this constructor's first effect.
 * Region list is the complete charged initial image/stack/TLS/control prefix;
 * each entry must be one actual VirtualQuery region, including reserved guards.
 * No initial storage or caller is created for free by these declarations. */
struct slcli_build_initial_windows_region {
 uintptr_t base; uint64_t bytes; DWORD state,type,protection; uint32_t category;
};
struct slcli_build_initial_windows_source {
 const struct slcli_native_clock *origin_original,*prior_original;
 uint64_t expires,owner_limit,arena_bytes;
 const struct slcli_build_initial_windows_region *regions_original;
 size_t region_count;
 const struct slcli_bq_expected_principal *effective_original,*primary_original;
 const struct slcli_build_windows_member *compiler_original;
 const struct slcli_bq_owner_resource_profile *profile_original;
 unsigned char birth[32],image[32],owner[32],source[32];
};
struct slcli_build_initial_windows_region_capture {
 struct slcli_build_windows_call query;
 MEMORY_BASIC_INFORMATION geometry_original;
 struct slcli_extent_request request_original;
};
struct slcli_build_bootstrap_windows {
 const struct slcli_build_initial_windows_source *source_original;
 struct slcli_memory_owner memory_original;
 SYSTEM_INFO pages_original;
 struct slcli_build_windows_call pages;
 struct slcli_build_initial_windows_region_capture *regions_original;
 struct slcli_memory_capture arena_acquisition_original;
 struct slcli_native_clock arena_before,arena_after;
 struct slcli_arena arena_original;
 struct slcli_build_principal_windows *effective_original,*primary_original;
 struct slcli_build_held_file_windows *compiler_original;
 struct slcli_build_windows_open *compiler_ancestors_original;
 struct slcli_windows_file_capture *compiler_reads_original;
 unsigned char *compiler_body_original;
 struct slcli_native_clock finish;
 uint64_t prior_tick;
 uint32_t started,failed,completed;
};
/* Caller preowns zeroed control/region rows inside the independently charged
 * initial prefix. The constructor retains all native/pending/failed results.
 * completed means first tool acquisition correspondence, NOT whole source,
 * loaded admission, compiler birth, numerical fit or runtime qualification. */
int slcli_build_bootstrap_windows_acquire_compiler_original(
 struct slcli_build_bootstrap_windows *,const struct slcli_build_initial_windows_source *,
 struct slcli_build_initial_windows_region_capture *);
#endif
#endif
