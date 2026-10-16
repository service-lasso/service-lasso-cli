#include "early_allocator.h"
#include "allocator_caller.h"
#include "../host-service/arena.h"
#include <stdatomic.h>
#if defined(_WIN32)
#include "capture_layout_windows.h"
#endif
/* BSS/lock/header storage belongs to the original full Node image owner and
 * must already be captured at preentry. Static initialization allocates none.
 * Genuine source entry, startup/stack/TLS and other native allocator boundaries
 * remain independent required integrations; this is not whole hook coverage. */
static atomic_flag lock=ATOMIC_FLAG_INIT;
atomic_flag *slcli_node_allocator_marker_original(void){return &lock;}
static atomic_int unresolved=ATOMIC_VAR_INIT(0);
_Static_assert(ATOMIC_INT_LOCK_FREE==2,"allocator unresolved gate must be lock-free");
static struct slcli_arena *original_arena;
static struct slcli_node_child_capture_schedule *child_capture;
static uint32_t child_capture_attempted;
static size_t child_capture_count,child_capture_used,child_capture_bytes,child_capture_events;
static struct slcli_node_child_reservation *child_capture_slots;
struct slcli_node_child_capture_schedule *slcli_node_allocator_child_capture_original(void){return child_capture;}
size_t slcli_node_allocator_child_capture_bytes_original(size_t slots,size_t birth,size_t events) {
  size_t a,b,c,d,e,per,total;
  if(!slots||!birth||events<5||slots>SIZE_MAX/sizeof(struct slcli_node_child_reservation)||
     events>SIZE_MAX/sizeof(struct slcli_node_allocator_wait_row))return 0;
  a=slcli_arena_allocation_bytes(sizeof(struct slcli_node_child_capture_schedule));
  b=slcli_arena_allocation_bytes(slots*sizeof(struct slcli_node_child_reservation));
  c=slcli_arena_allocation_bytes(birth);
  d=slcli_arena_allocation_bytes(sizeof(struct slcli_node_allocator_caller));
  e=slcli_arena_allocation_bytes(events*sizeof(struct slcli_node_allocator_wait_row));
  if(!a||!b||!c||!d||!e||c>SIZE_MAX-d||c+d>SIZE_MAX-e)return 0;
  per=c+d+e;if(slots>SIZE_MAX/per||a>SIZE_MAX-b)return 0;
  total=a+b;if(slots*per>SIZE_MAX-total)return 0;return total+slots*per;
}
static struct slcli_arena held_arena;
static _Thread_local struct slcli_node_allocator_caller *current_caller;
const void *slcli_node_allocator_tls_slot_original(void){return &current_caller;}
static uint32_t attempted,failed;
static uint64_t original_limit,original_page_size,original_allocation_floor;
static uintptr_t original_page_geometry[2];
static unsigned char original_birth[32],original_image[32],original_owner[32],original_source[32];
struct allocation { void *raw,*result;size_t bytes,alignment;uint64_t marker; };
#define MARKER UINT64_C(0x534c434c494e4f44)
/* No scheduler-dependent wait and no unlocked mutation of arena/owner state.
 * A contending operation retains custody and closes this original binding.
 * This is an availability failure, not proof of the original native deadline. */
