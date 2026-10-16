#include "store_deadline.h"
#if defined(_WIN32) || defined(__linux__)
static int before(void *,struct slcli_store_stream *,slcli_input_capture *);
static int after(void *,const slcli_input_capture *,const unsigned char *,size_t,int);
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
static size_t offset(void) {
  size_t a=_Alignof(struct slcli_native_clock),n=sizeof(struct slcli_store_deadline_owner);
  return (n+a-1)/a*a;
}
static size_t stream_bytes(const struct slcli_store_stream *s) {
  size_t n=sizeof(*s),a=_Alignof(slcli_input_capture),rows;
  if(!s||!s->capture_capacity||s->capture_capacity>SIZE_MAX/sizeof(*s->captures)||
     n>SIZE_MAX-(a-1)||s->unit_geometry>1)return 0;
  n=(n+a-1)/a*a;rows=s->capture_capacity*sizeof(*s->captures);
  if(rows>SIZE_MAX-n||s->captures!=(slcli_input_capture *)((unsigned char *)s+n))return 0;
  n+=rows;
  if(!s->unit_geometry)return !s->unit_chunk&&!s->unit_readback?n:0;
  if(n>SIZE_MAX-32769||s->unit_chunk!=(unsigned char *)s+n||
     s->unit_readback!=s->unit_chunk+16384)return 0;
  return n+32769;
}
static size_t owner_bytes(const struct slcli_store_deadline_owner *o) {
  size_t n=offset();
  if(!o||!o->row_capacity||o->row_capacity>SIZE_MAX/sizeof(*o->before)||
     o->row_capacity*sizeof(*o->before)>SIZE_MAX-n)return 0;
  return n+o->row_capacity*sizeof(*o->before);
}
static int source(const struct slcli_store_deadline_owner *o) {
  size_t i,rows=0,bytes=owner_bytes(o);
  if(!bytes||!o->prepared||!o->arena||!o->count||o->count>6||
     !slcli_arena_live_block(o->arena,o,bytes)||
     o->before!=(struct slcli_native_clock *)((unsigned char *)o+offset())||
     !slcli_arena_live_block(o->arena,o->caller_original,o->caller_bytes)||
     !slcli_arena_live_block(o->arena,o->capture_original,o->capture_original_bytes)||
     !slcli_source_deadline_original_matches(o->arena,o->deadline,o->caller_original)||
     o->deadline->origin!=o->origin||o->deadline->expires!=o->expires||
     o->origin->tick!=o->origin_tick||!o->predecessor||
     !slcli_arena_live_range(o->arena,o->predecessor,sizeof(*o->predecessor))||
     !o->predecessor->complete||o->predecessor->tick!=o->predecessor_tick||
     o->predecessor_tick<o->origin_tick||o->previous_tick<o->predecessor_tick||!o->sink)return 0;
  if(o->external_original) {
    uintptr_t p=(uintptr_t)o->external_original,q=(uintptr_t)o->external_before;
    if(!o->external_before||o->external_bytes<sizeof(*o->external_before)||
       p>UINTPTR_MAX-o->external_bytes||q<p||q>p+o->external_bytes-sizeof(*o->external_before)||
       !slcli_arena_live_block(o->arena,o->external_original,o->external_bytes)||
       o->in_flight||o->pending_capture||o->pending_stream)return 0;
  } else if(o->external_bytes||o->external_before)return 0;
  for(i=0;i<o->count;i++) {
    struct slcli_store_stream *s=o->streams[i];
    if(!s||o->offsets[i]!=rows||o->capacities[i]>SIZE_MAX-rows||
       s->capture_capacity!=o->capacities[i]||stream_bytes(s)!=o->storage_bytes[i]||
       !slcli_arena_live_block(o->arena,s,o->storage_bytes[i])||
       o->used[i]>o->capacities[i]||s->capture_before!=before||s->before_original!=o||
       s->capture_after!=after||s->after_original!=o||
       (s->attempted&&(s->capture_sink!=o->sink||s->capture_original!=o->capture_original)))return 0;
    rows+=o->capacities[i];
  }
  return rows==o->row_capacity;
}
struct slcli_store_deadline_owner *slcli_store_deadline_bind(struct slcli_arena *a,
  struct slcli_store_stream *const *streams,size_t count,const struct slcli_source_deadline_original *d,
  const void *caller,size_t caller_bytes,const struct slcli_native_clock *prior,
  slcli_store_capture_sink sink,void *owner,size_t owner_size) {
  struct slcli_store_deadline_owner *o;size_t i,j,n,total,rows=0,lengths[6];
  struct slcli_store_stream *selected[6];
  if(!a||!a->owner||!streams||!count||count>6||!sink||!caller_bytes||!owner_size||
     !slcli_arena_live_block(a,caller,caller_bytes)||!slcli_arena_live_block(a,owner,owner_size)||
     !slcli_source_deadline_original_matches(a,d,caller)||!prior||
     !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
     prior->tick<d->origin->tick||prior->tick>=d->expires||
     !away(caller,caller_bytes,owner,owner_size)||!away(caller,caller_bytes,prior,sizeof(*prior))||
     !away(owner,owner_size,prior,sizeof(*prior))||!slcli_source_deadline_away(d,owner,owner_size)||
     !slcli_source_deadline_away(d,caller,caller_bytes)||
     !away(owner,owner_size,a,sizeof(*a))||!away(owner,owner_size,a->owner,sizeof(*a->owner))||
     !away(caller,caller_bytes,a,sizeof(*a))||!away(caller,caller_bytes,a->owner,sizeof(*a->owner)))return 0;
  for(i=0;i<count;i++)selected[i]=streams[i];
  for(i=0;i<count;i++) {
    struct slcli_store_stream *s=selected[i];
    if(!(lengths[i]=stream_bytes(s))||!slcli_arena_live_block(a,s,lengths[i])||
       s->attempted||s->failed||s->capture_count||s->held_verified||
       s->capture_before||s->before_original||s->capture_after||s->after_original||
       s->capture_sink||s->capture_original||s->capture_capacity>SIZE_MAX-rows||
       !away(s,lengths[i],caller,caller_bytes)||!away(s,lengths[i],owner,owner_size)||
       !away(s,lengths[i],prior,sizeof(*prior))||!slcli_source_deadline_away(d,s,lengths[i])||
       !away(s,lengths[i],streams,count*sizeof(*streams))||
       !away(s,lengths[i],a,sizeof(*a))||!away(s,lengths[i],a->owner,sizeof(*a->owner)))return 0;
    for(j=0;j<i;j++)if(!away(s,lengths[i],selected[j],lengths[j]))return 0;
    rows+=s->capture_capacity;
  }
  n=offset();if(rows>SIZE_MAX/sizeof(struct slcli_native_clock)||
     rows*sizeof(struct slcli_native_clock)>SIZE_MAX-n)return 0;
  total=n+rows*sizeof(struct slcli_native_clock);o=slcli_arena_calloc(a,1,total);if(!o)return 0;
  if(!away(o,total,caller,caller_bytes)||!away(o,total,owner,owner_size)||
     !away(o,total,prior,sizeof(*prior))||!slcli_source_deadline_away(d,o,total)||
     !away(o,total,a,sizeof(*a))||!away(o,total,a->owner,sizeof(*a->owner))||
     !away(o,total,streams,count*sizeof(*streams)))return 0;
  o->arena=a;o->deadline=d;o->origin=d->origin;o->caller_original=caller;o->caller_bytes=caller_bytes;
  o->sink=sink;o->capture_original=owner;o->capture_original_bytes=owner_size;
  o->predecessor=prior;o->predecessor_tick=prior->tick;o->previous_tick=prior->tick;
  o->expires=d->expires;o->origin_tick=d->origin->tick;o->count=count;o->row_capacity=rows;
  o->before=(void *)((unsigned char *)o+n);rows=0;
  for(i=0;i<count;i++) {
    if(streams[i]!=selected[i]||!away(o,total,selected[i],lengths[i]))return 0;
    o->streams[i]=selected[i];o->offsets[i]=rows;o->capacities[i]=selected[i]->capture_capacity;
    o->storage_bytes[i]=lengths[i];rows+=o->capacities[i];
    selected[i]->capture_before=before;selected[i]->before_original=o;
    selected[i]->capture_after=after;selected[i]->after_original=o;
  }
  o->prepared=1;if(!source(o)){o->failed=1;return 0;}return o;
}
int slcli_store_deadline_away(const struct slcli_store_deadline_owner *o,const void *p,size_t n) {
  size_t i;
  if(!source(o)||!p||!n||!away(o,owner_bytes(o),p,n)||
     !away(o->caller_original,o->caller_bytes,p,n)||
     !away(o->capture_original,o->capture_original_bytes,p,n)||
     !away(o->predecessor,sizeof(*o->predecessor),p,n)||!slcli_source_deadline_away(o->deadline,p,n))return 0;
  for(i=0;i<o->count;i++)if(!away(o->streams[i],o->storage_bytes[i],p,n))return 0;
  return 1;
}
int slcli_store_deadline_precede(struct slcli_store_deadline_owner *o,const struct slcli_native_clock *prior) {
  size_t i;
  if(!source(o)||o->failed||o->in_flight||o->external_original||o->predecessor_advanced||!prior||
     !slcli_arena_live_range(o->arena,prior,sizeof(*prior))||!prior->complete||
     prior->tick<o->previous_tick||prior->tick>=o->expires||
     !away(o,owner_bytes(o),prior,sizeof(*prior))||
     !away(o->caller_original,o->caller_bytes,prior,sizeof(*prior))||
     !away(o->capture_original,o->capture_original_bytes,prior,sizeof(*prior)))return 0;
  for(i=0;i<o->count;i++)if(o->used[i]||o->streams[i]->attempted||
     !away(o->streams[i],o->storage_bytes[i],prior,sizeof(*prior)))return 0;
  o->predecessor=prior;o->predecessor_tick=prior->tick;o->previous_tick=prior->tick;
  o->predecessor_advanced=1;return 1;
}
int slcli_store_deadline_ready(const struct slcli_store_deadline_owner *o) {
  size_t i;if(!source(o)||o->failed||o->in_flight||o->pending_capture||o->pending_stream||o->external_original||
     o->previous_tick>=o->expires)return 0;
  for(i=0;i<o->count;i++)if(o->used[i]!=o->streams[i]->capture_count||
     (o->streams[i]->attempted&&o->streams[i]->failed))return 0;
  return 1;
}
static int contains_clock(void *original,size_t bytes,struct slcli_native_clock *r) {
  uintptr_t p=(uintptr_t)original,q=(uintptr_t)r;
  return original&&r&&bytes>=sizeof(*r)&&p<=UINTPTR_MAX-bytes&&q>=p&&
    q<=p+bytes-sizeof(*r)&&!r->started&&!r->observed&&!r->complete;
}
int slcli_store_deadline_external_begin(struct slcli_store_deadline_owner *o,
  void *original,size_t bytes,struct slcli_native_clock *r) {
  if(!slcli_store_deadline_ready(o)||!contains_clock(original,bytes,r)||
     !slcli_arena_live_block(o->arena,original,bytes)||
     !slcli_store_deadline_away(o,original,bytes))return 0;
  o->external_original=original;o->external_bytes=bytes;o->external_before=r;
  if(!slcli_native_clock_read(r)||r->tick<o->previous_tick||r->tick>=o->expires||!source(o)) {
    o->failed=1;return 0;
  }
  o->previous_tick=r->tick;return 1;
}
int slcli_store_deadline_external_end(struct slcli_store_deadline_owner *o,
  void *original,size_t bytes,struct slcli_native_clock *r,int native_ok) {
  if(!source(o)||o->failed||o->external_original!=original||o->external_bytes!=bytes||
     !o->external_before||!o->external_before->complete||
     o->external_before->tick!=o->previous_tick||!contains_clock(original,bytes,r)||
     !away(r,sizeof(*r),o->external_before,sizeof(*r))||
     !slcli_arena_live_block(o->arena,original,bytes))return 0;
  if(!slcli_native_clock_read(r)||r->tick<o->previous_tick||r->tick>=o->expires||!source(o)) {
    o->failed=1;return 0;
  }
  o->previous_tick=r->tick;if(!native_ok){o->failed=1;return 0;}
  o->external_original=0;o->external_bytes=0;o->external_before=0;return 1;
}
static int before(void *context,struct slcli_store_stream *s,slcli_input_capture *c) {
  struct slcli_store_deadline_owner *o=context;struct slcli_native_clock *r;size_t i,n;
  if(!source(o)||o->failed||o->in_flight||o->pending_capture||o->pending_stream||o->external_original||!c)return 0;
  /* begin_range marks its first row failed until native observation succeeds.
     Any later failure, or another failed attempted stream, closes this group. */
  for(i=0;i<o->count;i++)if(o->streams[i]->attempted&&o->streams[i]->failed&&
     (o->streams[i]!=s||o->used[i])) {o->failed=1;return 0;}
  for(i=0;i<o->count&&o->streams[i]!=s;i++);if(i==o->count)return 0;n=o->used[i];
  if(n>=o->capacities[i]||s->capture_count!=n+1||c!=&s->captures[n]||c->operation)return 0;
  o->in_flight=1;o->pending_capture=c;o->pending_stream=s;r=&o->before[o->offsets[i]+n];o->used[i]++;
  if(!slcli_native_clock_read(r)||r->tick<o->previous_tick||r->tick>=o->expires||!source(o)) {
    o->failed=1;return 0;
  }
  o->previous_tick=r->tick;return 1;
}
static int after(void *context,const slcli_input_capture *c,const unsigned char *raw,size_t bytes,int native_ok) {
  struct slcli_store_deadline_owner *o=context;size_t i,n;
  (void)raw;(void)bytes; /* The actual original sink already preserved these. */
  if(!source(o)||o->failed||!o->in_flight||o->pending_capture!=c||!o->pending_stream)return 0;
  for(i=0;i<o->count&&o->streams[i]!=o->pending_stream;i++);if(i==o->count)return 0;n=o->used[i];
  if(!n||n>o->capacities[i]||c!=&o->pending_stream->captures[n-1]||
     o->pending_stream->capture_count!=n||!o->before[o->offsets[i]+n-1].complete||
     o->before[o->offsets[i]+n-1].tick!=o->previous_tick) {o->failed=1;return 0;}
#if defined(_WIN32)
  if(!c->counter_status||!c->frequency_status)
#else
  if(!c->clock_observed||c->clock_status)
#endif
  {o->failed=1;return 0;}
  if(!c->tick||c->tick<o->previous_tick||c->tick>=o->expires||!source(o)) {o->failed=1;return 0;}
  /* Genuine diagnostic clock remains observed even on native failure. */
  o->previous_tick=c->tick;if(!native_ok){o->failed=1;return 0;}
  o->pending_capture=0;o->pending_stream=0;o->in_flight=0;return 1;
}
#endif