#include "bq_installation_capture_windows.h"
#include "evidence_deadline.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m){uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);}
static int zero(const void *p,size_t n){const unsigned char *s=p;size_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;}
static int stop(struct slcli_bq_installation_capture_windows *c,uint32_t stage){if(!c->first_failure)c->first_failure=stage;return 0;}
static int evidence_apart(struct slcli_arena *a,const struct slcli_evidence_file *e,const void *block,size_t bytes){
 const struct slcli_evidence_deadline_owner *o;const struct slcli_image_deadline_owner *d;
 size_t n,align;const void *p[9];size_t z[9],i;
 if(!e||!slcli_arena_live_range(a,e,sizeof(*e))||!slcli_evidence_deadline_ready(e))return 0;
 o=e->deadline_owner;d=o->rows.deadline_owner;
 align=_Alignof(slcli_input_capture);n=sizeof(*e);
 if(n>SIZE_MAX-(align-1))return 0;n=(n+align-1)/align*align;
 if(!e->capture_capacity||e->capture_capacity>(SIZE_MAX-n)/sizeof(*e->captures)||
  e->captures!=(slcli_input_capture *)((unsigned char *)e+n))return 0;
 n+=e->capture_capacity*sizeof(*e->captures);
 if(!slcli_arena_live_block(a,e,n)||d->arena!=a||!d->capacity||d->capacity>SIZE_MAX/sizeof(*d->clocks)||
  !o->bq_profile||!o->profile_bytes)return 0;
 p[0]=e;z[0]=n;p[1]=o;z[1]=sizeof(*o);p[2]=d;z[2]=sizeof(*d);
 p[3]=e->workspace;z[3]=sizeof(*e->workspace);p[4]=d->clocks;z[4]=d->capacity*sizeof(*d->clocks);
 p[5]=o->bq_profile;z[5]=sizeof(*o->bq_profile);p[6]=o->profile_raw;z[6]=o->profile_bytes;
 p[7]=d->capture_owner;z[7]=d->capture_owner_bytes;p[8]=d->birth_producer;z[8]=d->birth_producer_bytes;
 for(i=0;i<9;i++)if(!slcli_arena_live_range(a,p[i],z[i])||!apart(block,bytes,p[i],z[i]))return 0;
 return 1;
}
int slcli_bq_installation_capture_windows_original(struct slcli_bq_installation_capture_windows *c,
 struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_bq_installation_capture_windows_inputs *in){
 const struct slcli_bq_source_unit_windows_inputs *s;
 const void *blocks[7],*source[28];size_t sizes[7],source_sizes[28],i,j,n,align;
 if(!a||!c||!slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !in||!slcli_arena_live_block(a,in,sizeof(*in))||!caller_bytes||!slcli_arena_live_block(a,caller,caller_bytes)||
  !slcli_source_deadline_original_matches(a,d,caller)||
  !(s=in->source_inputs)||!slcli_arena_live_block(a,s,sizeof(*s))||
  !s->component_count||s->component_count>SIZE_MAX/sizeof(*s->components)||s->component_count>SIZE_MAX/sizeof(*s->targets)||
  s->range_count<2||s->range_count>SIZE_MAX/sizeof(*s->bodies)||s->range_count>SIZE_MAX/sizeof(*s->ranges)||
  !s->capture_target_count||s->capture_target_count>SIZE_MAX/sizeof(*s->capture_targets)||
  !slcli_bq_source_control_sink_windows_view_original(a,s->capture_factory)||
  !in->io_count||in->io_count>SIZE_MAX/sizeof(*in->io)||
  !in->foundation_count||in->foundation_count>SIZE_MAX/sizeof(*in->foundation_io)||
  !in->control_count||in->control_count>SIZE_MAX/sizeof(*in->control_io)||
  in->member_index>=s->range_count-1)return 0;
 if(!in->journal||!slcli_arena_live_block(a,in->journal,sizeof(*in->journal))||
  !slcli_writer_windows_readback_matches_original(in->journal)||in->journal->deadline!=d||
  in->journal->caller!=caller||in->journal->caller_bytes!=caller_bytes)return 0;
 {const struct slcli_writer_capture_ranges_windows *r[3]={s->capture_factory->ranges,
   s->capture_factory->output_ranges,s->capture_factory->control_ranges};
  const struct slcli_extent *e=&a->owner->extents[a->extent];
  for(i=0;i<3;i++)if(!r[i]||!slcli_arena_live_block(a,r[i],sizeof(*r[i]))||
   !slcli_writer_capture_ranges_windows_matches_original(r[i])||r[i]->extent->bytes>SIZE_MAX||
   !apart(r[i]->extent->views[0],(size_t)r[i]->extent->bytes,(const void *)e->base,(size_t)e->reserved)||
   !apart(r[i]->extent->views[1],(size_t)r[i]->extent->bytes,(const void *)e->base,(size_t)e->reserved))return 0;
 }
 blocks[0]=c;sizes[0]=sizeof(*c);blocks[1]=in;sizes[1]=sizeof(*in);
 blocks[2]=in->member;sizes[2]=sizeof(*in->member);blocks[3]=in->record;sizes[3]=sizeof(*in->record);
 blocks[4]=in->io;sizes[4]=in->io_count*sizeof(*in->io);
 blocks[5]=in->foundation_io;sizes[5]=in->foundation_count*sizeof(*in->foundation_io);
 blocks[6]=in->control_io;sizes[6]=in->control_count*sizeof(*in->control_io);
 source[0]=in->source;source_sizes[0]=sizeof(*in->source);source[1]=s;source_sizes[1]=sizeof(*s);
 source[2]=s->identity;source_sizes[2]=sizeof(*s->identity);
 source[3]=s->namespace_original;source_sizes[3]=sizeof(*s->namespace_original);
 source[4]=s->path_copy;source_sizes[4]=s->path_bytes;source[5]=s->root;source_sizes[5]=8*sizeof(WCHAR);
 source[6]=s->components;source_sizes[6]=s->component_count*sizeof(*s->components);
 source[7]=s->targets;source_sizes[7]=s->component_count*sizeof(*s->targets);
 source[8]=s->table;source_sizes[8]=sizeof(*s->table);
 source[9]=s->bodies;source_sizes[9]=s->range_count*sizeof(*s->bodies);
 source[10]=s->ranges;source_sizes[10]=s->range_count*sizeof(*s->ranges);
 source[11]=s->capture_sink;source_sizes[11]=sizeof(*s->capture_sink);
 source[12]=s->capture_targets;source_sizes[12]=s->capture_target_count*sizeof(*s->capture_targets);
 source[13]=s->capture_factory;source_sizes[13]=sizeof(*s->capture_factory);
 source[14]=s->capture_factory->control_ranges;source_sizes[14]=sizeof(*s->capture_factory->control_ranges);
 source[15]=in->journal;source_sizes[15]=sizeof(*in->journal);
 source[16]=s->capture_factory->backing;source_sizes[16]=sizeof(*s->capture_factory->backing);
 source[17]=s->capture_factory->ranges;source_sizes[17]=sizeof(*s->capture_factory->ranges);
 source[18]=s->capture_factory->output_ranges;source_sizes[18]=sizeof(*s->capture_factory->output_ranges);
 source[19]=s->capture_factory->creator;source_sizes[19]=sizeof(*s->capture_factory->creator);
 source[20]=s->capture_factory->readback;source_sizes[20]=sizeof(*s->capture_factory->readback);
 source[21]=s->capture_factory->output;source_sizes[21]=sizeof(*s->capture_factory->output);
 source[22]=in->journal->creator;source_sizes[22]=sizeof(*in->journal->creator);
 source[23]=in->journal->creator->evidence_file;source_sizes[23]=sizeof(*in->journal->creator->evidence_file);
 source[24]=s->capture_factory->inputs;source_sizes[24]=sizeof(*s->capture_factory->inputs);
 source[25]=s->capture_factory->ranges->extent;source_sizes[25]=sizeof(*s->capture_factory->ranges->extent);
 source[26]=s->capture_factory->output_ranges->extent;source_sizes[26]=sizeof(*s->capture_factory->output_ranges->extent);
 source[27]=s->capture_factory->control_ranges->extent;source_sizes[27]=sizeof(*s->capture_factory->control_ranges->extent);
 for(i=0;i<28;i++)if(i==23?!slcli_arena_live_range(a,source[i],source_sizes[i]):
  !slcli_arena_live_block(a,source[i],source_sizes[i]))return 0;
 for(i=0;i<7;i++){
  if(!slcli_arena_live_block(a,blocks[i],sizes[i])||(i!=1&&!zero(blocks[i],sizes[i]))||
   !apart(blocks[i],sizes[i],caller,caller_bytes)||!slcli_source_deadline_away(d,blocks[i],sizes[i])||
   !apart(blocks[i],sizes[i],s->path,s->path_bytes))return 0;
  for(j=0;j<i;j++)if(!apart(blocks[i],sizes[i],blocks[j],sizes[j]))return 0;
  for(j=0;j<28;j++)if(!apart(blocks[i],sizes[i],source[j],source_sizes[j]))return 0;
  if(!evidence_apart(a,in->journal->creator->evidence_file,blocks[i],sizes[i])||
   !evidence_apart(a,s->capture_factory->creator->evidence_file,blocks[i],sizes[i]))return 0;
 }
 for(i=0;i<s->range_count;i++){
  const struct slcli_input_body *b=s->bodies[i];
  if(!b||!slcli_arena_live_range(a,b,sizeof(*b))||!slcli_input_deadline_ready(b))return 0;
  align=_Alignof(struct slcli_native_clock);n=sizeof(*b->deadline_owner);
  if(n>SIZE_MAX-(align-1))return 0;n=(n+align-1)/align*align;
  if(b->capture_capacity>(SIZE_MAX-n)/sizeof(struct slcli_native_clock))return 0;
  n+=b->capture_capacity*sizeof(struct slcli_native_clock);
  if(!slcli_arena_live_block(a,b,b->deadline_owner->body_storage_bytes)||!slcli_arena_live_block(a,b->deadline_owner,n))return 0;
  for(j=0;j<7;j++)if(!apart(blocks[j],sizes[j],b,b->deadline_owner->body_storage_bytes)||
   !apart(blocks[j],sizes[j],b->deadline_owner,n))return 0;
 }
 c->started=1;c->arena=a;c->inputs=in;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->origin_original=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
 c->source_result=slcli_bq_source_unit_windows_observe_original(in->source,a,d,caller,caller_bytes,s);c->source_returned=1;
 if(!c->source_result)return stop(c,1);
 c->member_result=slcli_source_unit12_member_window_windows_read_original(in->member,a,s->table,in->member_index+1,s->capture_factory->ranges);
 c->member_returned=1;if(!c->member_result)return stop(c,2);
 c->record_result=slcli_writer_bq_record_windows_first_original(in->record,a,in->journal,in->member,
  s->capture_factory->ranges,s->capture_factory->output_ranges,s->capture_factory->control_ranges,s->capture_sink,
  in->io,in->io_count,in->foundation_io,in->foundation_count,in->control_io,in->control_count);
 c->record_returned=1;if(!c->record_result)return stop(c,3);
 /* Raw first-record/foundation observation is still APPEND_PENDING, not RANGE. */
 c->finish_result=slcli_native_clock_read(&c->finish);c->finish_returned=1;
 if(!c->finish_result||!c->finish.complete||c->finish.tick<in->record->control_finish.tick||
  c->finish.tick>=c->expires||!slcli_source_deadline_original_matches(a,d,caller)||
  d->origin!=c->origin_original||d->origin->tick!=c->origin_tick||d->expires!=c->expires)return stop(c,4);
 c->observed=1;return 1;
}
#endif
