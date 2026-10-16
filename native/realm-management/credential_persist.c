#include "credential_persist.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
static int capture_before(void *,struct slcli_store_stream *,slcli_input_capture *);
static int capture_after(void *,const slcli_input_capture *,const unsigned char *,size_t,int);
static size_t stream_bytes(void) {
  size_t n=sizeof(struct slcli_store_stream),a=_Alignof(slcli_input_capture);
  return (n+a-1)/a*a+68*sizeof(slcli_input_capture);
}
static int stream_storage(struct slcli_management_credential_persistence *p,
  struct slcli_store_stream *s) {
  size_t n=stream_bytes(),offset=n-68*sizeof(slcli_input_capture);
  struct slcli_management_credential_acquisition *c=p->acquisition;
  return s&&slcli_arena_live_block(p->arena,s,n)&&
    s->captures==(slcli_input_capture *)((unsigned char *)s+offset)&&
    s->capture_capacity==68&&!s->unit_geometry&&!s->unit_chunk&&!s->unit_readback&&
    s->capture_before==capture_before&&s->before_original==p&&
    s->capture_after==capture_after&&s->after_original==p&&
    away(s,n,p,sizeof(*p))&&away(s,n,c,sizeof(*c))&&
    away(s,n,c->caller_original,c->caller_bytes)&&
    away(s,n,p->capture_original,p->capture_original_bytes)&&
    slcli_source_deadline_away(c->deadline,s,n);
}
static int source(struct slcli_management_credential_persistence *p) {
  struct slcli_management_credential_acquisition *c;
  if(!p||!p->prepared||!p->arena||!slcli_arena_live_block(p->arena,p,sizeof(*p)))return 0;
  c=p->acquisition;
  return c&&slcli_arena_live_block(p->arena,c,sizeof(*c))&&c->arena==p->arena&&c->prepared&&
    slcli_arena_live_block(p->arena,c->caller_original,c->caller_bytes)&&
    slcli_source_deadline_original_matches(p->arena,c->deadline,c->caller_original)&&
    c->deadline->origin==c->origin&&c->deadline->expires==c->expires&&
    c->origin->tick==c->origin_tick&&p->capture_sink&&p->capture_original_bytes&&
    slcli_arena_live_block(p->arena,p->capture_original,p->capture_original_bytes)&&
    away(p->capture_original,p->capture_original_bytes,p,sizeof(*p))&&
    away(p->capture_original,p->capture_original_bytes,c,sizeof(*c))&&
    away(p->capture_original,p->capture_original_bytes,c->caller_original,c->caller_bytes)&&
    slcli_source_deadline_away(c->deadline,p->capture_original,p->capture_original_bytes)&&
    stream_storage(p,p->write)&&stream_storage(p,p->first)&&stream_storage(p,p->second)&&
    away(p->write,stream_bytes(),p->first,stream_bytes())&&
    away(p->write,stream_bytes(),p->second,stream_bytes())&&
    away(p->first,stream_bytes(),p->second,stream_bytes());
}
static int clock_row(struct slcli_management_credential_persistence *p,size_t row) {
  struct slcli_native_clock *c=&p->clocks[row];
  if(!source(p)||!slcli_native_clock_read(c)||c->tick<p->previous_tick||
     c->tick>=p->acquisition->expires||!source(p))return 0;
  p->previous_tick=c->tick;return 1;
}
static int capture_before(void *context,struct slcli_store_stream *s,slcli_input_capture *c) {
  struct slcli_management_credential_persistence *p=context;
  struct slcli_store_stream *streams[3];struct slcli_native_clock *r;size_t i,n;
  if(!source(p)||!p->started||!p->unresolved||p->pending_capture||p->pending_stream||!c)return 0;
  streams[0]=p->write;streams[1]=p->first;streams[2]=p->second;
  for(i=0;i<3&&streams[i]!=s;i++);if(i==3)return 0;n=p->before_used[i];
  if(n>=68||s->capture_count!=n+1||c!=&s->captures[n]||c->operation)return 0;
  p->pending_capture=c;p->pending_stream=s;r=&p->before[i][n];p->before_used[i]++;
  if(!slcli_native_clock_read(r)||r->tick<p->previous_tick||
     r->tick>=p->acquisition->expires||!source(p))return 0;
  p->previous_tick=r->tick;return 1;
}
static int capture_row(void *context,const slcli_input_capture *c,
  const unsigned char *raw,size_t bytes) {
  struct slcli_management_credential_persistence *p=context;
  struct slcli_store_stream *streams[3];size_t i,n;
  /* Transfer the actual private row/body first, including partial/error output.
   * Expiry diagnostics cannot replace it by empty successful evidence. */
  if(!source(p)||!c||p->pending_capture!=c||!p->pending_stream||
     !p->capture_sink(p->capture_original,c,raw,bytes))return 0;
  streams[0]=p->write;streams[1]=p->first;streams[2]=p->second;
  for(i=0;i<3&&streams[i]!=p->pending_stream;i++);if(i==3)return 0;
  n=p->before_used[i];
  if(!n||n>68||p->pending_stream->capture_count!=n||
     c!=&p->pending_stream->captures[n-1]||!p->before[i][n-1].complete||
     p->before[i][n-1].tick!=p->previous_tick)return 0;
#if defined(_WIN32)
  if(!c->counter_status||!c->frequency_status||!c->tick)return 0;
#else
  if(!c->clock_observed||c->clock_status||!c->tick)return 0;
#endif
  /* Bound every completed file-capture transition, including native short IO,
   * before the stream can submit its next row. Blocking time inside a native
   * operation is still an unresolved qualification obligation. */
  if(!source(p)||c->tick<p->previous_tick||c->tick>=p->acquisition->expires)return 0;
  p->previous_tick=c->tick;return 1;
}
static int capture_after(void *context,const slcli_input_capture *c,
  const unsigned char *raw,size_t bytes,int native_ok) {
  struct slcli_management_credential_persistence *p=context;
  (void)raw;(void)bytes; /* Source capture sink and post-clock already completed. */
  if(!source(p)||!native_ok||p->pending_capture!=c||!p->pending_stream||
     !c->tick||p->previous_tick!=c->tick)return 0;
  p->pending_capture=0;p->pending_stream=0;return 1;
}
struct slcli_management_credential_persistence *slcli_management_credential_persistence_prepare(
  struct slcli_arena *a,struct slcli_management_credential_acquisition *c,
  slcli_store_capture_sink sink,void *owner,size_t owner_bytes) {
  struct slcli_management_credential_persistence *p;
  if(!a||!c||!slcli_arena_live_block(a,c,sizeof(*c))||c->arena!=a||!c->prepared||
     c->started||c->unresolved||!sink||!owner_bytes||
     !slcli_arena_live_block(a,owner,owner_bytes)||
     !away(owner,owner_bytes,c,sizeof(*c))||
     !away(owner,owner_bytes,c->caller_original,c->caller_bytes)||
     !slcli_source_deadline_away(c->deadline,owner,owner_bytes))return 0;

  p=slcli_arena_calloc(a,1,sizeof(*p));if(!p)return 0;
  if(!away(p,sizeof(*p),c,sizeof(*c))||
     !slcli_source_deadline_away(c->deadline,p,sizeof(*p))||
     !away(p,sizeof(*p),c->caller_original,c->caller_bytes))return 0;
  p->arena=a;p->acquisition=c;p->capture_sink=sink;p->capture_original=owner;
  p->capture_original_bytes=owner_bytes;
  p->write=slcli_store_stream_allocate(a,68);
  p->first=slcli_store_stream_allocate(a,68);
  p->second=slcli_store_stream_allocate(a,68);
  if(!p->write||!p->first||!p->second)return 0;
  p->write->capture_before=p->first->capture_before=p->second->capture_before=capture_before;
  p->write->before_original=p->first->before_original=p->second->before_original=p;
  p->write->capture_after=p->first->capture_after=p->second->capture_after=capture_after;
  p->write->after_original=p->first->after_original=p->second->after_original=p;
  p->prepared=1;
  if(!source(p))return 0;return p;
}
struct slcli_management_credential_persistence *slcli_management_credential_persistence_allocate(
  struct slcli_arena *a,struct slcli_management_credential_acquisition *c,
  slcli_input_handle original,slcli_store_capture_sink sink,void *owner,size_t owner_bytes) {
  struct slcli_management_credential_persistence *p;
#if defined(_WIN32)
  if(!original||original==INVALID_HANDLE_VALUE)return 0;
#else
  if(original<0)return 0;
#endif
  p=slcli_management_credential_persistence_prepare(a,c,sink,owner,owner_bytes);
  if(!p)return 0;p->original=original;p->original_bound=1;return p;
}
static int pass(struct slcli_management_credential_persistence *p,
  struct slcli_store_stream *s,uint32_t mode,size_t row) {
  if(!clock_row(p,row)||!slcli_store_stream_begin(s,p->original,mode,64,
       capture_row,p)||
     !slcli_store_stream_append(s,p->acquisition->secret,64)||
     !slcli_store_stream_finish(s))return 0;
  /* Stream-held facts remain true even if the following caller clock fails.
   * Never label a real durable write as unchanged old state or retry its file. */
  if(!mode)p->write_verified=1;
  else if(s==p->first)p->first_verified=1;
  else p->second_verified=1;
  return clock_row(p,row+1);
}
int slcli_management_credential_persistence_completed_lineage(
  const struct slcli_management_credential_persistence *p) {
  const struct slcli_store_stream *s[3];uint64_t previous;size_t i,j;
  if(!p||!source((struct slcli_management_credential_persistence *)p)||
     !p->prepared||!p->started||!p->acquisition||!p->acquisition->after.complete||
     p->pending_capture||p->pending_stream)return 0;
  s[0]=p->write;s[1]=p->first;s[2]=p->second;previous=p->acquisition->after.tick;
  for(i=0;i<3;i++) {
    if(!s[i]||!s[i]->held_verified||s[i]->failed||!s[i]->capture_count||
       s[i]->capture_count>68||p->before_used[i]!=s[i]->capture_count||
       !p->clocks[2*i].complete||p->clocks[2*i].tick<previous||
       p->clocks[2*i].tick>=p->acquisition->expires)return 0;
    previous=p->clocks[2*i].tick;
    for(j=0;j<s[i]->capture_count;j++) {
      if(!p->before[i][j].complete||p->before[i][j].tick<previous||
         p->before[i][j].tick>=p->acquisition->expires||!s[i]->captures[j].operation||
         !s[i]->captures[j].tick||s[i]->captures[j].tick<p->before[i][j].tick||
         s[i]->captures[j].tick>=p->acquisition->expires)return 0;
      previous=s[i]->captures[j].tick;
    }
    if(!p->clocks[2*i+1].complete||p->clocks[2*i+1].tick<previous||
       p->clocks[2*i+1].tick>=p->acquisition->expires)return 0;
    previous=p->clocks[2*i+1].tick;
  }
  return previous==p->previous_tick;
}
int slcli_management_credential_persist(struct slcli_management_credential_persistence *p) {
  struct slcli_management_credential_acquisition *c;
  if(!source(p)||!p->original_bound||p->started||p->unresolved)return 0;c=p->acquisition;
  if(!c->complete||c->unresolved||!c->native_called||!c->result_observed||
     !c->destination_retained||!c->count_observed||c->returned_bytes!=64||
     !c->after.complete)return 0;
  p->started=1;p->unresolved=1;p->previous_tick=c->after.tick;
  if(!pass(p,p->write,0,0)||!pass(p,p->first,1,2)||!pass(p,p->second,1,4)||
     !slcli_store_original_same(&p->write->captures[0],&p->first->captures[0])||
     !slcli_store_original_same(&p->write->captures[0],&p->second->captures[0])||
     !slcli_equal(p->write->digest,p->first->digest,32)||
     !slcli_equal(p->write->digest,p->second->digest,32)||
     !slcli_management_credential_persistence_completed_lineage(p))return 0;
  p->complete=1;p->unresolved=0;return 1;
}
#endif
