#include "source_windows.h"
#if defined(_WIN32)
#include "service_channel.h"
static int live(struct slcli_source_windows *s) {
  return s&&s->arena&&slcli_arena_live_range(s->arena,s,sizeof(*s))&&
    s->store&&slcli_arena_live_range(s->arena,s->store,sizeof(*s->store))&&
    s->store->arena==s->arena&&s->store->source_owner==s&&s->arena->owner&&s->arena->owner->limit==1048576&&
    s->workspace&&slcli_arena_live_range(s->arena,s->workspace,sizeof(*s->workspace))&&
    s->path&&slcli_arena_live_range(s->arena,s->path,sizeof(*s->path))&&
    s->path->units==32772&&slcli_arena_live_range(s->arena,s->path->raw,32772*sizeof(WCHAR));
}
struct slcli_source_windows *slcli_source_windows_allocate(struct slcli_capture_store_owner *store) {
  struct slcli_source_windows *s;struct slcli_arena *a;
  if(!store||!(a=store->arena)||!slcli_arena_live_range(a,store,sizeof(*store))||
    !a->owner||a->owner->limit!=1048576||!store->primary||
    !slcli_arena_live_range(a,store->primary,sizeof(*store->primary))||
    store->primary->capture_store!=store||store->source_owner||store->count)return 0;
  s=slcli_arena_calloc(a,1,sizeof(*s));if(!s)return 0;
  s->arena=a;s->store=store;s->failed=1;store->source_owner=s;
  s->path=slcli_capture_windows_workspaces_allocate(a,&s->workspace);
  if(!s->path)return s; /* Partial owning allocation remains discoverable. */
  InitializeSRWLock(&s->lock);s->failed=0;return s;
}
int slcli_source_windows_held(struct slcli_source_windows *s) {
  return s&&!InterlockedCompareExchange(&s->transition_unresolved,0,0)&&
    InterlockedCompareExchange(&s->owning_thread,0,0)==(LONG)GetCurrentThreadId()&&live(s)&&
    s->lock_depth==1&&s->operation[0]==SLCLI_SOURCE_CAPTURE&&(!s->failed||s->active);
}
static struct slcli_source_windows *prepare(struct slcli_source_windows *s,
  uint32_t operation,const void *original) {
  struct slcli_source_transition_attempt *a;
  size_t original_bytes=operation==SLCLI_SOURCE_SERVICE?sizeof(struct slcli_service_request):
    operation==SLCLI_SOURCE_CAPTURE?sizeof(struct slcli_capture_windows_originals):
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
    } else if(operation==SLCLI_SOURCE_CAPTURE) {
      const struct slcli_capture_windows_originals *r=original;
      if(!slcli_arena_live_range(s->arena,r,sizeof(*r)))goto unresolved;
      s->deadline_original=r->deadline_original;
    } else {
      const struct slcli_primary_commit_originals *r=original;
      if(!slcli_arena_live_range(s->arena,r,sizeof(*r)))goto unresolved;
      s->deadline_original=r->deadline_original;
    }
  }
  if(!slcli_source_deadline_original_matches(s->arena,s->deadline_original,s->transaction_original)||
     !slcli_source_deadline_away(s->deadline_original,s,sizeof(*s))||
     !slcli_source_deadline_away(s->deadline_original,s->store,sizeof(*s->store))||
     !slcli_source_deadline_away(s->deadline_original,s->store->primary,sizeof(*s->store->primary))||
     !slcli_source_deadline_away(s->deadline_original,s->store->primary->ledger,sizeof(*s->store->primary->ledger))||
     !slcli_source_deadline_away(s->deadline_original,s->store->primary->candidate,sizeof(*s->store->primary->candidate))||
     !slcli_source_deadline_away(s->deadline_original,s->workspace,sizeof(*s->workspace))||
     !slcli_source_deadline_away(s->deadline_original,s->path->raw,32772*sizeof(WCHAR))||
     !slcli_source_deadline_away(s->deadline_original,original,original_bytes))goto unresolved;
  a=slcli_source_transition_begin(s->arena,s->deadline_original,s->transaction_original,
    original,operation,s->retained_attempts);
  if(!a)goto unresolved;
  s->retained_attempts=a;s->attempts[s->lock_depth-1]=a;
  if(!s->transaction_attempt) {
    s->transaction_attempt=a;
    if(!slcli_arena_live_range(s->arena,s->store->primary->ledger,sizeof(*s->store->primary->ledger))||
       !slcli_source_ledger_begin(a,s->store->primary))goto unresolved;
  }
  if(a->unresolved||a->entry.tick<s->observed_clock_frontier) {
    a->unresolved=1;goto unresolved;
  }
  s->observed_clock_frontier=a->entry.tick;
  return s;
