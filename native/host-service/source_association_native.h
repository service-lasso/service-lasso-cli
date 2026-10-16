/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_SOURCE_ASSOCIATION_NATIVE_H
#define SLCLI_SOURCE_ASSOCIATION_NATIVE_H
#include "source_association_index.h"
#include "input_deadline.h"
#if defined(_WIN32) || defined(__linux__)
struct slcli_source_association_native_range {
 struct slcli_input_body *body;
 uint64_t offset,bytes;
 size_t deadline_storage_bytes;
 /* Header range has no member. Record routing is retained before the final
  * clock; decoded alone cannot establish a completed observation/admission. */
 struct slcli_source_association_record member;
 uint32_t member_decoded;
 struct slcli_native_clock after;
 int read_result,after_result,decode_result;
 uint32_t read_returned,after_returned,decode_returned;
};
struct slcli_source_association_native {
 struct slcli_arena *arena;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_native_clock *prepare_prior_original,*read_prior_original;
 const struct slcli_native_clock *origin_original;
 uint64_t origin_tick,expires;
 slcli_input_handle original;
 uint64_t file_bytes,record_count,names_bytes;
 struct slcli_input_body *const *bodies_original;
 struct slcli_source_association_native_range *ranges;

 size_t range_count,attempted,completed;
 size_t table_storage_charge,preflighted_bodies;
 struct slcli_source_association_reader reader;
 struct slcli_native_clock before;
 struct slcli_native_clock prepare_after;
 int prepare_after_result;uint32_t prepare_after_returned,prepared,read_started;
 int before_result;uint32_t before_returned,started,failed,table_observed;
};
/* No allocation or source admission. Exact preowned input originals retain
 * both reads/captures independently, including all failures. Full member/Git
 * reads, A/actor/seal/rights/source authority remain separately required. */
/* Complete storage preparation precedes source-object acquisition. It does
 * not acquire a source handle or admit the source. No allocation occurs. */
int slcli_source_association_native_table_prepare_original(struct slcli_source_association_native *,
 struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,uint64_t,uint64_t,uint64_t,struct slcli_input_body *const *,
 struct slcli_source_association_native_range *,size_t);
int slcli_source_association_native_table_prepared_matches_original(
 const struct slcli_source_association_native *);
int slcli_source_association_native_table_read_prepared_original(struct slcli_source_association_native *,
 slcli_input_handle,const struct slcli_native_clock *);
/* Complete returning native/readback/structural observation reconstruction.
 * Still no authentic producer/A/member/provider/publication relation. */
int slcli_source_association_native_matches_original(const struct slcli_source_association_native *);
#endif
#endif
