/* SPDX-License-Identifier: Apache-2.0 */
#include "bq_readiness_linux.h"
#if defined(__linux__)
#include "../host-service/native_syscall_linux.h"
#include <limits.h>
static int separate(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int source(const struct slcli_bq_readiness_linux_original *r){
 if(!r||!r->arena||!r->deadline||!r->prior||!r->origin||!r->caller||!r->caller_bytes||
  !separate(r,sizeof(*r),r->arena,sizeof(*r->arena))||!r->arena->owner||
  !separate(r,sizeof(*r),r->arena->owner,sizeof(*r->arena->owner))||
  !slcli_memory_owner_mutable(r->arena->owner)||
  !separate(r,sizeof(*r),r->deadline,sizeof(*r->deadline))||
  !separate(r,sizeof(*r),r->prior,sizeof(*r->prior))||
  !separate(r,sizeof(*r),r->origin,sizeof(*r->origin))||
  !separate(r,sizeof(*r),r->caller,r->caller_bytes)||
  !slcli_arena_live_range(r->arena,r,sizeof(*r))||
  !slcli_arena_live_block(r->arena,r->caller,r->caller_bytes)||
  !slcli_arena_live_range(r->arena,r->prior,sizeof(*r->prior))||
  !slcli_source_deadline_original_matches(r->arena,r->deadline,r->caller))return 0;
 return r->endpoint>=0&&(r->requested_events==POLLIN||r->requested_events==POLLOUT)&&
  r->deadline->origin==r->origin&&r->origin->complete&&r->origin->observed&&r->origin->tick==r->origin_tick&&
  r->deadline->expires==r->expires&&r->prior->complete&&r->prior->observed&&r->prior->tick==r->prior_tick&&
  r->prior_tick>=r->origin_tick&&r->prior_tick<r->expires;
}
static int raw_matches(const struct slcli_bq_readiness_linux_original *r){
 uint64_t remaining;
 if(!source(r)||!r->started||!r->called||!r->returned||
  !r->before_returned||!r->after_returned||!r->before_result||!r->after_result||
  !r->before.complete||!r->before.observed||!r->after.complete||!r->after.observed||
  r->before.tick<r->prior_tick||r->before.tick>=r->expires||r->after.tick<r->before.tick||r->after.tick>=r->expires)return 0;
 remaining=r->expires-r->before.tick;
 return r->timeout.tv_sec>=0&&r->timeout.tv_nsec>=0&&r->timeout.tv_nsec<1000000000&&
  (r->timeout.tv_sec<r->timeout_input.tv_sec||
   (r->timeout.tv_sec==r->timeout_input.tv_sec&&r->timeout.tv_nsec<=r->timeout_input.tv_nsec))&&
  (uint64_t)r->timeout_input.tv_sec==remaining/1000000000&&
  (uint64_t)r->timeout_input.tv_nsec==remaining%1000000000&&
  r->args[0]==(long)&r->descriptor&&r->args[1]==1&&r->args[2]==(long)&r->timeout&&
  !r->args[3]&&!r->args[4]&&!r->args[5]&&r->raw_return==1&&
  r->descriptor.fd==r->endpoint&&r->descriptor.events==r->requested_events&&
  r->descriptor.revents==r->requested_events;
}
int slcli_bq_readiness_linux_matches_original(const struct slcli_bq_readiness_linux_original *r){
 return raw_matches(r)&&r->complete&&!r->failed&&r->validation_returned&&r->validation_clock_result&&
  r->validation_after.complete&&r->validation_after.observed&&
  r->validation_after.tick>=r->after.tick&&r->validation_after.tick<r->expires;
}
int slcli_bq_readiness_linux_preowned_original(struct slcli_bq_readiness_linux_original *r,
 struct slcli_arena *a,int endpoint,short events,const struct slcli_source_deadline_original *d,
 const void *caller,size_t caller_bytes,const struct slcli_native_clock *prior){
 size_t i;uint64_t remaining;
 if(!a||!a->owner||!slcli_memory_owner_mutable(a->owner)||!d||!prior||endpoint<0||(events!=POLLIN&&events!=POLLOUT)||
  !slcli_arena_live_range(a,r,sizeof(*r))||!slcli_arena_live_block(a,caller,caller_bytes)||
  !slcli_arena_live_range(a,prior,sizeof(*prior))||
  !separate(r,sizeof(*r),a,sizeof(*a))||!separate(r,sizeof(*r),a->owner,sizeof(*a->owner))||
  !separate(r,sizeof(*r),d,sizeof(*d))||!separate(r,sizeof(*r),prior,sizeof(*prior))||
  !separate(r,sizeof(*r),caller,caller_bytes)||!slcli_source_deadline_away(d,r,sizeof(*r))||
  !slcli_source_deadline_original_matches(a,d,caller)||!prior->complete||!prior->observed||
  prior->tick<d->origin->tick||prior->tick>=d->expires)return 0;
 for(i=0;i<sizeof(*r);i++)if(((const unsigned char *)r)[i])return 0;
 r->arena=a;r->deadline=d;r->caller=caller;r->caller_bytes=caller_bytes;r->prior=prior;r->prior_tick=prior->tick;
 r->origin=d->origin;r->origin_tick=d->origin->tick;r->expires=d->expires;
 r->endpoint=endpoint;r->requested_events=events;r->started=1;r->failed=1;
 r->before_result=slcli_native_clock_read(&r->before);r->before_returned=1;
 if(!r->before_result||!r->before.complete||!r->before.observed||!source(r)||
  r->before.tick<r->prior_tick||r->before.tick>=r->expires)return 0;
 remaining=r->expires-r->before.tick;
 r->timeout.tv_sec=(long long)(remaining/1000000000);r->timeout.tv_nsec=(long long)(remaining%1000000000);
 if(r->timeout.tv_sec<0||(uint64_t)r->timeout.tv_sec!=remaining/1000000000)return 0;
 r->timeout_input=r->timeout;
 r->descriptor.fd=endpoint;r->descriptor.events=events;
 r->args[0]=(long)&r->descriptor;r->args[1]=1;r->args[2]=(long)&r->timeout;
 r->called=1;r->raw_return=slcli_kernel_call(__NR_ppoll,r->args[0],1,r->args[2],0,0,0);r->returned=1;
 r->after_result=slcli_native_clock_read(&r->after);r->after_returned=1;
 if(!raw_matches(r))return 0;
 r->validation_clock_result=slcli_native_clock_read(&r->validation_after);r->validation_returned=1;
 if(!r->validation_clock_result||!r->validation_after.complete||!r->validation_after.observed||
  r->validation_after.tick<r->after.tick||r->validation_after.tick>=r->expires||!raw_matches(r))return 0;
 r->complete=1;r->failed=0;
 if(!slcli_bq_readiness_linux_matches_original(r)){r->failed=1;return 0;}return 1;
}
#endif
