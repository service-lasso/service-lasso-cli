#ifndef SLCLI_PRODUCT_SOURCE_WINDOWS_H
#define SLCLI_PRODUCT_SOURCE_WINDOWS_H
#include "../host-service/native_clock.h"
#include "../host-service/bq_provider_expectations.h"
#if defined(_WIN32)
struct slcli_build_initial_windows_region;
/* Private INITIAL preparation input. Its independently admitted occurrence pays
 * all pre-operation acquisition/control/native costs. These expectations are not
 * an issuer, serialized grant, or a product clock which can later be replaced. */
struct slcli_product_initial_windows_source_span {
 const void *original;size_t bytes;
 const void *actor_original;size_t actor_bytes;
 const void *occurrence_original;size_t occurrence_bytes;
};
struct slcli_product_storage_windows_source {
 const struct slcli_native_clock *initial_origin_original,*initial_prior_original;
 uint64_t initial_expires,operation_expires,owner_limit,arena_bytes;
 const struct slcli_build_initial_windows_region *regions_original;
 size_t region_count;
 const struct slcli_bq_owner_resource_profile *profile_original;
 const struct slcli_bq_expected_principal *effective_original,*primary_original;
 const struct slcli_bq_provider_expectations *initial_source_original;
 const struct slcli_product_initial_windows_source_span *source_graph_original;
 size_t source_graph_count;
 unsigned char birth[32],image[32],owner[32],source[32];
};
/* Readonly separation only. No graph-completeness/source/principal/finance/
 * native/deadline/family authority follows from a true result. */
int slcli_product_windows_initial_source_excludes_output(
 const struct slcli_product_storage_windows_source *,const void *,size_t);
#endif
#endif
