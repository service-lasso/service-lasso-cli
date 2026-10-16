#include "capture_backing_windows.h"
#include "capture_view_windows.h"
#include "capture_ranges_windows.h"
#include "bq_capture_schedule_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int zero(const void *p,size_t n){size_t i;const unsigned char *b=p;for(i=0;i<n;i++)if(b[i])return 0;return 1;}
static int same(const struct slcli_writer_capture_backing_windows *c){
 const struct slcli_extent *e;size_t i;
 const struct slcli_binding_storage_original *storage;
 if(!c||!c->started||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||
  !c->readback||!slcli_arena_live_block(c->arena,c->readback,sizeof(*c->readback))||
  c->readback->arena!=c->arena||!slcli_writer_windows_readback_matches_original(c->readback)||
  !c->owner||!slcli_arena_live_block(c->arena,c->owner,sizeof(*c->owner))||
  !slcli_memory_owner_writable(c->owner)||c->owner->failed||
  c->request.index>=c->owner->count||c->request.index>=256||!c->request.sequence||
  !c->file_bytes||c->file_bytes>INT64_MAX||!c->count||c->count>SIZE_MAX/sizeof(*c->plan)||
  !slcli_arena_live_block(c->arena,c->plan,c->count*sizeof(*c->plan)))return 0;
 if(c->bq_schedule_original){
  const struct slcli_bq_capture_schedule_windows *s=c->bq_schedule_original;
  if(c->storage_original||c->reference_plans||c->reference_ids||c->first_ordinal||c->claimed||c->reference_count||
   !slcli_bq_capture_schedule_windows_matches_original(s)||s->arena!=c->arena||s->backing!=c||
   s->owner!=c->owner||s->request.index!=c->request.index||s->request.sequence!=c->request.sequence||
   s->plan!=c->plan||s->count!=c->count||s->file_bytes!=c->file_bytes||s->next!=s->target_count)return 0;
 }else{
 storage=c->storage_original;
 if(!storage||!slcli_arena_live_block(c->arena,storage,sizeof(*storage))||storage->arena!=c->arena||
  !storage->prepared||!storage->book||!slcli_arena_live_block(c->arena,storage->book,sizeof(*storage->book))||
  c->count>(SIZE_MAX-2)/2||c->reference_count!=2+2*c->count||
  c->claimed!=c->reference_count||storage->attempted>storage->capacity||c->first_ordinal>storage->attempted||
  storage->capacity>SIZE_MAX/sizeof(*storage->references)||storage->capacity>SIZE_MAX/sizeof(*storage->ids)||
  storage->book->references!=storage->references||storage->book->original_ids!=storage->ids||
  !slcli_arena_live_block(c->arena,storage->references,storage->capacity*sizeof(*storage->references))||
  !slcli_arena_live_block(c->arena,storage->ids,storage->capacity*sizeof(*storage->ids))||
  c->reference_count>storage->attempted-c->first_ordinal||
  storage->capacity>SIZE_MAX/sizeof(*storage->claims)||
  !slcli_arena_live_block(c->arena,storage->claims,storage->capacity*sizeof(*storage->claims))||
  !slcli_arena_live_block(c->arena,c->reference_plans,c->reference_count*sizeof(*c->reference_plans))||
  !slcli_arena_live_block(c->arena,c->reference_ids,c->reference_count*sizeof(*c->reference_ids)))return 0;
 for(i=0;i<c->reference_count;i++){
  const struct slcli_binding_row_claim_original *claim=&storage->claims[c->first_ordinal+i];
  const void *container=i<2?(const void *)c:(const void *)c->plan[(i-2)/2].capture;
  size_t containing_bytes=i<2?sizeof(*c):sizeof(*c->plan[0].capture);
  const void *capture=i<2?(const void *)&c->sections[i]:
   (const void *)&c->plan[(i-2)/2].capture->views[(i-2)%2];
  enum slcli_native_original_class tag=i<2?SLCLI_NATIVE_ORIGINAL_BQ_CAPTURE_SECTION:SLCLI_NATIVE_ORIGINAL_BQ_CAPTURE_VIEW;
  if(!claim->claimed||claim->ordinal!=c->first_ordinal+i||claim->capture!=capture||
   claim->capture_bytes!=sizeof(void *)||claim->containing_block!=container||claim->containing_bytes!=containing_bytes||
   claim->capture_offset!=(size_t)((uintptr_t)capture-(uintptr_t)container)||claim->native_class!=tag||
   storage->book->original_ids[c->first_ordinal+i]!=c->reference_ids[i]||
   storage->book->references[c->first_ordinal+i].version!=c->reference_plans[i].version||
   storage->book->references[c->first_ordinal+i].kind!=(i<2?22:9)||
   storage->book->references[c->first_ordinal+i].physical_extent!=c->request.index||
   storage->book->references[c->first_ordinal+i].alias_of!=(i?c->first_ordinal:UINT16_MAX))return 0;
 }
 }
 e=&c->owner->extents[c->request.index];
 return e->reserved==c->file_bytes&&e->category==SLCLI_IO&&e->acquisition==c->request.sequence&&
  e->latest==c->request.sequence&&!e->base&&!e->arena_initialized&&!e->retirement&&
  e->state==(c->backing_registered?SLCLI_EXTENT_BACKING_ACQUIRED:SLCLI_EXTENT_RESERVED);
}
static int tick(struct slcli_writer_capture_backing_windows *c,unsigned i,unsigned after){
 struct slcli_native_clock *clock=&c->clocks[i][after];
 c->clock_result[i][after]=slcli_native_clock_read(clock);c->clock_returned[i][after]=1;
 if(!c->clock_result[i][after]||clock->tick<c->previous_tick||
  clock->tick>=c->readback->deadline->expires||!same(c)||
  !slcli_evidence_deadline_precede(c->readback->creator->evidence_file,clock))return 0;
 c->previous_tick=clock->tick;return 1;
}
static int acquire(struct slcli_writer_capture_backing_windows *c);
int slcli_writer_capture_backing_windows_prepare_original(
 struct slcli_writer_capture_backing_windows *c,struct slcli_arena *a,
 struct slcli_writer_windows_readback_original *r,struct slcli_memory_owner *owner,
 const struct slcli_extent_request *request,uint64_t file_bytes,
 const struct slcli_writer_capture_view_plan_windows *plan,size_t count,
 struct slcli_binding_storage_original *storage,const struct slcli_native_reference *plans,const uint32_t *ids){
 size_t i,plan_bytes,rows;
 if(!c||!a||!r||!owner||!slcli_memory_owner_mutable(owner)||!request||!plan||!count||count>SIZE_MAX/sizeof(*plan)||
  !slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !slcli_arena_live_block(a,r,sizeof(*r))||r->arena!=a||!slcli_writer_windows_readback_matches_original(r)||
  !slcli_arena_live_block(a,owner,sizeof(*owner))||!slcli_arena_live_range(a,request,sizeof(*request))||
  !file_bytes||file_bytes>INT64_MAX||file_bytes>r->creator->evidence_plan->maximum||
  r->observed.standard.EndOfFile.QuadPart||count>(SIZE_MAX-2)/2||!storage||!plans||!ids||
  !slcli_arena_live_block(a,storage,sizeof(*storage))||storage->arena!=a||!storage->prepared||storage->enrolled)return 0;
 rows=2+2*count;
 if(rows>UINT32_MAX||rows>SIZE_MAX/sizeof(*plans)||rows>SIZE_MAX/sizeof(*ids)||
  storage->attempted>storage->capacity||rows>storage->capacity-storage->attempted||
  storage->attempted>=UINT16_MAX||rows>UINT16_MAX-storage->attempted||
  !slcli_arena_live_block(a,plans,rows*sizeof(*plans))||!slcli_arena_live_block(a,ids,rows*sizeof(*ids))||
  !apart(c,sizeof(*c),storage,sizeof(*storage))||!apart(c,sizeof(*c),plans,rows*sizeof(*plans))||
  !apart(c,sizeof(*c),ids,rows*sizeof(*ids)))return 0;
 for(i=0;i<rows;i++)if(plans[i].kind!=(i<2?22:9)||plans[i].physical_extent!=request->index||
  plans[i].alias_of!=(i?storage->attempted:UINT16_MAX)||!ids[i]||(i&&ids[i]<=ids[i-1]))return 0;
 plan_bytes=count*sizeof(*plan);
 if(!slcli_arena_live_block(a,plan,plan_bytes)||!apart(c,sizeof(*c),plan,plan_bytes)||
  !apart(c,sizeof(*c),r,sizeof(*r))||!apart(c,sizeof(*c),owner,sizeof(*owner))||
  !apart(c,sizeof(*c),request,sizeof(*request))||!apart(c,sizeof(*c),r->creator,sizeof(*r->creator))||
  !apart(c,sizeof(*c),r->caller,r->caller_bytes)||!slcli_source_deadline_away(r->deadline,c,sizeof(*c)))return 0;
 for(i=0;i<count;i++)if(!plan[i].capture||!slcli_arena_live_block(a,plan[i].capture,sizeof(*plan[i].capture))||
  !apart(c,sizeof(*c),plan[i].capture,sizeof(*plan[i].capture))||!zero(plan[i].capture,sizeof(*plan[i].capture)))return 0;
 c->started=1;c->failed=1;c->arena=a;c->readback=r;c->owner=owner;c->request=*request;
 c->plan=plan;c->count=count;c->file_bytes=file_bytes;c->previous_tick=r->observe_after.tick;
 c->storage_original=storage;c->reference_plans=plans;c->reference_ids=ids;
 c->first_ordinal=storage->attempted;c->reference_count=rows;
 for(i=0;i<rows;i++){
  const void *container=i<2?(const void *)c:(const void *)plan[(i-2)/2].capture;
  size_t containing_bytes=i<2?sizeof(*c):sizeof(*plan[0].capture);
  const void *capture=i<2?(const void *)&c->sections[i]:(const void *)&plan[(i-2)/2].capture->views[(i-2)%2];
  enum slcli_native_original_class tag=i<2?SLCLI_NATIVE_ORIGINAL_BQ_CAPTURE_SECTION:SLCLI_NATIVE_ORIGINAL_BQ_CAPTURE_VIEW;
  if(!slcli_binding_storage_claim_pre_effect_original(storage,&plans[i],ids[i],container,containing_bytes,
   capture,sizeof(void *),tag))return 0;
  c->claimed=i+1;
 }
 return acquire(c);
}
int slcli_writer_capture_backing_windows_prepare_bq_original(
 struct slcli_writer_capture_backing_windows *c,struct slcli_arena *a,
 struct slcli_writer_windows_readback_original *r,struct slcli_memory_owner *owner,
 const struct slcli_extent_request *request,uint64_t file_bytes,
 const struct slcli_writer_capture_view_plan_windows *plan,size_t count,
 struct slcli_bq_capture_schedule_windows *s){
 size_t i;
 if(!c||!a||!r||!owner||!slcli_memory_owner_mutable(owner)||!request||!plan||!s||
  !slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !slcli_arena_live_block(a,r,sizeof(*r))||r->arena!=a||
  !slcli_writer_windows_readback_matches_original(r)||
  !slcli_bq_capture_schedule_windows_matches_original(s)||s->arena!=a||s->backing!=c||
  s->owner!=owner||s->plan!=plan||s->count!=count||s->file_bytes!=file_bytes||s->next||
  !slcli_arena_live_range(a,request,sizeof(*request))||
  s->request.index!=request->index||s->request.sequence!=request->sequence||
  file_bytes>r->creator->evidence_plan->maximum||r->observed.standard.EndOfFile.QuadPart||
  !apart(c,sizeof(*c),r,sizeof(*r))||!apart(c,sizeof(*c),r->creator,sizeof(*r->creator))||
  !apart(c,sizeof(*c),r->caller,r->caller_bytes)||!slcli_source_deadline_away(r->deadline,c,sizeof(*c)))return 0;
 for(i=0;i<count;i++)if(!zero(plan[i].capture,sizeof(*plan[i].capture))||
  !apart(plan[i].capture,sizeof(*plan[i].capture),r->caller,r->caller_bytes)||
  !apart(plan[i].capture,sizeof(*plan[i].capture),r,sizeof(*r))||
  !apart(plan[i].capture,sizeof(*plan[i].capture),r->creator,sizeof(*r->creator))||
  !slcli_source_deadline_away(r->deadline,plan[i].capture,sizeof(*plan[i].capture)))return 0;
 c->started=1;c->failed=1;c->arena=a;c->readback=r;c->owner=owner;c->request=*request;
 c->plan=plan;c->count=count;c->file_bytes=file_bytes;c->previous_tick=r->observe_after.tick;
 c->bq_schedule_original=s;
 /* Every target is consumed before the first section native call. A failed
  * take preserves both the backing control and the schedule's partial prefix. */
 for(i=0;i<s->target_count;i++){
  const void *container=i<2?(const void *)c:(const void *)plan[(i-2)/2].capture;
  size_t bytes=i<2?sizeof(*c):sizeof(*plan[0].capture);
  void *output=i<2?(void *)&c->sections[i]:(void *)&plan[(i-2)/2].capture->views[(i-2)%2];
  if(!slcli_bq_capture_schedule_windows_take_original(s,i,container,bytes,output))return 0;
 }
 return acquire(c);
}
static int acquire(struct slcli_writer_capture_backing_windows *c){
 size_t i;struct slcli_writer_windows_readback_original *r=c->readback;
 struct slcli_memory_owner *owner=c->owner;uint64_t file_bytes=c->file_bytes;
 const struct slcli_writer_capture_view_plan_windows *plan=c->plan;size_t count=c->count;
 if(!same(c)||!tick(c,0,0))return 0;
 c->system_output=(uintptr_t)&c->system;c->called[0]=1;GetSystemInfo(&c->system);c->returned[0]=1;
 if(!tick(c,0,1)||c->system.dwPageSize!=owner->native_page_size||
  !slcli_writer_capture_plan_windows_geometry_original(c->arena,plan,count,file_bytes,&c->system,&c->mapped_bytes))return 0;
 for(i=0;i<2;i++){
  unsigned call=(unsigned)i+1;
  c->section_args[i][0]=(uintptr_t)(i?r->original:r->creator->original);
  c->section_args[i][1]=0;c->section_args[i][2]=i?PAGE_READONLY:PAGE_READWRITE;
  c->section_args[i][3]=i?0:(DWORD)(file_bytes>>32);c->section_args[i][4]=i?0:(DWORD)file_bytes;c->section_args[i][5]=0;
  if(!tick(c,call,0))return 0;c->called[call]=1;
  c->sections[i]=CreateFileMappingW((HANDLE)(uintptr_t)c->section_args[i][0],0,
   (DWORD)c->section_args[i][2],(DWORD)c->section_args[i][3],(DWORD)c->section_args[i][4],0);
  c->returned[call]=1;if(!c->sections[i])c->section_error[i]=GetLastError();
  if(!i&&c->sections[i])c->backing_registered=(uint32_t)slcli_memory_backing_acquired(owner,&c->request);
  if(!tick(c,call,1)||!c->sections[i]||(!i&&!c->backing_registered))return 0;
 }
 if(!tick(c,3,0))return 0;c->called[3]=1;
 c->file_after_result=slcli_windows_source_observe(r->original,&c->file_after);c->returned[3]=1;
 if(!tick(c,3,1)||!c->file_after_result||c->file_after.standard.EndOfFile.QuadPart<0||
  (uint64_t)c->file_after.standard.EndOfFile.QuadPart!=file_bytes||
  c->file_after.tick<c->clocks[3][0].tick||c->file_after.tick>c->clocks[3][1].tick||
  c->file_after.identity.VolumeSerialNumber!=r->observed.identity.VolumeSerialNumber||
  !slcli_equal(c->file_after.identity.FileId.Identifier,r->observed.identity.FileId.Identifier,16))return 0;
 c->complete=1;c->failed=0;return 1;
}
int slcli_writer_capture_factory_windows_create_original(
 struct slcli_writer_capture_factory_windows *f,struct slcli_arena *a,
 struct slcli_writer_windows_path *creator,struct slcli_writer_windows_output *output,
 const struct slcli_writer_windows_evidence_originals *inputs,struct slcli_writer_windows_readback_original *readback,
 struct slcli_writer_capture_backing_windows *backing,struct slcli_writer_capture_ranges_windows *ranges,
 size_t range_index,size_t range_count,struct slcli_memory_owner *owner,const struct slcli_extent_request *request,
 uint64_t file_bytes,const struct slcli_writer_capture_view_plan_windows *plan,size_t count,
 struct slcli_binding_storage_original *storage,const struct slcli_native_reference *references,const uint32_t *ids) {
 const void *blocks[10];size_t bytes[10],i,j,rows,payload,data,payload_offset,stride;uint64_t mapped=0;
 if(!a||!f||!creator||!output||!inputs||!readback||!backing||!ranges||!owner||!request||
  !plan||!count||range_index>=count||!range_count||count>SIZE_MAX/sizeof(*plan)||count>(SIZE_MAX-2)/2||
  !storage||!references||!ids||!file_bytes||file_bytes>INT64_MAX||
  !slcli_arena_live_block(a,f,sizeof(*f))||!zero(f,sizeof(*f)))return 0;
 rows=2+2*count;
 if(rows>SIZE_MAX/sizeof(*references)||rows>SIZE_MAX/sizeof(*ids)||
  !slcli_arena_live_block(a,storage,sizeof(*storage))||storage->arena!=a||!storage->prepared||storage->enrolled||
  storage->attempted>storage->capacity||rows>storage->capacity-storage->attempted||
  !slcli_arena_live_block(a,inputs,sizeof(*inputs))||!inputs->bq_profile||file_bytes>inputs->maximum||
  !slcli_arena_live_block(a,request,sizeof(*request))||
  !slcli_arena_live_block(a,plan,count*sizeof(*plan))||
  !slcli_arena_live_block(a,references,rows*sizeof(*references))||
  !slcli_arena_live_block(a,ids,rows*sizeof(*ids)))return 0;
 if(!slcli_arena_live_range(a,inputs->bq_profile,sizeof(*inputs->bq_profile))||
  !inputs->bq_profile->raw_bytes||!slcli_arena_live_range(a,inputs->bq_profile->raw,inputs->bq_profile->raw_bytes)||
  inputs->bq_profile->retention_maximum!=inputs->maximum||!inputs->bq_profile->record_maximum||
  inputs->bq_profile->record_maximum>inputs->maximum||inputs->maximum>UINT64_C(9007199254740991)||
  !apart(plan,count*sizeof(*plan),references,rows*sizeof(*references))||
  !apart(plan,count*sizeof(*plan),ids,rows*sizeof(*ids))||
  !apart(references,rows*sizeof(*references),ids,rows*sizeof(*ids))||
  storage->attempted>=UINT16_MAX||rows>UINT16_MAX-storage->attempted)return 0;
 payload=inputs->bq_profile->record_maximum>16384?16384:(size_t)inputs->bq_profile->record_maximum;
 if(!slcli_writer_capture_ranges_windows_payload_geometry(range_count,payload,&data,&payload_offset,&stride)||
  range_count*stride>plan[range_index].bytes)return 0;
 for(i=0;i<rows;i++)if(references[i].kind!=(i<2?22:9)||
  references[i].physical_extent!=request->index||references[i].alias_of!=(i?storage->attempted:UINT16_MAX)||
  !ids[i]||(i&&ids[i]<=ids[i-1]))return 0;
 blocks[0]=f;bytes[0]=sizeof(*f);blocks[1]=creator;bytes[1]=sizeof(*creator);
 blocks[2]=output;bytes[2]=sizeof(*output);blocks[3]=readback;bytes[3]=sizeof(*readback);
 blocks[4]=backing;bytes[4]=sizeof(*backing);blocks[5]=ranges;bytes[5]=sizeof(*ranges);
 blocks[6]=inputs;bytes[6]=sizeof(*inputs);blocks[7]=owner;bytes[7]=sizeof(*owner);
 blocks[8]=storage;bytes[8]=sizeof(*storage);blocks[9]=request;bytes[9]=sizeof(*request);
 for(i=0;i<10;i++) {
  if(!slcli_arena_live_block(a,blocks[i],bytes[i])||
   !apart(blocks[i],bytes[i],inputs->bq_profile,sizeof(*inputs->bq_profile))||
   !apart(blocks[i],bytes[i],inputs->bq_profile->raw,inputs->bq_profile->raw_bytes)||
   !apart(blocks[i],bytes[i],plan,count*sizeof(*plan))||
   !apart(blocks[i],bytes[i],references,rows*sizeof(*references))||
   !apart(blocks[i],bytes[i],ids,rows*sizeof(*ids)))return 0;
  for(j=0;j<i;j++)if(!apart(blocks[i],bytes[i],blocks[j],bytes[j]))return 0;
 }
 if(!zero(creator,sizeof(*creator))||!zero(readback,sizeof(*readback))||
  !zero(backing,sizeof(*backing))||!zero(ranges,sizeof(*ranges)))return 0;
 for(i=0;i<count;i++) {
  if(!plan[i].bytes||plan[i].bytes>=file_bytes||plan[i].bytes>SIZE_MAX||
   plan[i].file_offset>file_bytes-plan[i].bytes||mapped>file_bytes-plan[i].bytes||
   !plan[i].capture||!slcli_arena_live_block(a,plan[i].capture,sizeof(*plan[i].capture))||
   !zero(plan[i].capture,sizeof(*plan[i].capture))||
   !apart(plan[i].capture,sizeof(*plan[i].capture),plan,count*sizeof(*plan))||
   !apart(plan[i].capture,sizeof(*plan[i].capture),references,rows*sizeof(*references))||
   !apart(plan[i].capture,sizeof(*plan[i].capture),ids,rows*sizeof(*ids))||
   !apart(plan[i].capture,sizeof(*plan[i].capture),inputs->bq_profile,sizeof(*inputs->bq_profile))||
   !apart(plan[i].capture,sizeof(*plan[i].capture),inputs->bq_profile->raw,inputs->bq_profile->raw_bytes))return 0;
  for(j=0;j<10;j++)if(!apart(plan[i].capture,sizeof(*plan[i].capture),blocks[j],bytes[j]))return 0;
  for(j=0;j<i;j++)if(!apart(plan[i].capture,sizeof(*plan[i].capture),plan[j].capture,sizeof(*plan[j].capture))||
   (plan[i].file_offset<plan[j].file_offset+plan[j].bytes&&plan[j].file_offset<plan[i].file_offset+plan[i].bytes))return 0;
  mapped+=plan[i].bytes;
 }
 if(mapped>=file_bytes)return 0;
 f->started=1;f->failed=1;f->arena=a;f->creator=creator;f->output=output;f->inputs=inputs;
 f->readback=readback;f->backing=backing;f->ranges=ranges;f->range_index=range_index;f->range_count=range_count;
 f->called[0]=1;f->results[0]=slcli_writer_create_windows_bq_original(a,creator,output,inputs);f->returned[0]=1;
 if(!f->results[0]){f->first_failure=1;return 0;}
 f->called[1]=1;f->results[1]=slcli_writer_duplicate_windows_readback_original(readback,a,creator,
  inputs->deadline,inputs->caller,inputs->caller_bytes,&creator->evidence_birth_clock);f->returned[1]=1;
 if(!f->results[1]){f->first_failure=2;return 0;}
 f->called[2]=1;f->results[2]=slcli_writer_capture_backing_windows_prepare_original(backing,a,readback,
  owner,request,file_bytes,plan,count,storage,references,ids);f->returned[2]=1;
 if(!f->results[2]){f->first_failure=3;return 0;}
 f->called[3]=1;f->results[3]=slcli_writer_capture_view_windows_prepare_batch_original(backing);f->returned[3]=1;
 if(!f->results[3]){f->first_failure=4;return 0;}
 f->called[4]=1;f->results[4]=slcli_writer_capture_ranges_windows_prepare_payload_original(ranges,a,
  plan[range_index].capture,range_count,payload);f->returned[4]=1;
 if(!f->results[4]){f->first_failure=5;return 0;}
 f->complete=1;f->failed=0;return 1;
}
static int factory_bq_raw_matches(const struct slcli_writer_capture_factory_windows *f){
 size_t i;const struct slcli_bq_capture_schedule_windows *s;
 if(!f||!f->started||!f->arena||!slcli_arena_live_block(f->arena,f,sizeof(*f))||
  !(s=f->bq_schedule_original)||!slcli_bq_capture_schedule_windows_matches_original(s)||
  s->arena!=f->arena||s->backing!=f->backing||!s->count||f->range_index>=s->count||
  f->output_range_index>=s->count||f->output_range_index==f->range_index||
  f->control_range_index>=s->count||f->control_range_index==f->range_index||f->control_range_index==f->output_range_index||
  !slcli_arena_live_block(f->arena,f->creator,sizeof(*f->creator))||
  !slcli_arena_live_block(f->arena,f->output,sizeof(*f->output))||
  !slcli_arena_live_block(f->arena,f->inputs,sizeof(*f->inputs))||
  !slcli_arena_live_range(f->arena,f->inputs->bq_profile,sizeof(*f->inputs->bq_profile))||
  !f->inputs->bq_profile->raw_bytes||
  !slcli_arena_live_range(f->arena,f->inputs->bq_profile->raw,f->inputs->bq_profile->raw_bytes)||
  f->inputs->bq_profile->retention_maximum!=f->inputs->maximum||s->file_bytes>f->inputs->maximum||
  !f->inputs->bq_profile->record_maximum||f->inputs->bq_profile->record_maximum>f->inputs->maximum||
  !slcli_source_deadline_original_matches(f->arena,f->inputs->deadline,f->inputs->caller)||
  f->inputs->deadline->origin!=f->origin_original||
  f->origin_original->tick!=f->origin_tick_original||f->inputs->deadline->expires!=f->expires_original||
  !slcli_arena_live_block(f->arena,f->ranges,sizeof(*f->ranges))||
  !slcli_arena_live_block(f->arena,f->output_ranges,sizeof(*f->output_ranges))||
  !slcli_arena_live_block(f->arena,f->control_ranges,sizeof(*f->control_ranges))||
  !slcli_writer_windows_readback_matches_original(f->readback)||f->readback->creator!=f->creator||
  f->creator->evidence_original!=f->inputs||f->output->file!=f->creator->evidence_file||
  f->backing->bq_schedule_original!=s||f->backing->readback!=f->readback||
  !slcli_writer_capture_view_windows_batch_matches_original(f->backing)||
  !slcli_writer_capture_ranges_windows_matches_original(f->ranges)||
  f->ranges->extent!=s->plan[f->range_index].capture||f->ranges->capacity!=f->range_count||
  f->ranges->version!=2||f->ranges->payload_capacity!=(f->inputs->bq_profile->record_maximum>16384?
   16384:(size_t)f->inputs->bq_profile->record_maximum)||
  !slcli_writer_capture_ranges_windows_matches_original(f->output_ranges)||
  f->output_ranges->extent!=s->plan[f->output_range_index].capture||
  f->output_ranges->capacity!=f->output_range_count||f->output_ranges->version!=2||
  f->output_ranges->payload_capacity!=f->ranges->payload_capacity||
  f->output_ranges->before.tick<f->ranges->after.tick||
  !slcli_writer_capture_ranges_windows_matches_original(f->control_ranges)||
  f->control_ranges->extent!=s->plan[f->control_range_index].capture||
  f->control_ranges->capacity!=f->control_range_count||f->control_ranges->version!=2||
  f->control_ranges->payload_capacity!=f->ranges->payload_capacity||
  f->control_ranges->before.tick<f->output_ranges->after.tick)return 0;
 for(i=0;i<7;i++)if(!f->called[i]||!f->returned[i]||!f->results[i])return 0;
 return 1;
}
int slcli_writer_capture_factory_windows_bq_preflight_original(
 struct slcli_writer_capture_factory_windows *f,struct slcli_arena *a,
 struct slcli_writer_windows_path *creator,struct slcli_writer_windows_output *output,
 const struct slcli_writer_windows_evidence_originals *inputs,struct slcli_writer_windows_readback_original *readback,
 struct slcli_writer_capture_backing_windows *backing,struct slcli_writer_capture_ranges_windows *ranges,
 size_t range_index,size_t range_count,struct slcli_writer_capture_ranges_windows *output_ranges,
 size_t output_index,size_t output_count,struct slcli_writer_capture_ranges_windows *control_ranges,
 size_t control_index,size_t control_count,struct slcli_bq_capture_schedule_windows *s){
 const void *blocks[12];size_t sizes[12],i,j,payload,data,offset,stride;
 if(!a||!f||!creator||!output||!inputs||!readback||!backing||!ranges||!s||
  !slcli_arena_live_block(a,f,sizeof(*f))||!zero(f,sizeof(*f))||
  !slcli_bq_capture_schedule_windows_matches_original(s)||s->arena!=a||s->backing!=backing||s->next||
  range_index>=s->count||!range_count||output_index>=s->count||output_index==range_index||
  !output_count||!output_ranges||!slcli_arena_live_block(a,inputs,sizeof(*inputs))||
  !control_count||!control_ranges||control_index>=s->count||control_index==range_index||control_index==output_index||
  !slcli_source_deadline_original_matches(a,inputs->deadline,inputs->caller)||
  !slcli_arena_live_block(a,inputs->caller,inputs->caller_bytes)||
  !slcli_arena_live_range(a,inputs->bq_profile,sizeof(*inputs->bq_profile))||
  !inputs->bq_profile->raw_bytes||!slcli_arena_live_range(a,inputs->bq_profile->raw,inputs->bq_profile->raw_bytes)||
  inputs->bq_profile->retention_maximum!=inputs->maximum||s->file_bytes>inputs->maximum||
  !inputs->bq_profile->record_maximum||inputs->bq_profile->record_maximum>inputs->maximum)return 0;
 payload=inputs->bq_profile->record_maximum>16384?16384:(size_t)inputs->bq_profile->record_maximum;
 if(!slcli_writer_capture_ranges_windows_payload_geometry(range_count,payload,&data,&offset,&stride)||
  range_count*stride>s->plan[range_index].bytes)return 0;
 if(!slcli_writer_capture_ranges_windows_payload_geometry(output_count,payload,&data,&offset,&stride)||
  output_count*stride>s->plan[output_index].bytes)return 0;
 if(!slcli_writer_capture_ranges_windows_payload_geometry(control_count,payload,&data,&offset,&stride)||
  control_count*stride>s->plan[control_index].bytes)return 0;
 blocks[0]=f;sizes[0]=sizeof(*f);blocks[1]=creator;sizes[1]=sizeof(*creator);
 blocks[2]=output;sizes[2]=sizeof(*output);blocks[3]=readback;sizes[3]=sizeof(*readback);
 blocks[4]=backing;sizes[4]=sizeof(*backing);blocks[5]=ranges;sizes[5]=sizeof(*ranges);
 blocks[6]=inputs;sizes[6]=sizeof(*inputs);blocks[7]=s;sizes[7]=sizeof(*s);
 blocks[8]=s->targets;sizes[8]=s->target_count*sizeof(*s->targets);blocks[9]=s->owner;sizes[9]=sizeof(*s->owner);
 blocks[10]=output_ranges;sizes[10]=sizeof(*output_ranges);
 blocks[11]=control_ranges;sizes[11]=sizeof(*control_ranges);
 for(i=0;i<12;i++){
  if(!slcli_arena_live_block(a,blocks[i],sizes[i])||
   !apart(blocks[i],sizes[i],s->plan,s->count*sizeof(*s->plan))||
   !apart(blocks[i],sizes[i],inputs->bq_profile,sizeof(*inputs->bq_profile))||
   !apart(blocks[i],sizes[i],inputs->bq_profile->raw,inputs->bq_profile->raw_bytes)||
   !apart(blocks[i],sizes[i],inputs->caller,inputs->caller_bytes)||
   !slcli_source_deadline_away(inputs->deadline,blocks[i],sizes[i]))return 0;
  for(j=0;j<i;j++)if(!apart(blocks[i],sizes[i],blocks[j],sizes[j]))return 0;
 }
 if(!zero(creator,sizeof(*creator))||!zero(readback,sizeof(*readback))||
  !zero(backing,sizeof(*backing))||!zero(ranges,sizeof(*ranges))||!zero(output_ranges,sizeof(*output_ranges))||
  !zero(control_ranges,sizeof(*control_ranges)))return 0;
 for(i=0;i<s->count;i++){
  const void *c=s->plan[i].capture;size_t bytes=sizeof(*s->plan[i].capture);
  if(!zero(c,bytes)||!slcli_source_deadline_away(inputs->deadline,c,bytes)||
   !apart(c,bytes,inputs->caller,inputs->caller_bytes)||
   !apart(c,bytes,inputs->bq_profile,sizeof(*inputs->bq_profile))||
   !apart(c,bytes,inputs->bq_profile->raw,inputs->bq_profile->raw_bytes)||
   !apart(c,bytes,s->plan,s->count*sizeof(*s->plan)))return 0;
  for(j=0;j<12;j++)if(!apart(c,bytes,blocks[j],sizes[j]))return 0;
  for(j=0;j<i;j++)if(!apart(c,bytes,s->plan[j].capture,sizeof(*s->plan[j].capture)))return 0;
 }
 return 1;
}
int slcli_writer_capture_factory_windows_create_bq_original(
 struct slcli_writer_capture_factory_windows *f,struct slcli_arena *a,
 struct slcli_writer_windows_path *creator,struct slcli_writer_windows_output *output,
 const struct slcli_writer_windows_evidence_originals *inputs,struct slcli_writer_windows_readback_original *readback,
 struct slcli_writer_capture_backing_windows *backing,struct slcli_writer_capture_ranges_windows *ranges,
 size_t range_index,size_t range_count,struct slcli_writer_capture_ranges_windows *output_ranges,
 size_t output_index,size_t output_count,struct slcli_writer_capture_ranges_windows *control_ranges,
 size_t control_index,size_t control_count,struct slcli_bq_capture_schedule_windows *s){
 size_t payload;
 if(!slcli_writer_capture_factory_windows_bq_preflight_original(f,a,creator,output,inputs,readback,
  backing,ranges,range_index,range_count,output_ranges,output_index,output_count,
  control_ranges,control_index,control_count,s))return 0;
 payload=inputs->bq_profile->record_maximum>16384?16384:(size_t)inputs->bq_profile->record_maximum;
 f->started=1;f->failed=1;f->arena=a;f->creator=creator;f->output=output;f->inputs=inputs;
 f->readback=readback;f->backing=backing;f->ranges=ranges;f->range_index=range_index;f->range_count=range_count;
 f->output_ranges=output_ranges;f->output_range_index=output_index;f->output_range_count=output_count;
 f->control_ranges=control_ranges;f->control_range_index=control_index;f->control_range_count=control_count;
 f->bq_schedule_original=s;
 f->origin_original=inputs->deadline->origin;f->origin_tick_original=f->origin_original->tick;
 f->expires_original=inputs->deadline->expires;
 f->called[0]=1;f->results[0]=slcli_writer_create_windows_bq_original(a,creator,output,inputs);f->returned[0]=1;
 if(!f->results[0]){f->first_failure=1;return 0;}
 f->called[1]=1;f->results[1]=slcli_writer_duplicate_windows_readback_original(readback,a,creator,
  inputs->deadline,inputs->caller,inputs->caller_bytes,&creator->evidence_birth_clock);f->returned[1]=1;
 if(!f->results[1]){f->first_failure=2;return 0;}
 f->called[2]=1;f->results[2]=slcli_writer_capture_backing_windows_prepare_bq_original(backing,a,readback,
  s->owner,&s->request,s->file_bytes,s->plan,s->count,s);f->returned[2]=1;
 if(!f->results[2]){f->first_failure=3;return 0;}
 f->called[3]=1;f->results[3]=slcli_writer_capture_view_windows_prepare_batch_original(backing);f->returned[3]=1;
 if(!f->results[3]){f->first_failure=4;return 0;}
 f->called[4]=1;f->results[4]=slcli_writer_capture_ranges_windows_prepare_payload_original(ranges,a,
  s->plan[range_index].capture,range_count,payload);f->returned[4]=1;
 if(!f->results[4]){f->first_failure=5;return 0;}
 /* Second unit is prepared only after the first's actual finishing frontier. */
 f->called[5]=1;f->results[5]=slcli_writer_capture_ranges_windows_prepare_payload_original(output_ranges,a,
  s->plan[output_index].capture,output_count,payload);f->returned[5]=1;
 if(!f->results[5]){f->first_failure=6;return 0;}
 f->called[6]=1;f->results[6]=slcli_writer_capture_ranges_windows_prepare_payload_original(control_ranges,a,
  s->plan[control_index].capture,control_count,payload);f->returned[6]=1;
 if(!f->results[6]){f->first_failure=7;return 0;}
 if(output_ranges->before.tick<ranges->after.tick||!factory_bq_raw_matches(f)){f->first_failure=8;return 0;}
 f->finish_result=slcli_native_clock_read(&f->finish);f->finish_returned=1;
 if(!f->finish_result||f->finish.tick<control_ranges->after.tick||f->finish.tick>=f->expires_original||
  inputs->deadline->origin!=f->origin_original||f->origin_original->tick!=f->origin_tick_original||
  inputs->deadline->expires!=f->expires_original||
  !slcli_evidence_deadline_precede(creator->evidence_file,&f->finish)){f->first_failure=9;return 0;}
 f->complete=1;f->failed=0;return 1;
}
int slcli_writer_capture_factory_windows_bq_matches_original(const struct slcli_writer_capture_factory_windows *f){
 return factory_bq_raw_matches(f)&&f->complete&&!f->failed&&!f->first_failure&&
  f->finish_returned&&f->finish_result&&f->finish.complete&&f->finish.tick>=f->control_ranges->after.tick&&
  f->finish.tick<f->expires_original;
}
int slcli_writer_capture_backing_windows_matches_original(
 const struct slcli_writer_capture_backing_windows *c){
 size_t i,j;uint64_t previous,total=0;
 if(!same(c)||!c->complete||c->failed||!c->backing_registered||
  !c->sections[0]||!c->sections[1]||c->sections[0]==c->sections[1]||
  c->system_output!=(uintptr_t)&c->system||c->system.dwPageSize!=c->owner->native_page_size||
  !c->system.dwPageSize||!c->system.dwAllocationGranularity||
  c->system.dwAllocationGranularity%c->system.dwPageSize)return 0;
 previous=c->readback->observe_after.tick;
 for(i=0;i<4;i++){
  if(!c->called[i]||!c->returned[i])return 0;
  for(j=0;j<2;j++){
   if(!c->clock_returned[i][j]||!c->clock_result[i][j]||!c->clocks[i][j].complete||
    c->clocks[i][j].tick<previous||c->clocks[i][j].tick>=c->readback->deadline->expires)return 0;
   previous=c->clocks[i][j].tick;
  }
 }
 for(i=0;i<c->count;i++){
  const struct slcli_writer_capture_view_plan_windows *p=&c->plan[i];
  if(!p->bytes||p->bytes>=c->file_bytes||p->bytes>SIZE_MAX||p->file_offset>c->file_bytes-p->bytes||
   p->file_offset%c->system.dwAllocationGranularity||p->bytes%c->system.dwPageSize||
   !slcli_arena_live_block(c->arena,p->capture,sizeof(*p->capture))||
   !apart(c,sizeof(*c),p->capture,sizeof(*p->capture))||
   !apart(c->plan,c->count*sizeof(*c->plan),p->capture,sizeof(*p->capture))||
   total>c->file_bytes-p->bytes)return 0;
  for(j=0;j<i;j++)if(!apart(p->capture,sizeof(*p->capture),c->plan[j].capture,sizeof(*p->capture))||
   (p->file_offset<c->plan[j].file_offset+c->plan[j].bytes&&
    c->plan[j].file_offset<p->file_offset+p->bytes))return 0;
  total+=p->bytes;
 }
 if(total>=c->file_bytes||total!=c->mapped_bytes)return 0;
 for(i=0;i<2;i++)if(c->section_args[i][0]!=(uintptr_t)(i?c->readback->original:c->readback->creator->original)||
  c->section_args[i][1]||c->section_args[i][2]!=(i?PAGE_READONLY:PAGE_READWRITE)||
  c->section_args[i][3]!=(i?0:(DWORD)(c->file_bytes>>32))||
  c->section_args[i][4]!=(i?0:(DWORD)c->file_bytes)||c->section_args[i][5])return 0;
 return c->previous_tick==previous&&c->file_after_result&&c->file_after.completed&&
  c->file_after.original==c->readback->original&&c->file_after.tick>=c->clocks[3][0].tick&&
  c->file_after.tick<=c->clocks[3][1].tick&&c->file_after.standard.EndOfFile.QuadPart>=0&&
  (uint64_t)c->file_after.standard.EndOfFile.QuadPart==c->file_bytes&&
  c->file_after.identity.VolumeSerialNumber==c->readback->observed.identity.VolumeSerialNumber&&
  slcli_equal(c->file_after.identity.FileId.Identifier,c->readback->observed.identity.FileId.Identifier,16);
}
#endif
