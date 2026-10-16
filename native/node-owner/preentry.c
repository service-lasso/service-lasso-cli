#include "preentry.h"
#include "early_allocator.h"
#include "thread_windows.h"
#if defined(_WIN32)
#include "entry_windows.h"
#endif
#if defined(__linux__)
#include "../host-service/native_syscall_linux.h"
static int apart(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_node_preentry_linux_bind_storage(struct slcli_node_preentry_linux *c,
  struct slcli_arena *bootstrap,const struct slcli_source_deadline_original *d,
  const void *caller,size_t caller_bytes,const struct slcli_native_clock *prior,
  int descriptor,uint64_t bytes,const struct stat *expected,size_t capacity,const struct slcli_extent_request *request_original) {
  size_t i;const struct slcli_extent *e;
  if(!c||!bootstrap||!slcli_arena_live_block(bootstrap,c,sizeof(*c))||
     capacity<4||capacity>SIZE_MAX/sizeof(*c->rows)||
     !slcli_source_deadline_original_matches(bootstrap,d,caller)||
     !slcli_source_deadline_away(d,c,sizeof(*c))||
     !apart(c,sizeof(*c),caller,caller_bytes)||!apart(c,sizeof(*c),prior,sizeof(*prior))||
     !apart(c,sizeof(*c),expected,sizeof(*expected)))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  c->started=1;c->capacity=capacity;c->deadline_original=d;c->origin_original=d->origin;
  c->expires_original=d->expires;
  for(i=0;i<sizeof(c->origin_before);i++)
    ((unsigned char *)&c->origin_before)[i]=((const unsigned char *)d->origin)[i];
  c->receiver=(struct slcli_preentry_backing_linux *)slcli_arena_calloc(bootstrap,1,sizeof(*c->receiver));
  if(!c->receiver)return 0;
  c->receive_result=slcli_preentry_backing_linux_receive(c->receiver,bootstrap,d,caller,
    caller_bytes,prior,descriptor,bytes,expected,request_original);c->receive_returned=1;
  if(!c->receive_result)return 0;
  e=&bootstrap->owner->extents[c->receiver->request.index];
  /* This dedicated child runtime backing contains no parent original records.
   * Preserve any nonzero contents rather than initializing over them. Actual
   * exclusive inherited backing/lifetime admission remains the constructor's
   * independent obligation; this scan does not acquire that authority. */
  if(!slcli_native_clock_read(&c->zero_before)||
     c->zero_before.tick<c->receiver->calls[3].after.tick||c->zero_before.tick>=d->expires)return 0;
  if(e->reserved>SIZE_MAX)return 0;
  if(e->state!=SLCLI_EXTENT_LIVE||e->base!=(uintptr_t)c->receiver->mapping||e->reserved!=bytes||
     e->acquisition!=c->receiver->request.sequence)return 0;
  for(i=0;i<e->reserved;i++) {
    unsigned char value=((const unsigned char *)e->base)[i];c->zero_scanned=i+1;
    if(value){c->zero_nonzero_offset=i;c->zero_nonzero_value=value;break;}
  }
  c->zero_result=i==e->reserved;c->zero_scan_returned=1;
  c->zero_clock_result=slcli_native_clock_read(&c->zero_after)&&
    c->zero_after.tick>=c->zero_before.tick&&c->zero_after.tick<c->expires_original&&
    slcli_source_deadline_original_matches(bootstrap,d,caller);
  if(!c->zero_clock_result||!c->zero_result)return 0;
  c->arena_result=slcli_arena_init(&c->child_arena,bootstrap->owner,c->receiver->request.index);
  c->arena_returned=1;if(!c->arena_result)return 0;
  c->child_caller=(struct slcli_node_preentry_caller *)slcli_arena_calloc(&c->child_arena,1,sizeof(*c->child_caller));
  if(!c->child_caller)return 0;
  c->child_caller->transported_deadline_original=d;
  c->child_caller->transported_caller_original=caller;c->child_caller->receiver_original=c->receiver;
  if(!slcli_native_clock_read(&c->child_caller->native_before[0])||
     c->child_caller->native_before[0].tick<c->zero_after.tick||
     c->child_caller->native_before[0].tick>=c->expires_original)return 0;
  c->child_caller->process_result=(uint64_t)slcli_kernel_call(__NR_getpid,0,0,0,0,0,0);
  c->child_caller->process_returned=1;
  if(!slcli_native_clock_read(&c->child_caller->native_before[1])||
     c->child_caller->native_before[1].tick<c->child_caller->native_before[0].tick||
     c->child_caller->native_before[1].tick>=c->expires_original)return 0;
  c->child_caller->thread_result=(uint64_t)slcli_kernel_call(__NR_gettid,0,0,0,0,0,0);
  c->child_caller->thread_returned=1;
  if(!slcli_native_clock_read(&c->child_caller->clock)||
     c->child_caller->clock.tick<c->child_caller->native_before[1].tick||c->child_caller->clock.tick>=d->expires||
     (int64_t)c->child_caller->process_result<=0||(int64_t)c->child_caller->thread_result<=0)return 0;
  c->retained_origin=(struct slcli_native_clock *)slcli_arena_alloc(&c->child_arena,sizeof(*c->retained_origin));
  c->child_deadline=(struct slcli_source_deadline_original *)slcli_arena_calloc(&c->child_arena,1,sizeof(*c->child_deadline));
  c->allocator=(struct slcli_node_allocator_caller *)slcli_arena_calloc(&c->child_arena,1,sizeof(*c->allocator));
  c->rows=(struct slcli_node_allocator_wait_row *)slcli_arena_calloc(&c->child_arena,capacity,sizeof(*c->rows));
  if(!c->retained_origin||!c->child_deadline||!c->allocator||!c->rows)return 0;
  /* Preserve the genuine transported native origin's full raw bytes and exact
   * expiry. This new local storage representation creates no clock/interval,
   * actor or source admission; parent/caller association stays explicitly held. */
  for(i=0;i<sizeof(*c->retained_origin);i++)
    ((unsigned char *)c->retained_origin)[i]=((const unsigned char *)&c->origin_before)[i];
  if(d->origin!=c->origin_original||d->expires!=c->expires_original||
     d->caller_original!=caller)return 0;
  for(i=0;i<sizeof(c->origin_before);i++)
    if(((const unsigned char *)d->origin)[i]!=((const unsigned char *)&c->origin_before)[i])return 0;
  c->child_deadline->origin=c->retained_origin;c->child_deadline->expires=c->expires_original;
  c->child_deadline->caller_original=c->child_caller;
  c->bind_result=slcli_node_allocator_bind_original(&c->child_arena);c->bind_returned=1;
  if(!c->bind_result)return 0;
  c->prepare_result=slcli_node_allocator_prepare_caller_original(&c->child_arena,c->allocator,
    c->child_deadline,c->child_caller,sizeof(*c->child_caller),c->rows,capacity);c->prepare_returned=1;
  if(!c->prepare_result||c->allocator->native_thread!=c->child_caller->thread_result)return 0;
  c->attach_result=slcli_node_allocator_attach_caller_original(c->allocator);c->attach_returned=1;
  return c->attach_result;
}
#endif

#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static void *prepared_stage(struct slcli_node_preentry_windows *c,
 struct slcli_node_bootstrap *b,const struct slcli_node_capture_layout_windows *shape,
 uint32_t stage,size_t bytes) {
 size_t i;unsigned char *value;
 if(stage>=SLCLI_CAPTURE_STAGE_COUNT||shape->stages[stage].requested!=bytes)return 0;
 value=(unsigned char *)b->storage_entry_original->route.base+shape->stages[stage].offset;
 if(!slcli_arena_live_block(&b->arena,value,bytes))return 0;
 c->storage_stage=stage;c->storage_scanned=0;
 for(i=0;i<bytes;i++) {
  unsigned char raw=value[i];c->storage_scanned=i+1;
  if(raw){c->storage_nonzero_offset=i;c->storage_nonzero_value=raw;return 0;}
 }
 return value;
}
int slcli_node_preentry_windows_bind_storage(struct slcli_node_preentry_windows *c,
  struct slcli_arena *bootstrap,const struct slcli_source_deadline_original *d,
  const void *caller,size_t caller_bytes,const struct slcli_native_clock *prior,
  HANDLE backing,HANDLE expected,uint64_t bytes,const FILETIME *expected_creation,size_t capacity,size_t thread_capture_attempts,const struct slcli_extent_request *request_original) {
  size_t i;struct slcli_node_bootstrap *b=slcli_node_bootstrap_image_original();
  struct slcli_node_capture_layout_windows shape;
  if(!b||!b->constructed||!b->tail_adopted||!b->storage_entry_original||
     bootstrap!=&b->arena||c!=b->node||d!=b->deadline||caller!=b->caller||
     !slcli_node_capture_layout_windows_live_headers_original(&b->storage_entry_original->route,&shape)||
     (void *)c!=(void *)((unsigned char *)b->storage_entry_original->route.base+shape.stages[SLCLI_CAPTURE_PREENTRY].offset)||
     capacity!=shape.events||thread_capture_attempts!=shape.threads||
     !c||!bootstrap||!slcli_arena_live_block(bootstrap,c,sizeof(*c))||
     !thread_capture_attempts||capacity<5||capacity>SIZE_MAX/sizeof(*c->rows)||
     !prior||!slcli_arena_live_range(bootstrap,prior,sizeof(*prior))||!prior->complete||
     !slcli_source_deadline_original_matches(bootstrap,d,caller)||
     !slcli_source_deadline_away(d,c,sizeof(*c))||
     !apart(c,sizeof(*c),caller,caller_bytes)||!apart(c,sizeof(*c),prior,sizeof(*prior))||
     !expected_creation||!slcli_arena_live_range(bootstrap,expected_creation,sizeof(*expected_creation))||
     !apart(c,sizeof(*c),expected_creation,sizeof(*expected_creation)))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  c->started=1;c->capacity=capacity;c->deadline_original=d;c->origin_original=d->origin;
  c->expires_original=d->expires;
  c->constructor_before_result=slcli_native_clock_read(&c->constructor_before);
  if(!c->constructor_before_result||c->constructor_before.tick<prior->tick||c->constructor_before.tick>=d->expires)return 0;
  for(i=0;i<sizeof(c->origin_before);i++)
    ((unsigned char *)&c->origin_before)[i]=((const unsigned char *)d->origin)[i];
  c->receiver=prepared_stage(c,b,&shape,SLCLI_CAPTURE_RUNTIME_RECEIVER,sizeof(*c->receiver));
  if(!c->receiver)return 0;
  c->receive_result=slcli_preentry_backing_windows_recheck_mapped(c->receiver,bootstrap,d,caller,
    caller_bytes,&c->constructor_before,backing,expected,bytes,request_original);c->receive_returned=1;
  if(!c->receive_result)return 0;
  c->child_arena=*bootstrap;
  c->child_caller=prepared_stage(c,b,&shape,SLCLI_CAPTURE_MAIN_CALLER,sizeof(*c->child_caller));
  if(!c->child_caller)return 0;
  c->child_caller->transported_deadline_original=d;
  c->child_caller->transported_caller_original=caller;c->child_caller->receiver_original=c->receiver;
  if(!slcli_native_clock_read(&c->child_caller->native_before[0])||
     c->child_caller->native_before[0].tick<c->receiver->after[4].tick||
     c->child_caller->native_before[0].tick>=c->expires_original)return 0;
  c->child_caller->process_result=GetCurrentProcessId();
  c->child_caller->process_returned=1;
  if(!slcli_native_clock_read(&c->child_caller->native_before[1])||
     c->child_caller->native_before[1].tick<c->child_caller->native_before[0].tick||
     c->child_caller->native_before[1].tick>=c->expires_original)return 0;
  c->child_caller->thread_result=GetCurrentThreadId();
  c->child_caller->thread_returned=1;
  c->child_caller->expected_creation_original=*expected_creation;
  if(!slcli_native_clock_read(&c->child_caller->native_before[2])||
     c->child_caller->native_before[2].tick<c->child_caller->native_before[1].tick||
     c->child_caller->native_before[2].tick>=c->expires_original)return 0;
  c->child_caller->times_args[0]=(uintptr_t)GetCurrentProcess();
  c->child_caller->times_args[1]=(uintptr_t)&c->child_caller->creation_time;
  c->child_caller->times_args[2]=(uintptr_t)&c->child_caller->exit_time;
  c->child_caller->times_args[3]=(uintptr_t)&c->child_caller->kernel_time;
  c->child_caller->times_args[4]=(uintptr_t)&c->child_caller->user_time;c->child_caller->times_called=1;
  c->child_caller->times_status=GetProcessTimes((HANDLE)(uintptr_t)c->child_caller->times_args[0],
    &c->child_caller->creation_time,&c->child_caller->exit_time,
    &c->child_caller->kernel_time,&c->child_caller->user_time);
  c->child_caller->times_returned=1;
  if(!c->child_caller->times_status)c->child_caller->times_error=GetLastError();
  if(!slcli_native_clock_read(&c->child_caller->clock)||
     c->child_caller->clock.tick<c->child_caller->native_before[2].tick||c->child_caller->clock.tick>=d->expires||
          !c->child_caller->process_result||!c->child_caller->thread_result||!c->child_caller->times_status||
     c->child_caller->creation_time.dwHighDateTime!=c->child_caller->expected_creation_original.dwHighDateTime||
     c->child_caller->creation_time.dwLowDateTime!=c->child_caller->expected_creation_original.dwLowDateTime)return 0;
  c->retained_origin=prepared_stage(c,b,&shape,SLCLI_CAPTURE_MAIN_ORIGIN,sizeof(*c->retained_origin));
  c->child_deadline=prepared_stage(c,b,&shape,SLCLI_CAPTURE_MAIN_DEADLINE,sizeof(*c->child_deadline));
  c->allocator=prepared_stage(c,b,&shape,SLCLI_CAPTURE_MAIN_ALLOCATOR,sizeof(*c->allocator));
  c->rows=prepared_stage(c,b,&shape,SLCLI_CAPTURE_MAIN_EVENTS,capacity*sizeof(*c->rows));
  if(!c->retained_origin||!c->child_deadline||!c->allocator||!c->rows)return 0;
  /* Preserve the genuine transported native origin's full raw bytes and exact
   * expiry. This new local storage representation creates no clock/interval,
   * actor or source admission; parent/caller association stays explicitly held. */
  for(i=0;i<sizeof(*c->retained_origin);i++)
    ((unsigned char *)c->retained_origin)[i]=((const unsigned char *)&c->origin_before)[i];
  if(d->origin!=c->origin_original||d->expires!=c->expires_original||
     d->caller_original!=caller)return 0;
  for(i=0;i<sizeof(c->origin_before);i++)
    if(((const unsigned char *)d->origin)[i]!=((const unsigned char *)&c->origin_before)[i])return 0;
  c->child_deadline->origin=c->retained_origin;c->child_deadline->expires=c->expires_original;
  c->child_deadline->caller_original=c->child_caller;
  c->bind_result=slcli_node_allocator_bind_original(&c->child_arena);c->bind_returned=1;
  if(!c->bind_result)return 0;
  c->schedule_before_result=slcli_native_clock_read(&c->schedule_before);
  if(!c->schedule_before_result||c->schedule_before.tick<c->child_caller->clock.tick||
    c->schedule_before.tick>=c->expires_original)return 0;
  c->thread_capture_attempts=thread_capture_attempts;
  slcli_node_allocator_adopt_child_capture_windows_original(&c->child_arena,&b->storage_entry_original->route);
  c->thread_capture=slcli_node_allocator_child_capture_original();c->thread_capture_returned=1;
  c->schedule_after_result=slcli_native_clock_read(&c->schedule_after);
  if(!c->schedule_after_result||c->schedule_after.tick<c->schedule_before.tick||
    c->schedule_after.tick>=c->expires_original)return 0;
  if(!c->thread_capture||!c->thread_capture->ready)return 0;
  c->prepare_result=slcli_node_allocator_prepare_caller_original(&c->child_arena,c->allocator,
    c->child_deadline,c->child_caller,sizeof(*c->child_caller),c->rows,capacity);c->prepare_returned=1;
  if(!c->prepare_result||c->allocator->native_thread!=c->child_caller->thread_result)return 0;
  c->attach_result=slcli_node_allocator_attach_caller_original(c->allocator);c->attach_returned=1;
  c->constructor_after_result=slcli_native_clock_read(&c->constructor_after);
  if(!c->constructor_after_result||c->constructor_after.tick<c->schedule_after.tick||
    c->constructor_after.tick<c->allocator->previous_tick||
    c->constructor_after.tick>=c->expires_original||
    !slcli_source_deadline_original_matches(&c->child_arena,c->child_deadline,c->child_caller))return 0;
  return c->attach_result;
}

#endif
