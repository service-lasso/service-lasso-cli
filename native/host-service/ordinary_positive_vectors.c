#include "ordinary_positive_vectors.h"
#include "store_publish_deadline.h"
#if defined(_WIN32) || defined(__linux__)
int slcli_positive_ledger_commit_vector(struct slcli_primary_engine *e,
                                        const struct slcli_primary_commit_originals *o) {
  uint64_t generation;uint8_t slot;unsigned char hash[32];
  if(!e||!e->ledger||!o||!o->commit||e->committed_generation==UINT64_MAX||e->selected_slot>1)return 0;
  generation=e->committed_generation+1;slot=(uint8_t)(1-e->selected_slot);
  if(!slcli_primary_engine_commit_original(e,o)){e->failed=1;return 0;}
  /* This vector requires actual generation write/held replay/rename/reopen
   * replay and selector write/held replay/rename/reopen replay, not a codec
   * expected-byte comparison or an all-denial result. */
  if(!o->commit->complete||!o->commit->durable_pair_complete||e->committed_pair_original!=o->commit||
     o->commit->failed||o->commit->committed_generation!=generation||
     o->commit->selected_slot!=slot)return 0;
  if(!slcli_ledger_selector_read(o->commit->selector,e->key,&slot,&generation,hash))return 0;
  return o->commit->deadline_owner&&slcli_store_deadline_ready(o->commit->deadline_owner)&&
    o->commit->deadline_owner->count==6&&
    o->commit->generation_publish.deadline_owner&&o->commit->selector_publish.deadline_owner&&
    slcli_store_publish_deadline_ready(&o->commit->generation_publish)&&
    slcli_store_publish_deadline_ready(&o->commit->selector_publish)&&
    o->commit->deadline_owner->deadline==o->deadline_original&&
    o->commit->deadline_owner->caller_bytes==o->caller_original_bytes&&
    o->commit->deadline_owner->capture_original==o->capture_original&&
    o->commit->deadline_owner->capture_original_bytes==o->capture_original_bytes&&
    slot==o->commit->selected_slot&&generation==o->commit->committed_generation&&
    slcli_equal(hash,o->commit->payload_hash,32);
}
int slcli_failed_pair_frontier_vector(const struct slcli_primary_engine *e,
  const struct slcli_primary_commit_originals *o,const struct slcli_ledger_state *before,
  uint64_t generation,uint32_t slot,uint32_t sequence,const unsigned char hash[32]) {
  uintptr_t a=(uintptr_t)before,b;
  if(!e||!o||!o->commit||!before||!hash||!e->ledger||!e->candidate||generation==UINT64_MAX)return 0;
  b=(uintptr_t)e->ledger;
  if(a>UINTPTR_MAX-sizeof(*before)||b>UINTPTR_MAX-sizeof(*e->ledger)||
     (a<b+sizeof(*e->ledger)&&b<a+sizeof(*before)))return 0;
  return e->failed&&e->candidate_started&&o->commit->started&&o->commit->failed&&
    !o->commit->complete&&!o->commit->durable_pair_complete&&e->committed_generation==generation&&e->selected_slot==slot&&
    e->next==sequence&&e->candidate_base_generation==generation&&
    slcli_equal(e->payload_hash,hash,32)&&
    slcli_equal((const unsigned char *)e->ledger,(const unsigned char *)before,sizeof(*before))&&
    e->candidate->generation==generation+1;
}
int slcli_late_pair_frontier_vector(struct slcli_primary_engine *e,
  const struct slcli_primary_commit_originals *o) {
  uint64_t generation;uint32_t slot,sequence;size_t counts[6];unsigned i;
  struct slcli_store_stream *streams[6];
  if(!e||!o||!o->commit||e->failed||e->committed_generation==UINT64_MAX||e->selected_slot>1||
     o->commit->started||o->commit->durable_pair_complete)return 0;
  generation=e->committed_generation+1;slot=1-e->selected_slot;sequence=e->next;
  /* The independently admitted native fixture must actually complete the
   * pair and then encounter a real late deadline/publication failure. This
   * calls the ordinary method; it does not install a fake completion bit. */
  if(slcli_primary_engine_commit_original(e,o)||!e->failed||
     !o->commit->durable_pair_complete||e->committed_pair_original!=o->commit||
     e->committed_generation!=generation||e->selected_slot!=slot||e->next!=sequence||
     o->commit->committed_generation!=generation||o->commit->selected_slot!=slot||
     !slcli_equal(e->payload_hash,o->commit->payload_hash,32))return 0;
  streams[0]=o->commit->generation_write;streams[1]=o->commit->generation_held;
  streams[2]=o->commit->generation_reopened;streams[3]=o->commit->selector_write;
  streams[4]=o->commit->selector_held;streams[5]=o->commit->selector_reopened;
  for(i=0;i<6;i++){if(!streams[i]||!streams[i]->held_verified)return 0;counts[i]=streams[i]->capture_count;}
  if(slcli_primary_engine_commit_original(e,o))return 0;
  for(i=0;i<6;i++)if(streams[i]->capture_count!=counts[i])return 0;
  return e->failed&&e->committed_pair_original==o->commit&&
    e->committed_generation==generation&&e->selected_slot==slot&&e->next==sequence&&
    o->commit->durable_pair_complete&&slcli_equal(e->payload_hash,o->commit->payload_hash,32);
}
int slcli_positive_namespace_publish_vector(struct slcli_store_publish *p,
  const struct slcli_store_stream *s,slcli_input_handle root,const unsigned char nonce[16],uint32_t leaf) {
  struct slcli_store_publish_deadline_owner *o;uint32_t count;uint64_t tick;
  if(!p||!(o=p->deadline_owner)||!o->stream_original||p->started||p->failed||p->completed)return 0;
  if(!slcli_store_publish_original(p,s,root,nonce,leaf)||!p->completed||p->failed||
     !slcli_store_publish_deadline_ready(p))return 0;
  count=o->count;tick=o->stream_original->previous_tick;
  if(slcli_store_publish_original(p,s,root,nonce,leaf))return 0;
  return p->completed&&!p->failed&&!o->failed&&!o->pending&&o->count==count&&
    o->stream_original->previous_tick==tick&&slcli_store_publish_deadline_ready(p);
}
int slcli_failed_namespace_repeat_vector(struct slcli_store_publish *p,
  const struct slcli_store_stream *s,slcli_input_handle root,const unsigned char nonce[16],uint32_t leaf) {
  struct slcli_store_publish_deadline_owner *o;uint32_t count,pending;uint64_t tick;
  const void *external;size_t external_bytes;struct slcli_native_clock *before;
  if(!p||!(o=p->deadline_owner)||!o->stream_original||!p->started||!p->failed||
     p->completed||!o->failed||!o->stream_original->failed||!o->count||
     !p->effect_called[0]||!p->effect_returned[0])return 0;
  count=o->count;pending=o->pending;tick=o->stream_original->previous_tick;
  external=o->stream_original->external_original;external_bytes=o->stream_original->external_bytes;
  before=o->stream_original->external_before;
  if(slcli_store_publish_original(p,s,root,nonce,leaf))return 0;
  return p->failed&&!p->completed&&o->failed&&o->stream_original->failed&&
    o->count==count&&o->pending==pending&&o->stream_original->previous_tick==tick&&
    o->stream_original->external_original==external&&
    o->stream_original->external_bytes==external_bytes&&o->stream_original->external_before==before;
}
int slcli_positive_archive_copy_vector(struct slcli_archive_copy *c,slcli_input_handle source,
                                       slcli_input_handle output,slcli_input_handle root,uint64_t bytes,
                                       const unsigned char hash[32],slcli_store_capture_sink sink,void *owner) {
  if(!slcli_archive_copy_original(c,source,output,root,bytes,hash,sink,owner))return 0;
  return c->complete&&!c->failed&&c->expected==bytes&&c->offset==bytes&&
    c->capture_count>0&&slcli_equal(c->digest,hash,32);
}
int slcli_positive_service_dispatch_vector(struct slcli_service_request *s,struct slcli_primary_engine *e,
                                           struct slcli_native_clock *clock,
                                           const struct slcli_primary_commit_originals *o,
                                           uint8_t response,uint64_t generation) {
  if(!s||!s->complete||!s->input_file||!s->input_file->sealed||s->input_file->failed||
     !slcli_service_request_dispatch(s,e,clock,o))return 0;
  return s->dispatched&&s->response_retained&&!s->failed&&s->response_file->sealed&&
    !s->response_file->failed&&s->response.body[0]==response&&
    e->committed_generation==generation&&!e->failed;
}
#endif
