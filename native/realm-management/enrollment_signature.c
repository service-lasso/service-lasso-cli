#include "enrollment_signature.h"
#include "../vendor/monocypher-4.0.3/monocypher-ed25519.h"
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
struct slcli_management_enrollment_signature *slcli_management_enrollment_signature_allocate(struct slcli_arena *a) {
  struct slcli_management_enrollment_signature *s;
  if(!a||!a->owner||!a->owner->limit)return 0;s=slcli_arena_calloc(a,1,sizeof(*s));
  if(s)s->arena=a;return s;
}
int slcli_management_enrollment_signature_original(struct slcli_management_enrollment_signature *s,
  const unsigned char seed[32],const unsigned char anchor[32],const unsigned char *prefix,size_t bytes) {
  size_t i;unsigned char wiped=0;struct slcli_arena *a;
  if(!s||s->started||s->failed||!(a=s->arena)||!bytes||bytes>SLCLI_HA2_ENROLLMENT_LIMIT||
     !slcli_arena_live_range(a,s,sizeof(*s))||!slcli_arena_live_range(a,seed,32)||
     !slcli_arena_live_range(a,anchor,32)||!slcli_arena_live_range(a,prefix,bytes)||
     !away(s,sizeof(*s),seed,32)||!away(s,sizeof(*s),anchor,32)||
     !away(s,sizeof(*s),prefix,bytes)||!away(seed,32,anchor,32)||
     !away(seed,32,prefix,bytes)||!away(anchor,32,prefix,bytes))return 0;
  s->started=1;s->failed=1;s->credential_original=seed;s->anchor_original=anchor;s->prefix_original=prefix;
  s->preimage_bytes=slcli_ha2_enrollment_preimage(s->preimage,sizeof(s->preimage),prefix,bytes);
  if(s->preimage_bytes!=SLCLI_HA2_ENROLLMENT_DOMAIN_BYTES+4+bytes)return 0;
  for(i=0;i<32;i++)s->temporary_seed[i]=seed[i];
  crypto_ed25519_key_pair(s->secret_key,s->public_key,s->temporary_seed);
  for(i=0;i<32;i++)wiped|=s->temporary_seed[i];
  if(wiped||!slcli_equal(s->public_key,anchor,32)||!slcli_equal(s->secret_key,seed,32)||
     !slcli_equal(s->secret_key+32,s->public_key,32))return 0;
  s->key_pair_complete=1;
  crypto_ed25519_sign(s->signature,s->secret_key,s->preimage,s->preimage_bytes);
  if(crypto_ed25519_check(s->signature,anchor,s->preimage,s->preimage_bytes))return 0;
  s->signed_complete=1;s->failed=0;return 1;
}
