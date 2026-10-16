#include "store_publish_deadline.h"
#include "ledger_commit.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int contained(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n>=m&&m&&x<=UINTPTR_MAX-n&&y>=x&&y<=x+n-m;
}
static unsigned limit(void) {
#if defined(_WIN32)
  return 5;
#else
  return 8;
#endif
}
static int source(const struct slcli_store_publish_deadline_owner *o) {
  unsigned i;
  if(!o||!o->prepared||!o->arena||!slcli_arena_live_block(o->arena,o,sizeof(*o))||
     !slcli_arena_live_block(o->arena,o->container_original,o->container_bytes)||
     !contained(o->container_original,o->container_bytes,o->publish_original,sizeof(*o->publish_original))||
     o->publish_original->deadline_owner!=o||!o->stream_original||
     o->stream_original->arena!=o->arena||!o->sink||o->count>limit()||
     !slcli_store_deadline_away(o->stream_original,o,sizeof(*o))||
     !slcli_store_deadline_away(o->stream_original,o->container_original,o->container_bytes)||
     o->capture_original!=o->stream_original->capture_original||
     o->capture_original_bytes!=o->stream_original->capture_original_bytes||
     !slcli_arena_live_block(o->arena,o->capture_original,o->capture_original_bytes))return 0;
  for(i=0;i<o->count;i++)if(o->kinds[i]!=i+1u||
     (i+1u<o->count&&(!o->publish_original->effect_called[i]||
       !o->publish_original->effect_returned[i])))return 0;
  return 1;
}
static int bind(void *container,size_t bytes,struct slcli_store_publish *p,
  struct slcli_store_deadline_owner *s,slcli_namespace_capture_sink sink) {
  struct slcli_store_publish_deadline_owner *o;
  if(!s||!slcli_store_deadline_ready(s)||!container||!p||!sink||p->started||p->failed||
     p->completed||p->deadline_owner||!slcli_arena_live_block(s->arena,container,bytes)||
     !contained(container,bytes,p,sizeof(*p))||!slcli_store_deadline_away(s,container,bytes))return 0;
  o=slcli_arena_calloc(s->arena,1,sizeof(*o));if(!o)return 0;
  /* Retain the exact new owner even when later private construction fails. */
  p->deadline_owner=o;o->arena=s->arena;o->container_original=container;o->container_bytes=bytes;
  o->publish_original=p;o->stream_original=s;o->sink=sink;
  o->capture_original=s->capture_original;o->capture_original_bytes=s->capture_original_bytes;
  o->prepared=1;
  if(!away(o,sizeof(*o),container,bytes)||!source(o)){o->failed=1;return 0;}return 1;
}
int slcli_store_publish_deadline_bind_ledger(struct slcli_ledger_commit *c,
  slcli_namespace_capture_sink sink) {
  if(!c||!c->deadline_owner||c->started||c->failed||c->complete)return 0;
  return bind(c,sizeof(*c),&c->generation_publish,c->deadline_owner,sink)&&
    bind(c,sizeof(*c),&c->selector_publish,c->deadline_owner,sink);
}
int slcli_store_publish_deadline_bind_artifact(struct slcli_store_publish *p,
  struct slcli_store_deadline_owner *s,slcli_namespace_capture_sink sink) {
  return bind(p,sizeof(*p),p,s,sink);
}
int slcli_store_publish_deadline_before(struct slcli_store_publish *p,uint32_t kind) {
  struct slcli_store_publish_deadline_owner *o;
  /* Transitional unbound publisher supplies no original deadline admission. */
  if(!p)return 0;if(!(o=p->deadline_owner))return 1;
  if(!source(o)||o->failed||o->pending||o->count>=limit()||kind!=o->count+1u||
     p->effect_called[o->count]||p->effect_returned[o->count])return 0;
  o->kinds[o->count]=kind;o->count++;o->pending=1;
  if(!slcli_store_deadline_external_begin(o->stream_original,o,sizeof(*o),
       &o->clocks[2u*(o->count-1u)])){o->failed=1;return 0;}return 1;
}
int slcli_store_publish_deadline_after(struct slcli_store_publish *p,int native_ok) {
  struct slcli_store_publish_deadline_owner *o;
  if(!p)return 0;if(!(o=p->deadline_owner))return native_ok;
  if(!source(o)||o->failed||!o->pending||!o->count||
     !p->effect_called[o->count-1u]||!p->effect_returned[o->count-1u])return 0;
  /* Actual typed raw native args/outputs/observations reach original custody
   * before the diagnostic post-clock. Failed effects are never erased. */
  if(!o->sink(o->capture_original,p,o->kinds[o->count-1u])){o->failed=1;return 0;}
  if(!slcli_store_deadline_external_end(o->stream_original,o,sizeof(*o),
       &o->clocks[2u*(o->count-1u)+1u],native_ok)){o->failed=1;return 0;}
  o->pending=0;return 1;
}
void slcli_store_publish_deadline_fail(struct slcli_store_publish *p) {
  struct slcli_store_publish_deadline_owner *o;
  if(!p)return;p->failed=1;
  if(!(o=p->deadline_owner))return;
  /* No rollback, pending clear, native close or fresh clock follows failure. */
  o->failed=1;
  if(source(o))o->stream_original->failed=1;
}
int slcli_store_publish_deadline_ready(const struct slcli_store_publish *p) {
  const struct slcli_store_publish_deadline_owner *o;unsigned i;uint64_t prior;
  if(!p)return 0;if(!(o=p->deadline_owner))return 1;
  if(!source(o)||o->failed||o->pending||o->count!=limit()||
     !slcli_store_deadline_ready(o->stream_original))return 0;
  prior=o->stream_original->origin_tick;
  for(i=0;i<o->count;i++) {
    const struct slcli_native_clock *a=&o->clocks[2*i],*b=&o->clocks[2*i+1];
    if(!p->effect_called[i]||!p->effect_returned[i]||!a->complete||!a->observed||
       !b->complete||!b->observed||a->tick<prior||b->tick<a->tick||
       b->tick>=o->stream_original->expires)return 0;
    prior=b->tick;
  }
  return prior<=o->stream_original->previous_tick;
}
#endif