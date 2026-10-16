#include "transport_linux.h"
#if defined(__linux__)
#include "../host-service/native_syscall_linux.h"
#include <linux/memfd.h>
#include <linux/fcntl.h>
size_t slcli_node_transport_linux_control_bytes(void) {
  size_t book=slcli_arena_allocation_bytes(sizeof(struct slcli_node_transport_linux));
  size_t encoded=slcli_arena_allocation_bytes(SLCLI_BOOTSTRAP_TRANSPORT_BYTES);
  size_t readback=slcli_arena_allocation_bytes(SLCLI_BOOTSTRAP_TRANSPORT_BYTES+1u);
  if(!book||!encoded||!readback||book>SIZE_MAX-encoded||book+encoded>SIZE_MAX-readback)return 0;
  return book+encoded+readback;
}
static int apart(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int same(struct slcli_node_transport_linux *c,struct slcli_arena *a) {
  return slcli_source_deadline_original_matches(a,c->deadline,c->caller)&&
    c->deadline->origin==c->origin&&c->origin->tick==c->origin_tick&&
    c->deadline->expires==c->expires&&c->predecessor->complete&&
    c->predecessor->tick==c->predecessor_tick;
}
static int before(struct slcli_node_transport_linux *c,struct slcli_arena *a,size_t i) {
  struct slcli_preentry_backing_call *r=&c->calls[i];
  uint64_t previous=i?c->calls[i-1].after.tick:c->constructor_before[2].tick;
  if(i==2)previous=c->constructor_before[3].tick;
  if(r->called||r->returned||!slcli_native_clock_read(&r->before)||!same(c,a)||
     r->before.tick<previous||r->before.tick>=c->expires)return 0;
  r->before_result=1;return 1;
}
static int after(struct slcli_node_transport_linux *c,struct slcli_arena *a,size_t i) {
  struct slcli_preentry_backing_call *r=&c->calls[i];
  r->after_result=r->returned&&slcli_native_clock_read(&r->after)&&same(c,a)&&
    r->after.tick>=r->before.tick&&r->after.tick<c->expires;
  return r->after_result;
}
static long actual(struct slcli_preentry_backing_call *r,long operation) {
  long result;r->called=1;
  result=slcli_kernel_call(operation,(long)r->args[0],(long)r->args[1],
    (long)r->args[2],(long)r->args[3],(long)r->args[4],(long)r->args[5]);
  r->result=(uint64_t)result;r->returned=1;return result;
}
int slcli_node_transport_linux_create(struct slcli_node_transport_linux *c,
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,
  size_t caller_bytes,const struct slcli_native_clock *prior) {
  size_t i;uint64_t page,remainder;long result;
  struct slcli_preentry_backing_call *r;
  if(!c||!a||!slcli_memory_owner_mutable(a->owner)||!caller_bytes||!slcli_arena_live_block(a,c,sizeof(*c))||
     !slcli_arena_live_block(a,caller,caller_bytes)||
     !slcli_source_deadline_original_matches(a,d,caller)||!prior||
     !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
     prior->tick<d->origin->tick||prior->tick>=d->expires||
     !apart(c,sizeof(*c),caller,caller_bytes)||!apart(c,sizeof(*c),prior,sizeof(*prior))||
     !slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  c->started=1;c->descriptor=-1;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
  c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
  c->predecessor=prior;c->predecessor_tick=prior->tick;
  if(!slcli_native_clock_read(&c->constructor_before[0])||!same(c,a)||
     c->constructor_before[0].tick<c->predecessor_tick||c->constructor_before[0].tick>=c->expires)goto failed;
  c->encoded=(unsigned char *)slcli_arena_calloc(a,1,SLCLI_BOOTSTRAP_TRANSPORT_BYTES);
  if(!c->encoded)goto failed;
  if(!slcli_native_clock_read(&c->constructor_before[1])||!same(c,a)||
     c->constructor_before[1].tick<c->constructor_before[0].tick||c->constructor_before[1].tick>=c->expires)goto failed;
  c->readback=(unsigned char *)slcli_arena_calloc(a,1,SLCLI_BOOTSTRAP_TRANSPORT_BYTES+1u);
  if(!c->readback)goto failed;
  if(!slcli_native_clock_read(&c->constructor_before[2])||!same(c,a)||
     c->constructor_before[2].tick<c->constructor_before[1].tick||c->constructor_before[2].tick>=c->expires)goto failed;
  page=a->owner->native_page_size;
  if(!page||SLCLI_BOOTSTRAP_TRANSPORT_BYTES>UINT64_MAX-page+1)goto failed;
  remainder=SLCLI_BOOTSTRAP_TRANSPORT_BYTES%page;
  c->physical_bytes=remainder?SLCLI_BOOTSTRAP_TRANSPORT_BYTES+page-remainder:SLCLI_BOOTSTRAP_TRANSPORT_BYTES;
  if(c->physical_bytes>INT64_MAX)goto failed;
  c->prepare_result=slcli_memory_prepare(a->owner,SLCLI_IO,c->physical_bytes,&c->request);
  c->prepare_returned=1;if(!c->prepare_result)goto failed;
  if(!before(c,a,0))goto retained;
  r=&c->calls[0];r->args[0]=(uintptr_t)"SLCLI-immutable-ingress";
  r->args[1]=MFD_CLOEXEC|MFD_ALLOW_SEALING;result=actual(r,__NR_memfd_create);
  if(result>=0&&result<=INT32_MAX)c->descriptor=(int)result;
  if(!after(c,a,0)||result<0||result>INT32_MAX)goto retained;
  if(!before(c,a,1))goto retained;
  r=&c->calls[1];r->args[0]=(uint64_t)c->descriptor;r->args[1]=SLCLI_BOOTSTRAP_TRANSPORT_BYTES;
  result=actual(r,__NR_ftruncate);
  if(!result){c->backing_result=slcli_memory_backing_acquired(a->owner,&c->request);c->backing_returned=1;}
  if(!after(c,a,1)||result||!c->backing_returned||!c->backing_result)goto retained;
  c->object_prepared=1;return 1;
retained:
  c->retained_result=slcli_memory_unobserved(a->owner,&c->request);
  c->retained_returned=1;
failed:c->terminal=1;return 0;
}
int slcli_node_transport_linux_publish(struct slcli_node_transport_linux *c,
  struct slcli_arena *a,const struct slcli_native_clock *prior,
  const struct slcli_node_bootstrap_transport *t) {
  size_t i;long result;struct slcli_preentry_backing_call *r;
  if(!c||!a||!slcli_memory_owner_mutable(a->owner)||!t||!prior||!slcli_arena_live_block(a,c,sizeof(*c))||
     !c->started||!c->object_prepared||c->publication_attempted||c->terminal||
     c->descriptor<0||!c->calls[0].returned||c->calls[0].result!=(uint64_t)c->descriptor||
     !c->calls[1].returned||c->calls[1].result||c->calls[1].args[0]!=(uint64_t)c->descriptor||
     !slcli_arena_live_block(a,c->encoded,SLCLI_BOOTSTRAP_TRANSPORT_BYTES)||
     !slcli_arena_live_block(a,c->readback,SLCLI_BOOTSTRAP_TRANSPORT_BYTES+1u)||
     !same(c,a)||!slcli_arena_live_range(a,t,sizeof(*t))||
     !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
     prior->tick<c->calls[1].after.tick||prior->tick>=c->expires||
     !apart(c,sizeof(*c),t,sizeof(*t))||!apart(c,sizeof(*c),prior,sizeof(*prior)))return 0;
  c->publication_attempted=1;c->publication_predecessor=prior;c->transport_original=t;
  if(t->expires!=c->expires)goto retained;
  for(i=0;i<2;i++)if(t->origin.args[i]!=c->origin->args[i]||
    t->origin.raw[i]!=c->origin->raw[i]||t->origin.status[i]!=c->origin->status[i])goto retained;
  if(t->origin.tick!=c->origin->tick||t->origin.started!=c->origin->started||
     t->origin.observed!=c->origin->observed||t->origin.complete!=c->origin->complete||
     t->origin.calls_called!=c->origin->calls_called||
     t->origin.calls_returned!=c->origin->calls_returned)goto retained;
  if(!slcli_native_clock_read(&c->constructor_before[3])||!same(c,a)||
     c->constructor_before[3].tick<prior->tick||c->constructor_before[3].tick>=c->expires)goto retained;
  /* Source serialization excludes input mutation; encoding cannot supply the
   * native launcher's actual birth/source/private object correspondence. */
  c->encode_result=slcli_node_bootstrap_transport_encode(c->encoded,SLCLI_BOOTSTRAP_TRANSPORT_BYTES,t);
  c->encode_returned=1;if(!c->encode_result)goto retained;
  if(!before(c,a,2))goto retained;
  r=&c->calls[2];r->args[0]=(uint64_t)c->descriptor;r->args[1]=(uintptr_t)c->encoded;
  r->args[2]=SLCLI_BOOTSTRAP_TRANSPORT_BYTES;result=actual(r,__NR_pwrite64);
  if(!after(c,a,2)||result!=SLCLI_BOOTSTRAP_TRANSPORT_BYTES)goto retained;
  if(!before(c,a,3))goto retained;
  r=&c->calls[3];r->args[0]=(uint64_t)c->descriptor;r->args[1]=F_ADD_SEALS;
  r->args[2]=F_SEAL_WRITE|F_SEAL_GROW|F_SEAL_SHRINK|F_SEAL_SEAL;
  result=actual(r,__NR_fcntl);if(!after(c,a,3)||result)goto retained;
  if(!before(c,a,4))goto retained;
  r=&c->calls[4];r->args[0]=(uint64_t)c->descriptor;r->args[1]=F_GET_SEALS;
  result=actual(r,__NR_fcntl);
  if(!after(c,a,4)||result!=(F_SEAL_WRITE|F_SEAL_GROW|F_SEAL_SHRINK|F_SEAL_SEAL))goto retained;
  c->seals_observed=1;
  if(!before(c,a,5))goto retained;
  r=&c->calls[5];r->args[0]=(uint64_t)c->descriptor;r->args[1]=(uintptr_t)c->readback;
  r->args[2]=SLCLI_BOOTSTRAP_TRANSPORT_BYTES+1u;result=actual(r,__NR_pread64);
  if(!after(c,a,5)||result!=SLCLI_BOOTSTRAP_TRANSPORT_BYTES)goto retained;
  for(i=0;i<SLCLI_BOOTSTRAP_TRANSPORT_BYTES;i++) {
    c->compared=i+1;if(c->encoded[i]!=c->readback[i])goto retained;
  }
  c->bytes_equal=1;
  if(!before(c,a,6))goto retained;
  r=&c->calls[6];r->args[0]=(uint64_t)c->descriptor;r->args[1]=(uintptr_t)&c->identity_original;
  result=actual(r,__NR_fstat);
  if(!after(c,a,6)||result||c->identity_original.st_size!=SLCLI_BOOTSTRAP_TRANSPORT_BYTES)goto retained;
  c->returned=1;c->terminal=1;return 1;
retained:
  c->retained_result=slcli_memory_unobserved(a->owner,&c->request);
  c->retained_returned=1;
  c->terminal=1;return 0;
}
#endif