static int take(void){
  if(atomic_load_explicit(&unresolved,memory_order_acquire))return 0;
  if(current_caller)return slcli_node_allocator_caller_take(current_caller,&lock);
  if(atomic_flag_test_and_set_explicit(&lock,memory_order_acquire)){
    atomic_store_explicit(&unresolved,1,memory_order_release);return 0;
  }
  return 1;
}
static int release(void){
  if(current_caller)return slcli_node_allocator_caller_release(current_caller,&lock);
  atomic_flag_clear_explicit(&lock,memory_order_release);return 1;
}
static void hold_failure(void);
static struct slcli_node_allocator_wait_row *operation_row(void) {
  if(!current_caller||!current_caller->acquired||
     current_caller->acquisition_row>=current_caller->used)return 0;
  return &current_caller->rows[current_caller->acquisition_row];
}
static void record_request(uint32_t operation,void *argument,size_t bytes,size_t alignment,size_t count) {
  struct slcli_node_allocator_wait_row *r=operation_row();if(!r)return;
  r->allocator_operation=operation;r->argument=(uintptr_t)argument;
  r->bytes=bytes;r->alignment=alignment;r->count=count;
}
static int live(void) {
  const struct slcli_extent *e;
  size_t i;
  /* Bind/prepare do not allocate through this payload path. Every real
   * allocation/header/resize/free requires its SAME pre-effect capture row;
   * an attached arena alone cannot permit unrecorded native CRT allocations. */
  if(!current_caller||!current_caller->prepared||!current_caller->acquired||
    current_caller->unresolved||!operation_row())return 0;
  if(atomic_load_explicit(&unresolved,memory_order_acquire)){hold_failure();return 0;}
  if(failed||!original_arena||!slcli_memory_owner_writable(original_arena->owner)||original_arena->owner->failed||
     original_arena->owner->count>256||original_arena->extent>=original_arena->owner->count)return 0;
  /* Binding storage is not actor admission, but it must preserve the actual
   * original owning tuple and quota. A later mutable owner field cannot renew
   * a ceiling, swap an image/birth/source, or alter native page provenance. */
  if(original_arena->allocation_floor!=original_allocation_floor||
     original_arena->owner->limit!=original_limit||
     original_arena->owner->native_page_size!=original_page_size||
     original_arena->owner->native_page_geometry[0]!=original_page_geometry[0]||
     original_arena->owner->native_page_geometry[1]!=original_page_geometry[1]) {
    hold_failure();return 0;
  }
  for(i=0;i<32;i++)if(original_arena->owner->original_birth[i]!=original_birth[i]||
    original_arena->owner->original_image[i]!=original_image[i]||
    original_arena->owner->original_owner[i]!=original_owner[i]||
    original_arena->owner->original_source[i]!=original_source[i]) {
    hold_failure();return 0;
  }
  e=&original_arena->owner->extents[original_arena->extent];
  return e->state==SLCLI_EXTENT_LIVE&&e->arena_initialized&&
    original_allocation_floor==((uint64_t)e->arena_initialized-1)*16&&
    e->acquisition==original_arena->acquisition;
}
static void hold_failure(void) {
  failed=1;if(original_arena&&slcli_memory_owner_writable(original_arena->owner))
    original_arena->owner->failed=1;
}
int slcli_node_allocator_bind_original(struct slcli_arena *a) {
  const struct slcli_extent *e;int ok=0;size_t i;if(!take())return 0;
  if(attempted)goto done;attempted=1;
  if(!a||!slcli_memory_owner_writable(a->owner)||a->owner->failed||
     a->owner->count>256||a->extent>=a->owner->count)goto bad;
  e=&a->owner->extents[a->extent];
  if(e->state!=SLCLI_EXTENT_LIVE||!e->arena_initialized||e->acquisition!=a->acquisition||
     !e->base||e->reserved<48||e->reserved>a->owner->limit||
     a->allocation_floor%16||a->allocation_floor>e->reserved-48||
     a->allocation_floor!=((uint64_t)e->arena_initialized-1)*16)goto bad;
  held_arena=*a;original_arena=&held_arena;
  original_limit=a->owner->limit;original_page_size=a->owner->native_page_size;
  original_allocation_floor=a->allocation_floor;
  original_page_geometry[0]=a->owner->native_page_geometry[0];
  original_page_geometry[1]=a->owner->native_page_geometry[1];
  for(i=0;i<32;i++) {
    original_birth[i]=a->owner->original_birth[i];original_image[i]=a->owner->original_image[i];
    original_owner[i]=a->owner->original_owner[i];original_source[i]=a->owner->original_source[i];
  }
  ok=1;goto done;
bad:failed=1;
done:if(atomic_load_explicit(&unresolved,memory_order_acquire)){hold_failure();ok=0;}if(!release())ok=0;return ok;
}
int slcli_node_allocator_bound(void){int ok;if(!current_caller||!take())return 0;ok=live();if(atomic_load_explicit(&unresolved,memory_order_acquire)){hold_failure();ok=0;}if(!release())ok=0;return ok;}
int slcli_node_allocator_prepare_caller_original(struct slcli_arena *a,
  struct slcli_node_allocator_caller *c,const struct slcli_source_deadline_original *d,
  const void *caller,size_t caller_bytes,struct slcli_node_allocator_wait_row *rows,size_t capacity) {
  /* Preparation must be inside the genuine constructor's original owning
   * serialization, before other threads can mutate these logical blocks.
   * This function does not acquire that authority or create a native caller. */
  if(!original_arena||!a||a->owner!=original_arena->owner||
     a->extent!=original_arena->extent||a->acquisition!=original_arena->acquisition||
     a->allocation_floor!=original_allocation_floor||current_caller||
     atomic_load_explicit(&unresolved,memory_order_acquire))return 0;
  return slcli_node_allocator_caller_prepare(a,c,d,caller,caller_bytes,rows,capacity,&lock);
}
int slcli_node_allocator_prepare_child_original(struct slcli_arena *a,
  struct slcli_node_allocator_caller *child,struct slcli_node_allocator_wait_row *rows,size_t capacity) {
  int ok;
  if(!current_caller||!a||!original_arena||a->owner!=original_arena->owner||
     a->extent!=original_arena->extent||a->acquisition!=original_arena->acquisition||
     a->allocation_floor!=original_allocation_floor||!take())return 0;
  ok=live()&&slcli_node_allocator_caller_prepare_unborn(a,child,current_caller,rows,capacity,&lock);
  if(!release()){if(ok)child->unresolved=1;return 0;}return ok;
}
int slcli_node_allocator_attach_caller_original(struct slcli_node_allocator_caller *c) {
  int ok;
  if(current_caller||!c||!c->prepared||c->unborn_thread||c->owning_lock!=&lock||
     atomic_load_explicit(&unresolved,memory_order_acquire))return 0;
  current_caller=c;
  if(!slcli_node_allocator_caller_take(c,&lock))return 0;
  ok=slcli_node_allocator_caller_release(c,&lock);
  /* Retain even a failed attachment's original TLS/capture custody. No fresh
   * caller/deadline binding can replace it later on this native thread. */
  return ok;
}
struct slcli_node_child_reservation *slcli_node_allocator_reserve_child_original(size_t bytes) {
  struct slcli_node_child_reservation *r=0;
  struct slcli_node_allocator_wait_row *operation;
  size_t capacity;
  if(!current_caller||!bytes||!take())return 0;
  record_request(9,0,bytes,0,current_caller->capacity);
  if(!live())goto done;
  capacity=current_caller->capacity;
  if(!child_capture||!child_capture->ready||child_capture->slots!=child_capture_slots||
     child_capture->count!=child_capture_count||child_capture->used!=child_capture_used||
     child_capture->birth_bytes!=child_capture_bytes||child_capture->event_capacity!=child_capture_events||
     child_capture_used>=child_capture_count||child_capture_bytes!=bytes||child_capture_events!=capacity)goto done;
  /* Consume BEFORE preparation or native birth. No failed attempt, completed
   * thread or parent exit permits this retained slot to be reused. All blocks
   * were allocated before attachment, inside the planned capture prefix. */
  r=&child_capture_slots[child_capture_used++];child_capture->used=child_capture_used;
  operation=operation_row();if(operation)operation->new_raw=(uintptr_t)r;
  if(r->storage_bytes!=bytes||r->capacity!=capacity||
     !slcli_arena_live_block(original_arena,r->storage,bytes)||
     !slcli_arena_live_block(original_arena,r->child,sizeof(*r->child))||
     !slcli_arena_live_block(original_arena,r->rows,capacity*sizeof(*r->rows)))goto done;
  if(!slcli_node_allocator_caller_prepare_unborn(original_arena,r->child,current_caller,
       r->rows,capacity,&lock))goto done;
  operation=operation_row();if(operation){operation->completed_result=(uintptr_t)r;
    operation->result_ready=1;}
  if(!release()){r->child->unresolved=1;return 0;}return r;
done:
  /* Already materialized blocks/addresses stay registered and charged. */
  release();return 0;
}
struct slcli_node_child_capture_schedule *
slcli_node_allocator_prepare_child_capture_original(struct slcli_arena *a,
  size_t slots,size_t birth_bytes,size_t events) {
  size_t i,total,next;uintptr_t base;
  /* Genuine serialized preentry only: no attached caller, payload allocation,
   * native birth, fallback pool or late expansion is available here. */
  if(child_capture_attempted||current_caller||!original_arena||!a||
     a->owner!=original_arena->owner||a->extent!=original_arena->extent||
     a->acquisition!=original_arena->acquisition||a->allocation_floor!=original_allocation_floor||failed||
     atomic_load_explicit(&unresolved,memory_order_acquire))return 0;
  child_capture_attempted=1;
  total=slcli_node_allocator_child_capture_bytes_original(slots,birth_bytes,events);
  if(!total||total>a->owner->extents[a->extent].reserved)return 0;
  child_capture=slcli_arena_calloc(a,1,sizeof(*child_capture));
  if(!child_capture)return 0;
  child_capture->started=1;child_capture->count=slots;
  child_capture->birth_bytes=birth_bytes;child_capture->event_capacity=events;
  base=a->owner->extents[a->extent].base;
  if(!base||(uintptr_t)child_capture<base)return 0;
  child_capture->capture_begin=(size_t)((uintptr_t)child_capture-base);
  child_capture->capture_bytes=total;
  if(child_capture->capture_begin<32||child_capture->capture_begin>SIZE_MAX-slcli_arena_allocation_bytes(sizeof(*child_capture)))return 0;
  next=child_capture->capture_begin+slcli_arena_allocation_bytes(sizeof(*child_capture));
  child_capture->capture_begin-=32;
  child_capture->slots=slcli_arena_calloc(a,slots,sizeof(*child_capture->slots));
  if(!child_capture->slots)return 0;
  child_capture->slots_offset=(size_t)((uintptr_t)child_capture->slots-base);
  if(child_capture->slots_offset!=next||next>SIZE_MAX-slcli_arena_allocation_bytes(slots*sizeof(*child_capture->slots)))return 0;
  next+=slcli_arena_allocation_bytes(slots*sizeof(*child_capture->slots));
  for(i=0;i<slots;i++) {
    struct slcli_node_child_reservation *r=&child_capture->slots[i];
    r->storage_bytes=birth_bytes;r->capacity=events;
    r->storage=slcli_arena_calloc(a,1,birth_bytes);if(!r->storage)return 0;
    r->storage_offset=(size_t)((uintptr_t)r->storage-base);
    if(r->storage_offset!=next||next>SIZE_MAX-slcli_arena_allocation_bytes(birth_bytes))return 0;
    next+=slcli_arena_allocation_bytes(birth_bytes);
    r->child=slcli_arena_calloc(a,1,sizeof(*r->child));if(!r->child)return 0;
    r->child_offset=(size_t)((uintptr_t)r->child-base);
    if(r->child_offset!=next||next>SIZE_MAX-slcli_arena_allocation_bytes(sizeof(*r->child)))return 0;
    next+=slcli_arena_allocation_bytes(sizeof(*r->child));
    r->rows=slcli_arena_calloc(a,events,sizeof(*r->rows));if(!r->rows)return 0;
    r->rows_offset=(size_t)((uintptr_t)r->rows-base);
    if(r->rows_offset!=next||next>SIZE_MAX-slcli_arena_allocation_bytes(events*sizeof(*r->rows)))return 0;
    next+=slcli_arena_allocation_bytes(events*sizeof(*r->rows));
  }
  /* next denotes the hypothetical next payload. Its preceding real32-byte
   * header is the first byte outside this complete logical contribution. */
  if(next<32||next-32<child_capture->capture_begin||
     next-32-child_capture->capture_begin!=total||next-32>a->owner->extents[a->extent].reserved)return 0;
  child_capture->capture_end=next-32;
  child_capture_slots=child_capture->slots;child_capture_count=slots;
  child_capture_bytes=birth_bytes;child_capture_events=events;
  child_capture->ready=1;return child_capture;
}
#if defined(_WIN32)
static int prepared_zero(struct slcli_arena *a,void *value,size_t bytes,size_t block) {
 size_t i;
 if(!slcli_arena_live_block(a,value,bytes))return 0;
 child_capture->prepared_block=block;child_capture->zero_scanned=0;
 for(i=0;i<bytes;i++) {
  unsigned char raw=((const unsigned char *)value)[i];child_capture->zero_scanned=i+1;
  if(raw){child_capture->nonzero_offset=i;child_capture->nonzero_value=raw;return 0;}
 }
 return 1;
}
struct slcli_node_child_capture_schedule *
slcli_node_allocator_adopt_child_capture_windows_original(struct slcli_arena *a,
 const struct slcli_node_route_windows *route) {
 struct slcli_node_capture_layout_windows shape;size_t i,j,total,begin,end,at;
 uintptr_t base;struct slcli_node_child_capture_schedule *schedule;
 if(child_capture_attempted||current_caller||!original_arena||!a||
  a->owner!=original_arena->owner||a->extent!=original_arena->extent||
  a->acquisition!=original_arena->acquisition||a->allocation_floor!=original_allocation_floor||
  !a->allocation_floor||failed||atomic_load_explicit(&unresolved,memory_order_acquire)||
  !slcli_node_capture_layout_windows_live_headers_original(route,&shape)||
  (uintptr_t)route->base!=a->owner->extents[a->extent].base||
  route->bytes!=a->owner->extents[a->extent].reserved||a->allocation_floor!=shape.capture_bytes)return 0;
 child_capture_attempted=1;base=(uintptr_t)route->base;
 schedule=(struct slcli_node_child_capture_schedule *)(base+shape.stages[SLCLI_CAPTURE_THREAD_SCHEDULE].offset);
 if(!slcli_arena_live_block(a,schedule,sizeof(*schedule)))return 0;
 for(j=0;j<sizeof(*schedule);j++)if(((const unsigned char *)schedule)[j])return 0;
 child_capture=schedule;child_capture->started=1;
 child_capture->count=shape.threads;child_capture->birth_bytes=shape.birth_bytes;
 child_capture->event_capacity=shape.events;
 total=slcli_node_allocator_child_capture_bytes_original(shape.threads,shape.birth_bytes,shape.events);
 begin=shape.stages[SLCLI_CAPTURE_THREAD_SCHEDULE].offset-32;
 end=shape.thread_begin+shape.threads*shape.thread_stride;
 if(!total||end<begin||end-begin!=total||end>shape.capture_bytes)return 0;
 child_capture->capture_begin=begin;child_capture->capture_end=end;child_capture->capture_bytes=total;
 child_capture->slots=(struct slcli_node_child_reservation *)(base+shape.stages[SLCLI_CAPTURE_THREAD_SLOTS].offset);
 child_capture->slots_offset=shape.stages[SLCLI_CAPTURE_THREAD_SLOTS].offset;
 if(!prepared_zero(a,child_capture->slots,shape.threads*sizeof(*child_capture->slots),0))return 0;
 for(i=0;i<shape.threads;i++) {
  struct slcli_node_child_reservation *r=&child_capture->slots[i];
  void *storage;struct slcli_node_allocator_caller *caller;struct slcli_node_allocator_wait_row *rows;
  child_capture->prepared_slot=i;at=shape.thread_begin+i*shape.thread_stride;
  storage=(void *)(base+at+32);caller=(void *)(base+at+shape.thread_caller_delta+32);
  rows=(void *)(base+at+shape.thread_events_delta+32);
  if(!prepared_zero(a,storage,shape.birth_bytes,1)||!prepared_zero(a,caller,sizeof(*caller),2)||
   !prepared_zero(a,rows,shape.events*sizeof(*rows),3))return 0;
  r->storage=storage;r->child=caller;r->rows=rows;r->storage_bytes=shape.birth_bytes;r->capacity=shape.events;
  r->storage_offset=at+32;r->child_offset=at+shape.thread_caller_delta+32;
  r->rows_offset=at+shape.thread_events_delta+32;
 }
 child_capture_slots=child_capture->slots;child_capture_count=shape.threads;
 child_capture_bytes=shape.birth_bytes;child_capture_events=shape.events;
 child_capture->ready=1;return child_capture;
}
#endif
int slcli_node_allocator_is_current_original(const struct slcli_node_allocator_caller *c) {
  return c&&current_caller==c&&c->prepared&&!c->unresolved&&!c->unborn_thread&&
    !c->acquired&&c->owning_lock==&lock&&
    (!c->child_phase||atomic_load_explicit(c->child_phase,memory_order_acquire)==2)&&
    !atomic_load_explicit(&unresolved,memory_order_acquire);
}
static void *allocate(size_t alignment,size_t bytes) {
  unsigned char *raw;uintptr_t at;struct allocation *h,*original;
  size_t payload=bytes?bytes:1,total;
  if(!live()||!alignment||(alignment&(alignment-1)))return 0;
  if(alignment<_Alignof(struct allocation))alignment=_Alignof(struct allocation);
  if(
     alignment>SIZE_MAX-2*sizeof(*h)||payload>SIZE_MAX-2*sizeof(*h)-(alignment-1))return 0;
  total=payload+2*sizeof(*h)+alignment-1;raw=slcli_arena_alloc(original_arena,total);if(!raw)return 0;
  {struct slcli_node_allocator_wait_row *r=operation_row();if(r)r->new_raw=(uintptr_t)raw;}
  at=(uintptr_t)raw;
  if(at>UINTPTR_MAX-2*sizeof(*h)-(alignment-1)){hold_failure();return 0;}
  at=(at+2*sizeof(*h)+alignment-1)&~(uintptr_t)(alignment-1);h=(void *)(at-sizeof(*h));
  original=(void *)raw;
  original->raw=raw;original->result=(void *)at;original->bytes=bytes;
  original->alignment=alignment;original->marker=MARKER;
  *h=*original;return (void *)at;
}
int slcli_node_allocator_handler_before_original(void (*handler)(void),size_t *ordinal) {
  if(!current_caller||atomic_load_explicit(&unresolved,memory_order_acquire))return 0;
  return slcli_node_allocator_caller_handler_before(current_caller,handler,ordinal);
}
int slcli_node_allocator_handler_after_original(void (*handler)(void),size_t ordinal,int exception) {
  if(!current_caller)return 0;
  /* Record completion even when a nested operation has already denied future
   * allocations. The real handler return/unwind must not disappear. */
  return slcli_node_allocator_caller_handler_after(current_caller,handler,ordinal,exception);
}
int slcli_node_allocator_crt_handler_before_original(int (*handler)(size_t),size_t bytes,size_t *ordinal) {
  if(!current_caller||atomic_load_explicit(&unresolved,memory_order_acquire))return 0;
  return slcli_node_allocator_caller_crt_handler_before(current_caller,handler,bytes,ordinal);
}
int slcli_node_allocator_crt_handler_after_original(int (*handler)(size_t),size_t bytes,
  size_t ordinal,int result,int exception) {
  if(!current_caller)return 0;
  return slcli_node_allocator_caller_crt_handler_after(current_caller,handler,bytes,ordinal,result,exception);
}
int slcli_node_allocator_native_before_original(uint32_t operation,const void *address,
  size_t bytes,uint64_t argument0,uint64_t argument1,size_t *ordinal) {
  if(!current_caller||atomic_load_explicit(&unresolved,memory_order_acquire))return 0;
  return slcli_node_allocator_caller_native_before(current_caller,operation,address,bytes,argument0,argument1,ordinal);
}
int slcli_node_allocator_native_raw_return_original(uint32_t operation,const void *address,
  size_t bytes,size_t ordinal,uint64_t result) {
  return slcli_node_allocator_caller_native_raw_return(current_caller,operation,address,bytes,ordinal,result);
}
int slcli_node_allocator_native_after_original(uint32_t operation,const void *address,
  size_t bytes,size_t ordinal,uint64_t result) {
  if(!current_caller)return 0;
  return slcli_node_allocator_caller_native_after(current_caller,operation,address,bytes,ordinal,result);
}
int slcli_node_allocator_wait_before_original(const void *address,size_t bytes,
  uint64_t handle,uint32_t *milliseconds,size_t *ordinal) {
  if(!current_caller||atomic_load_explicit(&unresolved,memory_order_acquire))return 0;
  return slcli_node_allocator_caller_wait_before(current_caller,address,bytes,handle,milliseconds,ordinal);
}
static struct allocation *header(void *p) {
  const struct slcli_extent *e;uintptr_t base,limit,at,raw;struct allocation *h;
  size_t payload,total;const struct allocation *original;
  if(!live()||!p)return 0;e=&original_arena->owner->extents[original_arena->extent];
  base=e->base;at=(uintptr_t)p;
  if(e->reserved<sizeof(*h)||e->reserved>UINTPTR_MAX-base)return 0;limit=base+(uintptr_t)e->reserved;
  if(at<base+sizeof(*h)||at>=limit)return 0;h=(void *)(at-sizeof(*h));
  if(!slcli_arena_live_range(original_arena,h,sizeof(*h)))return 0;
  if((uintptr_t)h%_Alignof(struct allocation)||h->marker!=MARKER||!h->alignment||
      (h->alignment&(h->alignment-1))||at%h->alignment||h->bytes>limit-at||h->result!=p)return 0;
  raw=(uintptr_t)h->raw;if(raw<base||raw>(uintptr_t)h)return 0;
  payload=h->bytes?h->bytes:1;
  if(h->alignment>SIZE_MAX-2*sizeof(*h)||
     payload>SIZE_MAX-2*sizeof(*h)-(h->alignment-1))return 0;
  total=payload+2*sizeof(*h)+h->alignment-1;
  /* An interior inline header cannot replace the canonical record at the
   * exact original live block prefix, outside its returned payload. Check both
   * original geometry and returned address before allocation/copy/free effects. */
  if(raw>UINTPTR_MAX-2*sizeof(*h)-(h->alignment-1)||
     ((raw+2*sizeof(*h)+h->alignment-1)&~(uintptr_t)(h->alignment-1))!=at||
     !slcli_arena_live_block(original_arena,h->raw,total))return 0;
  original=h->raw;
  if(original==h||original->raw!=h->raw||original->result!=p||
     original->bytes!=h->bytes||original->alignment!=h->alignment||
     original->marker!=MARKER)return 0;
  return h;
}
int slcli_node_allocator_allocation_original(void *p,size_t bytes) {
  struct allocation *h;int ok;
  if(!p||!bytes||!take())return 0;
  record_request(10,p,bytes,0,1);h=header(p);ok=h&&h->bytes==bytes;
  if(!release())return 0;return ok;
}
int slcli_node_allocator_child_storage_original(const void *p,size_t bytes) {
 size_t i;int ok=0;
 if(!p||!bytes||!take())return 0;
 record_request(11,(void *)p,bytes,0,1);
 if(live()&&child_capture&&child_capture->ready&&
  child_capture->slots==child_capture_slots&&child_capture->count==child_capture_count&&
  child_capture->used==child_capture_used&&child_capture_used<=child_capture_count&&
  child_capture->birth_bytes==child_capture_bytes&&bytes==child_capture_bytes&&
  child_capture->event_capacity==child_capture_events) {
  for(i=0;i<child_capture_used;i++)if(child_capture_slots[i].storage==p&&
    child_capture_slots[i].storage_bytes==bytes&&
    slcli_arena_live_block(original_arena,p,bytes)){ok=1;break;}
 }
 if(!release())return 0;return ok;
}
static int retire(void *p) {
  struct allocation *h;void *raw;if(!p)return 1;h=header(p);if(!h){hold_failure();return 0;}
  raw=h->raw;{struct slcli_node_allocator_wait_row *r=operation_row();if(r)r->old_raw=(uintptr_t)raw;}
  h->marker=0;if(!slcli_arena_free(original_arena,raw)){hold_failure();return 0;}
  {struct slcli_node_allocator_wait_row *r=operation_row();if(r)r->old_retired=1;}return 1;
}
static void *publish(void *p){
  struct slcli_node_allocator_wait_row *r=operation_row();
  if(r){r->completed_result=(uintptr_t)p;r->result_ready=1;}
  if(atomic_load_explicit(&unresolved,memory_order_acquire)){hold_failure();p=0;}
  /* Any already allocated block stays registered and charged on failure. */
  if(!release())p=0;if(r)r->caller_result=(uintptr_t)p;return p;
}
size_t slcli_node_msize(void *p) {
  struct allocation *h;struct slcli_node_allocator_wait_row *r;size_t result=SIZE_MAX;
  if(!take())return SIZE_MAX;record_request(6,p,0,0,0);
  h=header(p);if(h)result=h->bytes;else hold_failure();
  r=operation_row();if(r){r->count=result;r->result_ready=1;}
  if(!release())return SIZE_MAX;return result;
}
void *slcli_node_expand(void *p,size_t bytes) {
  struct allocation *h,*original;struct slcli_node_allocator_wait_row *r;
  size_t payload=bytes?bytes:1,total,old_total,old_bytes,offset,within,i;
  if(!take())return 0;record_request(7,p,bytes,0,1);
  h=header(p);if(!h){hold_failure();return publish(0);}
  if(h->alignment>SIZE_MAX-2*sizeof(*h)||payload>SIZE_MAX-2*sizeof(*h)-(h->alignment-1))return publish(0);
  old_bytes=h->bytes;old_total=(old_bytes?old_bytes:1)+2*sizeof(*h)+h->alignment-1;
  total=payload+2*sizeof(*h)+h->alignment-1;original=h->raw;
  if(!slcli_arena_resize_in_place(original_arena,h->raw,old_total,total))return publish(0);
  offset=(uintptr_t)p-(uintptr_t)h->raw;
  /* Arena contraction erases its dropped raw tail before free headers.
   * Erase remaining dropped payload within the new live raw request only;
   * never overwrite the newly materialized neighboring arena header. */
  if(bytes<old_bytes) {
    within=total-offset;if(within>old_bytes)within=old_bytes;
    for(i=bytes;i<within;i++)((volatile unsigned char *)p)[i]=0;
  }
  original->bytes=bytes;h->bytes=bytes;
  r=operation_row();if(r){r->old_raw=(uintptr_t)h->raw;r->new_raw=(uintptr_t)h->raw;
    r->completed_result=(uintptr_t)p;r->caller_result=(uintptr_t)p;r->result_ready=1;}
  /* The completed resize must return its real SAME pointer even if release
   * diagnostics fail; NULL would falsely promise the old geometry survives. */
  if(atomic_load_explicit(&unresolved,memory_order_acquire))hold_failure();
  release();return p;
}
void *slcli_node_malloc(size_t bytes){void *p;if(!take())return 0;record_request(1,0,bytes,_Alignof(max_align_t),1);p=allocate(_Alignof(max_align_t),bytes);return publish(p);}
void *slcli_node_aligned_alloc(size_t alignment,size_t bytes){void *p;if(!take())return 0;record_request(2,0,bytes,alignment,1);p=allocate(alignment,bytes);return publish(p);}
void *slcli_node_calloc(size_t count,size_t stride) {
  unsigned char *p;size_t n,i;if(count&&stride>SIZE_MAX/count)return 0;n=count*stride;
  if(!take())return 0;record_request(3,0,stride,_Alignof(max_align_t),count);p=allocate(_Alignof(max_align_t),n);if(p)for(i=0;i<n;i++)p[i]=0;return publish(p);
}
void slcli_node_free(void *p){if(!take())return;record_request(4,p,0,0,0);retire(p);if(atomic_load_explicit(&unresolved,memory_order_acquire))hold_failure();publish(0);}
size_t slcli_node_allocator_operation_frontier_original(void) {
  if(!current_caller||!current_caller->prepared||current_caller->acquired||
    current_caller->used>current_caller->capacity)return SIZE_MAX;
  return current_caller->used;
}
int slcli_node_allocator_free_retired_original(const void *p,size_t frontier) {
  const struct slcli_node_allocator_wait_row *r;
  if(!p||!current_caller||!current_caller->prepared||current_caller->acquired||
    frontier==SIZE_MAX||current_caller->acquisition_row<frontier||
    current_caller->acquisition_row>=current_caller->used||
    current_caller->used>current_caller->capacity||
    current_caller->used-current_caller->acquisition_row<4)return 0;
  r=&current_caller->rows[current_caller->acquisition_row];
  return r->allocator_operation==4&&r->argument==(uintptr_t)p&&
    r->old_raw&&r->old_retired&&r->result_ready&&
    current_caller->rows[current_caller->used-2].marker_released;
}
static void *reallocate(void *p,size_t bytes,int zero,size_t count,size_t stride) {
  struct allocation *h;unsigned char *next;size_t n,i;int retired=0;if(!take())return 0;
  record_request(zero?8:5,p,zero?stride:bytes,0,count);
  if(!p){next=allocate(_Alignof(max_align_t),bytes);if(next&&zero)for(i=0;i<bytes;i++)next[i]=0;goto done;}
  if(!bytes){retire(p);next=0;goto done;}
  h=header(p);if(!h){hold_failure();next=0;goto done;}
  n=h->bytes<bytes?h->bytes:bytes;next=allocate(h->alignment,bytes);if(!next)goto done;
  /* Old and new logical blocks coexist inside their fully charged extent until
   * actual copy finishes. Logical free never retires native extent charge. */
  for(i=0;i<n;i++)next[i]=((const unsigned char *)p)[i];
  if(zero)for(i=n;i<bytes;i++)next[i]=0;
  if(!retire(p)){next=0;goto done;}
  retired=1;
done:
  /* Once old storage was actually retired, returning NULL would falsely claim
   * the standard realloc failure guarantee that the old allocation survives.
   * Preserve the completed result and close all subsequent operations instead.
   * Contention observed before retire is rejected by header/live and retains
   * both old and newly registered storage. Native extent charge never retires. */
  if(retired){struct slcli_node_allocator_wait_row *r=operation_row();if(r){r->completed_result=(uintptr_t)next;r->caller_result=(uintptr_t)next;r->result_ready=1;}if(atomic_load_explicit(&unresolved,memory_order_acquire))hold_failure();release();return next;}
  return publish(next);
}
void *slcli_node_realloc(void *p,size_t bytes){return reallocate(p,bytes,0,1,bytes);}
void *slcli_node_recalloc(void *p,size_t count,size_t stride) {
  if(count&&stride>SIZE_MAX/count)return 0;
  return reallocate(p,count*stride,1,count,stride);
}
