#include "journal_vectors.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int protected_original(struct slcli_management_journal_vector *v,const void *raw,size_t bytes) {
  unsigned i;struct slcli_store_stream *s[6];
  if(!v->record->signer||
     (v->pair->deadline_owner&&!slcli_store_deadline_away(v->pair->deadline_owner,raw,bytes))||
     !slcli_arena_live_range(v->arena,raw,bytes)||
     !away(raw,bytes,v,sizeof(*v))||!away(raw,bytes,v->record,sizeof(*v->record))||
     !away(raw,bytes,v->record->signer,sizeof(*v->record->signer))||
     !away(raw,bytes,v->pair,sizeof(*v->pair)))return 0;
  for(i=0;i<2;i++){s[3*i]=v->pair->before[i];s[3*i+1]=v->pair->write[i];s[3*i+2]=v->pair->after[i];}
  for(i=0;i<6;i++) {
    size_t n;
    if(!s[i]||!s[i]->captures||!s[i]->capture_capacity||
       s[i]->capture_capacity>SIZE_MAX/sizeof(*s[i]->captures)||
       !slcli_arena_live_range(v->arena,s[i],sizeof(*s[i])))return 0;
    n=s[i]->capture_capacity*sizeof(*s[i]->captures);
    if(!slcli_arena_live_range(v->arena,s[i]->captures,n)||
       !away(raw,bytes,s[i],sizeof(*s[i]))||!away(raw,bytes,s[i]->captures,n))return 0;
  }
  return 1;
}
struct slcli_management_journal_vector *slcli_management_journal_vector_allocate(
  struct slcli_arena *a,size_t rows) {
  struct slcli_management_journal_vector *v;
  if(!a||!rows)return 0;v=slcli_arena_calloc(a,1,sizeof(*v));if(!v)return 0;v->arena=a;
  v->record=slcli_management_record_vector_allocate(a);
  v->pair=slcli_ha2_journal_pair_allocate(a,rows);
  if(!v->record||!v->pair||v->pair->failed)v->failed=1;
  return v;
}
int slcli_management_journal_positive_vector(struct slcli_management_journal_vector *v,
  const struct slcli_ha2_journal_prefix_originals *frontier,
  const unsigned char seed[32],const unsigned char anchor[32],
  const unsigned char *operation,size_t operation_bytes,const unsigned char *const *prior,
  uint32_t count,slcli_input_handle first,slcli_input_handle second,uint64_t store_bytes,
  slcli_store_capture_sink sink,void *owner) {
  struct slcli_ha2_operation o;const unsigned char *last=0;unsigned i;
  if(!v||!v->arena||v->started||v->failed||!v->record||!v->pair||count>=1024||
     !frontier||!operation||!sink||!owner||
     !slcli_arena_live_range(v->arena,v,sizeof(*v))||
     !slcli_arena_live_range(v->arena,v->record,sizeof(*v->record))||
     !slcli_arena_live_range(v->arena,v->pair,sizeof(*v->pair))||
     !protected_original(v,frontier,sizeof(*frontier))||
     !protected_original(v,frontier->associations,SLCLI_HA2_JOURNAL_ASSOCIATIONS*32u)||
     !frontier->key_version_bytes||frontier->key_version_bytes>64||
     !protected_original(v,frontier->key_version,frontier->key_version_bytes)||
     !protected_original(v,seed,32)||!protected_original(v,anchor,32)||
     !protected_original(v,operation,operation_bytes)||
     !slcli_arena_live_range(v->arena,operation,operation_bytes)||
     !away(v,sizeof(*v),operation,operation_bytes)||
     !away(v->record,sizeof(*v->record),operation,operation_bytes)||
     !away(v->record->signer,sizeof(*v->record->signer),operation,operation_bytes)||
     !slcli_ha2_operation_parse(operation,operation_bytes,&o)||
     o.sequence!=count+1u||o.sequence!=frontier->sequence||o.generation!=frontier->generation)return 0;
  if(count) {
    if(!prior||!protected_original(v,prior,count*sizeof(*prior))||
       !away(v,sizeof(*v),prior,count*sizeof(*prior))||
       !away(v->record,sizeof(*v->record),prior,count*sizeof(*prior))||
       !away(v->record->signer,sizeof(*v->record->signer),prior,count*sizeof(*prior)))return 0;
    for(i=0;i<count;i++)if(!protected_original(v,prior[i],SLCLI_HA2_JOURNAL_BYTES)||
      !away(v,sizeof(*v),prior[i],SLCLI_HA2_JOURNAL_BYTES)||
      !away(v->record,sizeof(*v->record),prior[i],SLCLI_HA2_JOURNAL_BYTES)||
      !away(v->record->signer,sizeof(*v->record->signer),prior[i],SLCLI_HA2_JOURNAL_BYTES))return 0;
    last=prior[count-1];
  }
  v->started=1;v->failed=1;
  if(!slcli_management_record_positive_vector(v->record,frontier,seed,anchor)||
     !slcli_ha2_transition_compare(last,last?SLCLI_HA2_JOURNAL_BYTES:0,
       operation,operation_bytes,v->record->raw,SLCLI_HA2_JOURNAL_BYTES)||
     !slcli_ha2_journal_pair_original(v->pair,prior,count,v->record->raw,first,second,
       store_bytes,sink,owner)||!v->pair->complete||v->pair->failed)return 0;
  v->complete=1;v->failed=0;return 1;
}
#endif
