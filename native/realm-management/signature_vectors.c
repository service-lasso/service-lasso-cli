#include "signature_vectors.h"
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
struct slcli_management_record_vector *slcli_management_record_vector_allocate(struct slcli_arena *a) {
  struct slcli_management_record_vector *v;
  if(!a||!a->owner||!a->owner->limit)return 0;
  v=slcli_arena_calloc(a,1,sizeof(*v));if(!v)return 0;
  v->failed=1;v->signer=slcli_management_signature_allocate(a);
  if(v->signer)v->failed=0;return v;
}
int slcli_management_signature_positive_vector(struct slcli_management_signature *s,
  const unsigned char seed[32],const unsigned char anchor[32],const unsigned char *prefix,size_t bytes) {
  size_t i;unsigned char any=0;
  static const unsigned char domain[]=SLCLI_HA2_JOURNAL_SIGNATURE_DOMAIN;
  if(!slcli_management_signature_original(s,seed,anchor,prefix,bytes))return 0;
  if(!s->started||s->failed||!s->key_pair_complete||!s->signed_complete||
     s->preimage_bytes!=741||s->preimage_bytes!=sizeof(s->preimage)||
     !slcli_equal(s->preimage,domain,sizeof(domain))||
     s->preimage[sizeof(domain)]||s->preimage[sizeof(domain)+1]||
     s->preimage[sizeof(domain)+2]!=2||s->preimage[sizeof(domain)+3]!=196||
     !slcli_equal(s->preimage+sizeof(domain)+4,prefix,SLCLI_HA2_JOURNAL_SIGNED_BYTES)||
     !slcli_equal(s->secret_key,seed,32)||!slcli_equal(s->public_key,anchor,32))return 0;
  for(i=0;i<32;i++)any|=s->temporary_seed[i];
  /* Positive requires actual exact Ed25519 key_pair/sign/check body completion,
   * not an all-denial preimage or byte-only signature assertion. */
  return !any;
}
int slcli_management_record_positive_vector(struct slcli_management_record_vector *v,
  const struct slcli_ha2_journal_prefix_originals *o,const unsigned char seed[32],const unsigned char anchor[32]) {
  struct slcli_ha2_journal_record parsed;size_t i;struct slcli_arena *a;
  if(!v||v->started||v->failed||!v->signer||!(a=v->signer->arena)||
     !slcli_arena_live_range(a,v,sizeof(*v))||!slcli_arena_live_range(a,o,sizeof(*o))||
     !slcli_arena_live_range(a,o->associations,18*32)||
     !slcli_arena_live_range(a,o->key_version,o->key_version_bytes)||
     !separate(v,sizeof(*v),v->signer,sizeof(*v->signer))||
     !separate(v,sizeof(*v),o,sizeof(*o))||
     !separate(v,sizeof(*v),o->associations,18*32)||
     !separate(v,sizeof(*v),o->key_version,o->key_version_bytes)||
     !separate(v,sizeof(*v),seed,32)||!separate(v,sizeof(*v),anchor,32))return 0;
  v->started=1;v->failed=1;
  if(!slcli_ha2_journal_prefix_original(v->prefix,sizeof(v->prefix),o)||
     !slcli_management_signature_positive_vector(v->signer,seed,anchor,v->prefix,sizeof(v->prefix)))return 0;
  for(i=0;i<SLCLI_HA2_JOURNAL_SIGNED_BYTES;i++)v->raw[i]=v->prefix[i];
  for(i=0;i<64;i++)v->raw[SLCLI_HA2_JOURNAL_SIGNED_BYTES+i]=v->signer->signature[i];
  if(!slcli_ha2_journal_record_parse(v->raw,sizeof(v->raw),&parsed)||
     parsed.sequence!=o->sequence||parsed.generation!=o->generation||parsed.state!=o->state)return 0;
  for(i=0;i<18;i++)if(!slcli_equal(parsed.associations[i],o->associations[i],32))return 0;
  v->complete=1;v->failed=0;return 1;
}
