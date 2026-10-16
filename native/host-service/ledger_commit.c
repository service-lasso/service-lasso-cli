#include "ledger_commit.h"
#if defined(_WIN32) || defined(__linux__)
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_ledger_commit_deadline_bind(struct slcli_arena *a,struct slcli_ledger_commit *c,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  const struct slcli_native_clock *prior,slcli_store_capture_sink sink,void *owner,size_t owner_bytes) {
  struct slcli_store_stream *s[6];
  if(!a||!c||!slcli_arena_live_block(a,c,sizeof(*c))||
     c->started||c->failed||c->complete||c->durable_pair_complete||c->deadline_owner||
     c->caller_attempted||c->caller_returned)return 0;
  s[0]=c->generation_write;s[1]=c->generation_held;s[2]=c->generation_reopened;
  s[3]=c->selector_write;s[4]=c->selector_held;s[5]=c->selector_reopened;
  c->deadline_owner=slcli_store_deadline_bind(a,s,6,d,caller,caller_bytes,prior,sink,owner,owner_bytes);
  if(!c->deadline_owner)return 0;
  if(!slcli_store_deadline_away(c->deadline_owner,c,sizeof(*c))||
     !slcli_store_deadline_ready(c->deadline_owner)){c->failed=1;return 0;}return 1;
}
static int streams_disjoint(struct slcli_ledger_commit *c,
                              const struct slcli_ledger_state *l,const unsigned char *key) {
  struct slcli_store_stream *s[6];unsigned i,j;size_t n,m;
  s[0]=c->generation_write;s[1]=c->generation_held;s[2]=c->generation_reopened;
  s[3]=c->selector_write;s[4]=c->selector_held;s[5]=c->selector_reopened;
  if((c->deadline_owner&&(!slcli_store_deadline_ready(c->deadline_owner)||
       !slcli_store_deadline_away(c->deadline_owner,l,sizeof(*l))||
       !slcli_store_deadline_away(c->deadline_owner,key,32)))||
     !separate(c,sizeof(*c),l,sizeof(*l))||!separate(c,sizeof(*c),key,32))return 0;
  for(i=0;i<6;i++) {
    if(!s[i]||!s[i]->captures||!s[i]->capture_capacity||
       s[i]->capture_capacity>SIZE_MAX/sizeof(*s[i]->captures)||s[i]->attempted)return 0;
    n=s[i]->capture_capacity*sizeof(*s[i]->captures);
    if(!separate(s[i],sizeof(*s[i]),c,sizeof(*c))||!separate(s[i]->captures,n,c,sizeof(*c))||
       !separate(s[i],sizeof(*s[i]),l,sizeof(*l))||!separate(s[i]->captures,n,l,sizeof(*l))||
       !separate(s[i],sizeof(*s[i]),key,32)||!separate(s[i]->captures,n,key,32)||
       !separate(s[i],sizeof(*s[i]),s[i]->captures,n))return 0;
    for(j=0;j<i;j++) {
      m=s[j]->capture_capacity*sizeof(*s[j]->captures);
      if(!separate(s[i],sizeof(*s[i]),s[j],sizeof(*s[j]))||
         !separate(s[i],sizeof(*s[i]),s[j]->captures,m)||
         !separate(s[i]->captures,n,s[j],sizeof(*s[j]))||
         !separate(s[i]->captures,n,s[j]->captures,m))return 0;
    }
  }
  return 1;
}
static int selector_pass(struct slcli_store_stream *s,slcli_input_handle h,uint32_t mode,
                          const unsigned char raw[74],slcli_store_capture_sink sink,void *owner) {
  return slcli_store_stream_begin(s,h,mode,74,sink,owner)&&
    slcli_store_stream_append(s,raw,74)&&slcli_store_stream_finish(s);
}
static int commit_body(struct slcli_ledger_commit *c,
                                  const struct slcli_ledger_state *l,const unsigned char key[32],
                                  slcli_input_handle root,slcli_input_handle generation_temp,
                                  slcli_input_handle selector_temp,uint8_t inactive,
                                  uint64_t prior_retained_bytes,slcli_store_capture_sink sink,void *owner) {
  uint32_t payload_bytes,frame_bytes,actual;unsigned char first[32],check[32];uint64_t reserve;
  if(!c||!l||!key||!sink||!owner||c->started||c->failed||c->complete||c->durable_pair_complete||inactive>1||
     generation_temp==selector_temp||root==generation_temp||root==selector_temp||
     !streams_disjoint(c,l,key)||!slcli_ledger_payload(l,0,0,&payload_bytes,first))return 0;
  frame_bytes=payload_bytes+36;
  /* Both physical generations and selector transaction copies remain charged.
   * This conservative reservation is not native TotalBytes or retirement; the
   * owning engine must reconcile the complete original store inventory before
   * genuine reuse. Unknown outstanding objects cannot reduce this charge. */
  reserve=2*(uint64_t)frame_bytes+148;
  if(prior_retained_bytes>SLCLI_STORE_LIMIT||reserve>SLCLI_STORE_LIMIT-prior_retained_bytes)return 0;
  c->started=1;c->failed=1;c->retained_store_bytes=prior_retained_bytes+reserve;
  if(!slcli_store_stream_begin(c->generation_write,generation_temp,0,frame_bytes,sink,owner)||
     !slcli_store_ledger_write(c->generation_write,l,key,&actual,c->payload_hash)||actual!=frame_bytes||
     !slcli_equal(first,c->payload_hash,32)||
     !slcli_store_stream_begin(c->generation_held,generation_temp,1,frame_bytes,sink,owner)||
     !slcli_store_ledger_compare(c->generation_held,l,key,frame_bytes,check)||
     !slcli_equal(check,c->payload_hash,32))return 0;
  if(!slcli_store_publish_original(&c->generation_publish,c->generation_write,root,
       c->generation_nonce,inactive?SLCLI_GENERATION_B:SLCLI_GENERATION_A)||
     !slcli_store_stream_begin(c->generation_reopened,c->generation_publish.reopened,1,
       frame_bytes,sink,owner)||
     !slcli_store_ledger_compare(c->generation_reopened,l,key,frame_bytes,check)||
     !slcli_equal(check,c->payload_hash,32))return 0;
  if(!slcli_ledger_selector(c->selector,inactive,l->generation,c->payload_hash,key)||
     !selector_pass(c->selector_write,selector_temp,0,c->selector,sink,owner)||
     !selector_pass(c->selector_held,selector_temp,1,c->selector,sink,owner)||
     !slcli_store_publish_original(&c->selector_publish,c->selector_write,root,
       c->selector_nonce,SLCLI_SELECTOR)||
     !selector_pass(c->selector_reopened,c->selector_publish.reopened,1,c->selector,sink,owner))return 0;
  /* Retain the actual successor before checking any remaining transition
   * predicate. Both native publications and held/reopened readbacks above
   * have completed; rejecting this operation later cannot restore the prior
   * selector or erase these original stream/capture owners. */
  c->committed_generation=l->generation;c->selected_slot=inactive;
  c->durable_pair_complete=1;
  if(c->deadline_owner&&!slcli_store_deadline_ready(c->deadline_owner))return 0;
  c->complete=1;c->failed=0;
  return 1;
}
int slcli_ledger_commit_original(struct slcli_ledger_commit *c,
  const struct slcli_ledger_state *l,const unsigned char key[32],slcli_input_handle root,
  slcli_input_handle generation_temp,slcli_input_handle selector_temp,uint8_t inactive,
  uint64_t prior_retained_bytes,slcli_store_capture_sink sink,void *owner) {
  int returned;
  if(!c||c->caller_attempted||c->caller_returned)return 0;
  c->caller_attempted=1;
  returned=commit_body(c,l,key,root,generation_temp,selector_temp,inactive,prior_retained_bytes,sink,owner);
  c->caller_result=returned;c->caller_returned=1;return returned;
}
#endif