unresolved:InterlockedExchange(&s->transition_unresolved,1);return 0;
}
int slcli_source_windows_graph_held(struct slcli_source_windows *s,
  const struct slcli_capture_windows *c,const struct slcli_capture_windows_originals *o) {
  return slcli_source_windows_held(s)&&s->active&&s->count&&s->count<=16&&
    s->graphs[s->count-1]==c&&s->originals[s->count-1]==o;
}
struct slcli_source_windows *slcli_source_windows_enter(struct slcli_primary_engine *e,uint32_t operation,
  const void *original,const void *pending) {
  struct slcli_source_windows *s;LONG thread=(LONG)GetCurrentThreadId();
  if(!e||!e->capture_store||!(s=e->capture_store->source_owner)||
    s->store!=e->capture_store||s->store->primary!=e||
    !original||operation<SLCLI_SOURCE_SERVICE||operation>SLCLI_SOURCE_CAPTURE)return 0;
  if(!TryAcquireSRWLockExclusive(&s->lock))return 0;
  if(InterlockedCompareExchange(&s->transition_unresolved,0,0))goto denied;
  if(s->continued_original&&s->continued_thread!=thread)goto denied;
  /* Check exclusive marker BEFORE mutable arena metadata. A conflicting
   * thread must not race the actual owner's retained allocation updates while
   * it is executing outside this short transition section. */
  if(InterlockedCompareExchange(&s->owning_thread,0,0)&&
     InterlockedCompareExchange(&s->owning_thread,0,0)!=thread)goto denied;
  if(InterlockedCompareExchange(&s->owning_thread,0,0)==thread) {
    uint32_t parent;
    if(s->failed||e->failed||!s->lock_depth||s->lock_depth>=3||pending||!live(s))goto denied;
    parent=s->operation[s->lock_depth-1];
    /* Only the real existing service -> dispatch -> commit call chain may
     * recurse. Native retention callbacks cannot start another transaction or
     * capture inside an in-progress original store effect. */
    if(!((parent==SLCLI_SOURCE_SERVICE&&operation==SLCLI_SOURCE_DISPATCH)||
         (parent==SLCLI_SOURCE_DISPATCH&&operation==SLCLI_SOURCE_COMMIT)))goto denied;
    s->operation[s->lock_depth++]=operation;
    ReleaseSRWLockExclusive(&s->lock);return prepare(s,operation,original);
  }
  if(s->lock_depth||InterlockedCompareExchange(&s->owning_thread,0,0))goto denied;
  if(s->pending_native_original) {
    /* Only the genuine SAME pending row may resume. A cancellation request,
     * new capture or another request cannot steal its buffer/workspace. */
    if(!pending||pending!=s->pending_native_original||original!=s->transaction_original||
       operation!=SLCLI_SOURCE_SERVICE||
       e->committed_generation!=s->expected_generation||e->next!=s->expected_sequence)goto denied;
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
  s->operation[0]=operation;s->lock_depth=1;InterlockedExchange(&s->owning_thread,thread);
  ReleaseSRWLockExclusive(&s->lock);return prepare(s,operation,original);
denied:ReleaseSRWLockExclusive(&s->lock);return 0;
}
int slcli_source_windows_continue(struct slcli_source_windows *s,const void *original,int active) {
  int ok=0;
  if(!s||!original||!TryAcquireSRWLockExclusive(&s->lock))return 0;
  if(InterlockedCompareExchange(&s->owning_thread,0,0)==(LONG)GetCurrentThreadId()&&
     !InterlockedCompareExchange(&s->transition_unresolved,0,0)&&live(s)&&!s->failed&&
     s->lock_depth==1&&s->operation[0]==SLCLI_SOURCE_SERVICE&&!s->pending_native_original&&
     s->transaction_original==original&&(!s->continued_original||s->continued_original==original)) {
    s->continued_original=active?original:0;
    s->continued_thread=active?(LONG)GetCurrentThreadId():0;ok=1;
  }
  ReleaseSRWLockExclusive(&s->lock);return ok;
}
int slcli_source_windows_pending(struct slcli_source_windows *s,const void *pending) {
  int ok=0;
  if(!s)return 0;
  if(!TryAcquireSRWLockExclusive(&s->lock)) {
    if(InterlockedCompareExchange(&s->owning_thread,0,0)==(LONG)GetCurrentThreadId())
      InterlockedExchange(&s->transition_unresolved,1);
    return 0;
  }
  if(InterlockedCompareExchange(&s->owning_thread,0,0)==(LONG)GetCurrentThreadId()&&
     !InterlockedCompareExchange(&s->transition_unresolved,0,0)&&live(s)&&
     s->lock_depth==1&&s->operation[0]==SLCLI_SOURCE_SERVICE&&
     s->transaction_original&&(!s->pending_native_original||
       !pending||s->pending_native_original==pending)) {
    s->pending_native_original=pending;ok=1;
  }
  ReleaseSRWLockExclusive(&s->lock);return ok;
}
int slcli_source_windows_pending_custody(struct slcli_source_windows *s,
  const void *original,const void *pending) {
  if(!s||!original||!pending||
     InterlockedCompareExchange(&s->owning_thread,0,0)!=(LONG)GetCurrentThreadId()||
     !InterlockedCompareExchange(&s->transition_unresolved,0,0))return 0;
  /* Mutable metadata is inspected only by the retained logical owner. Pending
   * request storage itself also retains the row if pending publication failed. */
  return live(s)&&s->lock_depth==1&&s->operation[0]==SLCLI_SOURCE_SERVICE&&
    s->transaction_original==original&&
    (!s->pending_native_original||s->pending_native_original==pending);
}
static int leave_original(struct slcli_source_windows *s,int ok) {
  struct slcli_source_transition_attempt *a;
  if(!s)return 0;
  if(InterlockedCompareExchange(&s->owning_thread,0,0)!=(LONG)GetCurrentThreadId()||
     !s->lock_depth)return 0;
  if(InterlockedCompareExchange(&s->transition_unresolved,0,0))return 0;
  a=s->attempts[s->lock_depth-1];
  if(!slcli_source_transition_before_leave(a)||a->leave.tick<s->observed_clock_frontier) {
    if(a)a->unresolved=1;
    InterlockedExchange(&s->transition_unresolved,1);return 0;
  }
  s->observed_clock_frontier=a->leave.tick;
  if(!slcli_source_transition_publication_clock(a)) {
    InterlockedExchange(&s->transition_unresolved,1);return 0;
  }
  /* One nonwaiting acquisition; the clock is retained outside SRW. This
   * capture does not establish a scheduler/native absolute completion bound. */
  if(!TryAcquireSRWLockExclusive(&s->lock)) {
    if(InterlockedCompareExchange(&s->owning_thread,0,0)==(LONG)GetCurrentThreadId())
      InterlockedExchange(&s->transition_unresolved,1);
    return 0;
  }
  if(InterlockedCompareExchange(&s->owning_thread,0,0)!=(LONG)GetCurrentThreadId()||
    !s->lock_depth||!live(s)||InterlockedCompareExchange(&s->transition_unresolved,0,0))
    {ReleaseSRWLockExclusive(&s->lock);return 0;}
  if(!slcli_source_transition_locked_leave(a)) {
    InterlockedExchange(&s->transition_unresolved,1);ReleaseSRWLockExclusive(&s->lock);return 0;
  }
  if(!slcli_source_ledger_unchanged(s->transaction_attempt,s->store->primary)) {
    a->unresolved=1;InterlockedExchange(&s->transition_unresolved,1);
    ReleaseSRWLockExclusive(&s->lock);return 0;
  }
  s->observed_clock_frontier=a->leave_locked.tick;
  if(!ok){s->failed=1;s->store->primary->failed=1;}
  a->completed=1;s->attempts[s->lock_depth-1]=0;
  s->operation[--s->lock_depth]=0;
  if(!s->lock_depth) {
    /* Failed/pending originals remain retained; no dispatch/reuse is enabled.
     * Successful native closure removes only this marker, never its original
     * request/capture/file/capacity ownership or archive obligations. */
    if(ok&&!s->failed&&!s->pending_native_original) {
      if(s->continued_original==s->transaction_original) {
        s->expected_generation=s->store->primary->committed_generation;
        s->expected_sequence=s->store->primary->next;
      } else {s->transaction_original=0;s->transaction_attempt=0;}
    }
    InterlockedExchange(&s->owning_thread,0);
  }
  ReleaseSRWLockExclusive(&s->lock);return ok;
}
int slcli_source_windows_leave(struct slcli_source_windows *s,int ok) {
  struct slcli_source_transition_attempt *a;int returned;
  if(!s||InterlockedCompareExchange(&s->owning_thread,0,0)!=(LONG)GetCurrentThreadId()||
     !s->lock_depth||s->lock_depth>3)return 0;
  a=s->attempts[s->lock_depth-1];
  if(!a||a->effect_returned||a->publication_returned)return 0;
  a->effect_result=ok;a->effect_returned=1;
  returned=leave_original(s,ok);
  a->publication_result=returned;a->publication_returned=1;return returned;
}
static int away(struct slcli_source_windows *s,const void *raw,size_t bytes) {
  uintptr_t x=(uintptr_t)raw,y=(uintptr_t)s->workspace,z=(uintptr_t)s->path->raw;
  size_t query=32772*sizeof(WCHAR);
  return raw&&bytes&&x<=UINTPTR_MAX-bytes&&y<=UINTPTR_MAX-sizeof(*s->workspace)&&
    z<=UINTPTR_MAX-query&&!(x<y+sizeof(*s->workspace)&&y<x+bytes)&&
    !(x<z+query&&z<x+bytes);
}
static int inputs(struct slcli_source_windows *s,const struct slcli_capture_windows_originals *o) {
  struct slcli_arena *a=s->arena;const struct slcli_peer_windows_originals *p;
  if(!o||!away(s,o,sizeof(*o))||!slcli_arena_live_range(a,o,sizeof(*o))||!(p=o->peer)||
    !away(s,p,sizeof(*p))||!slcli_arena_live_range(a,p,sizeof(*p))||!p->token||
    !away(s,p->token,sizeof(*p->token))||!slcli_arena_live_range(a,p->token,sizeof(*p->token))||!p->owner_bytes||p->owner_bytes>184||
    !away(s,p->owner,p->owner_bytes)||!slcli_arena_live_range(a,p->owner,p->owner_bytes)||
    !o->root_identity||!away(s,o->root_identity,sizeof(*o->root_identity))||!slcli_arena_live_range(a,o->root_identity,sizeof(*o->root_identity))||
    !o->file_token||!away(s,o->file_token,sizeof(*o->file_token))||!slcli_arena_live_range(a,o->file_token,sizeof(*o->file_token))||
    !o->file_owner_bytes||o->file_owner_bytes>184||
    !away(s,o->file_owner,o->file_owner_bytes)||!slcli_arena_live_range(a,o->file_owner,o->file_owner_bytes)||
    !o->descriptor_bytes||!away(s,o->descriptor,o->descriptor_bytes)||!slcli_arena_live_range(a,o->descriptor,o->descriptor_bytes)||
    !o->archive_original_bytes||!away(s,o->archive_original,o->archive_original_bytes)||
    !slcli_arena_live_range(a,o->archive_original,o->archive_original_bytes)||
    !o->leaf_units||o->leaf_units>32767||
    !away(s,o->leaf,o->leaf_units*sizeof(WCHAR))||!slcli_arena_live_range(a,o->leaf,o->leaf_units*sizeof(WCHAR)))return 0;
  /* The raw peer body will perform fresh native process/birth/pipe/token/image
   * observations. These original native values never come from a snapshot row. */
  return p->process&&p->process!=INVALID_HANDLE_VALUE&&
    p->channel&&p->channel!=INVALID_HANDLE_VALUE&&p->image&&p->image!=INVALID_HANDLE_VALUE&&
    (p->creation.dwLowDateTime||p->creation.dwHighDateTime)&&
    o->root==s->store->root&&o->effective_token&&o->effective_token!=INVALID_HANDLE_VALUE&&
    o->path_sink&&o->mode_sink&&o->file_sink&&o->archive_original;
}
static int hold(void *original,const struct slcli_capture_windows *c) {
  struct slcli_source_windows *s=original;uint32_t at;
  if(!live(s)||!s->active||!s->count||s->count>16)return 0;at=s->count-1;
  /* Graph and originals were attached BEFORE every native effect. This is a
   * concrete in-owner retention sink, never a fabricated private observation. */
  return s->graphs[at]==c&&s->originals[at]&&c->arena==s->arena&&
    c->store_loan&&c->store_loan->owner==s->store;
}
int slcli_source_windows_capture(struct slcli_source_windows *s,
  const struct slcli_capture_windows_originals *o,size_t file_rows,size_t record_rows,
  size_t held_rows,size_t route_rows) {
  struct slcli_capture_windows *c;uint32_t i,at;int ok=0;
  if(!s||!s->store||slcli_source_windows_enter(s->store->primary,SLCLI_SOURCE_CAPTURE,o,0)!=s)return 0;
  if(s->failed||s->active||s->count>=16||!inputs(s,o))goto done;
  for(i=0;i<s->count;i++)if(s->originals[i]==o||
    s->originals[i]->peer==o->peer||s->originals[i]->peer->process==o->peer->process||
    s->graphs[i]->failed||!s->graphs[i]->complete)goto done;
  c=slcli_capture_windows_allocate(s->store,s->workspace,s->path,
    file_rows,record_rows,held_rows,route_rows);
  if(!c){s->failed=1;goto done;}
  at=s->count++;s->graphs[at]=c;s->originals[at]=o;s->active=1;s->failed=1;
  ok=slcli_capture_windows_original(c,o,hold,s);
  s->active=0;if(ok)s->failed=0;
  /* Lock completion is only serialization. Every graph, private file loan,
   * failed raw row and original source remains held; no terminal/FREE route. */
done:
  return slcli_source_windows_leave(s,ok);
}
#endif
