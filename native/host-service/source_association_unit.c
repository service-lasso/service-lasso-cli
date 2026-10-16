/* SPDX-License-Identifier: Apache-2.0 */
#include "source_association_unit.h"
#if defined(_WIN32) || defined(__linux__)
static int apart(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_source_association_unit_matches_original(
 const struct slcli_source_association_native *a,const struct slcli_source_unit12_native *u){
 size_t i,j;
 if(!slcli_source_association_native_matches_original(a)||!slcli_source_unit12_native_matches_original(u)||
  a->arena!=u->arena||a->deadline!=u->deadline||a->caller!=u->caller||a->caller_bytes!=u->caller_bytes||
  a->origin_original!=u->origin_original||a->origin_tick!=u->origin_tick||a->expires!=u->expires||
  a->original==u->original||a->record_count!=u->reader.count||a->record_count!=u->range_count-1||
  !apart(a,sizeof(*a),u,sizeof(*u))||
  !apart(a->ranges,a->range_count*sizeof(*a->ranges),u->ranges,u->range_count*sizeof(*u->ranges))||
  !apart(a->bodies_original,a->range_count*sizeof(*a->bodies_original),
   u->bodies_original,u->range_count*sizeof(*u->bodies_original)))return 0;
 /* Retained tables are different actual originals, including every native
  * readback/capture/preclock block. Equal byte digests never collapse them. */
 for(i=0;i<a->range_count;i++){
  const struct slcli_source_association_native_range *ar=&a->ranges[i];
  for(j=0;j<u->range_count;j++){
   const struct slcli_source_unit12_native_range *ur=&u->ranges[j];
   if(!apart(ar->body,ar->body->deadline_owner->body_storage_bytes,
     ur->body,ur->body->deadline_owner->body_storage_bytes)||
    !apart(ar->body,ar->body->deadline_owner->body_storage_bytes,ur->body->deadline_owner,ur->deadline_storage_bytes)||
    !apart(ar->body->deadline_owner,ar->deadline_storage_bytes,ur->body,ur->body->deadline_owner->body_storage_bytes)||
    !apart(ar->body->deadline_owner,ar->deadline_storage_bytes,ur->body->deadline_owner,ur->deadline_storage_bytes))return 0;
  }
 }
 for(i=1;i<u->range_count;i++){
  const struct slcli_source_association_record *ar=&a->ranges[i].member;
  const struct slcli_source_unit12_record *ur=&u->ranges[i].member;
  if(ar->index!=i-1||ur->index!=i-1||ar->kind!=ur->kind||ar->project!=ur->source_project||
   ar->body_bytes!=ur->bytes||!slcli_equal(ar->association,ur->association,32)||
   !slcli_equal(ar->capture,ur->capture,32)||!slcli_equal(ar->digest,ur->digest,32))return 0;
 }
 /* No names/hash equality above stands in for native source authority or the
  * complete actual Git tree/commit/tag/non-Git provider/publication graph. */
 return 1;
}
#endif
