#include "credential_acquire.h"
#if defined(_WIN32)
#include <windows.h>
#include <bcrypt.h>
#elif defined(__linux__)
#include "../host-service/native_syscall_linux.h"
#endif
#if defined(_WIN32) || defined(__linux__)
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&
    !(a<b+m&&b<a+n);
}
static int original(struct slcli_management_credential_acquisition *c) {
  return c&&c->prepared&&c->arena&&
    slcli_arena_live_block(c->arena,c,sizeof(*c))&&
    slcli_arena_live_block(c->arena,c->caller_original,c->caller_bytes)&&
    slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller_original)&&
    c->deadline->origin==c->origin&&c->deadline->expires==c->expires&&
    c->origin->tick==c->origin_tick&&c->predecessor&&
    slcli_arena_live_range(c->arena,c->predecessor,sizeof(*c->predecessor))&&
    c->predecessor->complete&&c->predecessor->tick==c->predecessor_tick&&
    c->predecessor_tick>=c->origin_tick&&c->predecessor_tick<c->expires;
}
struct slcli_management_credential_acquisition *slcli_management_credential_acquisition_allocate(
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,
  const void *caller,size_t bytes) {
  struct slcli_management_credential_acquisition *c;
  if(!a||!bytes||!slcli_arena_live_block(a,caller,bytes)||
     !slcli_source_deadline_original_matches(a,d,caller)||
     !slcli_source_deadline_away(d,caller,bytes)||
     !away(a,sizeof(*a),caller,bytes)||!a->owner||
     !away(a->owner,sizeof(*a->owner),caller,bytes))return 0;
  c=slcli_arena_calloc(a,1,sizeof(*c));if(!c)return 0;
  /* The fresh entire capture/secret/clock allocation exists before effects;
   * no later diagnostics allocate or overwrite a native result destination. */
  if(!away(c,sizeof(*c),caller,bytes)||!slcli_source_deadline_away(d,c,sizeof(*c))||
     !away(c,sizeof(*c),a,sizeof(*a))||!away(c,sizeof(*c),a->owner,sizeof(*a->owner)))return 0;
  c->arena=a;c->deadline=d;c->origin=d->origin;c->caller_original=caller;
  c->caller_bytes=bytes;c->expires=d->expires;c->origin_tick=d->origin->tick;
  c->predecessor=d->origin;c->predecessor_tick=d->origin->tick;
  c->prepared=1;return c;
}
int slcli_management_credential_acquire_after(struct slcli_management_credential_acquisition *c,
  const struct slcli_native_clock *prior) {
  if(!original(c)||c->started||c->unresolved||c->predecessor_bound||c->predecessor!=c->origin||
     !prior||!slcli_arena_live_range(c->arena,prior,sizeof(*prior))||!prior->complete||
     prior->tick<c->origin_tick||prior->tick>=c->expires||
     !away(c,sizeof(*c),prior,sizeof(*prior))||
     !away(c->caller_original,c->caller_bytes,prior,sizeof(*prior)))return 0;
  c->predecessor=prior;c->predecessor_tick=prior->tick;c->predecessor_bound=1;return 1;
}
int slcli_management_credential_acquire(struct slcli_management_credential_acquisition *c) {
#if defined(_WIN32)
  NTSTATUS result;
#else
  long result;
#endif
  if(!original(c)||c->started||c->unresolved)return 0;
  c->started=1;c->unresolved=1;
  if(!slcli_native_clock_read(&c->before)||c->before.tick<c->predecessor_tick||
     c->before.tick>=c->expires||!original(c))return 0;
  c->args[0]=0;c->args[1]=(uintptr_t)c->secret;c->args[2]=64;
#if defined(_WIN32)
  c->args[3]=BCRYPT_USE_SYSTEM_PREFERRED_RNG;c->native_called=1;
  result=BCryptGenRandom(0,c->secret,64,BCRYPT_USE_SYSTEM_PREFERRED_RNG);
  c->status=(uint32_t)result;c->result_observed=1;
  /* BCrypt has no partial returned-count output. Preserve its entire actual
   * destination on failure, without inventing a count of produced bytes. */
  if(BCRYPT_SUCCESS(result)){c->returned_bytes=64;c->count_observed=1;}
#else
  c->args[0]=(uintptr_t)c->secret;c->args[1]=64;c->args[2]=0;
  c->args[3]=0;c->native_called=1;
  result=slcli_kernel_call(__NR_getrandom,(long)c->secret,64,0,0,0,0);
  c->status=(uint64_t)result;c->result_observed=1;
  if(result>=0&&result<=64){c->returned_bytes=(uint64_t)result;c->count_observed=1;}
#endif
  c->destination_retained=1;
  if(!slcli_native_clock_read(&c->after)||c->after.tick<c->before.tick||
     c->after.tick>=c->expires||!original(c)||!c->count_observed||
     c->returned_bytes!=64)return 0;
  c->complete=1;c->unresolved=0;return 1;
}
#endif
