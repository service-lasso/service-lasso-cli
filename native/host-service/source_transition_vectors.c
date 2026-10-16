#include "source_transition_vectors.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
static int unresolved(struct slcli_source_owner *s) {
#if defined(_WIN32)
  return InterlockedCompareExchange(&s->transition_unresolved,0,0)!=0;
#else
  return atomic_load_explicit(&s->transition_unresolved,memory_order_acquire)!=0;
#endif
}
static int qualification_budget(const struct slcli_source_deadline_original *d) {
  return d&&d->origin&&d->origin->tick<=UINT64_MAX-40000000000ULL&&
    d->expires>d->origin->tick&&d->expires<=d->origin->tick+40000000000ULL;
}
int slcli_source_nested_positive_vector(struct slcli_service_request *r,
  struct slcli_primary_engine *e,const struct slcli_primary_commit_originals *o) {
  struct slcli_source_owner *s;
  struct slcli_source_transition_attempt *a,*b,*c;
  if(!r||!e||!o||r->owning_primary!=e||
     r->deadline_original!=o->deadline_original||!qualification_budget(r->deadline_original))return 0;
  s=slcli_source_enter(e,SLCLI_SOURCE_SERVICE,r,0);if(!s)return 0;
  a=s->retained_attempts;
  if(slcli_source_enter(e,SLCLI_SOURCE_DISPATCH,o,0)!=s)return 0;b=s->retained_attempts;
  if(slcli_source_enter(e,SLCLI_SOURCE_COMMIT,o,0)!=s)return 0;c=s->retained_attempts;
  if(!slcli_source_leave(s,1)||!slcli_source_leave(s,1)||!slcli_source_leave(s,1))return 0;
  return !unresolved(s)&&!s->lock_depth&&!s->transaction_original&&
    a&&b&&c&&b->prior==a&&c->prior==b&&a->completed&&b->completed&&c->completed&&
    !a->unresolved&&!b->unresolved&&!c->unresolved&&
    a->root_original==r&&b->root_original==r&&c->root_original==r&&
    a->original_expires==b->original_expires&&b->original_expires==c->original_expires;
}
int slcli_source_native_expiry_vector(struct slcli_primary_engine *e,
  const struct slcli_primary_commit_originals *o,struct slcli_source_expiry_fixture *f) {
  struct slcli_source_owner *s;struct slcli_source_transition_attempt *a;
  uint64_t last,generation;uint32_t sequence;
  if(!e||!o||!f||f->started||!qualification_budget(o->deadline_original))return 0;
  s=slcli_source_enter(e,SLCLI_SOURCE_COMMIT,o,0);if(!s)return 0;
  a=s->retained_attempts;generation=e->committed_generation;sequence=e->next;
  if(!slcli_arena_live_range(s->arena,f,sizeof(*f))||!f->rows||!f->capacity||
     f->capacity>1048576/sizeof(*f->rows)||
     !slcli_arena_live_range(s->arena,f->rows,f->capacity*sizeof(*f->rows)))return 0;
  {
    const void *protected_ranges[7]={s,e,e->ledger,e->candidate,o,f,a};
    size_t sizes[7]={sizeof(*s),sizeof(*e),sizeof(*e->ledger),sizeof(*e->candidate),
      sizeof(*o),sizeof(*f),sizeof(*a)};unsigned i;
    for(i=0;i<7;i++)if(!away(f->rows,f->capacity*sizeof(*f->rows),protected_ranges[i],sizes[i]))return 0;
    if(!away(f,sizeof(*f),s,sizeof(*s))||!away(f,sizeof(*f),e,sizeof(*e))||
       !away(f,sizeof(*f),e->ledger,sizeof(*e->ledger))||
       !away(f,sizeof(*f),e->candidate,sizeof(*e->candidate))||!away(f,sizeof(*f),o,sizeof(*o))||
       !away(f,sizeof(*f),a,sizeof(*a))||
       !slcli_source_deadline_away(o->deadline_original,f,sizeof(*f))||
       !slcli_source_deadline_away(o->deadline_original,f->rows,f->capacity*sizeof(*f->rows)))return 0;
  }
  f->started=1;last=a->entry.tick;
  while(f->used<f->capacity) {
    struct slcli_native_clock *row=&f->rows[f->used++];
    if(!slcli_native_clock_read(row)||row->tick<last){f->clock_failed=1;return 0;}
    last=row->tick;
    if(row->tick>=a->original_expires){f->expired=1;break;}
  }
  if(!f->expired){f->exhausted=1;return 0;}
  if(slcli_source_leave(s,1))return 0;
  /* Native expiry occurred before the real leave. No durable pair was attempted
   * by this vector; actual committed/native-write preservation has a separate
   * positive commit caller and must not be inferred from this metadata case. */
  return unresolved(s)&&s->lock_depth==1&&s->operation[0]==SLCLI_SOURCE_COMMIT&&
    s->transaction_original==o&&s->retained_attempts==a&&a->unresolved&&!a->completed&&
    a->leave.complete&&a->leave.tick>=a->original_expires&&
    e->committed_generation==generation&&e->next==sequence;
}
#endif
