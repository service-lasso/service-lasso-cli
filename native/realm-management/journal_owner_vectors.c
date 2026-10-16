#include "journal_owner_vectors.h"
#if defined(__linux__)
#include "../host-service/native_syscall_linux.h"
#endif
#if defined(_WIN32) || defined(__linux__)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
struct slcli_management_expiry_fixture *slcli_management_expiry_fixture_allocate(
  struct slcli_arena *a,size_t count) {
  struct slcli_management_expiry_fixture *f;
  if(!a||!count||count>SIZE_MAX/sizeof(struct slcli_native_clock))return 0;
  f=slcli_arena_calloc(a,1,sizeof(*f));if(!f)return 0;f->arena=a;
  f->clocks=slcli_arena_calloc(a,count,sizeof(*f->clocks));if(f->clocks)f->capacity=count;
  return f;
}
int slcli_management_publication_expiry_original(const struct slcli_management_deadline_original *d,
  struct slcli_management_expiry_fixture *f) {
  size_t i;uint64_t prior;
  if(!d||!d->origin||!d->origin->complete||!f||!f->arena||f->started||!f->clocks||
     !f->capacity||f->capacity>SIZE_MAX/sizeof(*f->clocks)||d->expires<=d->origin->tick||
     !slcli_arena_live_range(f->arena,f,sizeof(*f))||
     !slcli_arena_live_range(f->arena,f->clocks,f->capacity*sizeof(*f->clocks))||
     !slcli_arena_live_range(f->arena,d,sizeof(*d))||
     !slcli_arena_live_range(f->arena,d->origin,sizeof(*d->origin))||
     !away(f,sizeof(*f),f->clocks,f->capacity*sizeof(*f->clocks))||
     !away(f,sizeof(*f),d,sizeof(*d))||!away(f,sizeof(*f),d->origin,sizeof(*d->origin))||
     !away(f->clocks,f->capacity*sizeof(*f->clocks),d,sizeof(*d))||
     !away(f->clocks,f->capacity*sizeof(*f->clocks),d->origin,sizeof(*d->origin)))return 0;
  f->started=1;f->deadline=d;prior=d->origin->tick;
  for(i=0;i<f->capacity;i++) {
    f->count=i+1;
    if(!slcli_native_clock_read(&f->clocks[i])||f->clocks[i].tick<prior)return 0;
    prior=f->clocks[i].tick;
    if(prior>=d->expires){f->expired=1;f->complete=1;return 1;}
  }
  return 0; /* Real capture capacity exhaustion, never fabricated expiry. */
}
int slcli_management_publication_capture_original(void *original,const slcli_input_capture *capture,
  const unsigned char *raw,size_t bytes) {
  struct slcli_management_publication_capture_original *f=original;
  struct slcli_store_stream *s;int ready;
  if(!f||!capture||!f->vector||!f->vector->pair||!f->deadline||!f->persistence||
     !f->persistence_original||f->persistence==slcli_management_publication_capture_original||
     !away(f,sizeof(*f),capture,sizeof(*capture))||
     (bytes&&!away(f,sizeof(*f),raw,bytes))||
     !f->persistence(f->persistence_original,capture,raw,bytes))return 0;
  s=f->vector->pair->after[1];
  if(!s||!s->captures||s->capture_count<3||s->capture_count>s->capture_capacity||
     capture!=&s->captures[s->capture_count-1u]||capture->operation!=1||
     s->mode!=1||s->offset!=s->expected||s->held_verified)return 1;
  if(f->cause_observed)return 0; /* One genuine final-observation cause only. */
  if(f->source_case==SLCLI_PUBLICATION_REAL_EXPIRY) {
    if(!f->expiry||!slcli_management_publication_expiry_original(f->deadline,f->expiry))return 0;
    f->cause_observed=1;return 1;
  }
  if(f->source_case!=SLCLI_PUBLICATION_REAL_CONTENTION||!f->contender||
     !f->contender->clocks||!f->contender->capacity)return 0;
#if defined(_WIN32)
  ready=InterlockedCompareExchange(&f->contender->ready,0,0)!=0;
#else
  ready=atomic_load_explicit(&f->contender->ready,memory_order_acquire)!=0;
#endif
  if(!ready)return 0;
  f->cause_observed=1;return 1;
}
struct slcli_management_contention_fixture *slcli_management_contention_fixture_allocate(
  struct slcli_arena *a,size_t count) {
  struct slcli_management_contention_fixture *f;
  if(!a||!count||count>SIZE_MAX/sizeof(struct slcli_native_clock))return 0;
  f=slcli_arena_calloc(a,1,sizeof(*f));if(!f)return 0;f->arena=a;
  f->clocks=slcli_arena_calloc(a,count,sizeof(*f->clocks));if(!f->clocks)return f;f->capacity=count;
#if defined(__linux__)
  atomic_init(&f->ready,0);atomic_init(&f->release,0);
#endif
  return f;
}
int slcli_management_publication_contender_original(struct slcli_management_journal_owner *o,
  const struct slcli_management_deadline_original *d,struct slcli_management_contention_fixture *f) {
  size_t i;long thread;uint64_t prior;
#if defined(_WIN32)
  thread=(long)GetCurrentThreadId();
#else
  thread=slcli_kernel_call(__NR_gettid,0,0,0,0,0,0);
  if(thread<=0)return 0;
#endif
  /* Original fixture producer allocates/adopts these retained blocks before
   * dispatching the independently admitted fixture thread. This contender
   * never walks mutable allocator headers while another logical BQ owner is
   * signing/persisting outside the short lock. A storage constructor or this
   * pointer/geometry check supplies no thread/source/native lease admission. */
  if(!o||!d||!f||!thread||!f->arena||f->arena!=o->arena||f->started||
     !f->clocks||!f->capacity||f->capacity>SIZE_MAX/sizeof(*f->clocks)||
     !d->origin||
     !d->origin->complete||d->expires<=d->origin->tick||
     !away(o,sizeof(*o),f,sizeof(*f))||!away(o,sizeof(*o),f->clocks,f->capacity*sizeof(*f->clocks))||
     !away(o,sizeof(*o),d,sizeof(*d))||!away(o,sizeof(*o),d->origin,sizeof(*d->origin))||
     !away(f,sizeof(*f),f->clocks,f->capacity*sizeof(*f->clocks))||
     !away(f,sizeof(*f),d,sizeof(*d))||!away(f,sizeof(*f),d->origin,sizeof(*d->origin))||
     !away(f->clocks,f->capacity*sizeof(*f->clocks),d,sizeof(*d))||
     !away(f->clocks,f->capacity*sizeof(*f->clocks),d->origin,sizeof(*d->origin)))return 0;
  f->started=1;f->actual_thread=thread;f->lock_observed=1;
  f->owner_original=o;f->deadline=d;f->lock_address=(uintptr_t)&o->transition;
#if defined(_WIN32)
  f->lock_acquired=(uint32_t)TryAcquireSRWLockExclusive(&o->transition);
#else
  f->lock_acquired=!atomic_flag_test_and_set_explicit(&o->transition,memory_order_acquire);
#endif
  if(!f->lock_acquired)return 0;
#if defined(_WIN32)
  InterlockedExchange(&f->ready,1);
#else
  atomic_store_explicit(&f->ready,1,memory_order_release);
#endif
  /* Finite precharged capture count AND the original deadline bound this
   * controlled metadata contention. Each fresh native clock row is retained;
   * no IO/sign/hash/evidence flush occurs while this actual flag is held. */
  prior=d->origin->tick;
  for(i=0;i<f->capacity;i++) {
    int release;
    f->count=i+1;
    if(!slcli_native_clock_read(&f->clocks[i])||f->clocks[i].tick<prior){f->clock_failed=1;break;}
    prior=f->clocks[i].tick;
    if(f->clocks[i].tick>=d->expires){f->expired=1;break;}
#if defined(_WIN32)
    release=InterlockedCompareExchange(&f->release,0,0)!=0;
#else
    release=atomic_load_explicit(&f->release,memory_order_acquire)!=0;
#endif
    if(release)break;
  }
  if(i==f->capacity)f->capacity_exhausted=1;
  /* Clear readiness BEFORE release; it never persists after actual unlock.
   * Real owner's captured failed try-lock remains the contention proof. */
#if defined(_WIN32)
  InterlockedExchange(&f->ready,0);
  ReleaseSRWLockExclusive(&o->transition);
#else
  atomic_store_explicit(&f->ready,0,memory_order_release);
  atomic_flag_clear_explicit(&o->transition,memory_order_release);
#endif
  f->release_observed=1;f->completed=1;
  if(!slcli_native_clock_read(&f->release_clock)||f->release_clock.tick<prior) {
    f->clock_failed=1;return 0;
  }
  return !f->clock_failed;
}
int slcli_management_journal_qualification_positive_vector(
  struct slcli_management_journal_owner *o,struct slcli_management_journal_vector *v,
  const struct slcli_ha2_journal_prefix_originals *f,const struct slcli_management_deadline_original *d,
  const struct slcli_native_clock *suite_origin,const unsigned char seed[32],const unsigned char anchor[32],
  const unsigned char *operation,size_t operation_bytes,slcli_input_handle first,slcli_input_handle second,
  slcli_store_capture_sink sink,void *capture) {
  if(!d||!suite_origin||d->origin!=suite_origin||!suite_origin->complete||
     suite_origin->tick>UINT64_MAX-UINT64_C(40000000000)||
     d->expires>suite_origin->tick+UINT64_C(40000000000))return 0;
  if(!slcli_management_journal_owner_attempt(o,v,f,d,seed,anchor,operation,operation_bytes,
    first,second,sink,capture))return 0;
  return v->pair->deadline_owner&&slcli_store_deadline_ready(v->pair->deadline_owner)&&
    v->pair->deadline_owner->count==6&&v->pair->deadline_owner->deadline==d->native_original&&
    v->pair->deadline_owner->caller_bytes==d->caller_bytes&&
    v->pair->deadline_owner->capture_original==capture&&
    v->pair->deadline_owner->capture_original_bytes==d->capture_owner_bytes&&
    v->pair->deadline_owner->predecessor==&v->entry_clock&&
    v->publication_clock.tick>=v->pair->deadline_owner->previous_tick&&
    v->publication_clock.complete&&v->publication_locked_clock.complete&&
    v->publication_lock_observed&&v->publication_lock_acquired&&
    v->publication_locked_clock.tick<d->expires&&v->pair->complete&&!v->pair->failed&&
    !o->pending&&!o->pending_operation&&!o->pending_deadline&&!o->unknown&&
    o->sequence==f->sequence&&o->generation==f->generation&&o->state==f->state&&
    o->records[o->sequence-1u]==v->record->raw;
}
int slcli_management_publication_failure_vector(const struct slcli_management_journal_owner *o,
  const struct slcli_management_journal_vector *v,const struct slcli_management_committed_original *before,
  const struct slcli_management_deadline_original *d) {
  int unresolved,thread,expired,contended;
  if(!o||!v||!before||!d)return 0;
#if defined(_WIN32)
  unresolved=InterlockedCompareExchange((LONG *)&o->publication_unresolved,0,0)!=0;
  thread=InterlockedCompareExchange((LONG *)&o->owning_thread,0,0)!=0;
#else
  unresolved=atomic_load_explicit(&o->publication_unresolved,memory_order_acquire)!=0;
  thread=atomic_load_explicit(&o->owning_thread,memory_order_acquire)!=0;
#endif
  if(!unresolved||!thread||before->owner!=o||before->sequence>=1024||
     o->pending!=v||o->pending_deadline!=d||o->pending_operation!=d->operation_original||
     o->sequence!=before->sequence||o->generation!=before->generation||o->state!=before->state||
     !slcli_equal(o->committed_hash,before->hash,32)||
     (before->sequence&&o->records[before->sequence-1u]!=before->last)||
     o->records[before->sequence]||!v->pair||!v->pair->complete||v->pair->failed||
     !v->record||!v->record->complete)return 0;
  expired=(v->publication_clock.complete&&v->publication_clock.tick>=d->expires)||
    (v->publication_locked_clock.complete&&v->publication_locked_clock.tick>=d->expires);
  contended=v->publication_clock.complete&&v->publication_clock.tick<d->expires&&
    v->publication_lock_observed&&!v->publication_lock_acquired;
  /* Success here means the negative fixture retained native UNKNOWN safely;
   * it never claims the failed publication committed or erased its charges. */
  return unresolved&&thread&&(expired||contended)&&
    v->pair->retained_store_bytes>=1544u&&v->pair->candidate==v->record->raw&&
    v->pair->committed==o->records&&v->pair->committed_count==before->sequence;
}
#endif
