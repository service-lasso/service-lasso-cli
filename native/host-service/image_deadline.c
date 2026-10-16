#include "image_deadline.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int source(const struct slcli_image_deadline_owner *o) {
  const struct slcli_image_reader *r;
  return o&&o->prepared&&o->arena&&o->capacity&&o->capacity<=SIZE_MAX/sizeof(*o->captures)&&
    o->capacity<=SIZE_MAX/sizeof(*o->clocks)&&
    slcli_arena_live_block(o->arena,o,sizeof(*o))&&
    (r=o->reader)&&slcli_arena_live_range(o->arena,r,sizeof(*r))&&
    slcli_arena_live_range(o->arena,o->caller,o->caller_bytes)&&
    slcli_arena_live_range(o->arena,o->capture_owner,o->capture_owner_bytes)&&
    slcli_arena_live_range(o->arena,o->captures,o->capacity*sizeof(*o->captures))&&
    slcli_arena_live_block(o->arena,o->clocks,o->capacity*sizeof(*o->clocks))&&
    slcli_arena_live_range(o->arena,o->workspace,sizeof(*o->workspace))&&
    (!o->birth_cell||slcli_arena_live_range(o->arena,o->birth_producer,o->birth_producer_bytes))&&
    slcli_source_deadline_original_matches(o->arena,o->deadline,o->caller)&&
    o->deadline->origin==o->origin&&o->deadline->expires==o->expires&&
    o->origin->tick==o->origin_tick&&r->deadline_owner==o&&r->original==o->original&&
    (!o->birth_joined||(o->birth_cell&&*o->birth_cell==o->original))&&
    r->workspace==o->workspace&&r->captures==o->captures&&r->capture_capacity==o->capacity&&
    r->capture_sink==o->sink&&r->capture_original==o->capture_owner;
}
int slcli_image_deadline_ready(const struct slcli_image_reader *r) {
  const struct slcli_image_deadline_owner *o=r?r->deadline_owner:0;
  return source(o)&&!o->failed&&!o->inflight&&o->used<=o->capacity&&
    o->previous_tick>=o->origin_tick&&o->previous_tick<o->expires;
}
static int bind(struct slcli_arena *a,struct slcli_image_reader *r,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  void *capture_owner,size_t capture_bytes,uint64_t predecessor,slcli_input_handle *birth,
  const void *producer,size_t producer_bytes) {
  struct slcli_image_deadline_owner *o;const void *p[7];size_t n[7],i,j;
  if(!a||!r||r->deadline_owner||r->used||r->failed||r->complete||r->offset||r->bytes||
    r->before||r->after||r->capture_count||!r->workspace||!r->captures||
    !r->capture_capacity||r->capture_capacity>SIZE_MAX/sizeof(*r->captures)||
    r->capture_capacity>SIZE_MAX/sizeof(struct slcli_native_clock)||!r->capture_sink||
    r->capture_original!=capture_owner||!caller_bytes||!capture_bytes||
    !slcli_arena_live_range(a,r,sizeof(*r))||!slcli_arena_live_range(a,caller,caller_bytes)||
    !slcli_arena_live_range(a,capture_owner,capture_bytes)||
    !slcli_arena_live_range(a,r->workspace,sizeof(*r->workspace))||
    !slcli_arena_live_range(a,r->captures,r->capture_capacity*sizeof(*r->captures))||
    !slcli_source_deadline_original_matches(a,d,caller)||predecessor<d->origin->tick||
    predecessor>=d->expires)return 0;
  p[0]=r;n[0]=sizeof(*r);p[1]=caller;n[1]=caller_bytes;p[2]=capture_owner;n[2]=capture_bytes;
  p[3]=r->workspace;n[3]=sizeof(*r->workspace);p[4]=r->captures;n[4]=r->capture_capacity*sizeof(*r->captures);
  p[5]=d;n[5]=sizeof(*d);p[6]=d->origin;n[6]=sizeof(*d->origin);
  for(i=0;i<7;i++)for(j=i+1;j<7;j++)if(!away(p[i],n[i],p[j],n[j]))return 0;
  if(birth) {
    uintptr_t cell=(uintptr_t)birth,base=(uintptr_t)producer;
    if(!producer||!producer_bytes||!slcli_arena_live_range(a,producer,producer_bytes)||
      !away(producer,producer_bytes,r,sizeof(*r))||
      !away(producer,producer_bytes,caller,caller_bytes)||
      !away(producer,producer_bytes,capture_owner,capture_bytes)||
      !slcli_source_deadline_away(d,producer,producer_bytes)||
      cell<base||cell-base>producer_bytes||producer_bytes-(cell-base)<sizeof(*birth)||
      cell%_Alignof(slcli_input_handle))return 0;
    for(i=0;i<7;i++)if(!away(producer,producer_bytes,p[i],n[i]))return 0;
#if defined(_WIN32)
    if(*birth||r->original)return 0;
#else
    if(*birth!=-1||r->original!=-1)return 0;
#endif
  }
  o=slcli_arena_calloc(a,1,sizeof(*o));if(!o)return 0;
  o->clocks=slcli_arena_calloc(a,r->capture_capacity,sizeof(*o->clocks));if(!o->clocks)return 0;
  for(i=0;i<7;i++)if(!away(o,sizeof(*o),p[i],n[i])||
    !away(o->clocks,r->capture_capacity*sizeof(*o->clocks),p[i],n[i]))return 0;
  if(!away(o,sizeof(*o),o->clocks,r->capture_capacity*sizeof(*o->clocks)))return 0;
  o->arena=a;o->reader=r;o->deadline=d;o->origin=d->origin;o->caller=caller;
  o->caller_bytes=caller_bytes;o->capture_owner=capture_owner;o->capture_owner_bytes=capture_bytes;
  o->workspace=r->workspace;o->captures=r->captures;o->sink=r->capture_sink;o->capacity=r->capture_capacity;
  o->original=r->original;o->expires=d->expires;o->origin_tick=d->origin->tick;
  o->birth_cell=birth;o->birth_producer=producer;o->birth_producer_bytes=producer_bytes;o->unborn=birth!=0;
  o->previous_tick=predecessor;o->prepared=1;r->deadline_owner=o;
  return slcli_image_deadline_ready(r);
}
int slcli_image_deadline_bind(struct slcli_arena *a,struct slcli_image_reader *r,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  void *capture_owner,size_t capture_bytes,uint64_t predecessor) {
  return bind(a,r,d,caller,caller_bytes,capture_owner,capture_bytes,predecessor,0,0,0);
}
int slcli_image_deadline_prepare_unborn(struct slcli_arena *a,struct slcli_image_reader *r,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  void *capture_owner,size_t capture_bytes,uint64_t predecessor,slcli_input_handle *birth,
  const void *producer,size_t producer_bytes) {
  return birth&&bind(a,r,d,caller,caller_bytes,capture_owner,capture_bytes,predecessor,birth,producer,producer_bytes);
}
int slcli_image_deadline_precede(struct slcli_image_reader *r,const struct slcli_native_clock *clock) {
  struct slcli_image_deadline_owner *o=r?r->deadline_owner:0;
  if(!slcli_image_deadline_ready(r)||r->used||o->used||!clock||!clock->complete||
    !slcli_arena_live_range(o->arena,clock,sizeof(*clock))||clock->tick<o->previous_tick)return 0;
  o->previous_tick=clock->tick;
  if(clock->tick>=o->expires){o->failed=1;return 0;}return source(o);
}
int slcli_image_deadline_born(struct slcli_image_reader *r,slcli_input_handle *birth,
  const struct slcli_native_clock *clock) {
  struct slcli_image_deadline_owner *o=r?r->deadline_owner:0;
  if(!slcli_image_deadline_ready(r)||!o->unborn||o->birth_joined||o->birth_cell!=birth||
    !birth)return 0;
  o->birth_joined=1; /* One actual producer output join, including failure. */
#if defined(_WIN32)
  if(!*birth||*birth==INVALID_HANDLE_VALUE){o->failed=1;return 0;}
#else
  if(*birth<0){o->failed=1;return 0;}
#endif
  /* Successful native object birth precedes diagnostic time. A late clock
   * cannot make that original handle unborn or permit replacing its cell. */
  o->original=*birth;r->original=*birth;o->unborn=0;
  if(!slcli_image_deadline_precede(r,clock)){o->failed=1;return 0;}
  return slcli_image_deadline_ready(r);
}
int slcli_image_deadline_before(struct slcli_image_reader *r,slcli_input_capture *c) {
  struct slcli_image_deadline_owner *o=r?r->deadline_owner:0;struct slcli_native_clock *clock;
  if(!slcli_image_deadline_ready(r)||o->unborn||!c||o->used>=o->capacity||
    !r->capture_count||r->capture_count!=o->used+1||c!=&o->captures[o->used])return 0;
  o->active=o->used++;o->inflight=1;clock=&o->clocks[o->active];
  if(!slcli_native_clock_read(clock)||clock->tick<o->previous_tick){o->failed=1;return 0;}
  o->previous_tick=clock->tick;
  if(clock->tick>=o->expires||!source(o)){o->failed=1;return 0;}return 1;
}
int slcli_image_deadline_after(struct slcli_image_reader *r,slcli_input_capture *c,int native_ok) {
  struct slcli_image_deadline_owner *o=r?r->deadline_owner:0;
  if(!source(o)||!o->inflight||o->active>=o->used||c!=&o->captures[o->active])return 0;
  /* Actual file adapter preserves its raw return and native postclock before
   * this call. The raw capture/body sink must have run even on native failure. */
  if(c->tick<o->previous_tick||!c->tick){o->failed=1;return 0;}
  o->previous_tick=c->tick;
  if(!native_ok||c->tick>=o->expires){o->failed=1;return 0;}
  o->inflight=0;return 1;
}
#endif
