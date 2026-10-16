#include "source_deadline.h"
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&
    !(a<b+m&&b<a+n);
}
int slcli_source_deadline_away(const struct slcli_source_deadline_original *d,
  const void *raw,size_t bytes) {
  return d&&d->origin&&away(d,sizeof(*d),raw,bytes)&&
    away(d->origin,sizeof(*d->origin),raw,bytes);
}
int slcli_source_deadline_original_matches(struct slcli_arena *a,
  const struct slcli_source_deadline_original *d,const void *caller) {
  return a&&d&&caller&&slcli_arena_live_range(a,d,sizeof(*d))&&d->origin&&
    slcli_arena_live_range(a,d->origin,sizeof(*d->origin))&&d->origin->started&&
    d->origin->observed&&d->origin->complete&&d->origin->tick&&
    away(d,sizeof(*d),d->origin,sizeof(*d->origin))&&
    d->caller_original==caller&&d->expires>d->origin->tick;
}
struct slcli_source_transition_attempt *slcli_source_transition_begin(struct slcli_arena *arena,
  const struct slcli_source_deadline_original *d,const void *root,const void *original,
  uint32_t operation,struct slcli_source_transition_attempt *prior) {
  struct slcli_source_transition_attempt *a;
  struct slcli_source_transition_attempt *p;
  size_t retained_count=0;
  if(!slcli_source_deadline_original_matches(arena,d,root))return 0;
  /* A retained caller cannot renew its deadline on a later service read/write
   * invocation or on completion of its SAME already submitted native row. */
  for(p=prior;p;p=p->prior) {
    if(++retained_count>1048576/sizeof(*p)||!slcli_arena_live_range(arena,p,sizeof(*p)))return 0;
    if(p->root_original==root&&(p->deadline!=d||p->original_expires!=d->expires||
       p->original_origin!=d->origin||p->original_origin_tick!=d->origin->tick))return 0;
  }
  a=slcli_arena_calloc(arena,1,sizeof(*a));if(!a)return 0;
  a->prior=prior;a->deadline=d;a->original=original;a->operation=operation;
  a->root_original=root;a->original_origin=d->origin;
  a->original_expires=d->expires;a->original_origin_tick=d->origin->tick;
  if(!slcli_native_clock_read(&a->entry)||a->entry.tick<d->origin->tick||
     a->entry.tick>=a->original_expires)a->unresolved=1;
  return a;
}
int slcli_source_transition_before_leave(struct slcli_source_transition_attempt *a) {
  if(!a||a->unresolved||a->completed||!slcli_native_clock_read(&a->leave)||
     a->leave.tick<a->entry.tick||a->leave.tick>=a->original_expires||
     a->deadline->expires!=a->original_expires||
     a->deadline->origin!=a->original_origin||a->deadline->caller_original!=a->root_original||
     a->deadline->origin->tick!=a->original_origin_tick) {
    if(a)a->unresolved=1;return 0;
  }
  return 1;
}
int slcli_source_transition_publication_clock(struct slcli_source_transition_attempt *a) {
  if(!a||a->unresolved||a->completed||a->leave_locked.started||
     !a->leave.started||!a->leave.observed||!a->leave.complete||
     !slcli_native_clock_read(&a->leave_locked)||
     a->leave_locked.tick<a->leave.tick||a->leave_locked.tick>=a->original_expires||
     a->deadline->expires!=a->original_expires||
     a->deadline->origin!=a->original_origin||a->deadline->caller_original!=a->root_original||
     a->deadline->origin->tick!=a->original_origin_tick) {
    if(a)a->unresolved=1;return 0;
  }
  return 1;
}
int slcli_source_transition_locked_leave(struct slcli_source_transition_attempt *a) {
  if(!a||a->unresolved||a->completed||!a->leave_locked.started||
     !a->leave_locked.observed||!a->leave_locked.complete||
     a->leave_locked.tick<a->leave.tick||a->leave_locked.tick>=a->original_expires||
     a->deadline->expires!=a->original_expires||
     a->deadline->origin!=a->original_origin||a->deadline->caller_original!=a->root_original||
     a->deadline->origin->tick!=a->original_origin_tick) {
    if(a)a->unresolved=1;return 0;
  }
  return 1;
}
