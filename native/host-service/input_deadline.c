#include "input_deadline.h"
#if defined(_WIN32)
#include "bq_source_control_sink_windows.h"
#include "bq_serialized_factory_windows.h"
#endif
#if defined(_WIN32) || defined(__linux__)
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
static size_t context_offset(void) {
  size_t a=_Alignof(struct slcli_native_clock),n=sizeof(struct slcli_input_deadline_owner);
  return (n+a-1)/a*a;
}
static size_t body_bytes(const struct slcli_input_body *b) {
  size_t n,a=_Alignof(slcli_input_capture);
  if(!b||!b->capacity||b->capacity!=b->readback_capacity||
     b->capacity>(SIZE_MAX-sizeof(*b))/2||b->capture_capacity>SIZE_MAX/sizeof(*b->captures))return 0;
  n=sizeof(*b)+2*b->capacity;if(n>SIZE_MAX-(a-1))return 0;n=(n+a-1)/a*a;
  if(b->capture_capacity*sizeof(*b->captures)>SIZE_MAX-n)return 0;
  if(b->body!=(unsigned char *)b+sizeof(*b)||b->readback!=b->body+b->capacity||
     b->captures!=(slcli_input_capture *)((unsigned char *)b+n))return 0;
  return n+b->capture_capacity*sizeof(*b->captures);
}
static int source(struct slcli_input_deadline_owner *o) {
  size_t n,total;
  if(!o||!o->prepared||!o->arena||!o->body||!o->row_capacity||
     o->row_capacity>SIZE_MAX/sizeof(*o->before))return 0;
  /* Attached BQ dispatch must use the actual fixed caller/issuing-thread and
   * current block-row join. That coupled entry is still unfinished: do not
   * turn constructed storage into a generic cross-thread native IO grant. */
  if(!slcli_memory_owner_unattached(o->arena->owner))return 0;
  n=context_offset();if(o->row_capacity*sizeof(*o->before)>SIZE_MAX-n)return 0;
  total=n+o->row_capacity*sizeof(*o->before);
  return slcli_arena_live_block(o->arena,o,total)&&
    o->before==(struct slcli_native_clock *)((unsigned char *)o+n)&&
    o->row_capacity==o->body->capture_capacity&&o->next<=o->row_capacity&&
    body_bytes(o->body)==o->body_storage_bytes&&
    slcli_arena_live_block(o->arena,o->body,o->body_storage_bytes)&&o->body->deadline_owner==o&&
    slcli_arena_live_block(o->arena,o->caller_original,o->caller_bytes)&&
    slcli_arena_live_block(o->arena,o->capture_original,o->capture_original_bytes)&&
    slcli_source_deadline_original_matches(o->arena,o->deadline,o->caller_original)&&
    o->deadline->origin==o->origin&&o->deadline->expires==o->expires&&
    o->origin->tick==o->origin_tick&&o->predecessor&&
    slcli_arena_live_range(o->arena,o->predecessor,sizeof(*o->predecessor))&&
    o->predecessor->complete&&o->predecessor->tick==o->predecessor_tick&&
    o->predecessor_tick>=o->origin_tick&&o->previous_tick>=o->predecessor_tick&&o->sink;
}
struct slcli_input_deadline_owner *slcli_input_deadline_bind(struct slcli_arena *a,
  struct slcli_input_body *b,const struct slcli_source_deadline_original *d,const void *caller,size_t bytes,
  const struct slcli_native_clock *prior,slcli_store_capture_sink sink,void *owner,size_t owner_bytes) {
  struct slcli_input_deadline_owner *o;size_t total,n,storage;
  if(!a||!a->owner||!b||!(storage=body_bytes(b))||!slcli_arena_live_block(a,b,storage)||
     b->started||b->failed||b->capture_count||b->deadline_owner||!sink||!bytes||!owner_bytes||
     !slcli_arena_live_block(a,caller,bytes)||!slcli_arena_live_block(a,owner,owner_bytes)||
     !slcli_source_deadline_original_matches(a,d,caller)||!prior||
     !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
     prior->tick<d->origin->tick||prior->tick>=d->expires||
     !away(b,storage,caller,bytes)||!away(b,storage,owner,owner_bytes)||
     !away(b,storage,prior,sizeof(*prior))||!away(caller,bytes,owner,owner_bytes)||
     !away(caller,bytes,prior,sizeof(*prior))||!away(owner,owner_bytes,prior,sizeof(*prior))||
     !slcli_source_deadline_away(d,b,storage)||!slcli_source_deadline_away(d,caller,bytes)||
     !slcli_source_deadline_away(d,owner,owner_bytes)||
     !away(b,storage,a,sizeof(*a))||!away(b,storage,a->owner,sizeof(*a->owner))||
     !away(owner,owner_bytes,a,sizeof(*a))||!away(owner,owner_bytes,a->owner,sizeof(*a->owner))||
     b->capture_capacity>SIZE_MAX/sizeof(struct slcli_native_clock))return 0;
  n=context_offset();if(b->capture_capacity*sizeof(struct slcli_native_clock)>SIZE_MAX-n)return 0;
  total=n+b->capture_capacity*sizeof(struct slcli_native_clock);o=slcli_arena_calloc(a,1,total);if(!o)return 0;
  if(!away(o,total,b,storage)||!away(o,total,caller,bytes)||!away(o,total,owner,owner_bytes)||
     !away(o,total,prior,sizeof(*prior))||!slcli_source_deadline_away(d,o,total)||
     !away(o,total,a,sizeof(*a))||!away(o,total,a->owner,sizeof(*a->owner)))return 0;
  o->arena=a;o->body=b;o->body_storage_bytes=storage;o->deadline=d;o->origin=d->origin;
  o->caller_original=caller;o->caller_bytes=bytes;o->sink=sink;o->capture_original=owner;
  o->capture_original_bytes=owner_bytes;o->predecessor=prior;o->predecessor_tick=prior->tick;
  o->expires=d->expires;o->origin_tick=d->origin->tick;o->previous_tick=prior->tick;
  o->row_capacity=b->capture_capacity;o->before=(void *)((unsigned char *)o+n);
  o->prepared=1;b->deadline_owner=o;if(!source(o)){o->failed=1;return 0;}return o;
}
int slcli_input_deadline_precede(struct slcli_input_deadline_owner *o,
  const struct slcli_native_clock *prior) {
  size_t bytes;
  if(!source(o)||o->failed||o->in_flight||o->next||o->body->started||o->predecessor_advanced||
     !prior||!slcli_arena_live_range(o->arena,prior,sizeof(*prior))||!prior->complete||
     prior->tick<o->previous_tick||prior->tick>=o->expires)return 0;
  bytes=context_offset()+o->row_capacity*sizeof(*o->before);
  if(!away(o,bytes,prior,sizeof(*prior))||!away(o->body,o->body_storage_bytes,prior,sizeof(*prior))||
     !away(o->caller_original,o->caller_bytes,prior,sizeof(*prior))||
     !away(o->capture_original,o->capture_original_bytes,prior,sizeof(*prior)))return 0;
  o->predecessor=prior;o->predecessor_tick=prior->tick;o->previous_tick=prior->tick;
  o->predecessor_advanced=1;return 1;
}
int slcli_input_deadline_ready(const struct slcli_input_body *b) {
  struct slcli_input_deadline_owner *o;
  return b&&(o=b->deadline_owner)&&source(o)&&o->body==b&&!o->failed&&!o->in_flight&&
    !o->pending&&o->next==b->capture_count&&o->previous_tick<o->expires;
}
int slcli_input_deadline_before(struct slcli_input_body *b,slcli_input_capture *c) {
  struct slcli_input_deadline_owner *o;struct slcli_native_clock *r;
  if(!b)return 0;if(!(o=b->deadline_owner))return 1; /* Transitional unbound reader is not admission. */
  if(!source(o)||o->failed||o->in_flight||o->next>=o->row_capacity||
     c!=&b->captures[o->next]||b->capture_count!=o->next+1||c->operation)return 0;
  o->in_flight=1;o->pending=c;r=&o->before[o->next];
  if(!slcli_native_clock_read(r)||r->tick<o->previous_tick||r->tick>=o->expires||!source(o)) {
    o->failed=1;return 0;
  }
  o->previous_tick=r->tick;
#if defined(_WIN32)
  if(o->source_fixed_caller) {
    /* The enclosing source gate remains closed until fixed source admission
     * is complete. This exact one-use native branch cannot open it. */
    if(!slcli_bq_serialized_factory_windows_body_storage_matches(o)||
       c->source_thread_called||c->source_thread_returned||c->source_thread_raw||
       c->source_thread_after.started||c->source_thread_clock_returned||
       c->source_thread_clock_result) {o->failed=1;return 0;}
    c->source_thread_called=1;c->source_thread_raw=GetCurrentThreadId();
    c->source_thread_returned=1;
    c->source_thread_clock_result=slcli_native_clock_read(&c->source_thread_after);
    c->source_thread_clock_returned=1;
    if(!c->source_thread_clock_result||
       !slcli_native_clock_windows_matches_original(&c->source_thread_after)||
       c->source_thread_after.tick<r->tick||
       c->source_thread_after.tick>=o->expires||!c->source_thread_raw||
       !slcli_bq_serialized_factory_windows_body_storage_matches(o)||
       c->source_thread_raw!=o->source_fixed_caller->issuing_thread) {
      o->failed=1;return 0;
    }
    o->previous_tick=c->source_thread_after.tick;
  }
#endif
  return 1;
}
int slcli_input_deadline_after(struct slcli_input_body *b,slcli_input_capture *c,
  const unsigned char *raw,size_t requested,int native_ok) {
  struct slcli_input_deadline_owner *o;size_t bytes=0;int observed;
  if(!b)return 0;if(!(o=b->deadline_owner))return native_ok;
  if(!source(o)||o->failed||!o->in_flight||o->pending!=c||c!=&b->captures[o->next])return 0;
#if defined(_WIN32)
  observed=c->bytes_observed!=0;
#else
  observed=c->returned_bytes_observed!=0;
#endif
  /* Actual native returned body survives an overall clock/helper failure.
   * Unobserved/invalid counts are never converted into invented empty success. */
  if(observed&&c->returned_bytes<=requested&&c->returned_bytes<=SIZE_MAX)bytes=(size_t)c->returned_bytes;
#if defined(_WIN32)
  if(o->sink==slcli_bq_source_control_sink_windows_write) {
    if(!slcli_bq_source_control_sink_windows_write_owned(o,c,bytes?raw:0,bytes))
      {o->failed=1;return 0;}
  } else
#endif
  if(!o->sink(o->capture_original,c,bytes?raw:0,bytes)) {o->failed=1;return 0;}
#if defined(_WIN32)
  if(!c->counter_status||!c->frequency_status)
#else
  if(!c->clock_observed||c->clock_status)
#endif
  {o->failed=1;return 0;}
  if(!c->tick||c->tick<o->previous_tick||c->tick>=o->expires||!source(o)) {o->failed=1;return 0;}
  /* Genuine completed diagnostic clock is retained even when native IO or
   * returned-count validation failed; no pending native state is cleared. */
  o->previous_tick=c->tick;
  if(!native_ok||(observed&&c->returned_bytes>requested)){o->failed=1;return 0;}
  o->next++;o->pending=0;o->in_flight=0;return 1;
}
#endif
