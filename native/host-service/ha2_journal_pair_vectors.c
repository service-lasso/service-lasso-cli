#include "ha2_journal_pair_vectors.h"
#if defined(_WIN32) || defined(__linux__)
int slcli_ha2_journal_boundary_positive_vector(struct slcli_ha2_journal_pair *p,
  const unsigned char *const *prefix,uint32_t count,const unsigned char candidate[772],
  slcli_input_handle first,slcli_input_handle second,uint64_t store,
  slcli_store_capture_sink sink,void *owner) {
  unsigned i;
  if(count!=338&&count!=339&&count!=1023)return 0;
  if(!slcli_ha2_journal_pair_original(p,prefix,count,candidate,first,second,store,sink,owner)||
     !p->complete||p->failed||p->final_bytes!=(uint64_t)(count+1u)*SLCLI_HA2_JOURNAL_BYTES||
     p->final_bytes>SLCLI_HA2_JOURNAL_FILE_BYTES||p->retained_store_bytes!=store+1544u)return 0;
  for(i=0;i<2;i++)if(!p->before[i]->held_verified||!p->write[i]->held_verified||
    !p->after[i]->held_verified||p->before[i]->failed||p->write[i]->failed||p->after[i]->failed||
    p->before[i]->offset!=p->prior_bytes||p->write[i]->offset!=p->final_bytes||
    p->after[i]->offset!=p->final_bytes)return 0;
  return 1;
}
int slcli_ha2_journal_bound_boundary_positive_vector(struct slcli_ha2_journal_pair *p,
  const unsigned char *const *prefix,uint32_t count,const unsigned char candidate[772],
  slcli_input_handle first,slcli_input_handle second,uint64_t store,
  slcli_store_capture_sink sink,void *owner,const struct slcli_source_deadline_original *d,
  const void *caller,size_t caller_bytes,const struct slcli_native_clock *prior,size_t owner_bytes) {
  struct slcli_store_deadline_owner *o;size_t rows[6];unsigned i;uint64_t tick;
  if(!slcli_ha2_journal_pair_deadline_bind(p,d,caller,caller_bytes,prior,sink,owner,owner_bytes)||
     !slcli_ha2_journal_boundary_positive_vector(p,prefix,count,candidate,first,second,store,sink,owner)||
     !(o=p->deadline_owner)||!slcli_store_deadline_ready(o)||o->count!=6||
     o->deadline!=d||o->caller_original!=caller||o->predecessor!=prior||
     o->capture_original!=owner||o->capture_original_bytes!=owner_bytes)return 0;
  for(i=0;i<6;i++) {
    if(o->used[i]!=o->streams[i]->capture_count)return 0;
    rows[i]=o->streams[i]->capture_count;
  }
  tick=o->previous_tick;
  if(slcli_ha2_journal_pair_original(p,prefix,count,candidate,first,second,store,sink,owner))return 0;
  for(i=0;i<6;i++)if(rows[i]!=o->streams[i]->capture_count||rows[i]!=o->used[i])return 0;
  return p->complete&&!p->failed&&slcli_store_deadline_ready(o)&&o->previous_tick==tick;
}
int slcli_ha2_journal_exhaustion_vector(struct slcli_ha2_journal_pair *p,
  const unsigned char *const *prefix,const unsigned char candidate[772],
  slcli_input_handle first,slcli_input_handle second,uint64_t store,
  slcli_store_capture_sink sink,void *owner) {
  unsigned i;struct slcli_store_stream *s[6];
  if(!p||!p->arena||!prefix||!candidate||p->started||p->failed||p->complete||
     p->retained_store_bytes||!slcli_arena_live_range(p->arena,p,sizeof(*p))||
     !slcli_arena_live_range(p->arena,prefix,1024*sizeof(*prefix))||
     !slcli_arena_live_range(p->arena,candidate,SLCLI_HA2_JOURNAL_BYTES))return 0;
  for(i=0;i<2;i++){s[3*i]=p->before[i];s[3*i+1]=p->write[i];s[3*i+2]=p->after[i];}
  for(i=0;i<6;i++)if(!s[i]||s[i]->attempted||s[i]->capture_count||s[i]->offset||
    s[i]->buffered||s[i]->held_verified||s[i]->failed)return 0;
  if(slcli_ha2_journal_pair_original(p,prefix,1024,candidate,first,second,store,sink,owner)||
    p->started||p->failed||p->complete||p->retained_store_bytes||p->candidate||p->committed)return 0;
  for(i=0;i<6;i++)if(s[i]->attempted||s[i]->capture_count||s[i]->offset||s[i]->buffered||
    s[i]->held_verified||s[i]->failed)return 0;
  return 1;
}
#endif
