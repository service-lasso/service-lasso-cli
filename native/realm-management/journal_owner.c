#include "journal_owner.h"
#if defined(__linux__)
#include "../host-service/native_syscall_linux.h"
#endif
#if defined(_WIN32) || defined(__linux__)
static long native_thread(void) {
#if defined(_WIN32)
  return (long)GetCurrentThreadId();
#else
  long t=slcli_kernel_call(__NR_gettid,0,0,0,0,0,0);return t>0?t:0;
#endif
}
static int lock(struct slcli_management_journal_owner *o) {
#if defined(_WIN32)
  return TryAcquireSRWLockExclusive(&o->transition)!=0;
#else
  return !atomic_flag_test_and_set_explicit(&o->transition,memory_order_acquire);
#endif
}
static void unlock(struct slcli_management_journal_owner *o) {
#if defined(_WIN32)
  ReleaseSRWLockExclusive(&o->transition);
#else
  atomic_flag_clear_explicit(&o->transition,memory_order_release);
#endif
}
static long owner_thread(struct slcli_management_journal_owner *o) {
#if defined(_WIN32)
  return InterlockedCompareExchange(&o->owning_thread,0,0);
#else
  return atomic_load_explicit(&o->owning_thread,memory_order_acquire);
#endif
}
static void set_thread(struct slcli_management_journal_owner *o,long t) {
#if defined(_WIN32)
  InterlockedExchange(&o->owning_thread,(LONG)t);
#else
  atomic_store_explicit(&o->owning_thread,t,memory_order_release);
#endif
}
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int unresolved(struct slcli_management_journal_owner *o) {
#if defined(_WIN32)
  return InterlockedCompareExchange(&o->publication_unresolved,0,0)!=0;
#else
  return atomic_load_explicit(&o->publication_unresolved,memory_order_acquire)!=0;
#endif
}
static void retain_unresolved(struct slcli_management_journal_owner *o) {
#if defined(_WIN32)
  InterlockedExchange(&o->publication_unresolved,1);
#else
  atomic_store_explicit(&o->publication_unresolved,1,memory_order_release);
#endif
}
static int outside(struct slcli_management_journal_owner *o,const void *p,size_t n) {
  return slcli_arena_live_range(o->arena,p,n)&&away(o,sizeof(*o),p,n);
}
static int deadline_away(const struct slcli_management_deadline_original *d,const void *p,size_t n) {
  return away(d,sizeof(*d),p,n)&&away(d->origin,sizeof(*d->origin),p,n);
}
struct slcli_management_journal_owner *slcli_management_journal_owner_allocate(struct slcli_arena *a) {
  struct slcli_management_journal_owner *o;
  if(!a||!a->owner||!a->owner->limit)return 0;
  o=slcli_arena_calloc(a,1,sizeof(*o));if(!o)return 0;o->arena=a;
#if defined(_WIN32)
  InitializeSRWLock(&o->transition);
#else
  atomic_flag_clear_explicit(&o->transition,memory_order_relaxed);atomic_init(&o->owning_thread,0);
  atomic_init(&o->publication_unresolved,0);
#endif
  return o;
}
int slcli_management_journal_owner_attempt(struct slcli_management_journal_owner *o,
  struct slcli_management_journal_vector *v,const struct slcli_ha2_journal_prefix_originals *frontier,
  const struct slcli_management_deadline_original *deadline,
  const unsigned char seed[32],const unsigned char anchor[32],const unsigned char *operation,
  size_t operation_bytes,slcli_input_handle first,slcli_input_handle second,
  slcli_store_capture_sink sink,void *capture_owner) {
  long thread=native_thread();struct slcli_ha2_operation request;
  struct slcli_ha2_journal_record candidate;struct slcli_sha256 hash;
  unsigned char digest[32];uint64_t old_sequence,old_generation;unsigned i;int ok;
  if(!thread||!o||!o->arena||!v||!frontier||!operation||!seed||!anchor||!sink||!capture_owner||
     !deadline||!lock(o))return 0;
  /* Check exclusive marker BEFORE mutable arena metadata. Physical lock is
   * released before hashing, signing, native IO, evidence flush or readback. */
  if(owner_thread(o)||o->pending||o->unknown||unresolved(o)||o->sequence>=SLCLI_HA2_JOURNAL_RECORD_LIMIT||
     !slcli_arena_live_range(o->arena,o,sizeof(*o))||
     !outside(o,v,sizeof(*v))||v->arena!=o->arena||v->started||v->failed||
     !v->record||!outside(o,v->record,sizeof(*v->record))||
     !v->record->signer||!outside(o,v->record->signer,sizeof(*v->record->signer))||
     !v->pair||!outside(o,v->pair,sizeof(*v->pair))||
     !outside(o,deadline,sizeof(*deadline))||!deadline->origin||
     !outside(o,deadline->origin,sizeof(*deadline->origin))||
     !deadline->origin->complete||!deadline->origin->started||!deadline->origin->observed||
     deadline->operation_original!=operation||deadline->expires<=deadline->origin->tick||
     !deadline->native_original||!deadline->caller_bytes||!deadline->capture_owner_bytes||
     !slcli_source_deadline_original_matches(o->arena,deadline->native_original,
       deadline->native_original->caller_original)||
     deadline->native_original->origin!=deadline->origin||
     deadline->native_original->expires!=deadline->expires||
     !slcli_arena_live_block(o->arena,deadline->native_original->caller_original,deadline->caller_bytes)||
     !slcli_arena_live_block(o->arena,capture_owner,deadline->capture_owner_bytes)||
     !away(capture_owner,deadline->capture_owner_bytes,o,sizeof(*o))||
     !away(capture_owner,deadline->capture_owner_bytes,v,sizeof(*v))||
     !away(capture_owner,deadline->capture_owner_bytes,v->record,sizeof(*v->record))||
     !away(capture_owner,deadline->capture_owner_bytes,v->record->signer,sizeof(*v->record->signer))||
     !away(capture_owner,deadline->capture_owner_bytes,deadline,sizeof(*deadline))||
     !away(deadline->native_original->caller_original,deadline->caller_bytes,o,sizeof(*o))||
     !away(deadline->native_original->caller_original,deadline->caller_bytes,v,sizeof(*v))||
     !away(deadline->native_original->caller_original,deadline->caller_bytes,v->record,sizeof(*v->record))||
     !away(deadline->native_original->caller_original,deadline->caller_bytes,
       v->record->signer,sizeof(*v->record->signer))||
     !away(deadline->native_original->caller_original,deadline->caller_bytes,deadline,sizeof(*deadline))||
     !slcli_source_deadline_away(deadline->native_original,o,sizeof(*o))||
     !slcli_source_deadline_away(deadline->native_original,v,sizeof(*v))||
     !slcli_source_deadline_away(deadline->native_original,v->record,sizeof(*v->record))||
     !slcli_source_deadline_away(deadline->native_original,v->record->signer,sizeof(*v->record->signer))||
     !away(deadline,sizeof(*deadline),v,sizeof(*v))||
     !away(deadline->origin,sizeof(*deadline->origin),v,sizeof(*v))||
     !deadline_away(deadline,v->record,sizeof(*v->record))||
     !deadline_away(deadline,v->record->signer,sizeof(*v->record->signer))||
     !deadline_away(deadline,v->pair,sizeof(*v->pair))||
     !outside(o,frontier,sizeof(*frontier))||
     !outside(o,frontier->associations,SLCLI_HA2_JOURNAL_ASSOCIATIONS*32u)||
     !outside(o,frontier->key_version,frontier->key_version_bytes)||
     !outside(o,seed,32)||!outside(o,anchor,32)||!outside(o,operation,operation_bytes)) {
    unlock(o);return 0;
  }
  for(i=0;i<2;i++) {
    struct slcli_store_stream *s[3]={v->pair->before[i],v->pair->write[i],v->pair->after[i]};unsigned j;
    for(j=0;j<3;j++)if(!s[j]||!outside(o,s[j],sizeof(*s[j]))||
      !s[j]->capture_capacity||s[j]->capture_capacity>SIZE_MAX/sizeof(*s[j]->captures)||
      !outside(o,s[j]->captures,s[j]->capture_capacity*sizeof(*s[j]->captures))||
      !deadline_away(deadline,s[j],sizeof(*s[j]))||
      !deadline_away(deadline,s[j]->captures,s[j]->capture_capacity*sizeof(*s[j]->captures))) {
      unlock(o);return 0;
    }
  }
  old_sequence=o->sequence;old_generation=o->generation;
  o->pending=v;o->pending_operation=operation;o->pending_operation_bytes=operation_bytes;
  o->pending_deadline=deadline;
  o->pending_sequence=old_sequence;o->pending_generation=old_generation;set_thread(o,thread);
  unlock(o);
  /* Pending closes every new operation, including revocation failure, before
   * the first crypto/native effect. No old signed ACTIVE bypasses this marker. */
  ok=slcli_native_clock_read(&v->entry_clock)&&v->entry_clock.tick>=deadline->origin->tick&&
    v->entry_clock.tick<deadline->expires&&slcli_ha2_operation_parse(operation,operation_bytes,&request)&&
    request.sequence==old_sequence+1u&&request.sequence==frontier->sequence&&
    request.generation==frontier->generation;
  if(ok&&old_sequence) {
    const unsigned char *last=o->records[old_sequence-1u];unsigned char actual[32];
    struct slcli_ha2_journal_record prior;
    ok=last&&slcli_ha2_journal_record_parse(last,SLCLI_HA2_JOURNAL_BYTES,&prior)&&
      prior.sequence==old_sequence&&prior.generation==old_generation&&prior.state==o->state;
    if(ok){slcli_sha256_init(&hash);ok=slcli_sha256_update(&hash,last,SLCLI_HA2_JOURNAL_BYTES)&&
      slcli_sha256_finish(&hash,actual)&&slcli_equal(actual,o->committed_hash,32)&&
      slcli_equal(actual,request.prior_record,32);}
  } else if(ok)ok=!old_generation&&!o->state&&request.event==SLCLI_HA2_INSTALL_PREPARE;
  if(ok)ok=slcli_ha2_journal_pair_deadline_bind(v->pair,deadline->native_original,
    deadline->native_original->caller_original,deadline->caller_bytes,&v->entry_clock,
    sink,capture_owner,deadline->capture_owner_bytes);
  if(ok)ok=slcli_management_journal_positive_vector(v,frontier,seed,anchor,operation,
    operation_bytes,o->records,(uint32_t)old_sequence,first,second,o->retained_store_bytes,
    sink,capture_owner)&&slcli_ha2_journal_record_parse(v->record->raw,SLCLI_HA2_JOURNAL_BYTES,&candidate);
  if(ok){slcli_sha256_init(&hash);ok=slcli_sha256_update(&hash,v->record->raw,SLCLI_HA2_JOURNAL_BYTES)&&
    slcli_sha256_finish(&hash,digest);}
  /* One finite publication attempt under SAME original remaining caller
   * deadline. No spin/sleep/retry resets its origin. Failure retains pending,
   * logical thread, full vector/native charge and atomic unresolved gate;
   * committed cells are never written without the physical lock. */
  if(!slcli_native_clock_read(&v->publication_clock)||
     v->publication_clock.tick<v->entry_clock.tick||
     (v->pair->deadline_owner&&v->publication_clock.tick<v->pair->deadline_owner->previous_tick)||
     v->publication_clock.tick>=deadline->expires) {
    retain_unresolved(o);return 0;
  }
  v->publication_lock_observed=1;v->publication_lock_acquired=(uint32_t)lock(o);
  if(!v->publication_lock_acquired){retain_unresolved(o);return 0;}
  /* Recheck after successful try-acquire: preemption between precheck and the
   * native lock cannot turn an expired attempt into committed publication.
   * This one-use monotonic query performs no hash/sign/file IO/capture flush. */
  if(!slcli_native_clock_read(&v->publication_locked_clock)||
     v->publication_locked_clock.tick<v->publication_clock.tick||
     v->publication_locked_clock.tick>=deadline->expires) {
    retain_unresolved(o);unlock(o);return 0;
  }
  if(!deadline->native_original||deadline->native_original->origin!=deadline->origin||
     deadline->native_original->expires!=deadline->expires||
     (v->pair->deadline_owner&&(!slcli_store_deadline_ready(v->pair->deadline_owner)||
       v->pair->deadline_owner->deadline!=deadline->native_original||
       v->pair->deadline_owner->caller_bytes!=deadline->caller_bytes||
       v->pair->deadline_owner->capture_original_bytes!=deadline->capture_owner_bytes))||
     owner_thread(o)!=thread||o->pending!=v||o->pending_operation!=operation||
     o->sequence!=old_sequence||o->generation!=old_generation||o->pending_deadline!=deadline)ok=0;
  if(v->pair->started&&v->pair->retained_store_bytes>o->retained_store_bytes)
    o->retained_store_bytes=v->pair->retained_store_bytes;
  if(!ok) {
    o->unknown=1;set_thread(o,0);unlock(o);return 0;
  }
  /* Only BOTH full durable native journal readbacks have reached this point.
   * Old committed cells never change on any failed pair/signature/frontier. */
  o->records[old_sequence]=v->record->raw;o->sequence=candidate.sequence;
  o->generation=candidate.generation;o->state=candidate.state;
  for(i=0;i<32;i++)o->committed_hash[i]=digest[i];
  o->pending=0;o->pending_operation=0;o->pending_operation_bytes=0;o->pending_deadline=0;
  o->pending_sequence=0;o->pending_generation=0;set_thread(o,0);unlock(o);return 1;
}
#endif
