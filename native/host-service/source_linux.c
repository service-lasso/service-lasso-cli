#include "source_linux.h"
#if defined(__linux__)
#include "native_syscall_linux.h"
#include "service_channel.h"
static long thread_original(void) {
  long thread=slcli_kernel_call(__NR_gettid,0,0,0,0,0,0);
  return thread>0?thread:0;
}
static int live(struct slcli_source_linux *s) {
  return s&&s->arena&&s->arena->owner&&s->arena->owner->limit==1048576&&
    slcli_arena_live_range(s->arena,s,sizeof(*s))&&s->primary&&
    slcli_arena_live_range(s->arena,s->primary,sizeof(*s->primary))&&
    s->primary->source_linux==s&&s->workspace&&
    slcli_arena_live_range(s->arena,s->workspace,sizeof(*s->workspace));
}
struct slcli_source_linux *slcli_source_linux_allocate(struct slcli_arena *a,
  struct slcli_primary_engine *e,struct slcli_shared_workspace *w) {
  struct slcli_source_linux *s;
  if(!a||!a->owner||a->owner->limit!=1048576||!e||e->source_linux||
     !slcli_arena_live_range(a,e,sizeof(*e))||
     !slcli_arena_live_range(a,w,sizeof(*w)))return 0;
  s=slcli_arena_calloc(a,1,sizeof(*s));if(!s)return 0;
  s->arena=a;s->primary=e;s->workspace=w;
  atomic_flag_clear_explicit(&s->transition,memory_order_relaxed);
  atomic_init(&s->owning_thread,0);atomic_init(&s->transition_unresolved,0);
  e->source_linux=s;return s;
}
static struct slcli_source_linux *prepare(struct slcli_source_linux *s,
  uint32_t operation,const void *original) {
  struct slcli_source_transition_attempt *a;
  size_t original_bytes=operation==SLCLI_SOURCE_SERVICE?sizeof(struct slcli_service_request):
    sizeof(struct slcli_primary_commit_originals);
  if(s->lock_depth>1) {
    const struct slcli_primary_commit_originals *r=original;
    if(!slcli_arena_live_range(s->arena,r,sizeof(*r))||
       r->deadline_original!=s->deadline_original)goto unresolved;
  }

  if(s->lock_depth==1&&!s->pending_native_original) {
    if(operation==SLCLI_SOURCE_SERVICE) {
      const struct slcli_service_request *r=original;
      if(!slcli_arena_live_range(s->arena,r,sizeof(*r)))goto unresolved;
      if(s->continued_original&&r->deadline_original!=s->deadline_original)goto unresolved;
      s->deadline_original=r->deadline_original;
    } else if(operation==SLCLI_SOURCE_COMMIT||operation==SLCLI_SOURCE_DISPATCH) {
      const struct slcli_primary_commit_originals *r=original;
      if(!slcli_arena_live_range(s->arena,r,sizeof(*r)))goto unresolved;
      s->deadline_original=r->deadline_original;
    } else goto unresolved;
  }
  if(!slcli_source_deadline_original_matches(s->arena,s->deadline_original,s->transaction_original)||
     !slcli_source_deadline_away(s->deadline_original,s,sizeof(*s))||
     !slcli_source_deadline_away(s->deadline_original,s->primary,sizeof(*s->primary))||
     !slcli_source_deadline_away(s->deadline_original,s->primary->ledger,sizeof(*s->primary->ledger))||
     !slcli_source_deadline_away(s->deadline_original,s->primary->candidate,sizeof(*s->primary->candidate))||
     !slcli_source_deadline_away(s->deadline_original,s->workspace,sizeof(*s->workspace))||
     !slcli_source_deadline_away(s->deadline_original,original,original_bytes))goto unresolved;
  a=slcli_source_transition_begin(s->arena,s->deadline_original,s->transaction_original,
    original,operation,s->retained_attempts);
  if(!a)goto unresolved;
  s->retained_attempts=a;s->attempts[s->lock_depth-1]=a;
  if(!s->transaction_attempt) {
    s->transaction_attempt=a;
    if(!slcli_arena_live_range(s->arena,s->primary->ledger,sizeof(*s->primary->ledger))||
       !slcli_source_ledger_begin(a,s->primary))goto unresolved;
  }
  if(a->unresolved||a->entry.tick<s->observed_clock_frontier) {
    a->unresolved=1;goto unresolved;
  }
  s->observed_clock_frontier=a->entry.tick;
  return s;
unresolved:
  atomic_store_explicit(&s->transition_unresolved,1,memory_order_release);return 0;
}
struct slcli_source_linux *slcli_source_linux_enter(struct slcli_primary_engine *e,
  uint32_t operation,const void *original,const void *pending) {
  struct slcli_source_linux *s;long thread=thread_original(),current;
  if(!thread||!e||!(s=e->source_linux)||s->primary!=e||!original||
     operation<SLCLI_SOURCE_SERVICE||operation>SLCLI_SOURCE_CAPTURE)return 0;
  /* A contended transition denies this attempt; it never waits across IO. */
  if(atomic_flag_test_and_set_explicit(&s->transition,memory_order_acquire))return 0;
  current=atomic_load_explicit(&s->owning_thread,memory_order_acquire);
  if(current&&current!=thread)goto denied;
  if(atomic_load_explicit(&s->transition_unresolved,memory_order_acquire))goto denied;
  if(s->continued_original&&s->continued_thread!=thread)goto denied;
  if(current==thread) {
    uint32_t parent;
    if(s->failed||e->failed||!s->lock_depth||s->lock_depth>=3||pending||!live(s))goto denied;
    parent=s->operation[s->lock_depth-1];
    if(!((parent==SLCLI_SOURCE_SERVICE&&operation==SLCLI_SOURCE_DISPATCH)||
         (parent==SLCLI_SOURCE_DISPATCH&&operation==SLCLI_SOURCE_COMMIT)))goto denied;
    s->operation[s->lock_depth++]=operation;
    atomic_flag_clear_explicit(&s->transition,memory_order_release);return prepare(s,operation,original);
  }
  if(s->lock_depth)goto denied;
  if(s->pending_native_original) {
    if(!pending||pending!=s->pending_native_original||original!=s->transaction_original||
       operation!=SLCLI_SOURCE_SERVICE||e->committed_generation!=s->expected_generation||
       e->next!=s->expected_sequence)goto denied;
  } else {
    if(pending||s->failed||e->failed)goto denied;
    if(s->transaction_original&&(!s->continued_original||original!=s->continued_original||
       original!=s->transaction_original||operation!=SLCLI_SOURCE_SERVICE||
       e->committed_generation!=s->expected_generation||e->next!=s->expected_sequence))goto denied;
  }
  if(!live(s))goto denied;
  if(s->transaction_attempt&&!slcli_source_ledger_unchanged(s->transaction_attempt,e))goto denied;
  if(!s->pending_native_original&&!s->transaction_original) {
    s->transaction_original=original;s->expected_generation=e->committed_generation;
    s->expected_sequence=e->next;
  }
  s->operation[0]=operation;s->lock_depth=1;
  atomic_store_explicit(&s->owning_thread,thread,memory_order_release);
  atomic_flag_clear_explicit(&s->transition,memory_order_release);return prepare(s,operation,original);
denied:
  atomic_flag_clear_explicit(&s->transition,memory_order_release);return 0;
}
int slcli_source_linux_continue(struct slcli_source_linux *s,const void *original,int active) {
  int ok=0;long thread=thread_original();
  if(!s||!original||!thread||atomic_flag_test_and_set_explicit(&s->transition,memory_order_acquire))return 0;
  if(atomic_load_explicit(&s->owning_thread,memory_order_acquire)==thread&&live(s)&&!s->failed&&
     !atomic_load_explicit(&s->transition_unresolved,memory_order_acquire)&&s->lock_depth==1&&
     s->operation[0]==SLCLI_SOURCE_SERVICE&&!s->pending_native_original&&
     s->transaction_original==original&&(!s->continued_original||s->continued_original==original)) {
    s->continued_original=active?original:0;s->continued_thread=active?thread:0;ok=1;
  }
  atomic_flag_clear_explicit(&s->transition,memory_order_release);return ok;
}
int slcli_source_linux_pending(struct slcli_source_linux *s,const void *pending) {
  int ok=0;long thread=thread_original();
  if(!s||!thread)return 0;
  if(atomic_flag_test_and_set_explicit(&s->transition,memory_order_acquire)) {
    if(atomic_load_explicit(&s->owning_thread,memory_order_acquire)==thread)
      atomic_store_explicit(&s->transition_unresolved,1,memory_order_release);
    return 0;
  }
  if(atomic_load_explicit(&s->owning_thread,memory_order_acquire)==thread&&live(s)&&
     !atomic_load_explicit(&s->transition_unresolved,memory_order_acquire)&&
     s->lock_depth==1&&s->operation[0]==SLCLI_SOURCE_SERVICE&&s->transaction_original&&
     (!s->pending_native_original||!pending||s->pending_native_original==pending)) {
    s->pending_native_original=pending;ok=1;
  }
  atomic_flag_clear_explicit(&s->transition,memory_order_release);return ok;
}
static int leave_original(struct slcli_source_linux *s,int ok) {
  long thread=thread_original();struct slcli_source_transition_attempt *a;
  if(!s||!thread)return 0;
  if(atomic_load_explicit(&s->owning_thread,memory_order_acquire)!=thread||!s->lock_depth)return 0;
  if(atomic_load_explicit(&s->transition_unresolved,memory_order_acquire))return 0;
  a=s->attempts[s->lock_depth-1];
  if(!slcli_source_transition_before_leave(a)||a->leave.tick<s->observed_clock_frontier) {
    if(a)a->unresolved=1;
    atomic_store_explicit(&s->transition_unresolved,1,memory_order_release);return 0;
  }
  s->observed_clock_frontier=a->leave.tick;
  if(!slcli_source_transition_publication_clock(a)) {
    atomic_store_explicit(&s->transition_unresolved,1,memory_order_release);return 0;
  }
  /* One nonwaiting acquisition after the retained original clock capture.
   * No native clock/IO runs under this flag. Scheduling between capture and
   * publication is not an independently qualified absolute completion bound. */
  if(atomic_flag_test_and_set_explicit(&s->transition,memory_order_acquire)) {
    if(atomic_load_explicit(&s->owning_thread,memory_order_acquire)==thread)
      atomic_store_explicit(&s->transition_unresolved,1,memory_order_release);
    return 0;
  }
  if(atomic_load_explicit(&s->owning_thread,memory_order_acquire)!=thread||
     !s->lock_depth||!live(s)||
     atomic_load_explicit(&s->transition_unresolved,memory_order_acquire)) {
    atomic_flag_clear_explicit(&s->transition,memory_order_release);return 0;
  }
  if(!slcli_source_transition_locked_leave(a)) {
    atomic_store_explicit(&s->transition_unresolved,1,memory_order_release);
    atomic_flag_clear_explicit(&s->transition,memory_order_release);return 0;
  }
  if(!slcli_source_ledger_unchanged(s->transaction_attempt,s->primary)) {
    a->unresolved=1;atomic_store_explicit(&s->transition_unresolved,1,memory_order_release);
    atomic_flag_clear_explicit(&s->transition,memory_order_release);return 0;
  }
  s->observed_clock_frontier=a->leave_locked.tick;
  if(!ok){s->failed=1;s->primary->failed=1;}
  a->completed=1;s->attempts[s->lock_depth-1]=0;
  s->operation[--s->lock_depth]=0;
  if(!s->lock_depth) {
    if(ok&&!s->failed&&!s->pending_native_original) {
      if(s->continued_original==s->transaction_original) {
        s->expected_generation=s->primary->committed_generation;s->expected_sequence=s->primary->next;
      } else {s->transaction_original=0;s->transaction_attempt=0;}
    }
    atomic_store_explicit(&s->owning_thread,0,memory_order_release);
  }
  atomic_flag_clear_explicit(&s->transition,memory_order_release);return ok;
}
int slcli_source_linux_leave(struct slcli_source_linux *s,int ok) {
  struct slcli_source_transition_attempt *a;int returned;
  if(!s||atomic_load_explicit(&s->owning_thread,memory_order_acquire)!=thread_original()||
     !s->lock_depth||s->lock_depth>3)return 0;
  a=s->attempts[s->lock_depth-1];
  if(!a||a->effect_returned||a->publication_returned)return 0;
  a->effect_result=ok;a->effect_returned=1;
  returned=leave_original(s,ok);
  a->publication_result=returned;a->publication_returned=1;return returned;
}
#endif
