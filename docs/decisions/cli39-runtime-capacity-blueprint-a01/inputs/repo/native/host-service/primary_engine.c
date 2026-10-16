#include "primary_engine.h"
#if defined(_WIN32) || defined(__linux__)
#include "source_transition.h"
#include "arena.h"
#include "store_publish_deadline.h"
#endif
#if defined(_WIN32) || defined(__linux__)
struct slcli_primary_engine *slcli_primary_engine_allocate(struct slcli_arena *a) {
  struct primary_block {
    struct slcli_primary_engine engine;
    struct slcli_ledger_state committed,candidate;
  } *b;
  if(!a||!a->owner||a->owner->limit!=1048576)return 0;
  b=slcli_arena_calloc(a,1,sizeof(*b));if(!b)return 0;
  b->engine.ledger=&b->committed;b->engine.candidate=&b->candidate;
  return &b->engine;
}
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static void copy32(unsigned char out[32],const unsigned char *raw) {
  unsigned i;for(i=0;i<32;i++)out[i]=raw[i];
}
static int nonzero(const unsigned char *p) {
  unsigned char n=0;unsigned i;for(i=0;i<32;i++)n|=p[i];return n!=0;
}
static int same_owner(struct slcli_slice a,struct slcli_slice b) {
  return a.raw&&b.raw&&a.bytes==b.bytes&&slcli_equal(a.raw,b.raw,a.bytes);
}
static int hash_snapshot(const unsigned char reservation[32],
                          const struct slcli_snapshot_claim *s,unsigned char out[32]) {
  static const unsigned char domain[]="SLCLI-HOST-SNAPSHOT-1";struct slcli_sha256 h;
  slcli_sha256_init(&h);
  return slcli_sha256_update(&h,domain,sizeof(domain))&&slcli_sha256_update(&h,reservation,32)&&
    slcli_sha256_update(&h,s->raw,s->bytes)&&slcli_sha256_finish(&h,out);
}
static int match_snapshot(const struct slcli_binding_originals *b,
                            const struct slcli_snapshot_claim *s) {
 uint32_t i;struct slcli_runtime_row_claim q;
 if(!b||!s||!b->references||!b->original_ids||!b->original_indices||!b->count||b->count>128||s->count!=b->count||s->bytes!=6+b->count*SLCLI_RUNTIME_ROW_BYTES)return 0;
 for(i=0;i<b->count;i++){
  const struct slcli_native_reference *r=&b->references[i];
  if(!slcli_runtime_row_claim_parse(s->raw+6+i*SLCLI_RUNTIME_ROW_BYTES,i,b->count,&q)||
     q.id!=b->original_ids[i]||q.kind!=r->kind||q.state!=r->state||q.charged!=r->charged||
     !slcli_equal(q.identity,r->identity,32)||!slcli_equal(q.observation,r->observation,32)||
     q.handle_index!=b->original_indices[i]||q.owner_domain!=r->owner_domain||
     q.process_role!=r->process_role||q.producer_slot!=r->producer_slot||q.consumer_slot!=r->consumer_slot||
     q.source_member!=r->source_member||q.native_rights!=r->native_rights||q.alias_of!=r->alias_of||
     q.physical_extent!=r->physical_extent||q.capture_first!=r->capture_first||q.capture_count!=r->capture_count||
     q.start_cut!=r->start_cut||q.end_cut!=r->end_cut||q.effect_flags!=r->effect_flags||q.input_slot!=r->input_slot||
     q.applicability!=r->applicability||((q.applicability&128)!=0)!=((b->primary_terminal_bitmap[i/8]&(1u<<(i%8)))!=0)||
     !r->native_original||!r->native_recheck||!r->native_recheck(r,&q))return 0;
 }
 /* The private source verifier, not equality, admits actual native originals.
  * Its genuine installed constructor/ranges remain required source work. */
 return 1;
}
static int candidate_begin(struct slcli_primary_engine *e) {
  if(!e||!e->ledger||!e->candidate||e->failed||e->candidate_started||
     e->committed_generation==UINT64_MAX||
     !separate(e,sizeof(*e),e->candidate,sizeof(*e->candidate))||
     !separate(e->ledger,sizeof(*e->ledger),e->candidate,sizeof(*e->candidate)))return 0;
#if defined(_WIN32)
  if(!e->capture_store||!e->capture_store->source_owner||
     !slcli_arena_live_range(e->capture_store->source_owner->arena,
       e->candidate,sizeof(*e->candidate)))return 0;
#elif defined(__linux__)
  if(!e->source_linux||!slcli_arena_live_range(e->source_linux->arena,
       e->candidate,sizeof(*e->candidate)))return 0;
#endif
  e->candidate_base_generation=e->committed_generation;e->candidate_started=1;
  *e->candidate=*e->ledger;
  e->candidate->generation=e->committed_generation+1;
  e->candidate->previous_present=e->committed_generation!=0;
  if(e->candidate->previous_present)copy32(e->candidate->previous,e->payload_hash);
  return 1;
}
static int commit_original(struct slcli_primary_engine *e,const struct slcli_primary_commit_originals *o) {
  struct slcli_ledger_state *l;unsigned i;int completed,progress=0;
  struct slcli_source_owner *owner;
  const struct slcli_primary_commit_originals *original;
  struct slcli_ledger_commit *pair;
  if(!e||!e->ledger||!e->candidate||e->failed||!e->candidate_started||
     e->candidate_base_generation!=e->committed_generation||
     e->committed_generation==UINT64_MAX||!o||!o->commit||
     !separate(e->ledger,sizeof(*e->ledger),e->candidate,sizeof(*e->candidate)))return 0;
  l=e->candidate;
  if(l->generation!=e->committed_generation+1||
     l->previous_present!=(e->committed_generation!=0)||
     (l->previous_present&&!slcli_equal(l->previous,e->payload_hash,32)))return 0;
#if defined(_WIN32)
  owner=e->capture_store?e->capture_store->source_owner:0;
#else
  owner=e->source_linux;
#endif
  if(!owner||!owner->lock_depth||owner->lock_depth>3||
     owner->operation[owner->lock_depth-1]!=SLCLI_SOURCE_COMMIT||
     !owner->attempts[owner->lock_depth-1]||owner->attempts[owner->lock_depth-1]->original!=o||
     !owner->transaction_attempt||owner->transaction_attempt->ledger_progress.commit_original!=o||
     !(original=owner->transaction_attempt->ledger_progress.commit_before)||
     !slcli_equal((const unsigned char *)original,(const unsigned char *)o,sizeof(*o)))return 0;
  pair=original->commit;
  completed=slcli_ledger_commit_original(pair,l,e->key,original->root,original->generation_temporary,
       original->selector_temporary,(uint8_t)(1-e->selected_slot),e->retained_store_bytes,
       original->capture_sink,original->capture_original);
  progress=slcli_source_ledger_record(owner->transaction_attempt,e,o,pair,completed);
  if(pair->started)e->retained_store_bytes=pair->retained_store_bytes;
  if(!pair->durable_pair_complete||pair->committed_generation!=l->generation||
     pair->selected_slot!=(uint32_t)(1-e->selected_slot))return 0;
  /* Publication follows BOTH actual durable held/reopened readbacks. Before
   * this cut old committed cells/selector/hash remain untouched. */
  *e->ledger=*l;e->committed_pair_original=pair;
  e->committed_generation=pair->committed_generation;
  e->selected_slot=pair->selected_slot;e->retained_store_bytes=pair->retained_store_bytes;
  for(i=0;i<32;i++)e->payload_hash[i]=pair->payload_hash[i];
  if(!progress||!completed||!pair->complete||pair->failed)return 0;
  e->candidate_started=0;return 1;
}
int slcli_primary_engine_commit_original(struct slcli_primary_engine *e,const struct slcli_primary_commit_originals *o) {
  int ok;
#if defined(_WIN32) || defined(__linux__)
  struct slcli_source_owner *owner=slcli_source_enter(e,SLCLI_SOURCE_COMMIT,o,0);if(!owner)return 0;
  if(!slcli_arena_live_range(owner->arena,o,sizeof(*o))||
     !slcli_arena_live_range(owner->arena,e->ledger,sizeof(*e->ledger))||
     !slcli_arena_live_range(owner->arena,e->candidate,sizeof(*e->candidate))||
     !slcli_arena_live_range(owner->arena,o->commit,sizeof(*o->commit))) {
    slcli_source_leave(owner,0);return 0;
  }
#endif
  ok=owner->lock_depth&&owner->lock_depth<=3&&owner->attempts[owner->lock_depth-1]&&
    o->deadline_original==owner->deadline_original&&o->caller_original_bytes&&o->capture_original_bytes&&
    slcli_source_ledger_prepare(owner->arena,owner->transaction_attempt,e,o)&&
    slcli_ledger_commit_deadline_bind(owner->arena,o->commit,owner->deadline_original,
      owner->transaction_original,o->caller_original_bytes,
      &owner->attempts[owner->lock_depth-1]->entry,o->capture_sink,o->capture_original,
      o->capture_original_bytes)&&
    slcli_store_publish_deadline_bind_ledger(o->commit,o->namespace_sink)&&
    e&&(e->candidate_started||candidate_begin(e))&&commit_original(e,o);
  if(o->commit->deadline_owner&&
     o->commit->deadline_owner->previous_tick>owner->observed_clock_frontier)
    owner->observed_clock_frontier=o->commit->deadline_owner->previous_tick;
  if(!ok&&e)e->failed=1;
#if defined(_WIN32) || defined(__linux__)
  ok=slcli_source_leave(owner,ok);
#endif
  if(!ok&&e)e->failed=1;
  return ok;
}
static void response_header(struct slcli_frame *out,uint8_t op,uint32_t sequence) {
  out->body[0]=op;slcli_put_u32(out->body+1,sequence);out->body[5]=0;
}
static int artifact(struct slcli_primary_engine *e,const struct slcli_primary_commit_originals *o,const unsigned char *raw,
                      uint32_t bytes,uint32_t leaf,uint32_t slot) {
  struct slcli_store_stream *streams[3];unsigned i,j;int ok=0;
  struct slcli_source_owner *owner;struct slcli_store_deadline_owner *d;
#if defined(_WIN32)
  if(!e||!e->capture_store||!(owner=e->capture_store->source_owner))return 0;
#else
  if(!e||!(owner=e->source_linux))return 0;
#endif
  if(!owner->lock_depth||owner->lock_depth>3||
     owner->operation[owner->lock_depth-1]!=SLCLI_SOURCE_DISPATCH||
     !owner->attempts[owner->lock_depth-1]||o->deadline_original!=owner->deadline_original)return 0;
  streams[0]=o->artifact_write;streams[1]=o->artifact_held;streams[2]=o->artifact_reopened;
  if(!raw||!bytes||!o->artifact_publish||o->artifact_publish->started)return 0;
  for(i=0;i<3;i++) {
    if(!streams[i]||!streams[i]->captures||!streams[i]->capture_capacity||
       streams[i]->capture_capacity>SIZE_MAX/sizeof(*streams[i]->captures)||
       !separate(streams[i],sizeof(*streams[i]),raw,bytes)||
       !separate(streams[i]->captures,streams[i]->capture_capacity*sizeof(*streams[i]->captures),raw,bytes)||
       !separate(streams[i],sizeof(*streams[i]),o->artifact_publish,sizeof(*o->artifact_publish))||
       !separate(streams[i]->captures,streams[i]->capture_capacity*sizeof(*streams[i]->captures),
         o->artifact_publish,sizeof(*o->artifact_publish)))return 0;
    for(j=0;j<i;j++)if(!separate(streams[i],sizeof(*streams[i]),streams[j],sizeof(*streams[j]))||
       !separate(streams[i],sizeof(*streams[i]),streams[j]->captures,streams[j]->capture_capacity*sizeof(*streams[j]->captures))||
       !separate(streams[i]->captures,streams[i]->capture_capacity*sizeof(*streams[i]->captures),streams[j],sizeof(*streams[j]))||
       !separate(streams[i]->captures,streams[i]->capture_capacity*sizeof(*streams[i]->captures),
         streams[j]->captures,streams[j]->capture_capacity*sizeof(*streams[j]->captures)))return 0;
  }
  d=slcli_store_deadline_bind(owner->arena,streams,3,owner->deadline_original,
    owner->transaction_original,o->caller_original_bytes,
    &owner->attempts[owner->lock_depth-1]->entry,o->capture_sink,o->capture_original,
    o->capture_original_bytes);
  if(!d||!slcli_store_publish_deadline_bind_artifact(o->artifact_publish,d,o->namespace_sink))return 0;
  if(!slcli_store_stream_begin(o->artifact_write,o->artifact_temporary,0,bytes,o->capture_sink,o->capture_original)||
     !slcli_store_stream_append(o->artifact_write,raw,bytes)||!slcli_store_stream_finish(o->artifact_write)||
     !slcli_store_stream_begin(o->artifact_held,o->artifact_temporary,1,bytes,o->capture_sink,o->capture_original)||
     !slcli_store_stream_append(o->artifact_held,raw,bytes)||!slcli_store_stream_finish(o->artifact_held))goto done;
  o->artifact_publish->index=slot;
  if(!slcli_store_publish_original(o->artifact_publish,o->artifact_write,o->root,o->artifact_nonce,leaf)||
     !slcli_store_stream_begin(o->artifact_reopened,o->artifact_publish->reopened,1,bytes,o->capture_sink,o->capture_original)||
     !slcli_store_stream_append(o->artifact_reopened,raw,bytes)||!slcli_store_stream_finish(o->artifact_reopened))goto done;
  ok=slcli_store_deadline_ready(d)&&slcli_store_publish_deadline_ready(o->artifact_publish);
done:
  if(d->previous_tick>owner->observed_clock_frontier)owner->observed_clock_frontier=d->previous_tick;
  return ok;
}
static int dispatch_original(struct slcli_primary_engine *e,const struct slcli_frame *in,
                                   const unsigned char mac[32],struct slcli_native_clock *clock,
                                   const struct slcli_primary_commit_originals *originals,
                                   struct slcli_frame *out) {
  struct slcli_primary_request r;struct slcli_ledger_slot *s;
  const struct slcli_binding_originals *b;unsigned i,used=0,user=0;uint32_t slot=SLCLI_BINDINGS;
  struct slcli_sha256 h;unsigned char raw_hash[32],snapshot_hash[32],intent_hash[32];
  static const unsigned char intent_domain[]="SLCLI-HOST-CLOSE-INTENT-2";
  if(!e||!in||!mac||!clock||!originals||!out||!e->ledger||!e->binding||
     !separate(in,sizeof(*in),out,sizeof(*out))||!separate(e,sizeof(*e),out,sizeof(*out))||
     !separate(e->ledger,sizeof(*e->ledger),out,sizeof(*out))||
     !separate(clock,sizeof(*clock),in,sizeof(*in))||
     !separate(clock,sizeof(*clock),e,sizeof(*e))||
     !separate(clock,sizeof(*clock),e->ledger,sizeof(*e->ledger)))return 0;
  b=e->binding;
  if(e->failed||e->closing||e->next>1025||e->selected_slot>1||
     !b->original_limit||!b->deadline||!nonzero(b->primary)||!nonzero(b->observer)||
     !nonzero(b->reservation)||!slcli_equal(b->epoch,e->ledger->epoch,32)||
     !nonzero(e->capability)||!nonzero(e->nonce)||
     !slcli_frame_verify(SLCLI_HOST,0,e->nonce,e->capability,e->next,in,mac)||
     !slcli_primary_request_parse(in,b->original_limit,&r)||r.sequence!=e->next||
     !slcli_native_clock_read(clock)||clock->tick>=b->deadline)goto failed;
  if(!candidate_begin(e))goto failed;
  if(r.operation==1) {
    if(e->reserved||e->next)goto failed;
    for(i=0;i<SLCLI_BINDINGS;i++) {
      const struct slcli_ledger_slot *row=&e->ledger->slots[i];
      if(row->state!=SLCLI_FREE){used++;if(same_owner(row->owner,b->owner))user++;}
      if(slot==SLCLI_BINDINGS&&row->state==SLCLI_FREE&&!e->ledger->terminal[i].occupied)slot=i;
    }
    if(used>=SLCLI_BINDINGS||user>=4||slot==SLCLI_BINDINGS)goto failed;
    {
      const uint64_t closure_reserve=3*SLCLI_RUNTIME_SNAPSHOT_BYTES+512;
      if(e->retained_store_bytes>SLCLI_STORE_LIMIT||
         closure_reserve>SLCLI_STORE_LIMIT-e->retained_store_bytes)goto failed;
      e->retained_store_bytes+=closure_reserve;
    }
    s=&e->candidate->slots[slot];
    slcli_erase(s,sizeof(*s));s->owner=b->owner;s->state=SLCLI_RESERVED;
    s->generation=s->created=e->committed_generation+1;s->deadline=b->deadline;
    copy32(s->reservation,b->reservation);copy32(s->primary,b->primary);copy32(s->observer,b->observer);
    /* The original source owner prebinds the actual initial native resource
     * digest before this dispatch. No RESERVE request supplies it. */
    if(!b->count||b->count>128||!b->references||!b->original_ids||
       b->initial_snapshot_bytes>SLCLI_RUNTIME_SNAPSHOT_BYTES||(!b->initial_snapshot&&b->initial_snapshot_bytes))goto failed;
    {
      struct slcli_snapshot_claim initial={0};
      if(!slcli_snapshot_claim_parse(b->initial_snapshot,b->initial_snapshot_bytes,b->original_limit,&initial)||
         !match_snapshot(b,&initial))goto failed;
      if(!hash_snapshot(b->reservation,&initial,s->resource)||!slcli_primary_engine_commit_original(e,originals))goto failed;
    }
    e->slot=slot;e->reserved=1;response_header(out,2,r.sequence);copy32(out->body+6,b->reservation);
    out->body[38]=(unsigned char)e->slot;slcli_put_u64(out->body+39,e->committed_generation);
    copy32(out->body+47,b->observer);slcli_put_u64(out->body+79,b->deadline);out->length=87;
  } else if(r.operation==3) {
    if(!e->reserved||e->slot>=SLCLI_BINDINGS||!slcli_equal(r.reservation,b->reservation,32)||
       r.snapshot.phase<e->last_phase||!match_snapshot(b,&r.snapshot)||
       !hash_snapshot(b->reservation,&r.snapshot,snapshot_hash))goto failed;
    s=&e->candidate->slots[e->slot];if(s->state!=SLCLI_RESERVED&&s->state!=SLCLI_LIVE)goto failed;
    if(e->retained_store_bytes>SLCLI_STORE_LIMIT||
       2*(uint64_t)r.snapshot.bytes>SLCLI_STORE_LIMIT-e->retained_store_bytes)goto failed;
    e->retained_store_bytes+=2*(uint64_t)r.snapshot.bytes;
    if(!artifact(e,originals,r.snapshot.raw,r.snapshot.bytes,SLCLI_OBSERVATION,e->slot))goto failed;
    slcli_sha256_init(&h);
    if(!slcli_sha256_update(&h,r.snapshot.raw,r.snapshot.bytes)||!slcli_sha256_finish(&h,raw_hash))goto failed;
    s->state=SLCLI_LIVE;s->retained=r.snapshot.charged;s->generation=e->committed_generation+1;
    s->observation_present=1;s->observation_bytes=r.snapshot.bytes;copy32(s->observation,raw_hash);
    copy32(s->resource,snapshot_hash);
    if(!slcli_primary_engine_commit_original(e,originals))goto failed;
    copy32(e->snapshot_hash,snapshot_hash);e->last_phase=r.snapshot.phase;
    e->snapshot_request_original=in;e->snapshot_raw_original=r.snapshot.raw;
    e->snapshot_bytes_original=r.snapshot.bytes;
    response_header(out,4,r.sequence);slcli_put_u64(out->body+6,e->committed_generation);
    copy32(out->body+14,e->snapshot_hash);out->length=46;
  } else {
    if(!e->reserved||e->slot>=SLCLI_BINDINGS||!slcli_equal(r.reservation,b->reservation,32)||
       !slcli_equal(r.observer,b->observer,32)||!slcli_equal(r.snapshot_digest,e->snapshot_hash,32)||
       r.deadline!=b->deadline)goto failed;
    s=&e->candidate->slots[e->slot];
    if(s->state!=SLCLI_LIVE||!s->observation_present||e->last_phase!=7)goto failed;
    {
      struct slcli_snapshot_claim original={0};
      struct slcli_primary_request retained_request;
      struct slcli_source_owner *owner;
#if defined(_WIN32)
      if(!e->capture_store||!(owner=e->capture_store->source_owner))goto failed;
#else
      if(!(owner=e->source_linux))goto failed;
#endif
      /* Service requests are one-use held allocations. Reuse/changed bytes
       * cannot turn an earlier observation hash into current intent custody. */
      if(!e->snapshot_request_original||!e->snapshot_raw_original||
         !slcli_arena_live_range(owner->arena,e->snapshot_request_original,
           sizeof(*e->snapshot_request_original))||
         !slcli_primary_request_parse(e->snapshot_request_original,b->original_limit,
           &retained_request)||retained_request.operation!=3||
         retained_request.snapshot.raw!=e->snapshot_raw_original||
         retained_request.snapshot.bytes!=e->snapshot_bytes_original||
         !slcli_arena_live_range(owner->arena,e->snapshot_raw_original,
           e->snapshot_bytes_original)||
         !slcli_snapshot_claim_parse(e->snapshot_raw_original,e->snapshot_bytes_original,
           b->original_limit,&original)||original.phase!=7||
         !hash_snapshot(b->reservation,&original,snapshot_hash)||
         !slcli_equal(snapshot_hash,e->snapshot_hash,32)||
         !match_snapshot(b,&original))goto failed;
    }
    {
      unsigned primary=0;uint64_t final_bytes=0;
      for(i=0;i<b->count;i++) {
        const struct slcli_native_reference *ref=&b->references[i];
        unsigned membership=(ref->applicability>>4)&7;
        unsigned terminal=(b->primary_terminal_bitmap[i/8]&(1u<<(i%8)))!=0;
        if(membership>4||terminal!=((ref->applicability&128)!=0))goto failed;
        /* Observer/facade/parent/service remain genuine charged originals;
         * their separate controlling process lifetimes are not primary FREE. */
        if(membership)continue;
        if(ref->kind==0&&ref->process_role==1) {
          if(ref->state||!terminal||!slcli_equal(ref->identity,b->primary,32))goto failed;primary++;
        }
        if(ref->state!=1) {
          if(!terminal||ref->state!=0||!ref->terminal_edge)goto failed;
          if(ref->kind==6||ref->kind==7) {
            if(ref->charged>512-final_bytes)goto failed;final_bytes+=ref->charged;
          }
        }
      }
      if(primary!=1)goto failed;
    }
    slcli_sha256_init(&h);
    if(!slcli_sha256_update(&h,intent_domain,sizeof(intent_domain))||
       !slcli_sha256_update(&h,b->reservation,32)||!slcli_sha256_update(&h,b->primary,32)||
       !slcli_sha256_update(&h,b->observer,32)||!slcli_sha256_update(&h,in->body,in->length)||
       !slcli_sha256_finish(&h,intent_hash))goto failed;
    s->state=SLCLI_CLOSE_PENDING;s->generation=s->intent_generation=e->committed_generation+1;
    s->intent_tick=clock->tick;s->intent_present=1;copy32(s->intent,intent_hash);
    if(!slcli_primary_engine_commit_original(e,originals))goto failed;
    copy32(e->intent_hash,intent_hash);e->closing=1;
    response_header(out,6,r.sequence);slcli_put_u64(out->body+6,e->committed_generation);
    copy32(out->body+14,e->intent_hash);slcli_put_u64(out->body+46,clock->tick);out->length=54;
  }
  e->next++;return 1;
failed:
  e->failed=1;
  /* No retry/reuse, close/kill, fabricated FREE or response follows an unknown
   * native/store outcome. The actual owner retains the original reservation,
   * pending candidate/old selected generations and every physical resource. */
  return 0;
}
int slcli_primary_engine_dispatch(struct slcli_primary_engine *e,const struct slcli_frame *in,
  const unsigned char mac[32],struct slcli_native_clock *clock,
  const struct slcli_primary_commit_originals *originals,struct slcli_frame *out) {
  int ok;
#if defined(_WIN32) || defined(__linux__)
  struct slcli_source_owner *owner=slcli_source_enter(e,SLCLI_SOURCE_DISPATCH,originals,0);if(!owner)return 0;
  if(!slcli_arena_live_range(owner->arena,in,sizeof(*in))||
     !slcli_arena_live_range(owner->arena,out,sizeof(*out))||
     !slcli_arena_live_range(owner->arena,clock,sizeof(*clock))||
     !slcli_arena_live_range(owner->arena,originals,sizeof(*originals))||
     !slcli_arena_live_range(owner->arena,e->ledger,sizeof(*e->ledger))||
     !slcli_arena_live_range(owner->arena,e->candidate,sizeof(*e->candidate))||
     !slcli_arena_live_range(owner->arena,e->binding,sizeof(*e->binding))) {
    slcli_source_leave(owner,0);return 0;
  }
#endif
  ok=dispatch_original(e,in,mac,clock,originals,out);
#if defined(_WIN32) || defined(__linux__)
  ok=slcli_source_leave(owner,ok);
#endif
  return ok;
}
#endif

