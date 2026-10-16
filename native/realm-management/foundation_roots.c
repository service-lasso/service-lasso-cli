#include "foundation_roots.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
static int original_source(struct slcli_management_foundation_roots *r) {
  return r&&r->prepared&&r->arena&&r->sink&&slcli_arena_live_block(r->arena,r,sizeof(*r))&&
    slcli_arena_live_block(r->arena,r->caller_original,r->caller_bytes)&&
    slcli_arena_live_block(r->arena,r->capture_original,r->capture_original_bytes)&&
    slcli_source_deadline_original_matches(r->arena,r->deadline,r->caller_original)&&
    r->deadline->origin==r->origin&&r->deadline->expires==r->expires&&
    r->origin->tick==r->origin_tick;
}
static int source(struct slcli_management_foundation_roots *r) {
  size_t i;uintptr_t cell,base;
  if(!original_source(r)||(r->unborn&&r->joined_mask!=31))return 0;
  for(i=0;i<5;i++) {
#if defined(_WIN32)
    if(!r->originals[i]||r->originals[i]==INVALID_HANDLE_VALUE)return 0;
#else
    if(r->originals[i]<0)return 0;
#endif
    if(r->unborn) {
      cell=(uintptr_t)r->birth_cells[i];base=(uintptr_t)r->birth_producers[i];
      if(!r->birth_cells[i]||!slcli_arena_live_block(r->arena,r->birth_producers[i],r->birth_producer_bytes[i])||
        cell<base||cell-base>r->birth_producer_bytes[i]||
        r->birth_producer_bytes[i]-(cell-base)<sizeof(*r->birth_cells[i])||
        cell%_Alignof(slcli_input_handle)||*r->birth_cells[i]!=r->originals[i])return 0;
    }
  }
  return 1;
}
static struct slcli_management_foundation_roots *allocate(
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,size_t bytes,
  slcli_store_capture_sink sink,void *owner,size_t owner_bytes) {
  struct slcli_management_foundation_roots *r;
  if(!a||!a->owner||!sink||!bytes||!owner_bytes||
    !slcli_arena_live_block(a,caller,bytes)||!slcli_arena_live_block(a,owner,owner_bytes)||
    !slcli_source_deadline_original_matches(a,d,caller)||
    !away(caller,bytes,owner,owner_bytes)||!slcli_source_deadline_away(d,caller,bytes)||
    !slcli_source_deadline_away(d,owner,owner_bytes)||
    !away(caller,bytes,a,sizeof(*a))||!away(caller,bytes,a->owner,sizeof(*a->owner))||
    !away(owner,owner_bytes,a,sizeof(*a))||!away(owner,owner_bytes,a->owner,sizeof(*a->owner)))return 0;
  r=slcli_arena_calloc(a,1,sizeof(*r));if(!r)return 0;
  if(!away(r,sizeof(*r),caller,bytes)||!away(r,sizeof(*r),owner,owner_bytes)||
    !slcli_source_deadline_away(d,r,sizeof(*r))||
    !away(r,sizeof(*r),a,sizeof(*a))||!away(r,sizeof(*r),a->owner,sizeof(*a->owner)))return 0;
  r->arena=a;r->deadline=d;r->origin=d->origin;r->caller_original=caller;r->caller_bytes=bytes;
  r->sink=sink;r->capture_original=owner;r->capture_original_bytes=owner_bytes;
  r->expires=d->expires;r->origin_tick=d->origin->tick;r->previous_tick=r->origin_tick;
  r->prepared=1;return r;
}
struct slcli_management_foundation_roots *slcli_management_foundation_roots_allocate(
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,size_t bytes,
  const slcli_input_handle handles[5],slcli_store_capture_sink sink,void *owner,size_t owner_bytes) {
  struct slcli_management_foundation_roots *r;size_t i,j;
  if(!a||!a->owner||!handles||!sink||!bytes||!owner_bytes||
     !slcli_arena_live_block(a,caller,bytes)||!slcli_arena_live_block(a,owner,owner_bytes)||
     !slcli_arena_live_range(a,handles,5*sizeof(*handles))||
     !slcli_source_deadline_original_matches(a,d,caller)||
     !away(caller,bytes,owner,owner_bytes)||!slcli_source_deadline_away(d,caller,bytes)||
     !slcli_source_deadline_away(d,owner,owner_bytes)||
     !away(caller,bytes,a,sizeof(*a))||!away(caller,bytes,a->owner,sizeof(*a->owner))||
     !away(owner,owner_bytes,a,sizeof(*a))||!away(owner,owner_bytes,a->owner,sizeof(*a->owner)))return 0;
  for(i=0;i<5;i++) {
#if defined(_WIN32)
    if(!handles[i]||handles[i]==INVALID_HANDLE_VALUE)return 0;
#else
    if(handles[i]<0)return 0;
#endif
    for(j=0;j<i;j++)if(handles[i]==handles[j])return 0;
  }
  r=allocate(a,d,caller,bytes,sink,owner,owner_bytes);if(!r)return 0;
  for(i=0;i<5;i++)r->originals[i]=handles[i];return r;
}
struct slcli_management_foundation_roots *slcli_management_foundation_roots_allocate_unborn(
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,size_t bytes,
  slcli_input_handle *const cells[5],const void *const producers[5],const size_t extents[5],
  slcli_store_capture_sink sink,void *owner,size_t owner_bytes) {
  struct slcli_management_foundation_roots *r;size_t i,j;uintptr_t cell,base;
  if(!a||!cells||!producers||!extents||
    !slcli_source_deadline_original_matches(a,d,caller))return 0;
  for(i=0;i<5;i++) {
    cell=(uintptr_t)cells[i];base=(uintptr_t)producers[i];
    if(!cells[i]||!extents[i]||!slcli_arena_live_block(a,producers[i],extents[i])||
      cell<base||cell-base>extents[i]||extents[i]-(cell-base)<sizeof(*cells[i])||
      cell%_Alignof(slcli_input_handle)||!away(producers[i],extents[i],caller,bytes)||
      !away(producers[i],extents[i],owner,owner_bytes)||
      !slcli_source_deadline_away(d,producers[i],extents[i]))return 0;
#if defined(_WIN32)
    if(*cells[i])return 0;
#else
    if(*cells[i]!=-1)return 0;
#endif
    for(j=0;j<i;j++)if(!away(producers[i],extents[i],producers[j],extents[j]))return 0;
  }
  r=allocate(a,d,caller,bytes,sink,owner,owner_bytes);if(!r)return 0;
  for(i=0;i<5;i++) {
    if(!away(r,sizeof(*r),producers[i],extents[i]))return 0;
    r->birth_cells[i]=cells[i];r->birth_producers[i]=producers[i];r->birth_producer_bytes[i]=extents[i];
    r->originals[i]=*cells[i];
  }
  r->unborn=1;return r;
}
int slcli_management_foundation_roots_born(struct slcli_management_foundation_roots *r,
  uint32_t ordinal,const struct slcli_native_clock *clock) {
  size_t j;slcli_input_handle original;uintptr_t cell,base;
  if(!original_source(r)||!r->unborn||r->started||ordinal>=5||
    r->joined_mask&(1u<<ordinal)||!r->birth_cells[ordinal]||
    !slcli_arena_live_block(r->arena,r->birth_producers[ordinal],r->birth_producer_bytes[ordinal]))return 0;
  cell=(uintptr_t)r->birth_cells[ordinal];base=(uintptr_t)r->birth_producers[ordinal];
  if(cell<base||cell-base>r->birth_producer_bytes[ordinal]||
    r->birth_producer_bytes[ordinal]-(cell-base)<sizeof(*r->birth_cells[ordinal])||
    cell%_Alignof(slcli_input_handle))return 0;
  original=*r->birth_cells[ordinal];
#if defined(_WIN32)
  if(!original||original==INVALID_HANDLE_VALUE)return 0;
#else
  if(original<0)return 0;
#endif
  /* Preserve real native birth BEFORE any diagnostic clock/equality gate. */
  r->originals[ordinal]=original;r->joined_mask|=1u<<ordinal;
  for(j=0;j<5;j++)if(j!=ordinal&&(r->joined_mask&(1u<<j))&&r->originals[j]==original) {
    r->unresolved=1;return 0;
  }
  if(!clock||!slcli_arena_live_range(r->arena,clock,sizeof(*clock))||!clock->complete||
    clock->tick<r->previous_tick||clock->tick>=r->expires) {r->unresolved=1;return 0;}
  r->previous_tick=clock->tick;return original_source(r)&&!r->unresolved;
}
static int clock_row(struct slcli_management_foundation_roots *r,size_t row) {
  struct slcli_native_clock *c=&r->clocks[row];
  if(!source(r)||!slcli_native_clock_read(c)||c->tick<r->previous_tick||
     c->tick>=r->expires||!source(r))return 0;
  r->previous_tick=c->tick;return 1;
}
static int identity(const slcli_input_capture *a,const slcli_input_capture *b) {
#if defined(_WIN32)
  return a->identity.VolumeSerialNumber==b->identity.VolumeSerialNumber&&
    slcli_equal(a->identity.FileId.Identifier,b->identity.FileId.Identifier,16);
#else
  return a->original_stat.st_dev==b->original_stat.st_dev&&a->original_stat.st_ino==b->original_stat.st_ino;
#endif
}
static int directory_same(const slcli_input_capture *a,const slcli_input_capture *b) {
  if(!slcli_store_original_same(a,b))return 0;
#if defined(_WIN32)
  return a->standard.Directory&&b->standard.Directory&&
    !a->standard.DeletePending&&!b->standard.DeletePending&&
    a->basic.LastWriteTime.QuadPart==b->basic.LastWriteTime.QuadPart&&
    a->basic.ChangeTime.QuadPart==b->basic.ChangeTime.QuadPart;
#else
  return a->original_stat.st_mtime==b->original_stat.st_mtime&&
    a->original_stat.st_mtime_nsec==b->original_stat.st_mtime_nsec&&
    a->original_stat.st_ctime==b->original_stat.st_ctime&&
    a->original_stat.st_ctime_nsec==b->original_stat.st_ctime_nsec;
#endif
}
int slcli_management_foundation_roots_observe(struct slcli_management_foundation_roots *r) {
  size_t i,j,role;int ok;slcli_input_capture *c;
  if(!source(r)||r->started||r->unresolved)return 0;r->started=1;r->unresolved=1;
  for(i=0;i<10;i++) {
    role=i%5;c=&r->captures[i];
    if(!clock_row(r,2*i))return 0;
    r->rows_attempted++;
#if defined(_WIN32)
    ok=slcli_windows_directory_source_observe(r->originals[role],c);
#else
    ok=slcli_linux_directory_source_observe(r->originals[role],c);
#endif
    if(!r->sink(r->capture_original,c,0,0))return 0;r->rows_preserved++;
    if(!ok||!source(r)||!c->tick||c->tick<r->previous_tick||c->tick>=r->expires)return 0;
    r->previous_tick=c->tick;
    if(!clock_row(r,2*i+1))return 0;
    if(i<5){for(j=0;j<i;j++)if(identity(c,&r->captures[j]))return 0;}
    else if(!directory_same(&r->captures[role],c))return 0;
  }
  r->complete=1;r->unresolved=0;return 1;
}
#endif
