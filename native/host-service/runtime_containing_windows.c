#include "runtime_containing_windows.h"
#if defined(_WIN32)
int slcli_runtime_source_schedule_windows_derive(size_t namespace_rows,
 uint64_t image_bytes,size_t workspace_bytes,size_t regions,
 struct slcli_runtime_source_schedule_windows *out){
 struct slcli_runtime_source_schedule_windows g={0};size_t counts[5],*offsets[5],i,total=0;
 if(!out||namespace_rows<2||!regions||regions>(SIZE_MAX-2)/3||
  !slcli_source_stream_geometry_windows_derive(image_bytes,workspace_bytes,&g.stream))return 0;
 g.namespace_rows=namespace_rows;g.file_rows=11;g.stream_rows=g.stream.references;
 g.containing_rows=5;g.route_rows=regions*3+2;
 counts[0]=g.namespace_rows;counts[1]=g.file_rows;counts[2]=g.stream_rows;
 counts[3]=g.containing_rows;counts[4]=g.route_rows;
 offsets[0]=&g.namespace_offset;offsets[1]=&g.file_offset;offsets[2]=&g.stream_offset;
 offsets[3]=&g.containing_offset;offsets[4]=&g.route_offset;
 for(i=0;i<5;i++){if(counts[i]>SIZE_MAX-total)return 0;*offsets[i]=total;total+=counts[i];}
 /* The current alias wire uses UINT16_MAX as absent. The selected finite-R
  * profile also excludes that value; this is representation, not a budget. */
 if(total>65534u||total>SIZE_MAX/sizeof(struct slcli_native_reference)||
  total>SIZE_MAX/sizeof(uint32_t))return 0;
 g.total_rows=total;g.plan_bytes=total*sizeof(struct slcli_native_reference);
 g.id_bytes=total*sizeof(uint32_t);g.ordinal_bytes=g.id_bytes;*out=g;return 1;
}
enum { CONTAIN_ALLOCATE=1,CONTAIN_LAYOUT,CONTAIN_SECTION,CONTAIN_DUPLICATE,
 CONTAIN_INITIALIZER_BIND,CONTAIN_INITIALIZER_MAP,CONTAIN_INITIALIZE,
 CONTAIN_UNMAP,CONTAIN_UNMAP_CLOCK,CONTAIN_ALIAS_BIND,CONTAIN_ALIAS_MAP,CONTAIN_VALIDATE,CONTAIN_ROUTE_PREPARE,
 CONTAIN_FINAL_CLOCK };
