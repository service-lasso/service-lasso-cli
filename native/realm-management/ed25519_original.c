#include "ed25519_original.h"
#include "../vendor/monocypher-4.0.3/monocypher-ed25519.h"
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int nonzero(const unsigned char *p,size_t bytes) {
  unsigned char n=0;size_t i;for(i=0;i<bytes;i++)n|=p[i];return n!=0;
}
struct slcli_management_signature *slcli_management_signature_allocate(struct slcli_arena *a) {
  struct slcli_management_signature *s;
  if(!a||!a->owner||!a->owner->limit)return 0;
  s=slcli_arena_calloc(a,1,sizeof(*s));if(s)s->arena=a;return s;
}
int slcli_management_signature_original(struct slcli_management_signature *s,
  const unsigned char seed[32],const unsigned char anchor[32],const unsigned char *prefix,size_t bytes) {
  size_t i;struct slcli_arena *a;
  if(!s||!(a=s->arena)||s->started||s->failed||
     !slcli_arena_live_range(a,s,sizeof(*s))||
     !slcli_arena_live_range(a,seed,32)||!slcli_arena_live_range(a,anchor,32)||
     !slcli_arena_live_range(a,prefix,bytes)||
     !away(s,sizeof(*s),seed,32)||!away(s,sizeof(*s),anchor,32)||
     !away(s,sizeof(*s),prefix,bytes)||!away(seed,32,anchor,32)||
     !away(seed,32,prefix,bytes)||!away(anchor,32,prefix,bytes)||
     bytes!=SLCLI_HA2_JOURNAL_SIGNED_BYTES||!nonzero(anchor,32))return 0;
  s->started=1;s->failed=1;s->credential_original=seed;s->anchor_original=anchor;
  s->prefix_original=prefix;
  s->preimage_bytes=slcli_ha2_journal_signature_preimage(s->preimage,sizeof(s->preimage),prefix,bytes);
  if(s->preimage_bytes!=SLCLI_HA2_JOURNAL_PREIMAGE_BYTES)return 0;
  /* The actual API wipes its seed argument. Never pass persisted credential or
   * ledger half: this separate charged copy stays retained after its wipe.
   * Seed value is not an association ID: the original API accepts every 32-byte
   * seed. Native credential provenance, not a nonzero-byte test, authenticates it. */
  for(i=0;i<32;i++)s->temporary_seed[i]=seed[i];
  crypto_ed25519_key_pair(s->secret_key,s->public_key,s->temporary_seed);
  if(nonzero(s->temporary_seed,32)||!slcli_equal(s->public_key,anchor,32)||
     !slcli_equal(s->secret_key,seed,32)||
     !slcli_equal(s->secret_key+32,s->public_key,32))return 0;
  s->key_pair_complete=1;
  crypto_ed25519_sign(s->signature,s->secret_key,s->preimage,s->preimage_bytes);
  if(crypto_ed25519_check(s->signature,anchor,s->preimage,s->preimage_bytes))return 0;
  s->signed_complete=1;s->failed=0;return 1;
}
