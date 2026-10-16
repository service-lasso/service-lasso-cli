#ifndef SLCLI_PRODUCT_STORAGE_WINDOWS_H
#define SLCLI_PRODUCT_STORAGE_WINDOWS_H
#include "product_source_windows.h"
#include "../build-owner/bootstrap_windows.h"
#include "../node-owner/transport_windows.h"
#if defined(_WIN32)
struct slcli_product_section_windows;
struct slcli_product_windows_deadline_publication;
struct slcli_product_windows_deadline_acquisition;
struct slcli_product_windows_operation_caller {
 const struct slcli_product_storage_windows_source *initial_source_original;
 struct slcli_memory_owner *memory_original;
 struct slcli_arena *arena_original;
 const struct slcli_source_deadline_original *deadline_original;
};
/* Control/ledger/arena descriptors live in the separately charged initial
 * prefix, OUTSIDE their arena payload. Every allocated pointer retains its own
 * exact logical allocation; failure never frees or resets an earlier output. */
struct slcli_product_storage_windows {
 const struct slcli_product_storage_windows_source *source_original;
 struct slcli_memory_owner memory_original;
 struct slcli_arena arena_original;
 SYSTEM_INFO pages_original;
 struct slcli_build_windows_call pages;
 struct slcli_build_initial_windows_region_capture *regions_original;
 struct slcli_memory_capture arena_acquisition_original;
 struct slcli_native_clock arena_before,arena_after,finish;
 struct slcli_native_clock section_entry_original;
 struct slcli_native_clock deadline_publication_entry_original;
 struct slcli_product_windows_operation_caller *caller_original;
 struct slcli_native_clock *operation_origin_original;
 struct slcli_source_deadline_original *deadline_original;
 struct slcli_node_transport_windows *transport_original;
 struct slcli_build_principal_windows *effective_original,*primary_original;
 struct slcli_product_section_windows *section_original;
 struct slcli_product_windows_deadline_publication *deadline_publication_original;
 struct slcli_product_windows_deadline_acquisition *deadline_acquisition_original;
 uint64_t prior_tick;
 uint32_t started,failed,origin_attempted,origin_returned,storage_constructed,section_attempted,prefix_prepared;
};
/* Constructs paid initial storage BEFORE publishing the first product operation
 * origin. It writes that original directly into its final separate live arena
 * cell. It cannot replace an existing caller/deadline/origin, create genuine
 * source membership, or activate a TUI/build/family operation. No child is born. */
int slcli_product_storage_windows_prepare_original(
 struct slcli_product_storage_windows *,const struct slcli_product_storage_windows_source *,
 struct slcli_build_initial_windows_region_capture *);
/* Paid INITIAL preparation only; it creates no product clock/deadline/caller.
 * Child native handoff acquisition runs under the retained INITIAL deadline
 * before the first final-cell local admission cut. No reset/renewal API. */
int slcli_product_storage_windows_prepare_prefix_original(
 struct slcli_product_storage_windows *,const struct slcli_product_storage_windows_source *,
 struct slcli_build_initial_windows_region_capture *);
int slcli_product_storage_windows_prefix_output_away_original(
 const struct slcli_product_storage_windows *,const void *,size_t);
int slcli_product_storage_windows_output_away_original(
 const struct slcli_product_storage_windows *,const void *,size_t);
#endif
#endif
