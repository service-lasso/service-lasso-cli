#include "ha2_large_consume.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
struct slcli_ha2_large_consumer *slcli_ha2_large_consumer_allocate(struct slcli_arena *a,
  const size_t rows[2],const struct slcli_json_ha2_plan *plan) {
  struct slcli_ha2_large_consumer *c;struct slcli_json_ha2_plan p;size_t i,counts[2];
  if(!a||!rows||!plan||!plan->tokens||!plan->members||!plan->array_items||
     plan->tokens>1048576||plan->members>1048576||plan->array_items>1048576)return 0;
  p=*plan;counts[0]=rows[0];counts[1]=rows[1];
  c=slcli_arena_calloc(a,1,sizeof(*c));if(!c)return 0;
  c->arena=a;c->failed=1;c->plan=p;
  /* All both-copy/capture storage exists before native body effects. Failed
   * allocations remain charged; nothing is freed or relabelled service fit. */
  for(i=0;i<2;i++) {
    c->ranges[i]=slcli_unit19_derivation_range_allocate(a,(uint32_t)(3+i),counts[i]);
    if(!c->ranges[i]||c->ranges[i]->failed)return 0;
  }
  c->failed=0;return c;
}
int slcli_ha2_large_consumer_bind(struct slcli_ha2_large_consumer *c,
  const struct slcli_unit19_reader *unit,const struct slcli_source_deadline_original *d,
  const void *caller,size_t caller_bytes,const struct slcli_native_clock *prior,
  slcli_store_capture_sink sink,void *capture,size_t capture_bytes) {
  size_t i;
  if(!c||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||
     c->prepared||c->started||c->failed||!unit||unit->type!=SLCLI_HA2_DERIVE19||
     !unit->complete||unit->failed||unit->count!=12||unit->next!=12||
     unit->next_offset!=unit->file_bytes||unit->file_bytes>1951296||
     !slcli_arena_live_range(c->arena,unit,sizeof(*unit))||
     !away(c,sizeof(*c),unit,sizeof(*unit))||!away(c,sizeof(*c),caller,caller_bytes)||
     !away(c,sizeof(*c),capture,capture_bytes)||!away(c,sizeof(*c),prior,sizeof(*prior))||
     !away(unit,sizeof(*unit),capture,capture_bytes)||
     !slcli_source_deadline_original_matches(c->arena,d,caller)||
     !slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
  c->failed=1;c->unit=unit;c->deadline=d;c->caller_original=caller;c->caller_bytes=caller_bytes;
  for(i=0;i<2;i++)if(!slcli_unit19_range_deadline_bind(c->ranges[i],d,caller,caller_bytes,
       prior,sink,capture,capture_bytes))return 0;
  c->prepared=1;c->failed=0;return 1;
}
static int current(const struct slcli_ha2_large_consumer *c) {
  return c&&c->arena&&slcli_arena_live_block(c->arena,c,sizeof(*c))&&c->prepared&&
    slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller_original)&&
    c->ranges[0]&&c->ranges[1]&&
    slcli_arena_live_block(c->arena,c->ranges[0],sizeof(*c->ranges[0]))&&
    slcli_arena_live_block(c->arena,c->ranges[1],sizeof(*c->ranges[1]))&&
    c->ranges[0]->copies&&c->ranges[1]->copies&&
    slcli_arena_live_range(c->arena,c->ranges[0]->copies,sizeof(*c->ranges[0]->copies))&&
    slcli_arena_live_range(c->arena,c->ranges[1]->copies,sizeof(*c->ranges[1]->copies))&&
    c->ranges[0]->copies->deadline_owner&&c->ranges[1]->copies->deadline_owner&&
    c->ranges[0]->copies->deadline_owner->deadline==c->deadline&&
    c->ranges[1]->copies->deadline_owner->deadline==c->deadline&&
    c->ranges[0]->copies->deadline_owner->caller_original==c->caller_original&&
    c->ranges[1]->copies->deadline_owner->caller_original==c->caller_original&&
    c->ranges[0]->copies->deadline_owner->caller_bytes==c->caller_bytes&&
    c->ranges[1]->copies->deadline_owner->caller_bytes==c->caller_bytes&&
    c->ranges[0]->copies->deadline_owner->capture_original==c->ranges[1]->copies->deadline_owner->capture_original&&
    c->ranges[0]->copies->deadline_owner->capture_original_bytes==c->ranges[1]->copies->deadline_owner->capture_original_bytes&&
    slcli_input_deadline_ready(c->ranges[0]->copies)&&slcli_input_deadline_ready(c->ranges[1]->copies);
}
int slcli_ha2_large_consume(struct slcli_ha2_large_consumer *c,slcli_input_handle original) {
  struct slcli_input_body *a,*b;struct slcli_input_deadline_owner *first,*second;int decoded;
  if(!current(c)||c->started||c->failed||c->structural_view_decoded)return 0;
  c->started=1;c->failed=1;a=c->ranges[0]->copies;b=c->ranges[1]->copies;
  first=a->deadline_owner;second=b->deadline_owner;
  if(!slcli_unit19_range_read(c->ranges[0],c->unit,3,original)||!slcli_input_deadline_ready(a))return 0;
  /* A genuine extra native clock bridges the actual observed first postclock;
   * no synthetic slcli_native_clock is built from a capture's numeric tick. */
  if(!current(c)||!slcli_native_clock_read(&c->bridge)||c->bridge.tick<first->previous_tick||
     c->bridge.tick>=first->expires||!slcli_input_deadline_precede(second,&c->bridge))return 0;
  if(!slcli_unit19_range_read(c->ranges[1],c->unit,4,original)||!slcli_input_deadline_ready(b)||
     !current(c)||!slcli_input_deadline_ready(a))return 0;
  if(!slcli_native_clock_read(&c->parse_before)||c->parse_before.tick<second->previous_tick||
     c->parse_before.tick>=second->expires||!current(c))return 0;
  decoded=slcli_ha2_large_members_view_decode(a->body,a->bytes,b->body,b->bytes,&c->plan,&c->view);
  /* Retain real post-parse clock on both successful and failed decoder paths. */
  if(!slcli_native_clock_read(&c->parse_after)||c->parse_after.tick<c->parse_before.tick||
     c->parse_after.tick>=second->expires||!current(c)||!decoded)return 0;
  c->structural_view_decoded=1;c->failed=0;return 1;
}
#endif
