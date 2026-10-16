#include "bq_source_unit_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m){uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);}
static int zero(const void *p,size_t n){const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;}
static int stop(struct slcli_bq_source_unit_windows *c,uint32_t stage){if(!c->first_failure)c->first_failure=stage;return 0;}
int slcli_bq_source_unit_windows_observe_original(struct slcli_bq_source_unit_windows *c,
 struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_bq_source_unit_windows_inputs *in){
 const void *blocks[15];size_t sizes[15],i,j,n,alignment,context,total=0;
 const struct slcli_native_clock *prepare_prior;
 if(!a||!slcli_memory_owner_unattached(a->owner)||!c||!in||!slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !slcli_arena_live_block(a,in,sizeof(*in))||!caller_bytes||!slcli_arena_live_block(a,caller,caller_bytes)||
  !slcli_source_deadline_original_matches(a,d,caller)||
  !in->identity||!slcli_arena_live_block(a,in->identity,sizeof(*in->identity))||!in->identity->observed||
  in->identity->arena!=a||in->identity->deadline!=d||in->identity->caller!=caller||in->identity->caller_bytes!=caller_bytes||
  !slcli_bootstrap_qualification_windows_identity_matches_original(in->identity)||
  !in->component_count||in->component_count>SIZE_MAX/sizeof(*in->components)||
  in->component_count>SIZE_MAX/sizeof(*in->targets)||!in->record_count||
  in->record_count>(UINT64_MAX-64)/128||in->record_count>=SIZE_MAX||
  in->file_bytes<64+128*in->record_count||
  in->range_count!=(size_t)in->record_count+1||
  in->range_count>SIZE_MAX/sizeof(*in->bodies)||in->range_count>SIZE_MAX/sizeof(*in->ranges)||
  !slcli_arena_live_range(a,in->path,in->path_bytes)||
  !in->capture_target_count||in->capture_target_count>SIZE_MAX/sizeof(*in->capture_targets)||
  !slcli_bq_source_control_sink_windows_view_original(a,in->capture_factory)||
  in->capture_factory->inputs->deadline!=d||in->capture_factory->inputs->caller!=caller||
  in->capture_factory->inputs->caller_bytes!=caller_bytes)return 0;
 blocks[0]=c;sizes[0]=sizeof(*c);blocks[1]=in;sizes[1]=sizeof(*in);
 blocks[2]=in->identity;sizes[2]=sizeof(*in->identity);
 blocks[3]=in->namespace_original;sizes[3]=sizeof(*in->namespace_original);
 blocks[4]=in->path_copy;sizes[4]=in->path_bytes;blocks[5]=in->root;sizes[5]=8*sizeof(WCHAR);
 blocks[6]=in->components;sizes[6]=in->component_count*sizeof(*in->components);
 blocks[7]=in->targets;sizes[7]=in->component_count*sizeof(*in->targets);
 blocks[8]=in->table;sizes[8]=sizeof(*in->table);
 blocks[9]=in->bodies;sizes[9]=in->range_count*sizeof(*in->bodies);
 blocks[10]=in->ranges;sizes[10]=in->range_count*sizeof(*in->ranges);
 blocks[11]=in->capture_sink;sizes[11]=sizeof(*in->capture_sink);
 blocks[12]=in->capture_targets;sizes[12]=in->capture_target_count*sizeof(*in->capture_targets);
 blocks[13]=in->capture_factory;sizes[13]=sizeof(*in->capture_factory);
 blocks[14]=in->capture_factory->control_ranges;sizes[14]=sizeof(*in->capture_factory->control_ranges);
 for(i=0;i<15;i++){
  if(!slcli_arena_live_block(a,blocks[i],sizes[i])||!slcli_source_deadline_away(d,blocks[i],sizes[i])||
   !apart(blocks[i],sizes[i],caller,caller_bytes)||!apart(blocks[i],sizes[i],in->path,in->path_bytes))return 0;
  for(j=0;j<i;j++)if(!apart(blocks[i],sizes[i],blocks[j],sizes[j]))return 0;
 }
 /* Join all borrowed table bodies/clock blocks to namespace controls BEFORE
  * either preparation mutation or any namespace native effect. */
 for(i=0;i<in->range_count;i++){
  const struct slcli_input_body *s=in->bodies[i];
  if(!s||!slcli_arena_live_range(a,s,sizeof(*s))||!slcli_input_deadline_ready(s)||
   s->deadline_owner->sink!=slcli_bq_source_control_sink_windows_write||
   s->deadline_owner->capture_original!=in->capture_sink||
   s->deadline_owner->capture_original_bytes!=sizeof(*in->capture_sink)||
   s->capture_capacity>SIZE_MAX-total)return 0;
  total+=s->capture_capacity;
  alignment=_Alignof(struct slcli_native_clock);context=sizeof(*s->deadline_owner);
  if(context>SIZE_MAX-(alignment-1))return 0;context=(context+alignment-1)/alignment*alignment;
  if(s->capture_capacity>(SIZE_MAX-context)/sizeof(struct slcli_native_clock))return 0;
  n=context+s->capture_capacity*sizeof(struct slcli_native_clock);
  if(!slcli_arena_live_block(a,s,s->deadline_owner->body_storage_bytes)||
   !slcli_arena_live_block(a,s->deadline_owner,n)||
   !apart(s,s->deadline_owner->body_storage_bytes,in->path,in->path_bytes)||
   !apart(s->deadline_owner,n,in->path,in->path_bytes)||
   !apart(s,s->deadline_owner->body_storage_bytes,caller,caller_bytes)||
   !apart(s->deadline_owner,n,caller,caller_bytes)||
   !slcli_source_deadline_away(d,s,s->deadline_owner->body_storage_bytes)||
   !slcli_source_deadline_away(d,s->deadline_owner,n))return 0;
  for(j=0;j<15;j++)if(!apart(blocks[j],sizes[j],s,s->deadline_owner->body_storage_bytes)||
   !apart(blocks[j],sizes[j],s->deadline_owner,n))return 0;
 }
 if(total!=in->capture_target_count)return 0;
 c->started=1;c->arena=a;c->inputs=in;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->sink_result=slcli_bq_source_control_sink_windows_prepare_original(in->capture_sink,a,in->capture_factory,
  in->capture_targets,in->capture_target_count,in->bodies,in->range_count);c->sink_returned=1;
 if(!c->sink_result)return stop(c,6);
 /* Rejoin each exact preparation-owned deadline lane before table effects. */
 total=0;
 for(i=0;i<in->range_count;i++){
  const struct slcli_input_body *b=in->bodies[i];
  if(b->deadline_owner->source_sink!=in->capture_sink||
   b->deadline_owner->source_captures!=b->captures||
   b->deadline_owner->source_capture_count!=b->capture_capacity||
   b->deadline_owner->source_first_target!=total||b->capture_capacity>SIZE_MAX-total)return stop(c,6);
  total+=b->capture_capacity;
 }
 if(total!=in->capture_target_count)return stop(c,6);
 prepare_prior=&in->identity->finish;
 if(in->capture_factory->finish.tick>prepare_prior->tick)prepare_prior=&in->capture_factory->finish;
 c->prepare_result=slcli_source_unit12_native_table_prepare_original(in->table,a,d,caller,caller_bytes,
  prepare_prior,in->file_bytes,in->bodies,in->ranges,in->range_count);c->prepare_returned=1;
 if(!c->prepare_result)return stop(c,1);
 if(!slcli_source_unit12_native_table_prepared_matches_original(in->table)||
  !slcli_bootstrap_qualification_windows_identity_matches_original(in->identity))return stop(c,2);
 c->joint_result=slcli_native_clock_read(&c->joint_after);c->joint_returned=1;
 if(!c->joint_result||c->joint_after.tick<in->table->prepare_after.tick||c->joint_after.tick>=in->identity->expires||
  !slcli_source_deadline_original_matches(a,d,caller)||d->expires!=in->identity->expires)return stop(c,2);
 c->namespace_result=slcli_bq_source_namespace_windows_acquire_original(in->namespace_original,a,d,caller,caller_bytes,
  &c->joint_after,in->path,in->path_bytes,in->path_copy,in->root,in->components,in->targets,in->component_count);
 c->namespace_returned=1;if(!c->namespace_result)return stop(c,3);
 if(!slcli_bq_source_namespace_windows_matches_original(in->namespace_original))return stop(c,4);
 c->read_result=slcli_source_unit12_native_table_read_prepared_original(in->table,
  in->components[in->component_count-1].handle,&in->namespace_original->finish);c->read_returned=1;
 if(!c->read_result||in->table->reader.count!=in->record_count||
  !slcli_source_unit12_native_matches_original(in->table)||
  !slcli_bq_source_control_sink_windows_matches_original(in->capture_sink))return stop(c,4);
 c->finish_result=slcli_native_clock_read(&c->finish);c->finish_returned=1;
 if(!c->finish_result||c->finish.tick<in->ranges[in->range_count-1].after.tick||c->finish.tick>=in->identity->expires||
  !slcli_source_deadline_original_matches(a,d,caller)||d->expires!=in->identity->expires||
  !in->table->table_observed||in->table->original!=in->components[in->component_count-1].handle)return stop(c,5);
 c->observed=1;return 1;
}
#endif
