#include "root_foundation.h"
#if defined(_WIN32) || defined(__linux__)
static const uint32_t roles[5]={97,98,21,15,6};
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int source(const struct slcli_root_foundation *f) {
  size_t i;const slcli_root_creator *c;
  if(!f||!f->prepared||!f->arena||!f->roots||
    !slcli_arena_live_block(f->arena,f,sizeof(*f))||
    !slcli_arena_live_block(f->arena,f->roots,sizeof(*f->roots))||
    !slcli_arena_live_range(f->arena,f->original_creators,5*sizeof(*f->original_creators))||
    !slcli_source_deadline_original_matches(f->arena,f->deadline,f->caller)||
    f->deadline->origin!=f->origin||f->deadline->expires!=f->expires||
    f->origin->tick!=f->origin_tick||f->roots->deadline!=f->deadline||
    f->roots->caller_original!=f->caller||f->roots->caller_bytes!=f->caller_bytes)return 0;
  for(i=0;i<5;i++) {
    c=f->creators[i];
    if(c!=f->original_creators[i]||!c||!slcli_arena_live_block(f->arena,c,sizeof(*c))||
      c->arena!=f->arena||!c->prepared||c->original_role!=roles[i]||
      c->deadline!=f->deadline||c->origin!=f->origin||c->origin_tick!=f->origin_tick||
      c->expires!=f->expires||c->caller_original!=f->caller||c->caller_bytes!=f->caller_bytes||
      c->sink!=f->roots->sink||c->capture_original!=f->roots->capture_original||
      c->capture_original_bytes!=f->roots->capture_original_bytes||
      f->birth_cells[i]!=&c->created_original||f->producers[i]!=c||
      f->producer_bytes[i]!=sizeof(*c)||f->roots->birth_cells[i]!=f->birth_cells[i]||
      f->roots->birth_producers[i]!=c||f->roots->birth_producer_bytes[i]!=sizeof(*c))return 0;
  }
  return 1;
}
struct slcli_root_foundation *slcli_root_foundation_allocate(struct slcli_arena *a,
  slcli_root_creator *const creators[5]) {
  struct slcli_root_foundation *f;const slcli_root_creator *first;size_t i,j;
  if(!a||!creators||!slcli_arena_live_range(a,creators,5*sizeof(*creators))||
    !(first=creators[0])||!slcli_arena_live_block(a,first,sizeof(*first)))return 0;
  for(i=0;i<5;i++) {
    const slcli_root_creator *c=creators[i];
    if(!c||!slcli_arena_live_block(a,c,sizeof(*c))||c->arena!=a||!c->prepared||c->started||
      c->complete||c->unresolved||c->original_role!=roles[i]||
      c->deadline!=first->deadline||c->caller_original!=first->caller_original||
      c->caller_bytes!=first->caller_bytes||c->origin!=first->origin||
      c->expires!=first->expires||c->origin_tick!=first->origin_tick||
      c->previous_tick!=c->origin_tick||c->sink!=first->sink||
      c->capture_original!=first->capture_original||
      c->capture_original_bytes!=first->capture_original_bytes||
      !away(creators,5*sizeof(*creators),c,sizeof(*c)))return 0;
#if defined(_WIN32)
    if(c->created_original||c->native_called||c->descriptor_acquired||c->directory_created)return 0;
#else
    if(c->created_original!=-1||c->uid_called||c->mkdir_called||c->open_called||
      c->descriptor_acquired||c->directory_created)return 0;
#endif
    for(j=0;j<i;j++)if(!away(c,sizeof(*c),creators[j],sizeof(*c)))return 0;
  }
  if(!slcli_source_deadline_original_matches(a,first->deadline,first->caller_original))return 0;
  f=slcli_arena_calloc(a,1,sizeof(*f));if(!f)return 0;
  f->arena=a;f->deadline=first->deadline;f->origin=first->origin;f->caller=first->caller_original;
  f->caller_bytes=first->caller_bytes;f->expires=first->expires;f->origin_tick=first->origin_tick;
  f->previous_tick=f->origin_tick;f->original_creators=creators;
  for(i=0;i<5;i++) {
    f->creators[i]=creators[i];f->birth_cells[i]=&creators[i]->created_original;
    f->producers[i]=creators[i];f->producer_bytes[i]=sizeof(*creators[i]);
  }
  f->roots=slcli_management_foundation_roots_allocate_unborn(a,f->deadline,f->caller,f->caller_bytes,
    f->birth_cells,f->producers,f->producer_bytes,first->sink,first->capture_original,
    first->capture_original_bytes);
  if(!f->roots)return 0;f->prepared=1;return source(f)?f:0;
}
static int stop(struct slcli_root_foundation *f) {
  size_t i;f->unresolved=1;
  if(source(f)) {
    f->roots->unresolved=1;
    /* Absorb the SAME already prepared creators, including uncalled ones.
     * No independent component entry may reuse the failed composition. */
    for(i=0;i<5;i++)f->creators[i]->unresolved=1;
  }
  return 0;
}
int slcli_root_foundation_create(struct slcli_root_foundation *f) {
  size_t i;int ok,joined,born;slcli_root_creator *c;const struct slcli_native_clock *last;
  if(!source(f)||f->started||f->unresolved)return 0;f->started=1;f->unresolved=1;
  for(i=0;i<5;i++) {
    c=f->creators[i];
    if(!source(f)||c->started||c->unresolved||c->previous_tick!=f->origin_tick||
      !slcli_native_clock_read(&f->clocks[i])||f->clocks[i].tick<f->previous_tick||
      f->clocks[i].tick>=f->expires||!source(f))return stop(f);
    f->previous_tick=f->clocks[i].tick;c->previous_tick=f->previous_tick;
    f->roots->previous_tick=f->previous_tick;f->attempted++;
#if defined(_WIN32)
    ok=slcli_management_foundation_windows_create(c);last=&c->clocks[9];
    born=c->descriptor_acquired&&c->created_original&&c->created_original!=INVALID_HANDLE_VALUE;
#else
    ok=slcli_management_foundation_linux_create(c);last=&c->clocks[11];
    born=c->descriptor_acquired&&c->created_original>=0;
#endif
    joined=born?slcli_management_foundation_roots_born(f->roots,(uint32_t)i,last):0;
    /* The roots owner retains actual born outputs even if the creator, sink,
     * final clock or independent comparison failed. No rollback/cleanup. */
    if(!ok||!joined||!source(f)||!c->complete||c->unresolved)return stop(f);
    f->previous_tick=f->roots->previous_tick;f->completed++;
  }
  if(!slcli_management_foundation_roots_observe(f->roots)||!source(f)||
    f->roots->previous_tick<f->previous_tick)return stop(f);
  f->previous_tick=f->roots->previous_tick;f->complete=1;f->unresolved=0;return 1;
}
#endif
