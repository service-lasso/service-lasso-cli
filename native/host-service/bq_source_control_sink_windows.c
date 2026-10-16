#include "bq_source_control_sink_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m){uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);}
static int zero(const void *p,size_t n){const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;}
static int contained(uintptr_t raw,size_t bytes,uintptr_t base,size_t capacity){
 return raw>=base&&raw-base<=capacity&&bytes<=capacity-(raw-base);
}
static int header(const unsigned char *h,const struct slcli_writer_capture_ranges_windows *r,
 const struct slcli_bq_source_control_target_windows *t){
 unsigned char expected[48]={'S','L','N','C','1','0','6','1'};size_t z;
 for(z=0;z<4;z++){
  expected[8+z]=(unsigned char)(UINT32_C(2)>>(8*z));
  expected[12+z]=(unsigned char)((uint32_t)sizeof(slcli_input_capture)>>(8*z));
  expected[24+z]=(unsigned char)(UINT32_C(1)>>(8*z));
  expected[28+z]=(unsigned char)(UINT32_C(2)>>(8*z));
 }
 for(z=0;z<8;z++){
  expected[16+z]=(unsigned char)((uint64_t)t->ordinal>>(8*z));
  expected[32+z]=(unsigned char)((uint64_t)r->payload_capacity>>(8*z));
  expected[40+z]=(unsigned char)((uint64_t)t->payload_maximum>>(8*z));
 }
 for(z=0;z<48;z++)if(h[z]!=expected[z])return 0;return 1;
}
static size_t payload_maximum(const struct slcli_writer_capture_ranges_windows *r,const struct slcli_input_body *b){
 uint64_t record;size_t maximum;
 if(!r||!r->extent||!r->extent->readback||!r->extent->readback->creator||
  !r->extent->readback->creator->evidence_file||!r->extent->readback->creator->evidence_file->deadline_owner||
  !r->extent->readback->creator->evidence_file->deadline_owner->bq_profile||!b||b->capacity<2||
  b->capacity>16385||b->capacity!=b->readback_capacity)return 0;
 record=r->extent->readback->creator->evidence_file->deadline_owner->record_maximum;
 if(record>16384)record=16384;
 if(record<=48+sizeof(slcli_input_capture))return 0;
 maximum=(size_t)(record-48-sizeof(slcli_input_capture));
 if(maximum>r->payload_capacity)maximum=r->payload_capacity;
 if(maximum>b->capacity-1)maximum=b->capacity-1;
 return maximum;
}
int slcli_bq_source_control_sink_windows_view_original(struct slcli_arena *a,
 const struct slcli_writer_capture_factory_windows *f){
 const struct slcli_writer_capture_ranges_windows *r;const struct slcli_extent *e;
 if(!a||!a->owner||a->extent>=a->owner->count||a->extent>=256||
  !f||!slcli_arena_live_block(a,f,sizeof(*f))||f->arena!=a||
  !slcli_writer_capture_factory_windows_bq_matches_original(f)||
  !(r=f->control_ranges)||!slcli_arena_live_block(a,r,sizeof(*r))||r->arena!=a||r->version!=2||
  r->write_close_bq_original||!slcli_writer_capture_ranges_windows_matches_original(r)||
  !r->payload_capacity||r->extent->owner!=a->owner||r->extent->request.index==a->extent||
  r->extent->bytes>SIZE_MAX)return 0;
 e=&a->owner->extents[a->extent];
 if(e->state!=SLCLI_EXTENT_LIVE||e->acquisition!=a->acquisition||!e->base||!e->reserved||e->reserved>SIZE_MAX)return 0;
 return apart(r->extent->views[0],(size_t)r->extent->bytes,(const void *)e->base,(size_t)e->reserved)&&
  apart(r->extent->views[1],(size_t)r->extent->bytes,(const void *)e->base,(size_t)e->reserved);
}
int slcli_bq_source_control_sink_windows_prepare_original(struct slcli_bq_source_control_sink_windows *s,
 struct slcli_arena *a,const struct slcli_writer_capture_factory_windows *f,
 struct slcli_bq_source_control_target_windows *targets,size_t count,struct slcli_input_body *const *bodies,size_t body_count){
 size_t i,j,k=0,total=0,maximum,minimum;
 if(!a||!s||!count||!body_count||count>SIZE_MAX/sizeof(*targets)||body_count>SIZE_MAX/sizeof(*bodies)||
  !slcli_arena_live_block(a,s,sizeof(*s))||!zero(s,sizeof(*s))||
  !slcli_arena_live_block(a,targets,count*sizeof(*targets))||!zero(targets,count*sizeof(*targets))||
  !slcli_arena_live_block(a,bodies,body_count*sizeof(*bodies))||
  !slcli_bq_source_control_sink_windows_view_original(a,f)||
  !apart(s,sizeof(*s),targets,count*sizeof(*targets))||!apart(s,sizeof(*s),f,sizeof(*f))||
  !apart(targets,count*sizeof(*targets),f,sizeof(*f))||
  !apart(s,sizeof(*s),bodies,body_count*sizeof(*bodies))||
  !apart(targets,count*sizeof(*targets),bodies,body_count*sizeof(*bodies)))return 0;
 for(i=0;i<body_count;i++){
  const struct slcli_input_body *b=bodies[i];
  if(!b||!slcli_arena_live_range(a,b,sizeof(*b))||!slcli_input_deadline_ready(b)||
   b->started||b->capture_count||b->source_native_read_maximum||b->capture_capacity>SIZE_MAX-total||
   b->deadline_owner->sink!=slcli_bq_source_control_sink_windows_write||
   b->deadline_owner->capture_original!=s||b->deadline_owner->capture_original_bytes!=sizeof(*s)||
   b->deadline_owner->source_sink||b->deadline_owner->source_captures||
   b->deadline_owner->source_capture_count||b->deadline_owner->source_first_target||
   !apart(s,sizeof(*s),b,b->deadline_owner->body_storage_bytes)||
   !apart(targets,count*sizeof(*targets),b,b->deadline_owner->body_storage_bytes))return 0;
  maximum=payload_maximum(f->control_ranges,b);if(!maximum)return 0;
  minimum=(b->capacity-1)/maximum+((b->capacity-1)%maximum!=0);
  if(minimum>(SIZE_MAX-6)/2||b->capture_capacity<2*(minimum+3))return 0;
  total+=b->capture_capacity;
  for(j=0;j<i;j++)if(b==bodies[j])return 0;
 }
 if(total!=count||f->control_ranges->attempted>f->control_ranges->capacity||
  count>f->control_ranges->capacity-f->control_ranges->attempted)return 0;
 s->started=1;s->failed=1;s->arena=a;s->factory=f;s->ranges=f->control_ranges;
 s->targets=targets;s->bodies=bodies;s->body_count=body_count;s->count=count;s->first_ordinal=s->ranges->attempted;
 for(i=0;i<body_count;i++){
  struct slcli_input_deadline_owner *o=bodies[i]->deadline_owner;
  /* Enroll the exact interval before its first claim; failed prefixes retain it. */
  o->source_sink=s;o->source_captures=bodies[i]->captures;
  o->source_capture_count=bodies[i]->capture_capacity;o->source_first_target=k;
  for(j=0;j<bodies[i]->capture_capacity;j++,k++){
  struct slcli_bq_source_control_target_windows *t=&targets[k];
  maximum=payload_maximum(s->ranges,bodies[i]);
  if(!maximum)return 0;
  bodies[i]->source_native_read_maximum=maximum;
  t->source=&bodies[i]->captures[j];t->ordinal=s->first_ordinal+k;
  t->payload_maximum=maximum;
  t->copy=slcli_writer_capture_ranges_windows_claim_read_original(s->ranges,maximum,&t->payload,&t->readonly_payload);
  if(!t->copy)return 0;s->claimed=k+1;
  }
 }
 s->prepared=1;s->failed=0;return 1;
}
int slcli_bq_source_control_sink_windows_write(void *original,const slcli_input_capture *source,
 const unsigned char *raw,size_t bytes){
 /* A generic callback supplies no original deadline/body lane. The source-fixed
  * input_deadline_after path below is the only issuing route for this sink. */
 (void)original;(void)source;(void)raw;(void)bytes;return 0;
}
int slcli_bq_source_control_sink_windows_write_owned(struct slcli_input_deadline_owner *o,
 const slcli_input_capture *source,const unsigned char *raw,size_t bytes){
 struct slcli_bq_source_control_sink_windows *s;const struct slcli_input_body *b;
 size_t j,n,align,total,ordinal,found,offset,context;uintptr_t address,captures,base,read,start,back;
 struct slcli_bq_source_control_target_windows *t;
 const struct slcli_writer_capture_extent_windows *e;
 if(!o||!o->arena||!slcli_arena_live_range(o->arena,o,sizeof(*o))||
  !(s=o->source_sink)||o->capture_original!=s||o->capture_original_bytes!=sizeof(*s)||
  o->sink!=slcli_bq_source_control_sink_windows_write||
  !slcli_arena_live_block(o->arena,s,sizeof(*s))||s->arena!=o->arena||
  !s->started||!s->prepared||s->failed||s->record_borrower)return 0;
 s->failed=1;
 if(!(b=o->body)||!slcli_arena_live_range(s->arena,b,sizeof(*b))||b->deadline_owner!=o||
  !o->prepared||!o->in_flight||o->pending!=source||!source||
  !b->capacity||b->capacity!=b->readback_capacity||b->capacity>(SIZE_MAX-sizeof(*b))/2||
  o->source_captures!=b->captures||o->source_capture_count!=b->capture_capacity||
  !o->source_capture_count||o->next>=o->source_capture_count||
  !s->count||s->claimed!=s->count||s->used>=s->count||s->count>SIZE_MAX/sizeof(*s->targets)||
  !slcli_arena_live_block(s->arena,s->targets,s->count*sizeof(*s->targets))||
  o->source_first_target>s->count||o->source_capture_count>s->count-o->source_first_target||
  !slcli_bq_source_control_sink_windows_view_original(s->arena,s->factory)||
  s->ranges!=s->factory->control_ranges||s->first_ordinal>s->ranges->attempted||
  s->count>s->ranges->attempted-s->first_ordinal||(bytes&&!raw))return 0;
 n=sizeof(*b)+2*b->capacity;align=_Alignof(slcli_input_capture);
 if(n>SIZE_MAX-(align-1))return 0;n=(n+align-1)/align*align;
 if(b->capture_capacity>(SIZE_MAX-n)/sizeof(*b->captures))return 0;
 total=n+b->capture_capacity*sizeof(*b->captures);
 align=_Alignof(struct slcli_native_clock);context=sizeof(*o);
 if(context>SIZE_MAX-(align-1))return 0;context=(context+align-1)/align*align;
 if(o->row_capacity>(SIZE_MAX-context)/sizeof(*o->before)||
  !slcli_arena_live_block(s->arena,o,context+o->row_capacity*sizeof(*o->before))||
  o->row_capacity!=b->capture_capacity||o->before!=(struct slcli_native_clock *)((unsigned char *)o+context))return 0;
 if(o->body_storage_bytes!=total||!slcli_arena_live_block(s->arena,b,total)||
  b->body!=(unsigned char *)b+sizeof(*b)||b->readback!=b->body+b->capacity||
  b->captures!=(slcli_input_capture *)((unsigned char *)b+n)||
  !b->source_native_read_maximum||b->source_native_read_maximum!=payload_maximum(s->ranges,b))return 0;
 address=(uintptr_t)source;captures=(uintptr_t)o->source_captures;
 if(captures>UINTPTR_MAX-o->source_capture_count*sizeof(*source)||address<captures||
  address-captures>=o->source_capture_count*sizeof(*source)||
  (address-captures)%sizeof(*source))return 0;
 ordinal=(size_t)((address-captures)/sizeof(*source));
 if(ordinal!=o->next||b->capture_count!=ordinal+1)return 0;
 found=o->source_first_target+ordinal;t=&s->targets[found];
 e=s->ranges->extent;base=(uintptr_t)e->views[0];read=(uintptr_t)e->views[1];
 start=(uintptr_t)b->body;back=(uintptr_t)b->readback;
 if(t->source!=source||t->ordinal!=s->first_ordinal+found||t->ordinal>=s->ranges->capacity||
  t->attempted||t->copied||t->payload_maximum!=b->source_native_read_maximum||
  bytes>t->payload_maximum||t->payload_maximum>s->ranges->payload_capacity||
  t->ordinal>SIZE_MAX/s->ranges->stride||
  (bytes&&(source->operation!=2||!source->bytes_observed||source->returned_bytes!=bytes||
   source->read_args[1]!=(uintptr_t)raw||
   !(contained((uintptr_t)raw,bytes,start,b->capacity)||
     contained((uintptr_t)raw,bytes,back,b->readback_capacity)))))return 0;
 offset=t->ordinal*s->ranges->stride;
 if(offset>e->bytes||s->ranges->stride>e->bytes-offset||base>UINTPTR_MAX-offset||read>UINTPTR_MAX-offset||
  t->copy!=(struct slcli_windows_file_capture *)(base+offset+s->ranges->data_offset)||
  t->payload!=(unsigned char *)(base+offset+s->ranges->payload_offset)||
  t->readonly_payload!=(const unsigned char *)(read+offset+s->ranges->payload_offset)||
  !zero(t->copy,sizeof(*t->copy))||!zero(t->payload,t->payload_maximum))return 0;
 if(!header((const unsigned char *)(read+offset),s->ranges,t))return 0;
 t->attempted=1;s->used++;t->raw_original=raw;t->bytes=bytes;
 for(j=0;j<sizeof(*source);j++)((unsigned char *)t->copy)[j]=((const unsigned char *)source)[j];
 for(j=0;j<bytes;j++)t->payload[j]=raw[j];t->copied=1;
 s->failed=0;return 1;
}
int slcli_bq_source_control_sink_windows_matches_original(const struct slcli_bq_source_control_sink_windows *s){
 size_t i,j,k=0,n,align,total,used=0,offset,minimum;uintptr_t rw,ro;
 if(!s||!s->arena||!slcli_arena_live_block(s->arena,s,sizeof(*s))||!s->started||!s->prepared||s->failed||
  !s->count||s->count>SIZE_MAX/sizeof(*s->targets)||s->claimed!=s->count||s->used>s->count||
  !slcli_arena_live_block(s->arena,s->targets,s->count*sizeof(*s->targets))||
  !slcli_bq_source_control_sink_windows_view_original(s->arena,s->factory)||s->ranges!=s->factory->control_ranges||
  s->first_ordinal>s->ranges->attempted||s->count>s->ranges->attempted-s->first_ordinal)return 0;
 if(!s->body_count||s->body_count>SIZE_MAX/sizeof(*s->bodies)||
  !slcli_arena_live_block(s->arena,s->bodies,s->body_count*sizeof(*s->bodies)))return 0;
 for(i=0;i<s->body_count;i++){
  const struct slcli_input_body *b=s->bodies[i];
  if(!b||!slcli_arena_live_range(s->arena,b,sizeof(*b))||!b->deadline_owner||
   !slcli_arena_live_range(s->arena,b->deadline_owner,sizeof(*b->deadline_owner))||
   b->deadline_owner->arena!=s->arena||b->deadline_owner->body!=b||
   b->deadline_owner->sink!=slcli_bq_source_control_sink_windows_write||
   b->deadline_owner->capture_original!=s||b->deadline_owner->capture_original_bytes!=sizeof(*s)||
   b->deadline_owner->source_sink!=s||b->deadline_owner->source_captures!=b->captures||
   b->deadline_owner->source_capture_count!=b->capture_capacity||b->deadline_owner->source_first_target!=k||
   b->capture_capacity>s->count-k||b->capture_count>b->capture_capacity||
   !b->capacity||b->capacity!=b->readback_capacity||b->capacity>(SIZE_MAX-sizeof(*b))/2)return 0;
  if(!b->source_native_read_maximum||b->source_native_read_maximum!=payload_maximum(s->ranges,b))return 0;
  minimum=(b->capacity-1)/b->source_native_read_maximum+((b->capacity-1)%b->source_native_read_maximum!=0);
  if(minimum>(SIZE_MAX-6)/2||b->capture_capacity<2*(minimum+3))return 0;
  n=sizeof(*b)+2*b->capacity;align=_Alignof(slcli_input_capture);
  if(n>SIZE_MAX-(align-1))return 0;n=(n+align-1)/align*align;
  if(b->capture_capacity>(SIZE_MAX-n)/sizeof(*b->captures))return 0;
  total=n+b->capture_capacity*sizeof(*b->captures);
  if(b->deadline_owner->body_storage_bytes!=total||!slcli_arena_live_block(s->arena,b,total)||
   b->body!=(unsigned char *)b+sizeof(*b)||b->readback!=b->body+b->capacity||
   b->captures!=(slcli_input_capture *)((unsigned char *)b+n))return 0;
  for(j=0;j<b->capture_capacity;j++,k++){
   const struct slcli_bq_source_control_target_windows *t=&s->targets[k];
   uintptr_t raw=(uintptr_t)t->raw_original,start=(uintptr_t)b->body,back=(uintptr_t)b->readback;
   if(t->source!=&b->captures[j]||t->payload_maximum!=b->source_native_read_maximum)return 0;
   if(t->attempted){
    if(j>=b->capture_count||t->bytes>t->payload_maximum||
     (t->bytes&&(t->source->operation!=2||!t->source->bytes_observed||
      t->source->returned_bytes!=t->bytes||t->source->read_args[1]!=raw))||
     (t->bytes&&(!t->raw_original||
      !((raw>=start&&raw-start<=b->capacity&&t->bytes<=b->capacity-(raw-start))||
        (raw>=back&&raw-back<=b->readback_capacity&&t->bytes<=b->readback_capacity-(raw-back)))))||
     (!t->bytes&&t->raw_original))return 0;
   }
  }
 }
 if(k!=s->count)return 0;
 rw=(uintptr_t)s->ranges->extent->views[0];ro=(uintptr_t)s->ranges->extent->views[1];
 for(i=0;i<s->count;i++){
  const struct slcli_bq_source_control_target_windows *t=&s->targets[i];
  if(t->ordinal!=s->first_ordinal+i||t->ordinal>=s->ranges->capacity||
   t->ordinal>SIZE_MAX/s->ranges->stride||!slcli_arena_live_range(s->arena,t->source,sizeof(*t->source)))return 0;
  offset=t->ordinal*s->ranges->stride;
  if(offset>s->ranges->extent->bytes||s->ranges->stride>s->ranges->extent->bytes-offset||
   rw>UINTPTR_MAX-offset||ro>UINTPTR_MAX-offset||
   t->copy!=(struct slcli_windows_file_capture *)(rw+offset+s->ranges->data_offset)||
   t->payload!=(unsigned char *)(rw+offset+s->ranges->payload_offset)||
   t->readonly_payload!=(const unsigned char *)(ro+offset+s->ranges->payload_offset))return 0;
  {const unsigned char *h=(const unsigned char *)(ro+offset);
   unsigned char expected[48]={'S','L','N','C','1','0','6','1'};uint64_t values[6];size_t z;
   values[0]=2;values[1]=sizeof(*t->source);values[2]=t->ordinal;
   values[3]=1;values[4]=2;values[5]=s->ranges->payload_capacity;
   for(z=0;z<4;z++){expected[8+z]=(unsigned char)(values[0]>>(8*z));expected[12+z]=(unsigned char)(values[1]>>(8*z));
    expected[24+z]=(unsigned char)(values[3]>>(8*z));expected[28+z]=(unsigned char)(values[4]>>(8*z));}
   for(z=0;z<8;z++){expected[16+z]=(unsigned char)(values[2]>>(8*z));expected[32+z]=(unsigned char)(values[5]>>(8*z));
    expected[40+z]=(unsigned char)((uint64_t)t->payload_maximum>>(8*z));}
   for(z=0;z<48;z++)if(h[z]!=expected[z])return 0;
  }
  if(t->attempted){
   const unsigned char *copy=(const unsigned char *)(ro+offset+s->ranges->data_offset);
   if(t->attempted!=1||t->copied!=1||t->bytes>t->payload_maximum)return 0;
   for(j=0;j<sizeof(*t->source);j++)if(copy[j]!=((const unsigned char *)t->source)[j])return 0;
   for(j=0;j<t->bytes;j++)if(t->readonly_payload[j]!=t->raw_original[j])return 0;
   if(!zero(t->readonly_payload+t->bytes,t->payload_maximum-t->bytes))return 0;
   used++;
  }else if(t->copied||t->bytes||t->raw_original||
   !zero((const unsigned char *)(ro+offset+s->ranges->data_offset),sizeof(*t->copy))||
   !zero(t->readonly_payload,t->payload_maximum))return 0;
 }
 return used==s->used;
}
#endif
