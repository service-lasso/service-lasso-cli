#include "ha2_journal_pair.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
struct slcli_ha2_journal_pair *slcli_ha2_journal_pair_allocate(struct slcli_arena *a,size_t rows) {
  struct slcli_ha2_journal_pair *p;unsigned i;
  if(!a||!rows)return 0;p=slcli_arena_calloc(a,1,sizeof(*p));if(!p)return 0;p->arena=a;
  for(i=0;i<2;i++) {
    p->before[i]=slcli_store_stream_allocate(a,rows);
    p->write[i]=slcli_store_stream_allocate(a,rows);
    p->after[i]=slcli_store_stream_allocate(a,rows);
    if(!p->before[i]||!p->write[i]||!p->after[i]){p->failed=1;return p;}
  }
  return p;
}
int slcli_ha2_journal_pair_deadline_bind(struct slcli_ha2_journal_pair *p,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  const struct slcli_native_clock *prior,slcli_store_capture_sink sink,void *owner,size_t owner_bytes) {
  struct slcli_store_stream *s[6];unsigned i;
  if(!p||!p->arena||!slcli_arena_live_block(p->arena,p,sizeof(*p))||
     p->started||p->failed||p->complete||p->deadline_owner)return 0;
  for(i=0;i<2;i++){s[3*i]=p->before[i];s[3*i+1]=p->write[i];s[3*i+2]=p->after[i];}
  p->deadline_owner=slcli_store_deadline_bind(p->arena,s,6,d,caller,caller_bytes,prior,sink,owner,owner_bytes);
  if(!p->deadline_owner)return 0;
  if(!slcli_store_deadline_away(p->deadline_owner,p,sizeof(*p))||
     !slcli_store_deadline_ready(p->deadline_owner)){p->failed=1;return 0;}return 1;
}
static int distinct(const slcli_input_capture *a,const slcli_input_capture *b) {
#if defined(_WIN32)
  return a->identity.VolumeSerialNumber!=b->identity.VolumeSerialNumber||
    !slcli_equal(a->identity.FileId.Identifier,b->identity.FileId.Identifier,16);
#else
  return a->original_stat.st_dev!=b->original_stat.st_dev||
    a->original_stat.st_ino!=b->original_stat.st_ino;
#endif
}
static int compare(struct slcli_store_stream *s,slcli_input_handle h,
  const unsigned char *const *records,uint32_t count,const unsigned char *last,
  slcli_store_capture_sink sink,void *owner) {
  uint32_t i;uint64_t bytes=(uint64_t)(count+(last?1u:0u))*SLCLI_HA2_JOURNAL_BYTES;
  if(!slcli_store_journal_compare_begin(s,h,bytes,sink,owner))return 0;
  for(i=0;i<count;i++)if(!slcli_store_stream_append(s,records[i],SLCLI_HA2_JOURNAL_BYTES))return 0;
  return (!last||slcli_store_stream_append(s,last,SLCLI_HA2_JOURNAL_BYTES))&&
    slcli_store_stream_finish(s);
}
int slcli_ha2_journal_pair_original(struct slcli_ha2_journal_pair *p,
  const unsigned char *const *records,uint32_t count,const unsigned char candidate[772],
  slcli_input_handle first,slcli_input_handle second,uint64_t prior_store,
  slcli_store_capture_sink sink,void *owner) {
  struct slcli_ha2_journal_record r;unsigned i,j;uint64_t reserve=2u*SLCLI_HA2_JOURNAL_BYTES;
  struct slcli_sha256 chain;unsigned char previous_hash[32]={0};
  slcli_input_handle h[2];struct slcli_store_stream *all[6];
  if(!p||!p->arena||p->started||p->failed||count>=1024||!candidate||!sink||!owner||
     (p->deadline_owner&&(!slcli_store_deadline_ready(p->deadline_owner)||
       !slcli_store_deadline_away(p->deadline_owner,candidate,SLCLI_HA2_JOURNAL_BYTES)||
       (count&&!slcli_store_deadline_away(p->deadline_owner,records,count*sizeof(*records)))))||
     first==second||prior_store>SLCLI_STORE_LIMIT||reserve>SLCLI_STORE_LIMIT-prior_store||
     !slcli_arena_live_range(p->arena,p,sizeof(*p))||
     !slcli_arena_live_range(p->arena,candidate,SLCLI_HA2_JOURNAL_BYTES)||
     !away(p,sizeof(*p),candidate,SLCLI_HA2_JOURNAL_BYTES)||
     (count&&(!records||!slcli_arena_live_range(p->arena,records,count*sizeof(*records))||
       !away(p,sizeof(*p),records,count*sizeof(*records))||
       !away(candidate,SLCLI_HA2_JOURNAL_BYTES,records,count*sizeof(*records)))))return 0;
  if(!slcli_ha2_journal_record_parse(candidate,SLCLI_HA2_JOURNAL_BYTES,&r)||r.sequence!=count+1u)return 0;
  for(i=0;i<count;i++) {
    if((p->deadline_owner&&!slcli_store_deadline_away(p->deadline_owner,records[i],SLCLI_HA2_JOURNAL_BYTES))||
       !slcli_arena_live_range(p->arena,records[i],SLCLI_HA2_JOURNAL_BYTES)||
       !away(records[i],SLCLI_HA2_JOURNAL_BYTES,p,sizeof(*p))||
       !away(records[i],SLCLI_HA2_JOURNAL_BYTES,candidate,SLCLI_HA2_JOURNAL_BYTES)||
       !away(records[i],SLCLI_HA2_JOURNAL_BYTES,records,count*sizeof(*records))||
       !slcli_ha2_journal_record_parse(records[i],SLCLI_HA2_JOURNAL_BYTES,&r)||r.sequence!=i+1u||
       !slcli_equal(r.associations[17],previous_hash,32))return 0;
    for(j=0;j<i;j++)if(!away(records[i],SLCLI_HA2_JOURNAL_BYTES,records[j],SLCLI_HA2_JOURNAL_BYTES))return 0;
    slcli_sha256_init(&chain);
    if(!slcli_sha256_update(&chain,records[i],SLCLI_HA2_JOURNAL_BYTES)||
       !slcli_sha256_finish(&chain,previous_hash))return 0;
  }
  if(!slcli_ha2_journal_record_parse(candidate,SLCLI_HA2_JOURNAL_BYTES,&r)||
     !slcli_equal(r.associations[17],previous_hash,32))return 0;
  for(i=0;i<2;i++){all[i*3]=p->before[i];all[i*3+1]=p->write[i];all[i*3+2]=p->after[i];}
  for(i=0;i<6;i++) {
    size_t n;
    if(!all[i]||all[i]->attempted||!all[i]->captures||!all[i]->capture_capacity||
       all[i]->capture_capacity>SIZE_MAX/sizeof(*all[i]->captures)||
       !slcli_arena_live_range(p->arena,all[i],sizeof(*all[i])))return 0;
    n=all[i]->capture_capacity*sizeof(*all[i]->captures);
    if(!slcli_arena_live_range(p->arena,all[i]->captures,n)||
       !away(all[i],sizeof(*all[i]),p,sizeof(*p))||!away(all[i]->captures,n,p,sizeof(*p))||
       !away(all[i],sizeof(*all[i]),candidate,SLCLI_HA2_JOURNAL_BYTES)||
       !away(all[i]->captures,n,candidate,SLCLI_HA2_JOURNAL_BYTES))return 0;
    if(count&&(!away(all[i],sizeof(*all[i]),records,count*sizeof(*records))||
       !away(all[i]->captures,n,records,count*sizeof(*records))))return 0;
    for(j=0;j<count;j++)if(!away(all[i],sizeof(*all[i]),records[j],SLCLI_HA2_JOURNAL_BYTES)||
       !away(all[i]->captures,n,records[j],SLCLI_HA2_JOURNAL_BYTES))return 0;
    for(j=0;j<i;j++) {
      size_t m=all[j]->capture_capacity*sizeof(*all[j]->captures);
      if(!away(all[i],sizeof(*all[i]),all[j],sizeof(*all[j]))||
         !away(all[i],sizeof(*all[i]),all[j]->captures,m)||
         !away(all[i]->captures,n,all[j],sizeof(*all[j]))||
         !away(all[i]->captures,n,all[j]->captures,m))return 0;
    }
  }
  p->started=1;p->failed=1;p->committed=records;p->committed_count=count;p->candidate=candidate;
  p->prior_bytes=(uint64_t)count*SLCLI_HA2_JOURNAL_BYTES;
  p->final_bytes=p->prior_bytes+SLCLI_HA2_JOURNAL_BYTES;p->retained_store_bytes=prior_store+reserve;
  h[0]=first;h[1]=second;
  for(i=0;i<2;i++)if(count&&!compare(p->before[i],h[i],records,count,0,sink,owner))return 0;
  /* Both original EOF/identity captures exist before the first write. Distinct
   * handle values alone cannot distinguish two aliases to the same journal. */
  for(i=0;i<2;i++)if(!slcli_store_journal_append_begin(p->write[i],h[i],p->prior_bytes,
    p->final_bytes,sink,owner))return 0;
  for(i=0;i<2;i++)if(count&&!slcli_store_original_same(&p->before[i]->captures[0],
    &p->write[i]->captures[0]))return 0;
  if(!distinct(&p->write[0]->captures[0],&p->write[1]->captures[0]))return 0;
  for(i=0;i<2;i++)if(!slcli_store_stream_append(p->write[i],candidate,SLCLI_HA2_JOURNAL_BYTES)||
    !slcli_store_stream_finish(p->write[i]))return 0;
  for(i=0;i<2;i++)if(!compare(p->after[i],h[i],records,count,candidate,sink,owner))return 0;
  for(i=0;i<2;i++)if(!slcli_store_original_same(&p->write[i]->captures[0],
    &p->after[i]->captures[0]))return 0;
  if(p->deadline_owner&&!slcli_store_deadline_ready(p->deadline_owner))return 0;
  p->complete=1;p->failed=0;return 1;
}
#endif