#if defined(_WIN32) || defined(__linux__)
/* Exact retained metadata equality is custody, never native authority. */
static int ledger_equal(const unsigned char *x,const unsigned char *y,size_t bytes) {
  unsigned char difference=0;size_t i;
  if(!x||!y)return 0;
  for(i=0;i<bytes;i++)difference|=(unsigned char)(x[i]^y[i]);
  return difference==0;
}
int slcli_source_ledger_begin(struct slcli_source_transition_attempt *a,
  const struct slcli_primary_engine *e) {
  struct slcli_source_ledger_progress *p;unsigned i;
  if(!a||!e||!e->ledger||a->unresolved||a->ledger_progress.started)return 0;
  p=&a->ledger_progress;p->started=1;p->primary_original=e;
  p->base_generation=e->committed_generation;p->base_slot=e->selected_slot;
  p->before_pair=e->committed_pair_original;
  for(i=0;i<32;i++)p->base_hash[i]=e->payload_hash[i];
  return p->base_slot<=1&&e->ledger->generation==p->base_generation;
}
int slcli_source_ledger_record(struct slcli_source_transition_attempt *a,
  const struct slcli_primary_engine *e,const struct slcli_primary_commit_originals *o,
  const struct slcli_ledger_commit *pair,int returned) {
  struct slcli_source_ledger_progress *p;unsigned i;
  if(!a||!e||!o||!pair)return 0;p=&a->ledger_progress;
  if(!p->started||p->pair_returned||p->primary_original!=e)return 0;
  /* Capture the actual returned pair and all originals before qualification.
   * Late failure keeps this roster even when no positive progress is admitted. */
  p->pair_original=pair;p->returned_commit_original=o;p->candidate_original=e->candidate;
  p->pair_result=returned;p->pair_returned=1;
  p->successor_generation=pair->committed_generation;p->successor_slot=pair->selected_slot;
  for(i=0;i<32;i++)p->successor_hash[i]=pair->payload_hash[i];
  if(!pair->caller_returned||pair->caller_result!=returned||!pair->durable_pair_complete||
     p->base_generation==UINT64_MAX||p->base_slot>1||
     e->committed_generation!=p->base_generation||e->selected_slot!=p->base_slot||
     e->committed_pair_original!=p->before_pair||!ledger_equal(e->payload_hash,p->base_hash,32)||
     !e->candidate||e->candidate_base_generation!=p->base_generation||
     e->candidate->generation!=p->base_generation+1||
     p->successor_generation!=p->base_generation+1||p->successor_slot!=(p->base_slot^1u)||
     o->deadline_original!=a->deadline||!p->commit_before||p->commit_original!=o||
     p->commit_before->commit!=pair||
     !ledger_equal((const unsigned char *)o,(const unsigned char *)p->commit_before,sizeof(*o)))return 0;
  p->qualified=1;return 1;
}
int slcli_source_ledger_unchanged(const struct slcli_source_transition_attempt *a,
  const struct slcli_primary_engine *e) {
  const struct slcli_source_ledger_progress *p;
  if(!a||!e||!e->ledger)return 0;p=&a->ledger_progress;
  if(!p->started||p->primary_original!=e)return 0;
  if(!p->pair_returned)return e->committed_generation==p->base_generation&&
    e->selected_slot==p->base_slot&&e->ledger->generation==p->base_generation&&
    e->committed_pair_original==p->before_pair&&ledger_equal(e->payload_hash,p->base_hash,32);
  return p->qualified&&p->pair_original&&p->commit_original&&
    p->returned_commit_original==p->commit_original&&p->pair_original->caller_returned&&
    p->pair_original->caller_result==p->pair_result&&
    p->commit_before&&ledger_equal((const unsigned char *)p->commit_original,
      (const unsigned char *)p->commit_before,sizeof(*p->commit_original))&&
    p->commit_original->commit==p->pair_original&&p->commit_original->deadline_original==a->deadline&&
    p->pair_original->durable_pair_complete&&p->pair_original->committed_generation==p->successor_generation&&
    p->pair_original->selected_slot==p->successor_slot&&
    ledger_equal(p->pair_original->payload_hash,p->successor_hash,32)&&
    e->candidate==p->candidate_original&&e->committed_pair_original==p->pair_original&&
    e->committed_generation==p->successor_generation&&e->ledger->generation==p->successor_generation&&
    e->selected_slot==p->successor_slot&&ledger_equal(e->payload_hash,p->successor_hash,32);
}
int slcli_source_ledger_prepare(struct slcli_arena *arena,struct slcli_source_transition_attempt *a,
  const struct slcli_primary_engine *e,const struct slcli_primary_commit_originals *o) {
  struct slcli_source_ledger_progress *p;struct slcli_primary_commit_originals *copy;size_t i;
  if(!arena||!a||!e||!o||!slcli_arena_live_range(arena,a,sizeof(*a))||
     !slcli_arena_live_range(arena,o,sizeof(*o)))return 0;
  p=&a->ledger_progress;
  if(!p->started||p->primary_original!=e||p->commit_before||p->pair_returned||
     !slcli_source_ledger_unchanged(a,e)||o->deadline_original!=a->deadline)return 0;
  copy=slcli_arena_calloc(arena,1,sizeof(*copy));if(!copy)return 0;
  /* Preserve the exact original argument representation before any pair
   * effect; its pointer/native-root/temporary/sink geometry cannot be swapped
   * merely by keeping the outer command pointer equal. */
  for(i=0;i<sizeof(*copy);i++)((unsigned char *)copy)[i]=((const unsigned char *)o)[i];
  p->commit_original=o;p->commit_before=copy;return 1;
}
#endif