static int separate(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int failed(struct slcli_runtime_containing_windows *c,unsigned reason) {
 if(!c->first_failure)c->first_failure=reason;return 0;
}
static int fresh(const struct slcli_native_reference *r,unsigned kind,uint32_t extent) {
 size_t i;
 if(!r||r->native_rights)return 0;
 for(i=0;i<32;i++)if(r->identity[i])return 0;
 return r->kind==kind&&r->version&&r->effect_flags==1&&r->state==2&&!r->handle&&
  !r->native_original&&!r->service_request_original&&
  r->native_class==SLCLI_NATIVE_ORIGINAL_UNCONSTRUCTED&&r->physical_extent==extent;
}
static int containing_claims_original(const struct slcli_runtime_containing_windows *c,struct slcli_arena *a){
 const struct slcli_runtime_containing_windows_claim_inputs *in=c->claims_original;
 const struct slcli_binding_storage_original *s;const struct slcli_binding_originals *b=c->binding_original;
 const struct slcli_binding_row_claim_original *claim;struct slcli_binding_storage_geometry g;
 const void *captures[5],*containers[5];size_t bytes[5],containing[5],i;
 uint32_t ordinals[5];enum slcli_native_original_class tags[5]={SLCLI_NATIVE_ORIGINAL_SECTION_BACKING,
  SLCLI_NATIVE_ORIGINAL_SECTION_DUPLICATE,SLCLI_NATIVE_ORIGINAL_CAPTURE_INITIALIZER_VIEW,
  SLCLI_NATIVE_ORIGINAL_CAPTURE_ALIAS,SLCLI_NATIVE_ORIGINAL_SUSPENDED_CHILD_VIEW};
 if(!in)return 1;
 if(!slcli_arena_live_block(a,in,sizeof(*in))||!(s=in->storage)||
  !slcli_arena_live_block(a,s,sizeof(*s))||!s->prepared||s->arena!=a||s->book!=b||
  b->storage_original!=s||s->attempted!=b->count||s->attempted>s->capacity||
  !slcli_binding_storage_geometry_derive(s->capacity,&g)||s->references!=b->references||s->ids!=b->original_ids||
  !slcli_arena_live_block(a,s->references,g.references_bytes)||!slcli_arena_live_block(a,s->ids,g.ids_bytes)||
  !slcli_arena_live_block(a,s->claims,g.claims_bytes)||
  !slcli_arena_live_block(a,in->plans,5*sizeof(*in->plans))||!slcli_arena_live_block(a,in->ids,5*sizeof(*in->ids)))return 0;
 captures[0]=captures[1]=containers[0]=containers[1]=c->backing;
 bytes[0]=bytes[1]=containing[0]=containing[1]=sizeof(*c->backing);
 captures[2]=containers[2]=c->initializer;bytes[2]=containing[2]=sizeof(*c->initializer);
 captures[3]=containers[3]=c->alias;bytes[3]=containing[3]=sizeof(*c->alias);
 captures[4]=&c->child;containers[4]=c;bytes[4]=sizeof(c->child);containing[4]=sizeof(*c);
 ordinals[0]=c->original.section_ordinal;ordinals[1]=c->original.duplicate_ordinal;
 ordinals[2]=c->original.initializer_ordinal;ordinals[3]=c->original.alias_ordinal;ordinals[4]=c->original.child_view_ordinal;
 for(i=0;i<5;i++){
  if(ordinals[i]>=s->attempted||!slcli_arena_live_block(a,containers[i],containing[i]))return 0;
  claim=&s->claims[ordinals[i]];
  if(!claim->claimed||claim->ordinal!=ordinals[i]||claim->capture!=captures[i]||claim->capture_bytes!=bytes[i]||
   claim->containing_block!=containers[i]||claim->containing_bytes!=containing[i]||
   claim->capture_offset!=(uintptr_t)captures[i]-(uintptr_t)containers[i]||claim->native_class!=tags[i]||
   b->original_ids[ordinals[i]]!=in->ids[i]||b->references[ordinals[i]].version!=in->plans[i].version)return 0;
 }
 return 1;
}
static int containing_prepare_original(
 struct slcli_runtime_containing_windows *c,struct slcli_arena *a,
 const struct slcli_binding_originals *b,struct slcli_memory_owner *runtime,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const struct slcli_runtime_containing_windows_inputs *in,
 const struct slcli_runtime_containing_windows_claim_inputs *claims) {
 struct slcli_native_reference *section,*duplicate,*initializer,*alias,*child;
 const struct slcli_native_reference *ps,*pd,*pi,*pa,*pc;
 struct slcli_binding_storage_original *storage=NULL;
 size_t i;const struct slcli_native_clock *next;
 if(!a||!c||!slcli_arena_live_block(a,c,sizeof(*c))||!in||
  !slcli_arena_live_block(a,in,sizeof(*in))||!b||!slcli_arena_live_range(a,b,sizeof(*b))||
  !b->count||b->count>SIZE_MAX/sizeof(*b->references)||!b->references||
  !slcli_arena_live_range(a,b->references,b->count*sizeof(*b->references))||
  b->deadline_original!=d||!slcli_source_deadline_original_matches(a,d,caller)||
  !caller_bytes||!slcli_arena_live_block(a,caller,caller_bytes)||!prior||
  !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  prior->tick<d->origin->tick||prior->tick>=d->expires||!runtime||runtime==a->owner||
  !slcli_arena_live_block(a,runtime,sizeof(*runtime))||!slcli_memory_owner_writable(runtime)||
  runtime->count>=256||!separate(c,sizeof(*c),in,sizeof(*in))||
  !separate(c,sizeof(*c),b->references,b->count*sizeof(*b->references))||
  !separate(c,sizeof(*c),runtime,sizeof(*runtime))||!separate(c,sizeof(*c),caller,caller_bytes)||
  !separate(c,sizeof(*c),prior,sizeof(*prior))||!slcli_source_deadline_away(d,c,sizeof(*c))||
  !in->descriptor_bytes||!slcli_arena_live_range(a,in->descriptor,in->descriptor_bytes)||
  !separate(c,sizeof(*c),in->descriptor,in->descriptor_bytes)||
  !slcli_source_file_windows_matches_original(in->source_file,a)||
  !slcli_source_stream_windows_matches_original(in->source_stream,a,in->source_file)||
  in->source_file->book!=b||in->source_file->deadline!=d||in->source_file->caller!=caller||
  prior->tick<in->source_stream->identity_after[1].tick||
  !separate(c,sizeof(*c),in->source_file,sizeof(*in->source_file))||
  !separate(c,sizeof(*c),b,sizeof(*b)))return 0;
 for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
 if(claims){
  if(!slcli_arena_live_block(a,claims,sizeof(*claims))||
   !separate(c,sizeof(*c),claims,sizeof(*claims))||!(storage=claims->storage)||
   !slcli_arena_live_block(a,storage,sizeof(*storage))||!storage->prepared||storage->arena!=a||
   storage->book!=b||b->storage_original!=storage||storage->attempted!=b->count||
   storage->attempted>storage->capacity||5>storage->capacity-storage->attempted||
   in->source_file->storage_original!=storage||in->source_stream->storage_original!=storage||
   !slcli_arena_live_block(a,claims->plans,5*sizeof(*claims->plans))||
   !slcli_arena_live_block(a,claims->ids,5*sizeof(*claims->ids))||
   !separate(c,sizeof(*c),claims->plans,5*sizeof(*claims->plans))||
   !separate(c,sizeof(*c),claims->ids,5*sizeof(*claims->ids))||
   in->section_ordinal!=storage->attempted||in->duplicate_ordinal!=storage->attempted+1||
   in->initializer_ordinal!=storage->attempted+2||in->alias_ordinal!=storage->attempted+3||
   in->child_view_ordinal!=storage->attempted+4)return 0;
 }
 c->started=1;c->inputs_original=in;c->original=*in;c->binding_original=b;
 c->claims_original=claims;
 c->runtime_owner_original=runtime;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;c->prior_original=prior;
 if(!in->threads||in->events<5||!in->regions||!in->modules||!in->name_units||!in->runtime_bytes||
  !in->descriptor_bytes||!slcli_arena_live_range(a,in->descriptor,in->descriptor_bytes)||
  (!claims&&(in->section_ordinal>=b->count||in->duplicate_ordinal>=b->count||
  in->initializer_ordinal>=b->count||in->alias_ordinal>=b->count||
  in->child_view_ordinal>=b->count))||in->section_ordinal>=in->child_view_ordinal||
  in->child_view_ordinal==in->duplicate_ordinal||in->child_view_ordinal==in->initializer_ordinal||
  in->child_view_ordinal==in->alias_ordinal||
  in->section_ordinal>=0xffffu||in->section_ordinal>=in->duplicate_ordinal||
  in->section_ordinal>=in->initializer_ordinal||in->section_ordinal>=in->alias_ordinal||
  in->duplicate_ordinal==in->initializer_ordinal||in->duplicate_ordinal==in->alias_ordinal||
  in->initializer_ordinal==in->alias_ordinal)return failed(c,CONTAIN_LAYOUT);
 ps=claims?&claims->plans[0]:&b->references[in->section_ordinal];
 pd=claims?&claims->plans[1]:&b->references[in->duplicate_ordinal];
 pi=claims?&claims->plans[2]:&b->references[in->initializer_ordinal];
 pa=claims?&claims->plans[3]:&b->references[in->alias_ordinal];
 pc=claims?&claims->plans[4]:&b->references[in->child_view_ordinal];
 if(!fresh(ps,22,runtime->count)||!fresh(pd,22,runtime->count)||
  !fresh(pi,9,runtime->count)||!fresh(pa,9,runtime->count)||!fresh(pc,9,runtime->count)||
  ps->alias_of!=UINT16_MAX||pd->alias_of!=in->section_ordinal||
  pi->alias_of!=in->section_ordinal||pa->alias_of!=in->section_ordinal||
  pc->alias_of!=in->section_ordinal||pc->state!=2)return failed(c,CONTAIN_LAYOUT);
 /* Allocate all actual parent captures before any section/view effect. Every
  * partial allocation remains linked from this original once-only root. */
 c->layout=slcli_arena_calloc(a,1,sizeof(*c->layout));if(!c->layout)return failed(c,CONTAIN_ALLOCATE);
 c->backing=slcli_arena_calloc(a,1,sizeof(*c->backing));if(!c->backing)return failed(c,CONTAIN_ALLOCATE);
 c->initializer=slcli_arena_calloc(a,1,sizeof(*c->initializer));if(!c->initializer)return failed(c,CONTAIN_ALLOCATE);
 c->alias=slcli_arena_calloc(a,1,sizeof(*c->alias));if(!c->alias)return failed(c,CONTAIN_ALLOCATE);
 if(claims){
  if(!slcli_binding_storage_claim_pre_effect_original(storage,ps,claims->ids[0],c->backing,sizeof(*c->backing),
    c->backing,sizeof(*c->backing),SLCLI_NATIVE_ORIGINAL_SECTION_BACKING)||
   !slcli_binding_storage_claim_pre_effect_original(storage,pd,claims->ids[1],c->backing,sizeof(*c->backing),
    c->backing,sizeof(*c->backing),SLCLI_NATIVE_ORIGINAL_SECTION_DUPLICATE)||
   !slcli_binding_storage_claim_pre_effect_original(storage,pi,claims->ids[2],c->initializer,sizeof(*c->initializer),
    c->initializer,sizeof(*c->initializer),SLCLI_NATIVE_ORIGINAL_CAPTURE_INITIALIZER_VIEW)||
   !slcli_binding_storage_claim_pre_effect_original(storage,pa,claims->ids[3],c->alias,sizeof(*c->alias),
    c->alias,sizeof(*c->alias),SLCLI_NATIVE_ORIGINAL_CAPTURE_ALIAS)||
   !slcli_binding_storage_claim_pre_effect_original(storage,pc,claims->ids[4],c,sizeof(*c),
    &c->child,sizeof(c->child),SLCLI_NATIVE_ORIGINAL_SUSPENDED_CHILD_VIEW))return failed(c,CONTAIN_ALLOCATE);
 }
 section=&b->references[in->section_ordinal];duplicate=&b->references[in->duplicate_ordinal];
 initializer=&b->references[in->initializer_ordinal];alias=&b->references[in->alias_ordinal];
 child=&b->references[in->child_view_ordinal];
 c->child.root_original=c;child->native_original=&c->child;
 child->native_class=SLCLI_NATIVE_ORIGINAL_SUSPENDED_CHILD_VIEW;c->child.enrolled=1;
 if(!containing_claims_original(c,a))return failed(c,CONTAIN_LAYOUT);
 c->layout_result=slcli_node_capture_layout_windows_prepare_original(c->layout,a,d,caller,caller_bytes,prior,
  c->original.threads,c->original.events,c->original.regions,c->original.modules,c->original.name_units);
 c->layout_returned=1;
 if(!c->layout_result||c->original.runtime_bytes<c->layout->minimum_runtime_bytes)return failed(c,CONTAIN_LAYOUT);
 c->route_prepare_result=claims?slcli_route_write_windows_prepare_claimed_original(&c->route_writer,c,a,
  c->original.source_file,c->original.route_buffer_ordinals,c->original.route_buffer_count,storage,claims->route_plans,claims->route_ids):
  slcli_route_write_windows_prepare_original(&c->route_writer,c,a,
  c->original.source_file,c->original.route_buffer_ordinals,c->original.route_buffer_count);
 c->route_prepare_returned=1;if(!c->route_prepare_result)return failed(c,CONTAIN_ROUTE_PREPARE);
 if(!containing_claims_original(c,a))return failed(c,CONTAIN_SECTION);
 c->section_result=slcli_preentry_section_windows_create_enrolled_original(c->backing,a,b,section,runtime,d,
  caller,caller_bytes,&c->layout->after,c->original.runtime_bytes,c->original.descriptor,c->original.descriptor_bytes);
 c->section_returned=1;if(!c->section_result)return failed(c,CONTAIN_SECTION);
 if(!containing_claims_original(c,a))return failed(c,CONTAIN_DUPLICATE);
 c->duplicate_result=slcli_preentry_section_windows_duplicate_enrolled_original(c->backing,a,b,section,duplicate,&c->backing->after[1]);
 c->duplicate_returned=1;if(!c->duplicate_result)return failed(c,CONTAIN_DUPLICATE);
 if(!containing_claims_original(c,a))return failed(c,CONTAIN_INITIALIZER_BIND);
 c->initializer_bind_result=slcli_capture_initializer_windows_bind_original(c->initializer,a,b,section,initializer,c->backing);
 c->initializer_bind_returned=1;if(!c->initializer_bind_result)return failed(c,CONTAIN_INITIALIZER_BIND);
 if(!containing_claims_original(c,a))return failed(c,CONTAIN_INITIALIZER_MAP);
 c->initializer_map_result=slcli_capture_initializer_windows_map_original(c->initializer,a,b,section,initializer,
  c->backing,d,caller,caller_bytes,&c->backing->duplicate_after,c->layout->capture_bytes);
 c->initializer_map_returned=1;
 if(!c->initializer_map_result){failed(c,CONTAIN_INITIALIZER_MAP);goto dispose_initializer;}
 if(!containing_claims_original(c,a)){failed(c,CONTAIN_INITIALIZE);goto dispose_initializer;}
 c->initialize_result=slcli_node_capture_layout_windows_initialize_original(c->layout,a,c->initializer);
 c->initialize_returned=1;if(!c->initialize_result)failed(c,CONTAIN_INITIALIZE);
 if(!containing_claims_original(c,a))failed(c,CONTAIN_INITIALIZE);
dispose_initializer:
 if(c->initializer->map_returned&&c->initializer->view) {
  c->unmap_result=slcli_capture_initializer_windows_unmap_original(c->initializer,a);
  c->unmap_returned=1;if(!c->unmap_result)failed(c,CONTAIN_UNMAP);
 }
 if(c->first_failure)return 0;
 if(!c->unmap_returned||!c->unmap_result||!c->initializer->unmap_before_result||
  c->initializer->unmap_before.tick<c->initializer->initialize_after.tick||
  !c->initializer->unmap_after_result||c->initializer->unmap_after.tick<c->initializer->unmap_before.tick||
  c->initializer->unmap_after.tick>=c->layout->expires)return failed(c,CONTAIN_UNMAP_CLOCK);
 next=&c->initializer->unmap_after;
 if(!containing_claims_original(c,a))return failed(c,CONTAIN_ALIAS_BIND);
 c->alias_bind_result=slcli_capture_alias_windows_bind_original(c->alias,a,b,section,alias,c->backing);
 c->alias_bind_returned=1;if(!c->alias_bind_result)return failed(c,CONTAIN_ALIAS_BIND);
 c->alias_map_result=slcli_capture_alias_windows_map_original(c->alias,a,b,section,alias,
  c->backing,d,caller,caller_bytes,next,c->layout->capture_bytes);
 c->alias_map_returned=1;if(!c->alias_map_result)return failed(c,CONTAIN_ALIAS_MAP);
 if(!containing_claims_original(c,a))return failed(c,CONTAIN_VALIDATE);
 c->validate_result=slcli_node_capture_layout_windows_validate_original(c->layout,a,c->alias);
 c->validate_returned=1;if(!c->validate_result||!containing_claims_original(c,a))return failed(c,CONTAIN_VALIDATE);
 c->constructor_after_result=slcli_native_clock_read(&c->constructor_after);c->constructor_after_returned=1;
 if(!c->constructor_after_result||!c->alias->validate_after.complete||
  c->constructor_after.tick<c->alias->validate_after.tick||c->constructor_after.tick>=d->expires)
  return failed(c,CONTAIN_FINAL_CLOCK);
 c->capture_prepared=1;return 1;
}
int slcli_runtime_containing_windows_prepare_original(
 struct slcli_runtime_containing_windows *c,struct slcli_arena *a,
 const struct slcli_binding_originals *b,struct slcli_memory_owner *runtime,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const struct slcli_runtime_containing_windows_inputs *in){
 return containing_prepare_original(c,a,b,runtime,d,caller,caller_bytes,prior,in,NULL);
}
int slcli_runtime_containing_windows_prepare_claimed_original(
 struct slcli_runtime_containing_windows *c,struct slcli_arena *a,
 const struct slcli_binding_originals *b,struct slcli_memory_owner *runtime,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const struct slcli_runtime_containing_windows_inputs *in,
 const struct slcli_runtime_containing_windows_claim_inputs *claims){
 if(!claims)return 0;return containing_prepare_original(c,a,b,runtime,d,caller,caller_bytes,prior,in,claims);
}
static int source_failure(struct slcli_runtime_source_windows *c,uint32_t reason){
 if(!c->first_failure)c->first_failure=reason;return 0;
}
static int source_acquire_original(struct slcli_runtime_source_windows *c,
 struct slcli_arena *a,const struct slcli_binding_originals *b,struct slcli_memory_owner *runtime,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const struct slcli_runtime_source_windows_inputs *in,
 const struct slcli_runtime_source_windows_claim_inputs *claims){
 struct slcli_source_stream_geometry_windows g;struct slcli_runtime_source_schedule_windows schedule;
 size_t i,j,k,total;uint32_t ordinal,other;
 const uint32_t *sets[3],*ids[3];size_t lengths[3],next_ordinal=0,route_count;
 uint32_t previous_id=0;int have_id=0;
 const struct slcli_native_reference *r,*plans[3];struct slcli_binding_storage_original *storage=NULL;
 struct slcli_binding_storage_geometry reserved;
 if(!c||!a||!slcli_arena_live_block(a,c,sizeof(*c))||!in||
  !slcli_arena_live_block(a,in,sizeof(*in))||!b||!slcli_arena_live_range(a,b,sizeof(*b))||
  (!claims&&!b->count)||!b->references||b->count>SIZE_MAX/sizeof(*r)||
  (!claims&&!slcli_arena_live_range(a,b->references,b->count*sizeof(*r)))||b->deadline_original!=d||
  !slcli_source_deadline_original_matches(a,d,caller)||!caller_bytes||
  !slcli_arena_live_block(a,caller,caller_bytes)||!prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||
  !prior->complete||prior->tick<d->origin->tick||prior->tick>=d->expires||
  !slcli_runtime_source_schedule_windows_derive(in->namespace_count,in->image_file_bytes,
   in->workspace_bytes,in->containing.regions,&schedule)||
  in->stream_count!=schedule.stream_rows||
  in->containing.route_buffer_count!=schedule.route_rows||
  in->namespace_count>SIZE_MAX/sizeof(uint32_t)||
  !in->path||!slcli_arena_live_range(a,in->path,in->path_bytes)||
  !in->namespace_ordinals||!slcli_arena_live_range(a,in->namespace_ordinals,in->namespace_count*sizeof(uint32_t))||
  !in->file_ordinals||!slcli_arena_live_range(a,in->file_ordinals,11*sizeof(uint32_t))||
  !in->stream_ordinals||!slcli_arena_live_range(a,in->stream_ordinals,schedule.stream.ordinal_bytes)||
  in->containing.source_file||in->containing.source_stream||
  !separate(c,sizeof(*c),in,sizeof(*in))||!separate(c,sizeof(*c),b,sizeof(*b))||
  (!claims&&!separate(c,sizeof(*c),b->references,b->count*sizeof(*r)))||
  !separate(c,sizeof(*c),caller,caller_bytes)||!separate(c,sizeof(*c),prior,sizeof(*prior))||
  !separate(c,sizeof(*c),in->path,in->path_bytes)||
  !separate(c,sizeof(*c),in->namespace_ordinals,in->namespace_count*sizeof(uint32_t))||
  !separate(c,sizeof(*c),in->file_ordinals,11*sizeof(uint32_t))||
  !separate(c,sizeof(*c),in->stream_ordinals,schedule.stream.ordinal_bytes)||
  !slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
 g=schedule.stream;
 for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
 sets[0]=in->namespace_ordinals;sets[1]=in->file_ordinals;sets[2]=in->stream_ordinals;
 lengths[0]=in->namespace_count;lengths[1]=11;lengths[2]=in->stream_count;
 if(claims){
  if(!slcli_arena_live_block(a,claims,sizeof(*claims))||!separate(c,sizeof(*c),claims,sizeof(*claims))||
   !(storage=claims->storage)||!slcli_arena_live_block(a,storage,sizeof(*storage))||
   !storage->prepared||storage->arena!=a||storage->book!=b||b->storage_original!=storage||
   storage->attempted!=b->count||storage->attempted>storage->capacity||
   schedule.total_rows>storage->capacity-storage->attempted||
   !slcli_binding_storage_geometry_derive(storage->capacity,&reserved)||storage->references!=b->references||storage->ids!=b->original_ids||
   !slcli_arena_live_block(a,b->references,reserved.references_bytes)||
   !slcli_arena_live_block(a,b->original_ids,reserved.ids_bytes)||
   !separate(c,sizeof(*c),b->references,reserved.references_bytes)||
   !claims->containing||!slcli_arena_live_block(a,claims->containing,sizeof(*claims->containing))||
   claims->containing->storage!=storage)return 0;
  plans[0]=claims->namespace_plans;plans[1]=claims->file_plans;plans[2]=claims->stream_plans;
  ids[0]=claims->namespace_ids;ids[1]=claims->file_ids;ids[2]=claims->stream_ids;
  next_ordinal=storage->attempted;
  if(next_ordinal){previous_id=b->original_ids[next_ordinal-1];have_id=1;}
  for(i=0;i<3;i++){
   if(lengths[i]>storage->capacity-next_ordinal||lengths[i]>SIZE_MAX/sizeof(*r)||
    !slcli_arena_live_block(a,plans[i],lengths[i]*sizeof(*r))||
    !slcli_arena_live_block(a,ids[i],lengths[i]*sizeof(uint32_t))||
    !separate(c,sizeof(*c),plans[i],lengths[i]*sizeof(*r))||
    !separate(c,sizeof(*c),ids[i],lengths[i]*sizeof(uint32_t)))return 0;
   for(j=0;j<lengths[i];j++){
    if(sets[i][j]!=next_ordinal+j||(have_id&&ids[i][j]<=previous_id))return 0;
    previous_id=ids[i][j];have_id=1;
   }
   next_ordinal+=lengths[i];
  }
  if(5>storage->capacity-next_ordinal||in->containing.section_ordinal!=next_ordinal||
   in->containing.duplicate_ordinal!=next_ordinal+1||in->containing.initializer_ordinal!=next_ordinal+2||
   in->containing.alias_ordinal!=next_ordinal+3||in->containing.child_view_ordinal!=next_ordinal+4||
   !runtime||!slcli_arena_live_block(a,runtime,sizeof(*runtime))||runtime->count>=256||
   !slcli_arena_live_block(a,claims->containing->plans,5*sizeof(*r))||
   !slcli_arena_live_block(a,claims->containing->ids,5*sizeof(uint32_t)))return 0;
  for(i=0;i<5;i++){
   r=&claims->containing->plans[i];
   if(!fresh(r,i<2?22:9,runtime->count)||r->alias_of!=(i?next_ordinal:UINT16_MAX)||
    (have_id&&claims->containing->ids[i]<=previous_id))return 0;
   previous_id=claims->containing->ids[i];have_id=1;
  }
  next_ordinal+=5;
  if(!in->containing.regions||in->containing.regions>(SIZE_MAX-2)/3)return 0;
  route_count=in->containing.regions*3+2;
  if(route_count!=in->containing.route_buffer_count||route_count>storage->capacity-next_ordinal||
   route_count>SIZE_MAX/sizeof(*r)||route_count>SIZE_MAX/sizeof(uint32_t)||
   !slcli_arena_live_block(a,in->containing.route_buffer_ordinals,route_count*sizeof(uint32_t))||
   !slcli_arena_live_block(a,claims->containing->route_plans,route_count*sizeof(*r))||
   !slcli_arena_live_block(a,claims->containing->route_ids,route_count*sizeof(uint32_t)))return 0;
  for(i=0;i<route_count;i++){
   r=&claims->containing->route_plans[i];
   if(in->containing.route_buffer_ordinals[i]!=next_ordinal+i||!fresh(r,6,a->extent)||
    r->alias_of!=UINT16_MAX||(have_id&&claims->containing->route_ids[i]<=previous_id))return 0;
   previous_id=claims->containing->route_ids[i];have_id=1;
  }
 }
 /* Full source plan rows are reserved and distinct before the first source
  * effect. Other whole-runtime rows/fit still belong to the owning admission. */
 for(i=0;i<3;i++)for(j=0;j<lengths[i];j++){
  ordinal=sets[i][j];if(ordinal>=(claims?storage->capacity:b->count))return 0;
  for(k=0;k<=i;k++)for(total=0;total<(k==i?j:lengths[k]);total++){
   other=sets[k][total];if(ordinal==other)return 0;
  }
  r=claims?&plans[i][j]:&b->references[ordinal];
  if(!fresh(r,r->kind,r->physical_extent))return 0;
  if(i==0){if(r->kind!=(j+1==lengths[i]?4:21)||r->alias_of!=UINT16_MAX||r->physical_extent!=UINT16_MAX)return 0;}
  else if(i==1){
   if(r->kind!=(j?(j%2?6:5):4)||r->physical_extent!=(j?a->extent:UINT16_MAX)||
    r->alias_of!=(j?UINT16_MAX:sets[0][lengths[0]-1]))return 0;
  }else if(r->kind!=(j?5:6)||r->alias_of!=UINT16_MAX||r->physical_extent!=a->extent)return 0;
 }
 if(sets[0][lengths[0]-1]>=UINT16_MAX||sets[0][lengths[0]-1]>=sets[1][0])return 0;
 c->started=1;c->inputs_original=in;c->book=b;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;c->stream_geometry=g;
 c->claims_original=claims;c->schedule=schedule;
 c->before_result=slcli_native_clock_read(&c->before);
 if(!c->before_result||c->before.tick<prior->tick||c->before.tick>=d->expires)return source_failure(c,1);
 c->namespace_capture=slcli_arena_calloc(a,1,sizeof(*c->namespace_capture));if(!c->namespace_capture)return source_failure(c,2);
 c->file=slcli_arena_calloc(a,1,sizeof(*c->file));if(!c->file)return source_failure(c,2);
 c->stream=slcli_arena_calloc(a,1,sizeof(*c->stream));if(!c->stream)return source_failure(c,2);
 c->containing=slcli_arena_calloc(a,1,sizeof(*c->containing));if(!c->containing)return source_failure(c,2);
 c->containing_inputs=slcli_arena_calloc(a,1,sizeof(*c->containing_inputs));if(!c->containing_inputs)return source_failure(c,2);
 *c->containing_inputs=in->containing;
 c->allocated_after_result=slcli_native_clock_read(&c->allocated_after);
 if(!c->allocated_after_result||c->allocated_after.tick<c->before.tick||c->allocated_after.tick>=d->expires)return source_failure(c,3);
 c->namespace_result=claims?slcli_installed_namespace_windows_acquire_claimed_original(c->namespace_capture,a,storage,d,caller,caller_bytes,
  &c->allocated_after,in->path,in->path_bytes,in->namespace_ordinals,in->namespace_count,claims->namespace_plans,claims->namespace_ids):
  slcli_installed_namespace_windows_acquire_original(c->namespace_capture,a,b,d,caller,caller_bytes,
  &c->allocated_after,in->path,in->path_bytes,in->namespace_ordinals,in->namespace_count);
 c->namespace_returned=1;if(!c->namespace_result)return source_failure(c,4);
 c->file_result=claims?slcli_source_file_windows_probe_held_claimed_original(c->file,a,storage,d,caller,caller_bytes,
  &c->namespace_capture->components[c->namespace_capture->count-1].query_after,c->namespace_capture,in->file_ordinals,claims->file_plans,claims->file_ids):
  slcli_source_file_windows_probe_held_original(c->file,a,b,d,caller,caller_bytes,
  &c->namespace_capture->components[c->namespace_capture->count-1].query_after,c->namespace_capture,in->file_ordinals);
 c->file_returned=1;if(!c->file_result)return source_failure(c,5);
 if((uint64_t)c->file->bytes.QuadPart!=in->image_file_bytes)return source_failure(c,6);
 c->stream_result=claims?slcli_source_stream_windows_claimed_original(c->stream,a,c->file,&c->file->after[3],
  in->workspace_bytes,in->stream_ordinals,in->stream_count,storage,claims->stream_plans,claims->stream_ids):
  slcli_source_stream_windows_original(c->stream,a,c->file,&c->file->after[3],in->workspace_bytes,in->stream_ordinals,in->stream_count);
 c->stream_returned=1;if(!c->stream_result)return source_failure(c,7);
 c->containing_inputs->source_file=c->file;c->containing_inputs->source_stream=c->stream;
 c->containing_result=claims?slcli_runtime_containing_windows_prepare_claimed_original(c->containing,a,b,runtime,d,caller,caller_bytes,
  &c->stream->identity_after[1],c->containing_inputs,claims->containing):
  slcli_runtime_containing_windows_prepare_original(c->containing,a,b,runtime,d,caller,caller_bytes,
  &c->stream->identity_after[1],c->containing_inputs);
 c->containing_returned=1;if(!c->containing_result)return source_failure(c,8);
 return 1;
}
int slcli_runtime_source_windows_acquire_original(struct slcli_runtime_source_windows *c,
 struct slcli_arena *a,const struct slcli_binding_originals *b,struct slcli_memory_owner *runtime,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const struct slcli_runtime_source_windows_inputs *in){
 return source_acquire_original(c,a,b,runtime,d,caller,caller_bytes,prior,in,NULL);
}
int slcli_runtime_source_windows_acquire_claimed_original(struct slcli_runtime_source_windows *c,
 struct slcli_arena *a,const struct slcli_binding_originals *b,struct slcli_memory_owner *runtime,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const struct slcli_runtime_source_windows_inputs *in,
 const struct slcli_runtime_source_windows_claim_inputs *claims){
 if(!claims)return 0;return source_acquire_original(c,a,b,runtime,d,caller,caller_bytes,prior,in,claims);
}
#endif
