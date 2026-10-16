#include "allocator_caller.h"
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__linux__)
#include "../host-service/native_syscall_linux.h"
#endif
static uint64_t thread_original(struct slcli_node_allocator_thread_row *c) {
  if(c->started)return 0;c->started=1;
#if defined(_WIN32)
  c->result=(uint64_t)GetCurrentThreadId();c->observed=1;
#elif defined(__linux__)
  c->args[0]=__NR_gettid;
  c->result=(uint64_t)slcli_kernel_call(__NR_gettid,0,0,0,0,0,0);c->observed=1;
  if((int64_t)c->result<=0)return 0;
#else
  c->result=0;
#endif
  return c->result;
}
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&
    !(a<b+m&&b<a+n);
}
static int prepare(struct slcli_arena *a,
  struct slcli_node_allocator_caller *c,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  struct slcli_node_allocator_wait_row *rows,size_t capacity,atomic_flag *lock,
  struct slcli_node_allocator_caller *creator) {
  size_t i,bytes;
  const unsigned char *raw;
  const struct slcli_extent *extent;
  if(!c||!rows||lock!=slcli_node_allocator_marker_original()||capacity<4||
    capacity>SIZE_MAX/sizeof(*rows)||
    !slcli_source_deadline_original_matches(a,d,caller))return 0;
  bytes=capacity*sizeof(*rows);
  if(!slcli_arena_live_block(a,c,sizeof(*c))||
    !slcli_arena_live_block(a,rows,bytes)||
    !slcli_arena_live_block(a,caller,caller_bytes)||
    !away(c,sizeof(*c),rows,bytes)||
    !away(c,sizeof(*c),caller,caller_bytes)||
    !away(rows,bytes,caller,caller_bytes)||
    !away(lock,sizeof(*lock),c,sizeof(*c))||
    !away(lock,sizeof(*lock),rows,bytes)||
    !away(lock,sizeof(*lock),a,sizeof(*a))||
    !away(lock,sizeof(*lock),a->owner,sizeof(*a->owner))||
    !slcli_source_deadline_away(d,lock,sizeof(*lock))||
    !away(lock,sizeof(*lock),caller,caller_bytes)||
    !slcli_source_deadline_away(d,caller,caller_bytes)||
    !slcli_source_deadline_away(d,c,sizeof(*c))||
    !slcli_source_deadline_away(d,rows,bytes))return 0;
  extent=&a->owner->extents[a->extent];
  if(extent->reserved>SIZE_MAX||
    !away(lock,sizeof(*lock),(const void *)extent->base,(size_t)extent->reserved))return 0;
  if(creator&&(!slcli_arena_live_block(a,creator,sizeof(*creator))||
    !creator->prepared||creator->unresolved||creator->unborn_thread||!creator->acquired||
    creator->owning_lock!=lock||creator->deadline!=d||creator->caller_original!=caller||
    creator->caller_bytes!=caller_bytes||!creator->native_thread||
    creator->used>creator->capacity||creator->capacity<4||
    creator->capacity>SIZE_MAX/sizeof(*creator->rows)||
    !slcli_arena_live_block(a,creator->rows,creator->capacity*sizeof(*creator->rows))||
    !away(c,sizeof(*c),creator,sizeof(*creator))||
    !away(rows,bytes,creator,sizeof(*creator))||
    !away(c,sizeof(*c),creator->rows,creator->capacity*sizeof(*creator->rows))||
    !away(rows,bytes,creator->rows,creator->capacity*sizeof(*creator->rows))))return 0;
  raw=(const unsigned char *)c;
  for(i=0;i<sizeof(*c);i++)if(raw[i])return 0;
  c->prepared=1; /* Failure after a native fact never grants storage reuse. */
  raw=(const unsigned char *)rows;
  for(i=0;i<bytes;i++)if(raw[i]){c->unresolved=1;return 0;}
  c->deadline=d;c->origin=d->origin;c->caller_original=caller;
  c->caller_bytes=caller_bytes;
  c->expires=d->expires;c->origin_tick=d->origin->tick;
  c->previous_tick=creator?creator->previous_tick:c->origin_tick;
  c->rows=rows;c->capacity=capacity;c->owning_lock=lock;
  if(creator){
    c->creator_original=creator;c->creator_native_thread=creator->native_thread;
    c->unborn_thread=1;return 1;
  }
  c->native_thread=thread_original(&c->thread);
  if(!c->native_thread){c->unresolved=1;return 0;}
  return 1;
}
int slcli_node_allocator_caller_prepare(struct slcli_arena *a,
  struct slcli_node_allocator_caller *c,const struct slcli_source_deadline_original *d,
  const void *caller,size_t caller_bytes,struct slcli_node_allocator_wait_row *rows,
  size_t capacity,atomic_flag *lock) {
  return prepare(a,c,d,caller,caller_bytes,rows,capacity,lock,0);
}
int slcli_node_allocator_caller_prepare_unborn(struct slcli_arena *a,
  struct slcli_node_allocator_caller *c,struct slcli_node_allocator_caller *creator,
  struct slcli_node_allocator_wait_row *rows,size_t capacity,atomic_flag *lock) {
  if(!creator||!slcli_arena_live_block(a,creator,sizeof(*creator)))return 0;
  return prepare(a,c,creator->deadline,creator->caller_original,creator->caller_bytes,
    rows,capacity,lock,creator);
}
static int clock_at(struct slcli_node_allocator_caller *c,struct slcli_node_allocator_wait_row *row) {
  if(thread_original(&row->thread)!=c->native_thread)return 0;
  if(c->deadline->origin!=c->origin||
    c->deadline->caller_original!=c->caller_original||
    c->deadline->expires!=c->expires||c->origin->tick!=c->origin_tick||
    !c->origin->started||!c->origin->observed||!c->origin->complete)
    return 0;
  if(!slcli_native_clock_read(&row->clock)||row->clock.tick<c->previous_tick||
    row->clock.tick>=c->expires)return 0;
  c->previous_tick=row->clock.tick;return 1;
}
static int clock_original(struct slcli_node_allocator_caller *c) {
  if(c->used>=c->capacity)return 0;
  return clock_at(c,&c->rows[c->used++]);
}
static int handler_before(struct slcli_node_allocator_caller *c,
  void (*handler)(void),int (*crt_handler)(size_t),size_t bytes,uint32_t kind,size_t *ordinal) {
  struct slcli_node_allocator_wait_row *row;size_t i,start;
  if(!c||!c->prepared||c->unresolved||c->unborn_thread||c->acquired||!ordinal||
    (c->child_phase&&atomic_load_explicit(c->child_phase,memory_order_acquire)!=2)||
    (kind==1?(!handler||crt_handler||bytes):(kind!=2||handler||!crt_handler))||
    c->owning_lock!=slcli_node_allocator_marker_original()||c->used>c->capacity||
    c->capacity>SIZE_MAX/sizeof(*c->rows)||c->capacity-c->used<2||
    !away(ordinal,sizeof(*ordinal),c,sizeof(*c))||
    !away(ordinal,sizeof(*ordinal),c->rows,c->capacity*sizeof(*c->rows))||
    !away(ordinal,sizeof(*ordinal),c->caller_original,c->caller_bytes)||
    !slcli_source_deadline_away(c->deadline,ordinal,sizeof(*ordinal))||
    !away(ordinal,sizeof(*ordinal),c->owning_lock,sizeof(*c->owning_lock)))return 0;
  start=c->used;row=&c->rows[start];
  for(i=0;i<2*sizeof(*row);i++)if(((const unsigned char *)row)[i]) {
    c->unresolved=1;return 0;
  }
  c->used+=2;row->handler_original=handler;row->crt_handler_original=crt_handler;
  row->handler_bytes=bytes;row->handler_kind=kind;
  if(!clock_at(c,row)){c->unresolved=1;return 0;}
  /* Reserve post row before the callback: nested allocations consume later
   * rows, never this original completion or its predecessor. */
  row->handler_called=1;*ordinal=start;return 1;
}
static int handler_after(struct slcli_node_allocator_caller *c,
  void (*handler)(void),int (*crt_handler)(size_t),size_t bytes,uint32_t kind,
  size_t ordinal,int result,int exception) {
  struct slcli_node_allocator_wait_row *row;int prior_unresolved,ok;
  if(!c||!c->prepared||ordinal>=c->used||c->used-ordinal<2||
    ordinal>=c->capacity||c->capacity-ordinal<2||(exception!=0&&exception!=1))return 0;
  row=&c->rows[ordinal];
  if(!row->handler_called||row->handler_returned||row->handler_exception||
    row->handler_original!=handler||row->crt_handler_original!=crt_handler||
    row->handler_bytes!=bytes||row->handler_kind!=kind)return 0;
  if(exception)row->handler_exception=1;else {row->handler_returned=1;row->handler_result=result;}
  if(c->acquired){c->unresolved=1;return 0;}
  prior_unresolved=(int)c->unresolved;ok=clock_at(c,&c->rows[ordinal+1]);
  if(!ok||prior_unresolved){c->unresolved=1;return 0;}return 1;
}
int slcli_node_allocator_caller_handler_before(struct slcli_node_allocator_caller *c,
  void (*handler)(void),size_t *ordinal){return handler_before(c,handler,0,0,1,ordinal);}
