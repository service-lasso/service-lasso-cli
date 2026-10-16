#include "credential_factory_linux.h"
#if defined(__linux__)
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
static size_t stream_bytes(void) {
  size_t n=sizeof(struct slcli_store_stream),a=_Alignof(slcli_input_capture);
  return (n+a-1)/a*a+68*sizeof(slcli_input_capture);
}
static int source(struct slcli_management_credential_linux_factory *f) {
  struct slcli_management_credential_linux_creation *c;
  struct slcli_management_credential_acquisition *a;
  struct slcli_management_credential_persistence *p;
  struct slcli_store_stream *streams[3];size_t i,j,n=stream_bytes();
  if(!f||!f->prepared||!f->arena||!slcli_arena_live_block(f->arena,f,sizeof(*f)))return 0;
  c=f->creation;a=f->acquisition;p=f->persistence;
  if(!c||!a||!p||!slcli_arena_live_block(f->arena,c,sizeof(*c))||
     !slcli_arena_live_block(f->arena,a,sizeof(*a))||!slcli_arena_live_block(f->arena,p,sizeof(*p))||
     c->arena!=f->arena||a->arena!=f->arena||p->arena!=f->arena||!c->prepared||!a->prepared||!p->prepared||
     p->acquisition!=a||c->deadline!=a->deadline||c->origin!=a->origin||
     c->caller_original!=a->caller_original||c->caller_bytes!=a->caller_bytes||
     c->expires!=a->expires||c->origin_tick!=a->origin_tick||
     p->capture_sink!=c->sink||p->capture_original!=c->capture_original||
     p->capture_original_bytes!=c->capture_original_bytes||
     !slcli_source_deadline_original_matches(f->arena,c->deadline,c->caller_original)||
     !away(f,sizeof(*f),c,sizeof(*c))||!away(f,sizeof(*f),a,sizeof(*a))||
     !away(f,sizeof(*f),p,sizeof(*p))||!away(c,sizeof(*c),a,sizeof(*a))||
     !away(c,sizeof(*c),p,sizeof(*p))||!away(a,sizeof(*a),p,sizeof(*p))||
     !away(f,sizeof(*f),c->caller_original,c->caller_bytes)||
     !away(f,sizeof(*f),c->capture_original,c->capture_original_bytes)||
     !away(f,sizeof(*f),c->name_original,c->name_bytes)||
     !slcli_source_deadline_away(c->deadline,f,sizeof(*f)))return 0;
  streams[0]=p->write;streams[1]=p->first;streams[2]=p->second;
  for(i=0;i<3;i++) {
    if(!streams[i]||!slcli_arena_live_block(f->arena,streams[i],n)||
       !away(streams[i],n,c,sizeof(*c))||!away(streams[i],n,a,sizeof(*a))||
       !away(streams[i],n,p,sizeof(*p))||!away(streams[i],n,f,sizeof(*f))||
       !away(streams[i],n,c->name_original,c->name_bytes)||
       streams[i]->captures!=(slcli_input_capture *)((unsigned char *)streams[i]+n-68*sizeof(slcli_input_capture))||
       streams[i]->capture_capacity!=68||streams[i]->unit_geometry||
       streams[i]->unit_chunk||streams[i]->unit_readback)return 0;
    for(j=0;j<i;j++)if(!away(streams[i],n,streams[j],n))return 0;
  }
  if(f->birth_bound&&(!p->original_bound||p->original!=c->created_original||
     !c->complete||c->unresolved||!c->file_created||!c->descriptor_acquired))return 0;
  return 1;
}
struct slcli_management_credential_linux_factory *slcli_management_credential_linux_factory_allocate(
  struct slcli_arena *arena,struct slcli_management_credential_linux_creation *c,
  struct slcli_management_credential_acquisition *a) {
  struct slcli_management_credential_linux_factory *f;
  if(!arena||!c||!a||!slcli_arena_live_block(arena,c,sizeof(*c))||
     !slcli_arena_live_block(arena,a,sizeof(*a))||c->arena!=arena||a->arena!=arena||
     c->started||c->unresolved||a->started||a->unresolved||a->predecessor_bound||
     c->deadline!=a->deadline||c->caller_original!=a->caller_original)return 0;
  f=slcli_arena_calloc(arena,1,sizeof(*f));if(!f)return 0;
  f->arena=arena;f->creation=c;f->acquisition=a;
  f->persistence=slcli_management_credential_persistence_prepare(arena,a,c->sink,
    c->capture_original,c->capture_original_bytes);
  if(!f->persistence)return 0;f->prepared=1;if(!source(f))return 0;return f;
}
int slcli_management_credential_linux_factory_original_matches(struct slcli_management_credential_linux_factory *f) {
  return source(f);
}
int slcli_management_credential_linux_factory_run(struct slcli_management_credential_linux_factory *f) {
  struct slcli_management_credential_linux_creation *c;
  struct slcli_management_credential_acquisition *a;
  struct slcli_management_credential_persistence *p;
  if(!source(f)||f->started||f->unresolved)return 0;
  c=f->creation;a=f->acquisition;p=f->persistence;
  if(c->started||c->unresolved||a->started||a->unresolved||a->predecessor_bound||
     p->started||p->unresolved||p->original_bound||
     p->write->attempted||p->first->attempted||p->second->attempted||
     p->write->failed||p->first->failed||p->second->failed||
     p->write->capture_count||p->first->capture_count||p->second->capture_count)return 0;
  f->started=1;f->unresolved=1;
  if(!slcli_management_credential_linux_create(c)||!source(f)||
     !c->complete||c->unresolved||!c->file_created||!c->descriptor_acquired||
     !c->clocks[9].complete||c->previous_tick!=c->clocks[9].tick)return 0;
  /* Bind only the actual once-created handle. No fd/path/digest assertion
   * from incoming data and no post-birth allocation or replacement stream. */
  p->original=c->created_original;p->original_bound=1;f->birth_bound=1;
  if(!slcli_management_credential_acquire_after(a,&c->clocks[9])||
     a->predecessor!=&c->clocks[9]||!slcli_management_credential_acquire(a)||
     !source(f)||!slcli_management_credential_persist(p)||!source(f)||
     p->original!=c->created_original||!p->write->capture_count||
     !slcli_store_original_same(&c->created,&p->write->captures[0])||
     !slcli_store_original_same(&c->created,&p->first->captures[0])||
     !slcli_store_original_same(&c->created,&p->second->captures[0]))return 0;
  f->complete=1;f->unresolved=0;return 1;
}
int slcli_management_credential_linux_factory_native_positive(struct slcli_management_credential_linux_factory *f) {
  uint64_t tick,status;size_t n;int fd;
  if(!f||!slcli_management_credential_linux_factory_run(f)||!f->complete||f->unresolved||
     !f->birth_bound||!f->persistence->complete||
     f->acquisition->predecessor!=&f->creation->clocks[9])return 0;
  tick=f->persistence->previous_tick;status=f->creation->open_status;
  n=f->persistence->second->capture_count;fd=f->creation->created_original;
  if(slcli_management_credential_linux_factory_run(f))return 0;
  return f->complete&&!f->unresolved&&f->persistence->previous_tick==tick&&
    f->creation->open_status==status&&f->creation->created_original==fd&&
    f->persistence->second->capture_count==n;
}
#endif