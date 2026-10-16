#include "bq_serialized_factory_windows.h"
#include "arena_layout.h"
#include "evidence_deadline.h"
#if defined(_WIN32)
_Static_assert(_Alignof(struct slcli_bq_serialized_factory_windows)<=16,"BQ caller arena alignment");
_Static_assert(_Alignof(struct slcli_bq_factory_block_windows)<=16,"BQ block roster alignment");
static int zero_original(const void *p,size_t n){
 const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;
}
static int apart(const void *p,size_t n,const void *q,size_t m){
 uintptr_t x=(uintptr_t)p,y=(uintptr_t)q;
 return p&&q&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static struct slcli_arena_block_header *header(struct slcli_extent *e,uint64_t at){
 struct slcli_arena_block_header *h;
 if(e->reserved<sizeof(*h)||e->reserved>UINTPTR_MAX-e->base||
  at%16||at>e->reserved-sizeof(*h))return 0;
 h=(void *)(e->base+(uintptr_t)at);
 if(h->requested>SIZE_MAX||h->bytes<48||h->bytes%16||h->bytes>e->reserved-at||
  (h->state!=SLCLI_ARENA_BLOCK_FREE&&h->state!=SLCLI_ARENA_BLOCK_LIVE)||
  (h->state==SLCLI_ARENA_BLOCK_FREE&&h->requested)||
  (h->state==SLCLI_ARENA_BLOCK_LIVE&&(!h->requested||h->requested>h->bytes-sizeof(*h)))||
  (h->next?h->next!=at+h->bytes:at+h->bytes!=e->reserved))return 0;
 return h;
}
/* Only the unpublished bootstrap and synchronous fixed constructor call this
 * splitter. There is no exported cursor/allocator or caller-supplied lock bit. */
static void *split_tail(struct slcli_extent *e,uint64_t *at,size_t n){
 struct slcli_arena_block_header *h,*tail;size_t need=slcli_arena_allocation_bytes(n);
 void *p;size_t i;
 if(!need||*at==UINT64_MAX||!(h=header(e,*at))||h->state!=SLCLI_ARENA_BLOCK_FREE||
  h->next||need>h->bytes||h->bytes-need<48)return 0;
 tail=(void *)(e->base+(uintptr_t)(*at+need));
 tail->bytes=h->bytes-need;tail->requested=0;tail->next=0;tail->state=SLCLI_ARENA_BLOCK_FREE;
 h->bytes=need;h->requested=n;h->next=*at+need;h->state=SLCLI_ARENA_BLOCK_LIVE;
 p=(void *)(e->base+(uintptr_t)*at+sizeof(*h));*at=h->next;
 for(i=0;i<n;i++)((unsigned char *)p)[i]=0;
 return p;
}
static void *take(struct slcli_bq_serialized_factory_windows *c,size_t n){
 struct slcli_extent *e;uint64_t at=c->tail_offset;void *p;
 if(c->failed||!c->acquired||!c->try_returned||!c->try_raw||!c->issuing_thread||
  !c->init_returned||c->block_count>=c->block_capacity||
  c->owner_original->bq_owner_original!=c->owner_original||
  c->owner_original->bq_arena_original!=c->arena||
  c->owner_original->bq_caller_original!=c||c->owner_original->bq_attachment_state!=1||
  c->arena->owner!=c->owner_original||c->arena->extent!=c->extent_original||
  c->arena->acquisition!=c->acquisition_original||
  c->owner_original->bq_extent_original!=c->extent_original||
  c->owner_original->bq_acquisition_original!=c->acquisition_original)return 0;
 e=&c->owner_original->extents[c->arena->extent];
 if(e->acquisition!=c->arena->acquisition||e->state!=SLCLI_EXTENT_LIVE||
  e->base!=c->extent_base_original||e->reserved!=c->extent_bytes_original)return 0;
 p=split_tail(e,&c->tail_offset,n);if(!p)return 0;
 c->blocks[c->block_count].value=p;c->blocks[c->block_count].requested=n;
 c->blocks[c->block_count].header_offset=at;c->block_count++;return p;
}
static int add(size_t *total,size_t requested){
 size_t n=slcli_arena_allocation_bytes(requested);
 if(!n||n>SIZE_MAX-*total)return 0;*total+=n;return 1;
}
static int row_current(const struct slcli_bq_serialized_factory_windows *c,
 const struct slcli_bq_factory_block_windows *r){
 uintptr_t first=(uintptr_t)c->blocks,address=(uintptr_t)r;size_t bytes;
 struct slcli_extent *e;struct slcli_arena_block_header *h;
 if(!r||c->block_count!=c->block_capacity||
  c->block_capacity>SIZE_MAX/sizeof(*r))return 0;
 bytes=c->block_capacity*sizeof(*r);
 if(first>UINTPTR_MAX-bytes||address<first||address>=first+bytes||
  (address-first)%sizeof(*r))return 0;
 e=&c->owner_original->extents[c->extent_original];h=header(e,r->header_offset);
 return h&&h->state==SLCLI_ARENA_BLOCK_LIVE&&r->state==h->state&&
  r->bytes==h->bytes&&r->next==h->next&&r->requested==h->requested&&
  r->value==(void *)(e->base+(uintptr_t)r->header_offset+sizeof(*h));
}
static int row_original(const struct slcli_bq_serialized_factory_windows *c,
 const struct slcli_bq_factory_block_windows *r,const void *p,size_t n){
 return n&&row_current(c,r)&&r->requested==n&&r->value==p;
}
static int subject_current(const struct slcli_bq_serialized_factory_windows *c,
 const struct slcli_bq_factory_subject_windows *s){
 const struct slcli_bq_factory_block_windows *r=s->block;
 return s->bytes&&row_current(c,r)&&s->offset<=r->requested&&
  s->bytes<=r->requested-s->offset&&
  (uintptr_t)s->value==(uintptr_t)r->value+s->offset;
}
int slcli_bq_serialized_factory_windows_body_storage_matches(
 const struct slcli_input_deadline_owner *o){
 const struct slcli_bq_serialized_factory_windows *c;
 const struct slcli_memory_owner *owner;const struct slcli_extent *e;
 size_t i,n,alignment=_Alignof(struct slcli_native_clock);uint64_t previous;
 if(!o||!(c=o->source_fixed_caller)||!c->arena||!(owner=c->owner_original)||
  !slcli_memory_owner_writable(owner)||owner->failed||owner->count>256||c->extent_original>=256||
  c->failed||!c->storage_prepared||!c->source_borrow||!c->acquired||
  !c->init_called||!c->init_returned||!c->thread_called||!c->thread_returned||
  !c->issuing_thread||!c->try_called||!c->try_returned||!c->try_raw||
  c->init_operand!=(uintptr_t)&c->lock||c->try_operand!=(uintptr_t)&c->lock||
  owner->bq_owner_original!=owner||owner->bq_arena_original!=c->arena||
  owner->bq_caller_original!=c||owner->bq_attachment_state!=2||
  c->arena->owner!=owner||c->arena->extent!=c->extent_original||
  c->arena->acquisition!=c->acquisition_original||c->extent_original>=owner->count||
  owner->bq_extent_original!=c->extent_original||
  owner->bq_acquisition_original!=c->acquisition_original)return 0;
 e=&owner->extents[c->extent_original];
 if(e->state!=SLCLI_EXTENT_LIVE||!e->arena_initialized||e->base!=c->extent_base_original||
  e->reserved!=c->extent_bytes_original||e->acquisition!=c->acquisition_original||
  !row_original(c,c->caller_block,c,sizeof(*c))||
  c->block_capacity>SIZE_MAX/sizeof(*c->blocks)||
  !row_original(c,c->roster_block,c->blocks,c->block_capacity*sizeof(*c->blocks))||
  o->arena!=c->arena||!o->body||o->body->deadline_owner!=o||
  !row_original(c,o->source_body_block,o->body,o->body_storage_bytes))return 0;
 for(i=0;i<7;i++)if(!subject_current(c,&c->subjects[i]))return 0;
 if(c->subjects[0].value!=c->installer_original||c->subjects[0].bytes!=c->installer_bytes||
  c->subjects[1].value!=c->deadline||c->subjects[1].bytes!=sizeof(*c->deadline)||
  c->subjects[2].value!=c->deadline->origin||c->subjects[2].bytes!=sizeof(*c->deadline->origin)||
  c->subjects[3].value!=c->prior_original||c->subjects[3].bytes!=sizeof(*c->prior_original)||
  c->subjects[4].value!=c->association_template||c->subjects[4].bytes!=sizeof(*c->association_template)||
  c->subjects[5].value!=c->unit_template||c->subjects[5].bytes!=sizeof(*c->unit_template)||
  c->subjects[6].value!=c->bootstrap_original||c->subjects[6].bytes!=sizeof(*c->bootstrap_original))return 0;
 if(!slcli_native_clock_windows_matches_original(c->deadline->origin)||
  !slcli_native_clock_windows_matches_original(c->prior_original)||
  c->prior_original->tick<c->deadline->origin->tick||
  c->prior_original->tick>=c->deadline->expires)return 0;
 previous=c->prior_original->tick;
 for(i=0;i<7;i++){
  const struct slcli_native_clock *clock=&c->clocks[i/2][i%2];
  if(c->clock_returned[i/2][i%2]!=1||c->clock_result[i/2][i%2]!=1||
   !slcli_native_clock_windows_matches_original(clock)||clock->tick<previous||
   clock->tick>=c->deadline->expires)return 0;
  previous=clock->tick;
 }
 n=sizeof(*o);if(n>SIZE_MAX-(alignment-1))return 0;n=(n+alignment-1)/alignment*alignment;
 if(o->row_capacity>(SIZE_MAX-n)/sizeof(*o->before))return 0;
 return o->before==(void *)((unsigned char *)o+n)&&
  row_original(c,o->source_deadline_block,o,n+o->row_capacity*sizeof(*o->before));
}
static int body_shape(size_t bytes,size_t maximum,size_t *rows,size_t *storage,size_t *context){
 size_t n,align,chunks,capacity;
 if(!bytes||bytes>16384||!maximum)return 0;
 chunks=bytes/maximum+(bytes%maximum!=0);
 if(chunks>(SIZE_MAX/2)-3)return 0;*rows=2*(chunks+3);capacity=bytes+1;
 if(capacity>(SIZE_MAX-sizeof(struct slcli_input_body))/2)return 0;
 n=sizeof(struct slcli_input_body)+2*capacity;align=_Alignof(slcli_input_capture);
 if(n>SIZE_MAX-(align-1))return 0;n=(n+align-1)/align*align;
 if(*rows>(SIZE_MAX-n)/sizeof(slcli_input_capture))return 0;
 *storage=n+*rows*sizeof(slcli_input_capture);
 n=sizeof(struct slcli_input_deadline_owner);align=_Alignof(struct slcli_native_clock);
 if(n>SIZE_MAX-(align-1))return 0;n=(n+align-1)/align*align;
 if(*rows>(SIZE_MAX-n)/sizeof(struct slcli_native_clock))return 0;
 *context=n+*rows*sizeof(struct slcli_native_clock);return 1;
}
static size_t range_bytes(uint64_t records,uint64_t names,size_t i,int association){
 uint64_t at,left;
 if(!association)return i?128:64;
 if(!i)return 128;if((uint64_t)i<=records)return 320;
 at=((uint64_t)i-records-1)*16384;left=names-at;
 return left>16384?16384:(size_t)left;
}
static int maximum(struct slcli_arena *a,const struct slcli_writer_capture_factory_windows *f,size_t *out){
 const struct slcli_evidence_deadline_owner *o;uint64_t limit;
 if(!slcli_bq_source_control_sink_windows_view_original(a,f))return 0;
 o=f->ranges->extent->readback->creator->evidence_file->deadline_owner;
 limit=o->record_maximum;if(limit>16384)limit=16384;
 if(!o->bq_profile||limit<=48+sizeof(slcli_input_capture))return 0;
 *out=(size_t)(limit-48-sizeof(slcli_input_capture));return 1;
}
static int clock_original(struct slcli_bq_serialized_factory_windows *c,unsigned stage,unsigned after){
 uint64_t previous;
 c->clock_result[stage][after]=slcli_native_clock_read(&c->clocks[stage][after]);
 c->clock_returned[stage][after]=1;
 previous=after?c->clocks[stage][0].tick:stage?c->clocks[stage-1][1].tick:c->prior_original->tick;
 return c->clock_result[stage][after]&&
  slcli_native_clock_windows_matches_original(&c->clocks[stage][after])&&
  c->clocks[stage][after].tick>=previous&&
  c->clocks[stage][after].tick<c->deadline->expires;
}
static struct slcli_input_body *body_original(struct slcli_bq_serialized_factory_windows *c,
 size_t bytes,size_t maximum,struct slcli_bq_source_control_sink_windows *sink){
 size_t rows,storage,context,n,align,body_row;struct slcli_input_body *b;
 struct slcli_input_deadline_owner *o;
 if(!body_shape(bytes,maximum,&rows,&storage,&context))return 0;
 body_row=c->block_count;b=take(c,storage);if(!b)return 0;
 o=take(c,context);if(!o)return 0;
 o->source_fixed_caller=c;o->source_body_block=&c->blocks[body_row];
 o->source_deadline_block=&c->blocks[body_row+1];
 b->capacity=bytes+1;b->readback_capacity=b->capacity;b->body=(unsigned char *)b+sizeof(*b);
 b->readback=b->body+b->capacity;n=sizeof(*b)+2*b->capacity;align=_Alignof(slcli_input_capture);
 n=(n+align-1)/align*align;b->captures=(void *)((unsigned char *)b+n);b->capture_capacity=rows;
 o->arena=c->arena;o->body=b;o->body_storage_bytes=storage;o->deadline=c->deadline;
 o->origin=c->deadline->origin;o->caller_original=c->installer_original;o->caller_bytes=c->installer_bytes;
 o->sink=slcli_bq_source_control_sink_windows_write;o->capture_original=sink;o->capture_original_bytes=sizeof(*sink);
 o->predecessor=&c->clocks[2][1];o->predecessor_tick=o->predecessor->tick;
 o->expires=c->deadline->expires;o->origin_tick=o->origin->tick;o->previous_tick=o->predecessor_tick;
 o->row_capacity=rows;n=sizeof(*o);align=_Alignof(struct slcli_native_clock);n=(n+align-1)/align*align;
 o->before=(void *)((unsigned char *)o+n);o->prepared=1;b->deadline_owner=o;return b;
}
int slcli_bq_serialized_factory_windows_construct_original(
 struct slcli_bq_factory_bootstrap_windows *b,struct slcli_arena *a,
 const struct slcli_source_deadline_original *d,const void *installer,size_t installer_bytes,
 const struct slcli_native_clock *prior,const struct slcli_bq_source_association_windows_inputs *ai,
 const struct slcli_bq_source_unit_windows_inputs *ui){
 struct slcli_memory_owner *owner;struct slcli_extent *e;struct slcli_arena_block_header *h;
 struct slcli_bq_serialized_factory_windows *c;uint64_t at=0,first,records,names,name_ranges;
 size_t ac,uc,all,blocks,prefix=0,total=0,am,um,ar=0,ur=0,i,k,rows,storage,context,joined=0;
 if(!b||!a||!(owner=a->owner)||!slcli_memory_owner_mutable(owner)||owner->failed||
  !slcli_arena_live_block(a,b,sizeof(*b))||!zero_original(b,sizeof(*b))||
  !slcli_arena_live_block(a,ai,sizeof(*ai))||!slcli_arena_live_block(a,ui,sizeof(*ui))||
  !installer_bytes||!slcli_arena_live_block(a,installer,installer_bytes)||
  !slcli_source_deadline_original_matches(a,d,installer)||!prior||
  !slcli_arena_live_range(a,prior,sizeof(*prior))||
  !slcli_native_clock_windows_matches_original(prior)||
  !slcli_native_clock_windows_matches_original(d->origin)||
  prior->tick<d->origin->tick||prior->tick>=d->expires||
  !apart(b,sizeof(*b),ai,sizeof(*ai))||!apart(b,sizeof(*b),ui,sizeof(*ui))||
  !apart(b,sizeof(*b),a,sizeof(*a))||!apart(b,sizeof(*b),owner,sizeof(*owner))||
  !apart(b,sizeof(*b),installer,installer_bytes)||!apart(b,sizeof(*b),prior,sizeof(*prior))||
  !slcli_source_deadline_away(d,b,sizeof(*b))||
  !maximum(a,ai->capture_factory,&am)||!maximum(a,ui->capture_factory,&um))return 0;
 records=ai->record_count;names=ai->names_bytes;
 /* An entered physical mutation cannot be stranded behind this permanent
  * attachment. Complete native/CPU borrower reconstruction remains a genuine
  * installer obligation; ledger state alone never proves that closure. */
 for(i=0;i<owner->count;i++)
  if(owner->extents[i].state!=SLCLI_EXTENT_LIVE&&
   owner->extents[i].state!=SLCLI_EXTENT_BACKING_ACQUIRED&&
   owner->extents[i].state!=SLCLI_EXTENT_RETIRED)return 0;
 if(!records||records!=ui->record_count||!names||names>UINT64_MAX-128||records>(UINT64_MAX-128-names)/320||
  ai->file_bytes!=128+records*320+names||records>(UINT64_MAX-64)/128||
  ui->file_bytes<64+records*128)return 0;
 name_ranges=names/16384+(names%16384!=0);
 if(records>SIZE_MAX-1||name_ranges>SIZE_MAX-1-(size_t)records)return 0;
 ac=1+(size_t)records+(size_t)name_ranges;uc=1+(size_t)records;
 if(ac>SIZE_MAX-uc)return 0;all=ac+uc;if(all>(SIZE_MAX-16)/2)return 0;blocks=16+2*all;
 /* Genuine unpublished installer exclusion already holds. Count every real
  * header before sizing the roster; its final free header becomes the caller
  * header, and a distinct final free row remains after all private splits. */
 e=&owner->extents[a->extent];
 for(;;){h=header(e,at);if(!h||prefix==SIZE_MAX)return 0;prefix++;
  if(!h->next)break;at=h->next;}
 if(h->state!=SLCLI_ARENA_BLOCK_FREE||at<a->allocation_floor||blocks>SIZE_MAX-prefix)return 0;
 blocks+=prefix;first=at;
 if(blocks>SIZE_MAX/sizeof(*c->blocks)||ac>SIZE_MAX/sizeof(*c->association_ranges)||
  uc>SIZE_MAX/sizeof(*c->unit_ranges)||ac>SIZE_MAX/sizeof(*c->association_bodies)||
  uc>SIZE_MAX/sizeof(*c->unit_bodies))return 0;
 if(!add(&total,sizeof(*c))||!add(&total,blocks*sizeof(*c->blocks))||
  !add(&total,sizeof(*c->association_table))||!add(&total,sizeof(*c->unit_table))||
  !add(&total,ac*sizeof(*c->association_bodies))||!add(&total,uc*sizeof(*c->unit_bodies))||
  !add(&total,ac*sizeof(*c->association_ranges))||!add(&total,uc*sizeof(*c->unit_ranges))||
  !add(&total,sizeof(*c->association_sink))||!add(&total,sizeof(*c->unit_sink))||
  !add(&total,sizeof(*c->association_inputs))||!add(&total,sizeof(*c->unit_inputs))||
  !add(&total,sizeof(*c->association_original))||!add(&total,sizeof(*c->unit_original)))return 0;
 for(i=0;i<all;i++){
  int association=i<ac;size_t bytes=range_bytes(records,names,association?i:i-ac,association);
  if(!body_shape(bytes,association?am:um,&rows,&storage,&context)||
   !add(&total,storage)||!add(&total,context))return 0;
  if(association){if(rows>SIZE_MAX-ar)return 0;ar+=rows;}else{if(rows>SIZE_MAX-ur)return 0;ur+=rows;}
 }
 if(ar>SIZE_MAX/sizeof(*c->association_target_rows)||ur>SIZE_MAX/sizeof(*c->unit_target_rows)||
  !add(&total,ar*sizeof(*c->association_target_rows))||!add(&total,ur*sizeof(*c->unit_target_rows)))return 0;
 b->started=1;b->arena_original=a;b->owner_original=owner;b->deadline_original=d;
 b->installer_original=installer;b->installer_bytes=installer_bytes;b->required_bytes=total;
 b->chain_headers=prefix;
 if(h->state!=SLCLI_ARENA_BLOCK_FREE||at<a->allocation_floor||total>h->bytes||h->bytes-total<48)goto done;
 first=at;b->tail_offset=at;b->caller_address=e->base+(uintptr_t)at+sizeof(*h);
 /* Genuine unpublished installer ownership precedes this transition. No lock
  * inside not-yet-carved caller storage is claimed to validate its own birth. */
 owner->bq_owner_original=owner;owner->bq_arena_original=a;owner->bq_caller_original=(void *)b->caller_address;
 owner->bq_acquisition_original=a->acquisition;owner->bq_extent_original=a->extent;owner->bq_attachment_state=1;
 b->attached=1;c=split_tail(e,&at,sizeof(*c));if(!c)goto done;
 b->caller_header_written=1;b->caller_zeroed=1;b->caller_original=c;
 c->arena=a;c->owner_original=owner;c->bootstrap_original=b;c->deadline=d;c->installer_original=installer;c->installer_bytes=installer_bytes;
 c->subjects[0].value=installer;c->subjects[0].bytes=installer_bytes;
 c->subjects[1].value=d;c->subjects[1].bytes=sizeof(*d);
 c->subjects[2].value=d->origin;c->subjects[2].bytes=sizeof(*d->origin);
 c->subjects[3].value=prior;c->subjects[3].bytes=sizeof(*prior);
 c->subjects[4].value=ai;c->subjects[4].bytes=sizeof(*ai);
 c->subjects[5].value=ui;c->subjects[5].bytes=sizeof(*ui);
 c->subjects[6].value=b;c->subjects[6].bytes=sizeof(*b);
 c->extent_base_original=e->base;c->extent_bytes_original=e->reserved;
 c->acquisition_original=a->acquisition;c->extent_original=a->extent;
 c->prior_original=prior;c->association_template=ai;c->unit_template=ui;c->failed=1;
 c->tail_offset=at;c->chain_headers=b->chain_headers;c->association_count=ac;c->unit_count=uc;
 c->association_targets=ar;c->unit_targets=ur;c->block_capacity=blocks;
 if(!clock_original(c,0,0))goto done;c->init_operand=(uintptr_t)&c->lock;c->init_called=1;
 InitializeSRWLock(&c->lock);c->init_returned=1;if(!clock_original(c,0,1))goto done;
 if(!clock_original(c,1,0))goto done;c->thread_called=1;c->issuing_thread=GetCurrentThreadId();c->thread_returned=1;
 if(!clock_original(c,1,1)||!c->issuing_thread)goto done;
 if(!clock_original(c,2,0))goto done;c->try_operand=(uintptr_t)&c->lock;c->try_called=1;
 c->try_raw=TryAcquireSRWLockExclusive(&c->lock);c->try_returned=1;c->acquired=c->try_raw!=0;
 if(!clock_original(c,2,1)||!c->acquired)goto done;
 /* Roster itself is the one bootstrap allocation not yet recordable in it. */
 {uint64_t roster_at=c->tail_offset;c->blocks=split_tail(e,&c->tail_offset,blocks*sizeof(*c->blocks));
  if(!c->blocks)goto done;i=prefix-1;c->caller_block=&c->blocks[i];c->roster_block=&c->blocks[i+1];
  c->blocks[i].value=c;c->blocks[i].requested=sizeof(*c);c->blocks[i].header_offset=first;i++;
  c->blocks[i].value=c->blocks;c->blocks[i].requested=blocks*sizeof(*c->blocks);
  c->blocks[i].header_offset=roster_at;c->block_count=i+1;}
 c->failed=0;
#define TAKE(field,n) do {c->field=take(c,(n));if(!c->field){c->failed=1;goto done;}} while(0)
 TAKE(association_table,sizeof(*c->association_table));TAKE(unit_table,sizeof(*c->unit_table));
 TAKE(association_bodies,ac*sizeof(*c->association_bodies));TAKE(unit_bodies,uc*sizeof(*c->unit_bodies));
 TAKE(association_ranges,ac*sizeof(*c->association_ranges));TAKE(unit_ranges,uc*sizeof(*c->unit_ranges));
 TAKE(association_sink,sizeof(*c->association_sink));TAKE(unit_sink,sizeof(*c->unit_sink));
 TAKE(association_target_rows,ar*sizeof(*c->association_target_rows));TAKE(unit_target_rows,ur*sizeof(*c->unit_target_rows));
 TAKE(association_inputs,sizeof(*c->association_inputs));TAKE(unit_inputs,sizeof(*c->unit_inputs));
 TAKE(association_original,sizeof(*c->association_original));TAKE(unit_original,sizeof(*c->unit_original));
#undef TAKE
 for(i=0;i<all;i++){
  int association=i<ac;size_t index=association?i:i-ac;
  struct slcli_input_body *body=body_original(c,range_bytes(records,names,index,association),association?am:um,
   association?c->association_sink:c->unit_sink);
  if(!body){c->failed=1;goto done;}
  if(association)c->association_bodies[index]=body;else c->unit_bodies[index]=body;
 }
 *c->association_inputs=*ai;c->association_inputs->table=c->association_table;
 c->association_inputs->bodies=c->association_bodies;c->association_inputs->ranges=c->association_ranges;
 c->association_inputs->range_count=ac;c->association_inputs->capture_sink=c->association_sink;
 c->association_inputs->capture_targets=c->association_target_rows;c->association_inputs->capture_target_count=ar;
 *c->unit_inputs=*ui;c->unit_inputs->table=c->unit_table;c->unit_inputs->bodies=c->unit_bodies;
 c->unit_inputs->ranges=c->unit_ranges;c->unit_inputs->range_count=uc;c->unit_inputs->capture_sink=c->unit_sink;
 c->unit_inputs->capture_targets=c->unit_target_rows;c->unit_inputs->capture_target_count=ur;
 if(c->block_count!=c->block_capacity-1){c->failed=1;goto done;}
 /* One whole real-chain replay retains and joins every actual header,
  * including unrelated prefix blocks/free holes and the final free tail.
  * These tuples are operands for fresh header checks, never cached authority. */
 at=0;for(;;){h=header(e,at);if(!h){c->failed=1;goto done;}c->joined_headers++;
  if(joined>=c->block_capacity){c->failed=1;goto done;}
  if(joined>=prefix-1&&joined<c->block_count){
   if(at!=c->blocks[joined].header_offset||h->state!=SLCLI_ARENA_BLOCK_LIVE||
    h->requested!=c->blocks[joined].requested||
    c->blocks[joined].value!=(void *)(e->base+(uintptr_t)at+sizeof(*h))){c->failed=1;goto done;}
  }else if(joined==c->block_count&&
   (h->next||at!=c->tail_offset||h->state!=SLCLI_ARENA_BLOCK_FREE||h->requested)){
   c->failed=1;goto done;
  }
  c->blocks[joined].value=(void *)(e->base+(uintptr_t)at+sizeof(*h));
  c->blocks[joined].requested=(size_t)h->requested;c->blocks[joined].header_offset=at;
  c->blocks[joined].bytes=h->bytes;c->blocks[joined].next=h->next;
  c->blocks[joined].state=h->state;
  if(joined<prefix-1&&h->state==SLCLI_ARENA_BLOCK_LIVE){
   uintptr_t payload=(uintptr_t)c->blocks[joined].value;
   for(k=0;k<7;k++){
    uintptr_t value=(uintptr_t)c->subjects[k].value;
    if(value>=payload&&value-payload<=h->requested&&
     c->subjects[k].bytes<=h->requested-(value-payload)){
     if(c->subjects[k].block){c->failed=1;goto done;}
     c->subjects[k].block=&c->blocks[joined];c->subjects[k].offset=(size_t)(value-payload);
    }
   }
  }
  joined++;
  if(!h->next)break;at=h->next;
 }
 if(joined!=c->block_capacity){c->failed=1;goto done;}c->block_count=joined;
 for(k=0;k<7;k++)if(!c->subjects[k].block){c->failed=1;goto done;}
 /* Permanent source-read borrow precedes publication. No general mutation or
  * unlock follows even if subsequent table/native/consumer work fails. Actual
  * same-thread fixed A/unit dispatch/currentness and last-borrower are still
  * coupled implementation obligations; prepared storage grants no activation. */
 c->source_borrow=1;
 if(!clock_original(c,3,0)){c->failed=1;goto done;}
 c->storage_prepared=1;owner->bq_attachment_state=2;b->result=1;
done:b->returned=1;return b->result;
}
#endif