int slcli_node_allocator_caller_handler_after(struct slcli_node_allocator_caller *c,
  void (*handler)(void),size_t ordinal,int exception){return handler_after(c,handler,0,0,1,ordinal,0,exception);}
int slcli_node_allocator_caller_crt_handler_before(struct slcli_node_allocator_caller *c,
  int (*handler)(size_t),size_t bytes,size_t *ordinal){return handler_before(c,0,handler,bytes,2,ordinal);}
int slcli_node_allocator_caller_crt_handler_after(struct slcli_node_allocator_caller *c,
  int (*handler)(size_t),size_t bytes,size_t ordinal,int result,int exception) {
  return handler_after(c,0,handler,bytes,2,ordinal,result,exception);
}
static int native_before(struct slcli_node_allocator_caller *c,
  uint32_t operation,const void *address,size_t bytes,uint64_t argument0,uint64_t argument1,
  size_t *ordinal,int remaining_wait) {
  struct slcli_node_allocator_wait_row *row;size_t start,i,total;
  if(!c||!c->prepared||c->unresolved||c->unborn_thread||c->acquired||!ordinal||
    (c->child_phase&&atomic_load_explicit(c->child_phase,memory_order_acquire)!=2)||
    operation<1||operation>11||((operation==1||operation==2||operation==4)&&(argument0||argument1))||!address||!bytes||
    c->owning_lock!=slcli_node_allocator_marker_original()||
    c->capacity>SIZE_MAX/sizeof(*c->rows)||c->used>c->capacity||
    c->capacity-c->used<2)return 0;
  total=c->capacity*sizeof(*c->rows);
  if(!away(address,bytes,c,sizeof(*c))||!away(address,bytes,c->rows,total)||
    !away(address,bytes,c->caller_original,c->caller_bytes)||
    !slcli_source_deadline_away(c->deadline,address,bytes)||
    !away(address,bytes,c->owning_lock,sizeof(*c->owning_lock))||
    !away(ordinal,sizeof(*ordinal),address,bytes)||
    !away(ordinal,sizeof(*ordinal),c,sizeof(*c))||
    !away(ordinal,sizeof(*ordinal),c->rows,total)||
    !away(ordinal,sizeof(*ordinal),c->caller_original,c->caller_bytes)||
    !slcli_source_deadline_away(c->deadline,ordinal,sizeof(*ordinal))||
    !away(ordinal,sizeof(*ordinal),c->owning_lock,sizeof(*c->owning_lock)))return 0;
  start=c->used;row=&c->rows[start];
  for(i=0;i<2*sizeof(*row);i++)if(((const unsigned char *)row)[i]){
    c->unresolved=1;return 0;
  }
  c->used+=2;row->native_operation=operation;
  row->native_address=(uintptr_t)address;row->native_bytes=bytes;
  row->native_arguments[0]=argument0;row->native_arguments[1]=argument1;
  if(!clock_at(c,row)){c->unresolved=1;return 0;}
  if(remaining_wait) {
    uint64_t value=(c->expires-row->clock.tick)/UINT64_C(1000000);
    if(value>=UINT32_MAX)value=UINT32_MAX-1;
    row->native_arguments[1]=value;
  }
  row->native_called=1;*ordinal=start;return 1;
}
int slcli_node_allocator_caller_native_before(struct slcli_node_allocator_caller *c,
  uint32_t operation,const void *address,size_t bytes,uint64_t argument0,uint64_t argument1,size_t *ordinal) {
  return native_before(c,operation,address,bytes,argument0,argument1,ordinal,0);
}
int slcli_node_allocator_caller_native_raw_return(struct slcli_node_allocator_caller *c,
  uint32_t operation,const void *address,size_t bytes,size_t ordinal,uint64_t result) {
  struct slcli_node_allocator_wait_row *row;
  if(!c||!c->prepared||ordinal>=c->used||c->used-ordinal<2||
    ordinal>=c->capacity||c->capacity-ordinal<2)return 0;
  row=&c->rows[ordinal];
  if(!row->native_called||row->native_returned||row->native_raw_returned||
    row->native_operation!=operation||row->native_address!=(uintptr_t)address||
    row->native_bytes!=bytes)return 0;
  row->native_raw_result=result;row->native_raw_returned=1;return 1;
}
int slcli_node_allocator_caller_native_after(struct slcli_node_allocator_caller *c,
  uint32_t operation,const void *address,size_t bytes,size_t ordinal,uint64_t result) {
  struct slcli_node_allocator_wait_row *row;int prior_unresolved,ok;
  if(!c||!c->prepared||ordinal>=c->used||c->used-ordinal<2||
    ordinal>=c->capacity||c->capacity-ordinal<2)return 0;
  row=&c->rows[ordinal];
  if(!row->native_called||row->native_returned||row->native_operation!=operation||
    row->native_address!=(uintptr_t)address||row->native_bytes!=bytes)return 0;
  /* A real return/release cannot disappear when a subsequent clock fails. */
  row->native_result=result;row->native_returned=1;
  if(c->acquired){c->unresolved=1;return 0;}
  prior_unresolved=(int)c->unresolved;ok=clock_at(c,&c->rows[ordinal+1]);
  if(!ok||prior_unresolved){c->unresolved=1;return 0;}return 1;
}
int slcli_node_allocator_caller_wait_before(struct slcli_node_allocator_caller *c,
  const void *address,size_t bytes,uint64_t handle,uint32_t *milliseconds,size_t *ordinal) {
  struct slcli_node_allocator_wait_row *row;size_t total;
  if(!c||!c->prepared||!milliseconds||!ordinal||!handle||
     c->capacity>SIZE_MAX/sizeof(*c->rows))return 0;
  total=c->capacity*sizeof(*c->rows);
  if(!away(milliseconds,sizeof(*milliseconds),ordinal,sizeof(*ordinal))||
     !away(milliseconds,sizeof(*milliseconds),address,bytes)||
     !away(milliseconds,sizeof(*milliseconds),c,sizeof(*c))||
     !away(milliseconds,sizeof(*milliseconds),c->rows,total)||
     !away(milliseconds,sizeof(*milliseconds),c->caller_original,c->caller_bytes)||
     !slcli_source_deadline_away(c->deadline,milliseconds,sizeof(*milliseconds))||
     !away(milliseconds,sizeof(*milliseconds),c->owning_lock,sizeof(*c->owning_lock)))return 0;
  if(!native_before(c,7,address,bytes,handle,0,ordinal,1))return 0;
  row=&c->rows[*ordinal];
  /* clock_at has observed this exact caller/thread/origin/expiry and advanced
   * its retained monotonic predecessor. Floor relative ms never renews it. */
  /* The argument was formed before native_called publication and stays
   * immutable afterwards. It never equals Windows INFINITE. */
  *milliseconds=(uint32_t)row->native_arguments[1];return 1;
}
int slcli_node_allocator_caller_take(struct slcli_node_allocator_caller *c,atomic_flag *lock) {
  if(!c||!c->prepared||c->unresolved||c->unborn_thread||c->acquired||
    (c->child_phase&&atomic_load_explicit(c->child_phase,memory_order_acquire)!=2)||
    c->owning_lock!=lock||lock!=slcli_node_allocator_marker_original())return 0;
  /* Each retry consumes fresh precharged raw native clock storage. Neither
   * scheduler delay nor unavailable clocks can create an unbounded wait.
   * Contention itself does not close other normal allocator callers. */
  /* Keep TWO fresh release identity/clock rows BEFORE acquiring the marker.
   * Actual clear and its post-clock share the same original deadline. */
  while(c->used<=c->capacity&&c->capacity-c->used>=4) {
    struct slcli_node_allocator_wait_row *row;
    if(!clock_original(c))break;
    row=&c->rows[c->used-1];row->lock_called=1;
    row->lock_was_set=(uint32_t)atomic_flag_test_and_set_explicit(lock,memory_order_acquire);
    if(!row->lock_was_set) {
      c->acquisition_row=c->used-1;
      c->acquired=1;
      if(clock_original(c))return 1;
      c->unresolved=1;
      slcli_node_allocator_caller_release(c,lock);return 0;
    }
  }
  c->unresolved=1;return 0;
}
int slcli_node_allocator_caller_release(struct slcli_node_allocator_caller *c,atomic_flag *lock) {
  int pre_ok,post_ok,prior_unresolved;size_t start,i;
  struct slcli_node_allocator_wait_row *pre,*post;
  if(!c||!c->acquired||c->owning_lock!=lock||lock!=slcli_node_allocator_marker_original()||
    c->used>c->capacity||c->capacity-c->used<2)return 0;
  start=c->used;pre=&c->rows[start];post=&c->rows[start+1];
  for(i=0;i<2*sizeof(*pre);i++)if(((const unsigned char *)pre)[i]){
    c->unresolved=1;return 0;
  }
  /* Reserve both original rows before even the pre-clear native clock. */
  c->used+=2;prior_unresolved=(int)c->unresolved;pre_ok=clock_at(c,pre);
  if(!pre_ok)c->unresolved=1;
  /* A different native thread may not release the original holder's marker.
   * An expired clock on the SAME holder still retains actual release facts. */
  if(!pre->thread.observed||pre->thread.result!=c->native_thread)return 0;
  c->acquired=0;atomic_flag_clear_explicit(lock,memory_order_release);
  pre->marker_released=1; /* Actual effect precedes fallible post-clock. */
  post_ok=clock_at(c,post);
  if(!pre_ok||!post_ok||prior_unresolved){c->unresolved=1;return 0;}
  return 1;
}
