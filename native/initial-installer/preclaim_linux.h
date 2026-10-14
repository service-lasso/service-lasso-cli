#ifndef SLCLI_INITIAL_PRECLAIM_LINUX_H
#define SLCLI_INITIAL_PRECLAIM_LINUX_H
#include <stddef.h>
#include <stdint.h>
#include "../host-service/native_clock.h"
#include "../host-service/bq_provider_expectations.h"
/* Complete source roster; accounting category is correspondence only. No
 * caller-supplied row issues ROOT, principal or native qualification. */
struct slcli_initial_preclaim_request {
 uint64_t ordinal,category,user_bytes;
 struct slcli_bq_resource_cost effects;
};
struct slcli_initial_preclaim_row {
 struct slcli_initial_preclaim_request request;
 uint64_t payload_offset;
};
struct slcli_initial_launcher_linux;
struct slcli_initial_preclaim_plan_linux {
 uint64_t metadata_bytes,user_bytes,mapping_bytes;
 struct slcli_bq_owner_resource_profile resource_profile;
};
/* Pure geometry, transactional output; never source/finance qualification. */
int slcli_initial_preclaim_linux_plan_original(
 struct slcli_initial_preclaim_plan_linux *,
 const struct slcli_initial_preclaim_request *,uint64_t,uint64_t,uint64_t);
struct slcli_initial_preclaim_linux {
 const struct slcli_initial_launcher_linux *launcher_original;
 const struct slcli_initial_preclaim_request *requests_original;
 const struct slcli_native_clock *origin_original,*prior_original;
 uint64_t count,page_bytes,ceiling_bytes,expires,origin_tick,prior_tick;
 uint64_t metadata_bytes,user_bytes,mapping_bytes;
 struct slcli_bq_owner_resource_profile resource_profile;
 long args[6];
 long number_original,result_original;
 struct slcli_native_clock before,after,finish;
 uint32_t started,attempted,returned,initialized,failed;
};
/* Storage construction only, no child invocation. External exact launch owns
 * fresh capture/inputs/stack and this acquisition's failure/pending costs. */
int slcli_initial_preclaim_linux_capture(
 struct slcli_initial_preclaim_linux *,const struct slcli_initial_launcher_linux *,
 const struct slcli_initial_preclaim_request *,uint64_t,uint64_t,uint64_t,
 const struct slcli_native_clock *,const struct slcli_native_clock *,uint64_t);
int slcli_initial_preclaim_linux_matches_original(
 const struct slcli_initial_preclaim_linux *);
/* Pure checked SAME ledger subspan; refusal leaves output unchanged. */
int slcli_initial_preclaim_linux_span_original(
 const struct slcli_initial_preclaim_linux *,uint64_t,uint64_t,uint64_t,void **);
#endif
