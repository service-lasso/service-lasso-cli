#include "preentry_backing_linux.h"
#if defined(__linux__)
#include "native_syscall_linux.h"
#include <linux/memfd.h>
#include <linux/mman.h>
#include <linux/fcntl.h>
static int apart(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int same(struct slcli_preentry_backing_linux *c,struct slcli_arena *a) {
  return slcli_source_deadline_original_matches(a,c->deadline,c->caller)&&
    c->deadline->origin==c->origin&&c->origin->tick==c->origin_tick&&
    c->deadline->expires==c->expires&&c->predecessor->complete&&
    c->predecessor->tick==c->predecessor_tick;
}
static int before(struct slcli_preentry_backing_linux *c,struct slcli_arena *a,size_t n) {
  struct slcli_preentry_backing_call *r=&c->calls[n];
  uint64_t previous=n?c->calls[n-1].after.tick:c->predecessor_tick;
  if(r->called||r->returned||!slcli_native_clock_read(&r->before)||!same(c,a)||
     r->before.tick<previous||r->before.tick>=c->expires)return 0;
  r->before_result=1;return 1;
}
static int after(struct slcli_preentry_backing_linux *c,struct slcli_arena *a,size_t n) {
  struct slcli_preentry_backing_call *r=&c->calls[n];
  r->after_result=r->returned&&slcli_native_clock_read(&r->after)&&same(c,a)&&
    r->after.tick>=r->before.tick&&r->after.tick<c->expires;
  return r->after_result;
}
static long actual(struct slcli_preentry_backing_call *r,long operation) {
  long result;r->called=1;
  result=slcli_kernel_call(operation,(long)r->args[0],(long)r->args[1],(long)r->args[2],
    (long)r->args[3],(long)r->args[4],(long)r->args[5]);
  r->result=(uint64_t)result;r->returned=1;return result;
}
static int create_original(struct slcli_preentry_backing_linux *c,
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,
  size_t caller_bytes,const struct slcli_native_clock *prior,uint64_t requested,
  struct slcli_memory_owner *physical,uint32_t section_only) {
  uint64_t page,remainder;size_t i;long result;struct slcli_preentry_backing_call *r;
  if(!c||!a||!slcli_memory_owner_mutable(a->owner)||!slcli_arena_live_block(a,c,sizeof(*c))||!caller_bytes||
     !slcli_arena_live_block(a,caller,caller_bytes)||!slcli_source_deadline_original_matches(a,d,caller)||
     !prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
     prior->tick<d->origin->tick||prior->tick>=d->expires||
     !apart(c,sizeof(*c),caller,caller_bytes)||!apart(c,sizeof(*c),prior,sizeof(*prior))||
     !slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  if(!slcli_memory_owner_mutable(physical)||section_only>1||
     (physical!=a->owner&&(!slcli_arena_live_block(a,physical,sizeof(*physical))||
      !apart(c,sizeof(*c),physical,sizeof(*physical))||
      !apart(physical,sizeof(*physical),caller,caller_bytes)||
      !apart(physical,sizeof(*physical),prior,sizeof(*prior))||
      !slcli_source_deadline_away(d,physical,sizeof(*physical)))))return 0;
  c->physical_owner=physical;c->section_only=section_only;
  c->started=1;c->operation=1;c->descriptor=-1;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
  c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
  c->predecessor=prior;c->predecessor_tick=prior->tick;
  page=c->physical_owner->native_page_size;
  if(!page||!requested||requested>INT64_MAX||requested>UINT64_MAX-page+1)return 0;
  remainder=requested%page;c->bytes=remainder?requested+page-remainder:requested;
  if(c->bytes>INT64_MAX)return 0;
  c->prepare_result=slcli_memory_prepare(c->physical_owner,SLCLI_RUNTIME,c->bytes,&c->request);
  c->prepare_returned=1;if(!c->prepare_result)return 0;
  c->prepared=1;
  if(!before(c,a,0))goto retained;
  r=&c->calls[0];r->args[0]=(uintptr_t)"SLCLI-preentry";r->args[1]=MFD_CLOEXEC|MFD_ALLOW_SEALING;
  result=actual(r,__NR_memfd_create);
  if(result>=0&&result<=INT32_MAX)c->descriptor=(int)result;
  if(!after(c,a,0)||result<0||result>INT32_MAX)goto retained;
  if(!before(c,a,1))goto retained;
  r=&c->calls[1];r->args[0]=(uint64_t)c->descriptor;r->args[1]=c->bytes;
  result=actual(r,__NR_ftruncate);
  if(!result) {
    c->backing_result=slcli_memory_backing_acquired(c->physical_owner,&c->request);
    c->backing_returned=1;
  }
  if(!after(c,a,1)||result||!c->backing_returned||!c->backing_result)goto retained;
  if(!before(c,a,2))goto retained;
  r=&c->calls[2];r->args[0]=(uint64_t)c->descriptor;r->args[1]=F_ADD_SEALS;
  r->args[2]=F_SEAL_GROW|F_SEAL_SHRINK|F_SEAL_SEAL;
  result=actual(r,__NR_fcntl);
  if(!after(c,a,2)||result)goto retained;
  if(!before(c,a,3))goto retained;
  r=&c->calls[3];r->args[0]=(uint64_t)c->descriptor;r->args[1]=F_GET_SEALS;
  result=actual(r,__NR_fcntl);
  if(!after(c,a,3)||result!=(F_SEAL_GROW|F_SEAL_SHRINK|F_SEAL_SEAL))goto retained;
  if(!before(c,a,4))goto retained;
  r=&c->calls[4];r->args[0]=(uint64_t)c->descriptor;r->args[1]=(uintptr_t)&c->backing_original;
  result=actual(r,__NR_fstat);
  if(!after(c,a,4)||result||c->backing_original.st_size<0||
     (uint64_t)c->backing_original.st_size!=c->bytes)goto retained;
  if(c->section_only){c->qualified=1;return 1;}
  if(!before(c,a,5))goto retained;
  r=&c->calls[5];r->args[1]=c->bytes;r->args[2]=PROT_READ|PROT_WRITE;
  r->args[3]=MAP_SHARED;r->args[4]=(uint64_t)c->descriptor;
  result=actual(r,__NR_mmap);
  if(!(result<0&&result>=-4095)) {
    /* Address zero is a genuine successful mmap output, not errno. Existing
     * arena/owner policy cannot use base zero; retain its exact successful
     * acquisition and prepared charge without retry, LIVE publication or FREE. */
    c->mapping=(uintptr_t)result;c->mapped=1;
    if(!result)c->zero_address_retained=1;
    else {
      c->partial_result=slcli_memory_backing_mapped(c->physical_owner,&c->request,c->mapping);
      c->partial_returned=1;
    }
  }
  if(!after(c,a,5)||!c->mapped||!c->partial_returned||!c->partial_result)goto retained;
  c->acquired_result=slcli_memory_acquired(c->physical_owner,&c->request,c->mapping,c->bytes,0,0);
  c->acquired_returned=1;if(!c->acquired_result)goto retained;
  c->qualified=1;return 1;
retained:
  c->retained_result=slcli_memory_unobserved(c->physical_owner,&c->request);c->retained_returned=1;return 0;
}
int slcli_preentry_backing_linux_create(struct slcli_preentry_backing_linux *c,
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,
  size_t caller_bytes,const struct slcli_native_clock *prior,uint64_t requested) {
  return create_original(c,a,d,caller,caller_bytes,prior,requested,a?a->owner:NULL,0);
}
int slcli_preentry_backing_linux_create_section(struct slcli_preentry_backing_linux *c,
  struct slcli_arena *a,struct slcli_memory_owner *physical,
  const struct slcli_source_deadline_original *d,const void *caller,
  size_t caller_bytes,const struct slcli_native_clock *prior,uint64_t requested) {
  return create_original(c,a,d,caller,caller_bytes,prior,requested,physical,1);
}
int slcli_preentry_backing_linux_receive(struct slcli_preentry_backing_linux *c,
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,
  size_t caller_bytes,const struct slcli_native_clock *prior,int descriptor,uint64_t bytes,
  const struct stat *expected,const struct slcli_extent_request *request_original) {
  size_t i;long result;struct slcli_preentry_backing_call *r;
  if(!c||!a||!slcli_memory_owner_mutable(a->owner)||!expected||!slcli_arena_live_block(a,c,sizeof(*c))||!caller_bytes||
     !slcli_arena_live_block(a,caller,caller_bytes)||!slcli_arena_live_range(a,expected,sizeof(*expected))||
     !slcli_source_deadline_original_matches(a,d,caller)||!prior||
     !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
     prior->tick<d->origin->tick||prior->tick>=d->expires||descriptor<0||!bytes||bytes>INT64_MAX||
     !a->owner->native_page_size||bytes%a->owner->native_page_size||
     !request_original||!slcli_arena_live_range(a,request_original,sizeof(*request_original))||
     !apart(c,sizeof(*c),request_original,sizeof(*request_original))||
     !slcli_memory_backing_request_matches(a->owner,request_original->index,request_original->sequence,bytes)||
     !apart(c,sizeof(*c),caller,caller_bytes)||!apart(c,sizeof(*c),prior,sizeof(*prior))||
     !apart(c,sizeof(*c),expected,sizeof(*expected))||!slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  c->started=1;c->operation=2;c->descriptor=descriptor;c->bytes=bytes;
  c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
  c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
  c->predecessor=prior;c->predecessor_tick=prior->tick;c->expected_backing_original=*expected;
  c->physical_owner=a->owner;c->request=*request_original;c->prepared=1;
  if(!before(c,a,0))goto retained_receive;
  r=&c->calls[0];r->args[0]=(uint64_t)descriptor;r->args[1]=F_GETFD;
  result=actual(r,__NR_fcntl);
  if(!after(c,a,0)||result<0||(result&FD_CLOEXEC))goto retained_receive;
  if(!before(c,a,1))goto retained_receive;
  r=&c->calls[1];r->args[0]=(uint64_t)descriptor;r->args[1]=F_GET_SEALS;
  result=actual(r,__NR_fcntl);
  if(!after(c,a,1)||result!=(F_SEAL_GROW|F_SEAL_SHRINK|F_SEAL_SEAL))goto retained_receive;
  if(!before(c,a,2))goto retained_receive;
  r=&c->calls[2];r->args[0]=(uint64_t)descriptor;r->args[1]=(uintptr_t)&c->backing_original;
  result=actual(r,__NR_fstat);
  if(!after(c,a,2)||result||c->backing_original.st_size<0||
     (uint64_t)c->backing_original.st_size!=bytes||c->expected_backing_original.st_size<0||
     (uint64_t)c->expected_backing_original.st_size!=bytes||
     c->backing_original.st_dev!=c->expected_backing_original.st_dev||
     c->backing_original.st_ino!=c->expected_backing_original.st_ino||
     c->backing_original.st_mode!=c->expected_backing_original.st_mode)goto retained_receive;
  if(!before(c,a,3))goto retained_receive;
  r=&c->calls[3];r->args[1]=bytes;r->args[2]=PROT_READ|PROT_WRITE;
  r->args[3]=MAP_SHARED;r->args[4]=(uint64_t)descriptor;
  result=actual(r,__NR_mmap);
  if(!(result<0&&result>=-4095)) {
    c->mapping=(uintptr_t)result;c->mapped=1;
    if(!result)c->zero_address_retained=1;
    else {c->partial_result=slcli_memory_backing_mapped(a->owner,&c->request,c->mapping);c->partial_returned=1;}
  }
  if(!after(c,a,3)||!c->mapped||!c->partial_returned||!c->partial_result)goto retained_receive;
  c->acquired_result=slcli_memory_acquired(a->owner,&c->request,c->mapping,bytes,0,0);
  c->acquired_returned=1;if(!c->acquired_result)goto retained_receive;
  c->qualified=1;return 1;
retained_receive:
  c->retained_result=slcli_memory_unobserved(a->owner,&c->request);c->retained_returned=1;return 0;
}
#endif
