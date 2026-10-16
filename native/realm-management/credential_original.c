#include "credential_original.h"
#include "../host-service/input_deadline.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
struct slcli_management_credential *slcli_management_credential_allocate(struct slcli_arena *a) {
  struct slcli_management_credential *c;
  if(!a)return 0;c=slcli_arena_calloc(a,1,sizeof(*c));if(c)c->arena=a;return c;
}
int slcli_management_credential_consume(struct slcli_management_credential *c,struct slcli_input_body *b) {
  size_t n,i;struct slcli_sha256 hash;unsigned char digest[32];
  if(!c||c->started||c->failed||!c->arena||!b||
     !slcli_arena_live_block(c->arena,c,sizeof(*c))||
     !slcli_arena_live_range(c->arena,b,sizeof(*b))||!b->complete||b->failed||
     b->bytes!=64||b->consumed||b->recheck_started||!b->eof_observed||
     !slcli_input_deadline_ready(b)||
     b->capacity<65||b->readback_capacity<65||
     b->capture_capacity>SIZE_MAX/sizeof(*b->captures))return 0;
  n=b->capture_capacity*sizeof(*b->captures);
  if(!slcli_arena_live_range(c->arena,b->body,b->capacity)||
     !slcli_arena_live_range(c->arena,b->readback,b->readback_capacity)||
     !slcli_arena_live_range(c->arena,b->captures,n)||
     !away(c,sizeof(*c),b,sizeof(*b))||!away(c,sizeof(*c),b->body,b->capacity)||
     !away(c,sizeof(*c),b->readback,b->readback_capacity)||!away(c,sizeof(*c),b->captures,n))return 0;
  c->started=1;c->failed=1;c->original=b;
  if(!slcli_input_body_recheck(b)||!slcli_input_deadline_ready(b))return 0;
  b->consumed=2;b->failed=1;
  slcli_sha256_init(&hash);
  if(!slcli_sha256_update(&hash,b->body,64)||!slcli_sha256_finish(&hash,digest)||
     !slcli_equal(digest,b->digest,32)||!slcli_equal(b->body,b->readback,64))return 0;
  for(i=0;i<32;i++)c->raw_digest[i]=digest[i];
  c->seed=b->body;c->ledger=b->body+32;
  c->complete=1;c->failed=0;b->consumed=1;b->failed=0;return 1;
}
#endif
