#include "capture_layout_windows.h"
#if defined(_WIN32)
#include "early_allocator.h"
#include "thread_windows.h"
#include "route_windows.h"
#include "entry_windows.h"
#include "../host-service/capture_alias_windows.h"
#include "../host-service/arena_layout.h"
#include <stdatomic.h>
static int away(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int same(struct slcli_node_capture_layout_windows *c,struct slcli_arena *a) {
 return slcli_source_deadline_original_matches(a,c->deadline,c->caller)&&
  c->deadline->origin==c->origin&&c->origin->tick==c->origin_tick&&
  c->deadline->expires==c->expires&&c->predecessor->complete&&
  c->predecessor->tick==c->predecessor_tick;
}
static int block(struct slcli_node_capture_layout_windows *c,unsigned stage,
 size_t count,size_t stride,size_t *next) {
 size_t bytes,charge;
 if(stage>=SLCLI_CAPTURE_STAGE_COUNT||!count||!stride||count>SIZE_MAX/stride)return 0;
 bytes=count*stride;charge=slcli_arena_allocation_bytes(bytes);
 if(!charge||*next>SIZE_MAX-charge||*next>SIZE_MAX-32)return 0;
 c->stages[stage].offset=*next+32;c->stages[stage].requested=bytes;
 c->stages[stage].charge=charge;*next+=charge;return 1;
}
static int derive_original(struct slcli_node_capture_layout_windows *c) {
 size_t next=0,birth_charge,caller_charge,event_charge,total,padding,page,remainder;
 size_t threads=c->threads,events=c->events,regions=c->regions,modules=c->modules,name_units=c->name_units;
 if(!threads||events<5||!regions||!modules||!name_units||modules>SIZE_MAX/name_units||
  !c->geometry.dwPageSize||c->geometry.dwPageSize%16||!c->geometry.dwAllocationGranularity||
  c->geometry.dwAllocationGranularity%c->geometry.dwPageSize)return 0;
#define ONE(stage,type) if(!block(c,stage,1,sizeof(type),&next))return 0
 ONE(SLCLI_CAPTURE_ENTRY,struct slcli_node_entry_windows_capture);
 ONE(SLCLI_CAPTURE_RECEIVE,struct slcli_node_receive_windows);
 ONE(SLCLI_CAPTURE_PREFIX,struct slcli_node_prefix_windows);
 if(!block(c,SLCLI_CAPTURE_REGIONS,regions,sizeof(struct slcli_node_prefix_windows_region),&next))return 0;
 ONE(SLCLI_CAPTURE_LOADER,struct slcli_node_loader_windows);
 if(!block(c,SLCLI_CAPTURE_MODULES,modules,sizeof(struct slcli_node_loader_windows_module),&next)||
  !block(c,SLCLI_CAPTURE_NAMES,modules*name_units,sizeof(WCHAR),&next))return 0;
 ONE(SLCLI_CAPTURE_BOOTSTRAP,struct slcli_node_bootstrap);
 ONE(SLCLI_CAPTURE_BOOTSTRAP_CALLER,struct slcli_node_bootstrap_caller);
 ONE(SLCLI_CAPTURE_BOOTSTRAP_ORIGIN,struct slcli_native_clock);
 ONE(SLCLI_CAPTURE_BOOTSTRAP_PREDECESSOR,struct slcli_native_clock);
 ONE(SLCLI_CAPTURE_BOOTSTRAP_DEADLINE,struct slcli_source_deadline_original);
 ONE(SLCLI_CAPTURE_RUNTIME_REQUEST,struct slcli_extent_request);
 ONE(SLCLI_CAPTURE_CREATION,FILETIME);
 ONE(SLCLI_CAPTURE_PREENTRY,struct slcli_node_preentry_windows);
 ONE(SLCLI_CAPTURE_RUNTIME_RECEIVER,struct slcli_preentry_backing_windows);
 ONE(SLCLI_CAPTURE_MAIN_CALLER,struct slcli_node_preentry_windows_caller);
 ONE(SLCLI_CAPTURE_MAIN_ORIGIN,struct slcli_native_clock);
 ONE(SLCLI_CAPTURE_MAIN_DEADLINE,struct slcli_source_deadline_original);
 ONE(SLCLI_CAPTURE_MAIN_ALLOCATOR,struct slcli_node_allocator_caller);
 if(!block(c,SLCLI_CAPTURE_MAIN_EVENTS,events,sizeof(struct slcli_node_allocator_wait_row),&next))return 0;
 ONE(SLCLI_CAPTURE_THREAD_SCHEDULE,struct slcli_node_child_capture_schedule);
 if(!block(c,SLCLI_CAPTURE_THREAD_SLOTS,threads,sizeof(struct slcli_node_child_reservation),&next))return 0;
#undef ONE
 c->birth_bytes=slcli_node_thread_birth_bytes_original();
 birth_charge=slcli_arena_allocation_bytes(c->birth_bytes);
 caller_charge=slcli_arena_allocation_bytes(sizeof(struct slcli_node_allocator_caller));
 if(events>SIZE_MAX/sizeof(struct slcli_node_allocator_wait_row))return 0;
 event_charge=slcli_arena_allocation_bytes(events*sizeof(struct slcli_node_allocator_wait_row));
 if(!birth_charge||!caller_charge||!event_charge||birth_charge>SIZE_MAX-caller_charge||
  birth_charge+caller_charge>SIZE_MAX-event_charge)return 0;
 c->thread_begin=next;c->thread_caller_delta=birth_charge;c->thread_events_delta=birth_charge+caller_charge;
 c->thread_stride=birth_charge+caller_charge+event_charge;
 if(threads>SIZE_MAX/c->thread_stride||next>SIZE_MAX-threads*c->thread_stride)return 0;
 next+=threads*c->thread_stride;c->typed_bytes=next;
 /* C=round_page(S+48) leaves a genuine split tail after the last typed block.
  * The final padding is itself a retained live allocation, never free payload. */
 page=c->geometry.dwPageSize;if(next>SIZE_MAX-48)return 0;
 total=next+48;remainder=total%page;
 if(remainder){if(total>SIZE_MAX-(page-remainder))return 0;total+=page-remainder;}
 padding=total-next-32;
 if(!padding||slcli_arena_allocation_bytes(padding)!=total-next||total>SIZE_MAX-48)return 0;
 c->padding_offset=next+32;c->padding_requested=padding;c->capture_bytes=total;
 c->minimum_runtime_bytes=total+48;c->ready=1;return 1;
}
int slcli_node_capture_layout_windows_route_shape_original(
 const struct slcli_node_route_windows *r,struct slcli_node_capture_layout_windows *c) {
 size_t i;
 if(!r||!c||!away(r,sizeof(*r),c,sizeof(*c))||!r->base||!r->bytes||
  r->bytes>SIZE_MAX||r->capture_bytes>SIZE_MAX||r->threads>SIZE_MAX||r->events>SIZE_MAX||
  r->regions>SIZE_MAX||r->modules>SIZE_MAX||r->name_units>SIZE_MAX||
  !r->page_bytes||r->page_bytes>UINT32_MAX||!r->granularity||r->granularity>UINT32_MAX||
  (uintptr_t)r->base>UINTPTR_MAX-(uintptr_t)r->bytes||
  (uintptr_t)r->base%r->granularity||r->bytes%r->page_bytes)return 0;
 for(i=0;i<sizeof(*c);i++)((unsigned char *)c)[i]=0;
 c->threads=(size_t)r->threads;c->events=(size_t)r->events;c->regions=(size_t)r->regions;
 c->modules=(size_t)r->modules;c->name_units=(size_t)r->name_units;
 c->geometry.dwPageSize=(DWORD)r->page_bytes;c->geometry.dwAllocationGranularity=(DWORD)r->granularity;
 return derive_original(c)&&c->capture_bytes==r->capture_bytes&&c->minimum_runtime_bytes<=r->bytes;
}
int slcli_node_capture_layout_windows_prepare_original(
 struct slcli_node_capture_layout_windows *c,struct slcli_arena *a,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,size_t threads,size_t events,size_t regions,
 size_t modules,size_t name_units) {
 size_t i;
 if(!a||!c||!slcli_arena_live_block(a,c,sizeof(*c))||
  !slcli_source_deadline_original_matches(a,d,caller)||!caller_bytes||
  !slcli_arena_live_block(a,caller,caller_bytes)||!prior||
  !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  prior->tick<d->origin->tick||prior->tick>=d->expires||
  !away(c,sizeof(*c),caller,caller_bytes)||!away(c,sizeof(*c),prior,sizeof(*prior))||
  !slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
 for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
 c->started=1;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
 c->predecessor=prior;c->predecessor_tick=prior->tick;
 c->threads=threads;c->events=events;c->regions=regions;c->modules=modules;c->name_units=name_units;
 if(!threads||events<5||!regions||!modules||!name_units||modules>SIZE_MAX/name_units)return 0;
 if(!slcli_native_clock_read(&c->before)||!same(c,a)||
  c->before.tick<c->predecessor_tick||c->before.tick>=c->expires)return 0;
 c->geometry_args[0]=(uintptr_t)&c->geometry;
 c->geometry_called=1;GetSystemInfo(&c->geometry);c->geometry_returned=1;
 if(!slcli_native_clock_read(&c->after)||!same(c,a)||c->after.tick<c->before.tick||
  c->after.tick>=c->expires||!c->geometry.dwPageSize||!c->geometry.dwAllocationGranularity||
  c->geometry.dwAllocationGranularity%c->geometry.dwPageSize||c->geometry.dwPageSize%16)return 0;
 return derive_original(c);
}
static int placement_matches(const struct slcli_node_capture_layout_windows *c) {
 struct slcli_node_capture_layout_windows expected={0};size_t i;
 expected.threads=c->threads;expected.events=c->events;expected.regions=c->regions;
 expected.modules=c->modules;expected.name_units=c->name_units;expected.geometry=c->geometry;
 if(!derive_original(&expected)||!c->ready||c->birth_bytes!=expected.birth_bytes||
  c->thread_begin!=expected.thread_begin||c->thread_stride!=expected.thread_stride||
  c->thread_caller_delta!=expected.thread_caller_delta||c->thread_events_delta!=expected.thread_events_delta||
  c->typed_bytes!=expected.typed_bytes||c->padding_offset!=expected.padding_offset||
  c->padding_requested!=expected.padding_requested||c->capture_bytes!=expected.capture_bytes||
  c->minimum_runtime_bytes!=expected.minimum_runtime_bytes)return 0;
 for(i=0;i<SLCLI_CAPTURE_STAGE_COUNT;i++)if(c->stages[i].offset!=expected.stages[i].offset||
  c->stages[i].requested!=expected.stages[i].requested||c->stages[i].charge!=expected.stages[i].charge)return 0;
 return 1;
}
static int initialize_header(struct slcli_capture_alias_windows *v,size_t payload,
 size_t requested,size_t charge) {
 struct slcli_arena_block_header *h;size_t offset;
 if(payload<32||!requested||charge!=slcli_arena_allocation_bytes(requested))return 0;
 offset=payload-32;
 if(offset%16||offset>v->capture_bytes||charge>v->capture_bytes-offset||charge<48)return 0;
 h=(struct slcli_arena_block_header *)((unsigned char *)v->view+offset);
 h->bytes=charge;h->requested=requested;h->next=offset+charge;h->state=SLCLI_ARENA_BLOCK_LIVE;
 v->headers_written++;return 1;
}
int slcli_node_capture_layout_windows_initialize_original(
 const struct slcli_node_capture_layout_windows *c,struct slcli_arena *a,
 struct slcli_capture_alias_windows *v) {
 size_t i,at,birth_charge,caller_charge,event_charge;
 if(!a||!c||!slcli_arena_live_block(a,c,sizeof(*c))||!v||
  !slcli_arena_live_block(a,v,sizeof(*v))||!away(c,sizeof(*c),v,sizeof(*v))||
  !c->ready||!v->qualified||!v->view||v->initialize_started||
  v->native_class_original!=SLCLI_NATIVE_ORIGINAL_CAPTURE_INITIALIZER_VIEW||
  v->access_original!=(FILE_MAP_READ|FILE_MAP_WRITE)||v->capture_bytes!=c->capture_bytes||
  v->deadline!=c->deadline||v->caller!=c->caller||v->expires!=c->expires||
  !placement_matches(c)||!same((struct slcli_node_capture_layout_windows *)c,a)||
  v->backing_bytes_original<c->minimum_runtime_bytes||
  !v->map_returned||!v->query_returned||v->query_status!=sizeof(v->geometry)||
  v->geometry.BaseAddress!=v->view||v->geometry.AllocationBase!=v->view||
  v->geometry.RegionSize!=c->capture_bytes||v->geometry.Protect!=PAGE_READWRITE||
  v->geometry.State!=MEM_COMMIT||v->geometry.Type!=MEM_MAPPED)return 0;
 v->initialize_started=1;
 if(!slcli_native_clock_read(&v->initialize_before)||!same((struct slcli_node_capture_layout_windows *)c,a)||
  v->initialize_before.tick<v->query_after.tick||v->initialize_before.tick>=c->expires)goto finished;
 for(i=0;i<v->capture_bytes;i++) {
  unsigned char raw=((const unsigned char *)v->view)[i];v->zero_scanned=i+1;
  if(raw){v->zero_nonzero_offset=i;v->zero_nonzero_value=raw;goto finished;}
 }
 for(i=0;i<SLCLI_CAPTURE_STAGE_COUNT;i++)if(!initialize_header(v,c->stages[i].offset,
  c->stages[i].requested,c->stages[i].charge,1))goto finished;
 birth_charge=slcli_arena_allocation_bytes(c->birth_bytes);
 caller_charge=slcli_arena_allocation_bytes(sizeof(struct slcli_node_allocator_caller));
 event_charge=slcli_arena_allocation_bytes(c->events*sizeof(struct slcli_node_allocator_wait_row));
 for(i=0;i<c->threads;i++) {
  at=c->thread_begin+i*c->thread_stride;
  if(!initialize_header(v,at+32,c->birth_bytes,birth_charge)||
   !initialize_header(v,at+c->thread_caller_delta+32,sizeof(struct slcli_node_allocator_caller),caller_charge)||
   !initialize_header(v,at+c->thread_events_delta+32,c->events*sizeof(struct slcli_node_allocator_wait_row),event_charge))goto finished;
 }
 if(!initialize_header(v,c->padding_offset,c->padding_requested,
  slcli_arena_allocation_bytes(c->padding_requested)))goto finished;
 /* No child exists in the containing caller at this phase. Its tail header at
  * C is outside this prefix mapping and will be written only after exact child
  * stage validation and source-qualified owner promotion, never trusted here. */
 atomic_thread_fence(memory_order_release);
 v->initialized=1;
finished:
 v->initialize_returned=1;
 if(!slcli_native_clock_read(&v->initialize_after)||!same((struct slcli_node_capture_layout_windows *)c,a)||
  v->initialize_after.tick<v->initialize_before.tick||v->initialize_after.tick>=c->expires)return 0;
 return v->initialized!=0;
}
static int validate_block(struct slcli_capture_alias_windows *v,size_t payload,
 size_t requested,size_t charge,int zero_payload) {
 struct slcli_arena_block_header expected;size_t offset,i;
 if(payload<32||!requested||charge!=slcli_arena_allocation_bytes(requested))return 0;
 offset=payload-32;
 if(offset!=v->validate_scanned||offset%16||offset>v->capture_bytes||
  charge>v->capture_bytes-offset||charge<48)return 0;
 expected.bytes=charge;expected.requested=requested;expected.next=offset+charge;
 expected.state=SLCLI_ARENA_BLOCK_LIVE;
 for(i=0;i<(zero_payload?charge:sizeof(expected));i++) {
  unsigned char wanted=i<sizeof(expected)?((const unsigned char *)&expected)[i]:0;
  unsigned char raw=((const unsigned char *)v->view)[offset+i];
  v->validate_scanned=offset+i+1;
  if(raw!=wanted){v->validate_mismatch_offset=offset+i;v->validate_actual=raw;
   v->validate_expected=wanted;return 0;}
 }
 /* In header-only mode this is a logical next-block frontier, not a claim
  * that the already-used payload was read, zero or immutable. */
 if(!zero_payload)v->validate_scanned=offset+charge;
 return 1;
}
static int route_headers(const struct slcli_node_route_windows *r,
 struct slcli_node_capture_layout_windows *c,int zero_payload) {
 struct slcli_capture_alias_windows v;size_t i,at,birth_charge,caller_charge,event_charge;
 if(!slcli_node_capture_layout_windows_route_shape_original(r,c))return 0;
 for(i=0;i<sizeof(v);i++)((unsigned char *)&v)[i]=0;
 v.view=r->base;v.capture_bytes=c->capture_bytes;
 for(i=0;i<SLCLI_CAPTURE_STAGE_COUNT;i++)if(!validate_block(&v,c->stages[i].offset,
  c->stages[i].requested,c->stages[i].charge,zero_payload))return 0;
 birth_charge=slcli_arena_allocation_bytes(c->birth_bytes);
 caller_charge=slcli_arena_allocation_bytes(sizeof(struct slcli_node_allocator_caller));
 event_charge=slcli_arena_allocation_bytes(c->events*sizeof(struct slcli_node_allocator_wait_row));
 for(i=0;i<c->threads;i++) {
  at=c->thread_begin+i*c->thread_stride;
  if(!validate_block(&v,at+32,c->birth_bytes,birth_charge,zero_payload)||
   !validate_block(&v,at+c->thread_caller_delta+32,sizeof(struct slcli_node_allocator_caller),caller_charge,zero_payload)||
   !validate_block(&v,at+c->thread_events_delta+32,c->events*sizeof(struct slcli_node_allocator_wait_row),event_charge,zero_payload))return 0;
 }
 return validate_block(&v,c->padding_offset,c->padding_requested,
  slcli_arena_allocation_bytes(c->padding_requested),zero_payload)&&v.validate_scanned==c->capture_bytes;
}
int slcli_node_capture_layout_windows_route_headers_original(
 const struct slcli_node_route_windows *r,struct slcli_node_capture_layout_windows *c){return route_headers(r,c,1);}
int slcli_node_capture_layout_windows_live_headers_original(
 const struct slcli_node_route_windows *r,struct slcli_node_capture_layout_windows *c){return route_headers(r,c,0);}
int slcli_node_capture_layout_windows_validate_original(
 const struct slcli_node_capture_layout_windows *c,struct slcli_arena *a,
 struct slcli_capture_alias_windows *v) {
 size_t i,at,birth_charge,caller_charge,event_charge;
 if(!a||!c||!slcli_arena_live_block(a,c,sizeof(*c))||!v||
  !slcli_arena_live_block(a,v,sizeof(*v))||!away(c,sizeof(*c),v,sizeof(*v))||
  !c->ready||!v->qualified||!v->view||v->validate_started||v->unmap_attempted||
  v->native_class_original!=SLCLI_NATIVE_ORIGINAL_CAPTURE_ALIAS||v->access_original!=FILE_MAP_READ||
  v->capture_bytes!=c->capture_bytes||v->deadline!=c->deadline||v->caller!=c->caller||
  v->expires!=c->expires||!placement_matches(c)||!same((struct slcli_node_capture_layout_windows *)c,a)||
  !v->map_returned||!v->query_returned||v->query_status!=sizeof(v->geometry)||
  v->geometry.BaseAddress!=v->view||v->geometry.AllocationBase!=v->view||
  v->geometry.RegionSize!=c->capture_bytes||v->geometry.Protect!=PAGE_READONLY||
  v->geometry.State!=MEM_COMMIT||v->geometry.Type!=MEM_MAPPED)return 0;
 v->validate_started=1;
 if(!slcli_native_clock_read(&v->validate_before)||!same((struct slcli_node_capture_layout_windows *)c,a)||
  v->validate_before.tick<v->query_after.tick||v->validate_before.tick>=c->expires)goto finished;
 atomic_thread_fence(memory_order_acquire);
 for(i=0;i<SLCLI_CAPTURE_STAGE_COUNT;i++)if(!validate_block(v,c->stages[i].offset,
  c->stages[i].requested,c->stages[i].charge,1))goto finished;
 birth_charge=slcli_arena_allocation_bytes(c->birth_bytes);
 caller_charge=slcli_arena_allocation_bytes(sizeof(struct slcli_node_allocator_caller));
 event_charge=slcli_arena_allocation_bytes(c->events*sizeof(struct slcli_node_allocator_wait_row));
 for(i=0;i<c->threads;i++) {
  at=c->thread_begin+i*c->thread_stride;
  if(!validate_block(v,at+32,c->birth_bytes,birth_charge,1)||
   !validate_block(v,at+c->thread_caller_delta+32,sizeof(struct slcli_node_allocator_caller),caller_charge,1)||
   !validate_block(v,at+c->thread_events_delta+32,c->events*sizeof(struct slcli_node_allocator_wait_row),event_charge,1))goto finished;
 }
 if(!validate_block(v,c->padding_offset,c->padding_requested,
  slcli_arena_allocation_bytes(c->padding_requested),1)||v->validate_scanned!=c->capture_bytes)goto finished;
 v->validated=1;
finished:
 v->validate_returned=1;
 if(!slcli_native_clock_read(&v->validate_after)||!same((struct slcli_node_capture_layout_windows *)c,a)||
  v->validate_after.tick<v->validate_before.tick||v->validate_after.tick>=c->expires)return 0;
 return v->validated!=0;
}
#endif
