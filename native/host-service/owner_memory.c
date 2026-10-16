#include "owner_memory.h"

int slcli_memory_owner_writable(const struct slcli_memory_owner *o) {
  return o && (o->handoff_state == SLCLI_OWNER_LOCAL ||
               o->handoff_state == SLCLI_OWNER_CHILD);
}
int slcli_memory_owner_unattached(const struct slcli_memory_owner *o) {
  return o&&!o->bq_owner_original&&!o->bq_arena_original&&!o->bq_caller_original&&
    !o->bq_acquisition_original&&!o->bq_extent_original&&!o->bq_attachment_state;
}
int slcli_memory_owner_mutable(const struct slcli_memory_owner *o) {
  return slcli_memory_owner_writable(o)&&slcli_memory_owner_unattached(o);
}
/* This closed prebirth shape has never owned an initialized process view.
 * Transport decoding reconstructs this local representation field by field;
 * this is deliberately not a codec for foreign native C struct bytes. */
static int target_original(const struct slcli_memory_owner *o) {
  uint64_t charged=0, category[3]={0,0,0}, previous=0;
  size_t i,j;unsigned char birth=0,image=0,owner=0,source=0;
  if(!slcli_memory_owner_unattached(o)||o->failed||!o->limit||!o->charged||o->charged>o->limit||
     o->peak!=o->charged||!o->count||o->count>256||
     !o->native_page_size||!o->next_sequence)return 0;
  for(i=0;i<32;i++) {
    birth|=o->original_birth[i];image|=o->original_image[i];
    owner|=o->original_owner[i];source|=o->original_source[i];
  }
  if(!birth||!image||!owner||!source)return 0;
  for(i=0;i<o->count;i++) {
    const struct slcli_extent *e=&o->extents[i];
    if((e->state!=SLCLI_EXTENT_RESERVED&&e->state!=SLCLI_EXTENT_BACKING_ACQUIRED)||
       e->base||!e->reserved||e->reserved%o->native_page_size||e->backed||
       e->backed_observed||e->arena_initialized||e->retirement||e->category>SLCLI_IO||
       !e->acquisition||e->acquisition<=previous||e->latest!=e->acquisition||
       e->acquisition>=o->next_sequence||e->reserved>UINT64_MAX-charged||
       e->reserved>UINT64_MAX-category[e->category])return 0;
    charged+=e->reserved;category[e->category]+=e->reserved;previous=e->acquisition;
  }
  if(charged!=o->charged)return 0;
  for(i=0;i<3;i++)if(o->category_live[i]!=category[i]||
    o->category_peak[i]!=category[i])return 0;
  for(i=o->count;i<256;i++)for(j=0;j<sizeof(o->extents[i]);j++)
    if(((const unsigned char *)&o->extents[i])[j])return 0;
  return 1;
}
int slcli_memory_owner_prebirth_shape(const struct slcli_memory_owner *o) {
  return o&&o->handoff_state==SLCLI_OWNER_LOCAL&&target_original(o);
}
int slcli_memory_owner_freeze(struct slcli_memory_owner *o) {
  if(!o||o->handoff_state!=SLCLI_OWNER_LOCAL||!target_original(o))return 0;
  /* Original native source serialization excludes every concurrent mutator.
   * This marker is an accounting/lifetime boundary, not native admission. */
  o->handoff_state=SLCLI_OWNER_FROZEN;return 1;
}
int slcli_memory_owner_frozen(const struct slcli_memory_owner *o) {
  return o&&o->handoff_state==SLCLI_OWNER_FROZEN&&target_original(o);
}
int slcli_memory_backing_request_matches(const struct slcli_memory_owner *o,
  uint32_t index,uint64_t sequence_original,uint64_t bytes) {
  const struct slcli_extent *e;
  if(!o||o->handoff_state!=SLCLI_OWNER_CHILD||o->failed||!bytes||
     !sequence_original||index>=o->count||index>=256)return 0;
  e=&o->extents[index];
  return e->state==SLCLI_EXTENT_BACKING_ACQUIRED&&!e->base&&
    e->reserved==bytes&&e->acquisition==sequence_original&&e->latest==sequence_original&&
    !e->backed&&!e->backed_observed&&!e->arena_initialized&&!e->retirement;
}
int slcli_memory_owner_continue(struct slcli_memory_owner *child,
                                 const struct slcli_memory_owner *original) {
  uintptr_t c=(uintptr_t)child,p=(uintptr_t)original;size_t i;
  if(!child||!original||c>UINTPTR_MAX-sizeof(*child)||
     p>UINTPTR_MAX-sizeof(*original)||
     (c<p+sizeof(*original)&&p<c+sizeof(*child))||
     !slcli_memory_owner_frozen(original))return 0;
  for(i=0;i<sizeof(*child);i++)if(((const unsigned char *)child)[i])return 0;
  /* Exact SAME charged predecessor, sequence and extent keys. Caller has
   * independently joined whole private transport/source/native child identity
   * before invoking this pure local reconstruction. No memory_prepare occurs. */
  for(i=0;i<sizeof(*child);i++)
    ((unsigned char *)child)[i]=((const unsigned char *)original)[i];
  child->handoff_state=SLCLI_OWNER_CHILD;return 1;
}
static int prefix_matches(const struct slcli_memory_owner *child,
 const struct slcli_memory_owner *original,const struct slcli_extent_request *runtime,
 uintptr_t runtime_base,uint64_t runtime_bytes) {
  size_t i,j;
  if(!child||!slcli_memory_owner_frozen(original)||child==original||
     !slcli_memory_owner_unattached(child)||child->handoff_state!=SLCLI_OWNER_CHILD||child->failed||
     child->limit!=original->limit||child->charged!=original->charged||
     child->peak!=original->peak||child->count!=original->count||
     child->next_sequence!=original->next_sequence||
     child->native_page_size!=original->native_page_size)return 0;
  for(i=0;i<3;i++)if(child->category_live[i]!=original->category_live[i]||
    child->category_peak[i]!=original->category_peak[i])return 0;
  for(i=0;i<2;i++)if(child->native_page_geometry[i]!=original->native_page_geometry[i])return 0;
  for(i=0;i<32;i++)if(child->original_birth[i]!=original->original_birth[i]||
    child->original_image[i]!=original->original_image[i]||
    child->original_owner[i]!=original->original_owner[i]||
    child->original_source[i]!=original->original_source[i])return 0;
  for(i=0;i<child->count;i++) {
    const struct slcli_extent *e=&child->extents[i],*p=&original->extents[i];
    if(e->reserved!=p->reserved||e->category!=p->category||
       e->acquisition!=p->acquisition||e->latest!=p->latest||
       e->retirement||e->arena_initialized)return 0;
    if(runtime&&i==runtime->index) {
      if(p->state!=SLCLI_EXTENT_BACKING_ACQUIRED||p->category!=SLCLI_RUNTIME||
        p->acquisition!=runtime->sequence||p->reserved!=runtime_bytes||
        e->state!=SLCLI_EXTENT_LIVE||e->base!=runtime_base||
        e->backed_observed>1||(!e->backed_observed&&e->backed)||
        e->backed>e->reserved)return 0;
    } else if(e->state==p->state) {
      if(e->base||e->backed||e->backed_observed)return 0;
    } else {
      /* Only independently observed prefix mappings can have advanced here.
       * Inherited BACKING_ACQUIRED runtime stays unmapped until its receiver. */
      if(p->state!=SLCLI_EXTENT_RESERVED||e->state!=SLCLI_EXTENT_LIVE||
         !e->base||e->reserved>UINTPTR_MAX||e->base>UINTPTR_MAX-e->reserved||
         e->backed_observed>1||(!e->backed_observed&&e->backed)||
         e->backed>e->reserved)return 0;
    }
  }
  for(i=child->count;i<256;i++)for(j=0;j<sizeof(child->extents[i]);j++)
    if(((const unsigned char *)&child->extents[i])[j])return 0;
  return 1;
}
int slcli_memory_owner_prefix_matches(const struct slcli_memory_owner *child,
 const struct slcli_memory_owner *original) {
 return prefix_matches(child,original,0,0,0);
}
int slcli_memory_owner_mapped_runtime_prefix_matches(
 const struct slcli_memory_owner *child,const struct slcli_memory_owner *original,
 const struct slcli_extent_request *runtime,uintptr_t base,uint64_t bytes) {
 if(!runtime||!original||runtime->index>=original->count||runtime->index>=256||
  !runtime->sequence||!base||!bytes||bytes>UINTPTR_MAX||base>UINTPTR_MAX-bytes)return 0;
 return prefix_matches(child,original,runtime,base,bytes);
}
static int fail(struct slcli_memory_owner *o) {
  /* A denied late parent mutation must not alter the frozen original. Its
   * independently charged caller/result capture owns that denial instead. */
  if (slcli_memory_owner_mutable(o)) o->failed = 1;
  return 0;
}
static int sequence(struct slcli_memory_owner *o, uint64_t *out) {
  if(!slcli_memory_owner_mutable(o))return 0;
  if (!o || o->failed || !o->next_sequence || o->next_sequence == UINT64_MAX)
    return fail(o);
  *out = o->next_sequence++;
  return 1;
}
int slcli_memory_prepare(struct slcli_memory_owner *o, uint32_t category,
                         uint64_t bytes, struct slcli_extent_request *r) {
  uint32_t i;
  uint64_t s;
  if (!slcli_memory_owner_mutable(o)) return 0;
  if (!o || !r || category > SLCLI_IO || !bytes || !o->limit ||
      o->charged > o->limit || o->count > 256 ||
      o->category_live[category] > o->charged ||
      bytes > o->limit - o->charged)
    return fail(o);
  /* Records never recycle in an attempt: retired original captures must remain
   * correlatable. Capacity denial precedes the native acquisition/effects. */
  i = o->count;
  if (i == 256 || o->extents[i].state != SLCLI_EXTENT_UNUSED ||
      !sequence(o, &s)) return fail(o);
  o->extents[i].base = 0;
  o->extents[i].reserved = bytes;
  o->extents[i].backed = 0;
  o->extents[i].backed_observed = 0;
  o->extents[i].arena_initialized = 0;
  o->extents[i].acquisition = s;
  o->extents[i].latest = s;
  o->extents[i].retirement = 0;
  o->extents[i].category = category;
  o->extents[i].state = SLCLI_EXTENT_RESERVED;
  o->charged += bytes;
  o->category_live[category] += bytes;
  if (o->charged > o->peak) o->peak = o->charged;
  if (o->category_live[category] > o->category_peak[category])
    o->category_peak[category] = o->category_live[category];
  ++o->count;
  r->index = i;
  r->sequence = s;
  return 1;
}
int slcli_memory_partial_acquired(struct slcli_memory_owner *o,
                                  const struct slcli_extent_request *r,
                                  uintptr_t base) {
  struct slcli_extent *e;
  if (!slcli_memory_owner_mutable(o)) return 0;
  if (!o || !r || r->index >= 256) return fail(o);
  e = &o->extents[r->index];
  if (e->state != SLCLI_EXTENT_RESERVED ||
      e->acquisition != r->sequence || !base ||
      e->reserved > UINTPTR_MAX || base > UINTPTR_MAX - (uintptr_t)e->reserved) {
    e->state = SLCLI_EXTENT_RETAINED;
    return fail(o);
  }
  /* Actual successful native return is registered BEFORE subsequent clock,
   * query/capture/reader work can fail. This is not positive geometry proof. */
  e->base = base;
  /* An already absorbing owner failure cannot erase an authentic successful
   * acquisition tied to this SAME prepared request. Retain its address/charge;
   * it is neither a LIVE geometry publication nor permission for another call. */
  if (o->failed) { e->state = SLCLI_EXTENT_RETAINED; return 0; }
  e->state = SLCLI_EXTENT_PARTIAL_ACQUIRED;
  return 1;
}
int slcli_memory_acquired(struct slcli_memory_owner *o,
                          const struct slcli_extent_request *r, uintptr_t base,
                          uint64_t reserved, uint64_t backed, uint32_t known) {
  struct slcli_extent *e;
  if (!slcli_memory_owner_mutable(o)) return 0;
  if (!o || !r || r->index >= 256) return fail(o);
  e = &o->extents[r->index];
  if (o->failed || e->state != SLCLI_EXTENT_PARTIAL_ACQUIRED ||
      e->acquisition != r->sequence || e->base != base || !base ||
      reserved != e->reserved ||
      known > 1 || (!known && backed) || backed > reserved ||
      reserved > UINTPTR_MAX ||
      base > UINTPTR_MAX - (uintptr_t)reserved) {
    e->state = SLCLI_EXTENT_RETAINED;
    return fail(o);
  }
  e->base = base;
  e->backed = backed;
  e->backed_observed = known;
  e->state = SLCLI_EXTENT_LIVE;
  return 1;
}
int slcli_memory_backing_acquired(struct slcli_memory_owner *o,
                                  const struct slcli_extent_request *r) {
  struct slcli_extent *e;
  if (!slcli_memory_owner_mutable(o)) return 0;
  if (!o || !r || r->index >= o->count || r->index >= 256) return fail(o);
  e = &o->extents[r->index];
  if (e->state != SLCLI_EXTENT_RESERVED || e->acquisition != r->sequence ||
      e->latest != r->sequence || e->base || !e->reserved) return fail(o);
  /* Called immediately after actual successful section creation, before clock
   * qualification. No address or second quota is invented for unmapped backing. */
  e->state = o->failed ? SLCLI_EXTENT_RETAINED : SLCLI_EXTENT_BACKING_ACQUIRED;
  return !o->failed;
}
int slcli_memory_backing_mapped(struct slcli_memory_owner *o,
                                const struct slcli_extent_request *r,
                                uintptr_t base) {
  struct slcli_extent *e;
  if (!slcli_memory_owner_mutable(o)) return 0;
  if (!o || !r || r->index >= o->count || r->index >= 256) return fail(o);
  e = &o->extents[r->index];
  if (e->state != SLCLI_EXTENT_BACKING_ACQUIRED ||
      e->acquisition != r->sequence || e->latest != r->sequence ||
      e->base || !base || e->reserved > UINTPTR_MAX ||
      base > UINTPTR_MAX - (uintptr_t)e->reserved) return fail(o);
  e->base = base;
  e->state = o->failed ? SLCLI_EXTENT_RETAINED : SLCLI_EXTENT_PARTIAL_ACQUIRED;
  return !o->failed;
}
int slcli_memory_unobserved(struct slcli_memory_owner *o,
                            const struct slcli_extent_request *r) {
  struct slcli_extent *e;
  if (!slcli_memory_owner_mutable(o)) return 0;
  if (!o || !r || r->index >= 256) return fail(o);
  e = &o->extents[r->index];
  if (e->latest != r->sequence ||
      (e->state != SLCLI_EXTENT_RESERVED &&
       e->state != SLCLI_EXTENT_BACKING_ACQUIRED &&
       e->state != SLCLI_EXTENT_PARTIAL_ACQUIRED && e->state != SLCLI_EXTENT_LIVE &&
       e->state != SLCLI_EXTENT_RELEASE_PENDING))
    return fail(o);
  e->state = SLCLI_EXTENT_RETAINED;
  return fail(o);
}
int slcli_memory_prepare_release(struct slcli_memory_owner *o, uint32_t index,
                                 struct slcli_extent_request *r,
                                 uint64_t *related) {
  struct slcli_extent *e;
  uint64_t s;
  if (!slcli_memory_owner_mutable(o)) return 0;
  if (!o || !r || !related || index >= 256) return fail(o);
  e = &o->extents[index];
  if (o->failed || e->state != SLCLI_EXTENT_LIVE || !e->base ||
      !sequence(o, &s)) return fail(o);
  *related = e->latest;
  e->latest = s;
  e->state = SLCLI_EXTENT_RELEASE_PENDING;
  r->index = index;
  r->sequence = s;
  return 1;
}
int slcli_memory_backing_view_unmapped(struct slcli_memory_owner *o,
  const struct slcli_extent_request *release,uint64_t related,
  uintptr_t original_base,uint64_t original_reserved) {
  struct slcli_extent *e;
  if(!slcli_memory_owner_mutable(o)||!release||o->failed||
     release->index>=o->count||release->index>=256)return 0;
  e=&o->extents[release->index];
  if(e->state!=SLCLI_EXTENT_RELEASE_PENDING||!original_base||
     !original_reserved||e->base!=original_base||e->reserved!=original_reserved||
     e->arena_initialized||e->retirement||!related||e->acquisition!=related||
     e->latest!=release->sequence||release->sequence<=related||
     release->sequence>=o->next_sequence)return 0;
  /* SAME physical backing is still owned through actual readonly aliases.
   * Keep every charge/peak/key/sequence; the native capture keeps old address.
   * latest!=acquisition denies backing_mapped; non-LIVE denies arena init. */
  e->base=0;e->backed=0;e->backed_observed=0;
  e->state=SLCLI_EXTENT_BACKING_ACQUIRED;
  return 1;
}
int slcli_memory_retired(struct slcli_memory_owner *o, uint32_t index,
                         uint64_t previous, uintptr_t base, uint64_t bytes) {
  struct slcli_extent *e;
  if (!slcli_memory_owner_mutable(o)) return 0;
  if (!o || index >= 256) return fail(o);
  e = &o->extents[index];
  /* Only the native adapter after ORIGINAL successful release plus independent
   * raw observation may call here. Unknown release never reaches this function.
   * Parent-owned public copies use process-exit retirement, not JS disposal. */
  if (o->failed || e->state != SLCLI_EXTENT_RELEASE_PENDING ||
      e->latest != previous ||
      e->base != base || !base || e->reserved != bytes ||
      o->charged < bytes || e->category > SLCLI_IO ||
      o->category_live[e->category] < bytes) {
    if (e->state != SLCLI_EXTENT_UNUSED && e->state != SLCLI_EXTENT_RETIRED)
      e->state = SLCLI_EXTENT_RETAINED;
    return fail(o);
  }
  o->charged -= bytes;
  o->category_live[e->category] -= bytes;
  /* Independent validation completes the SAME original release event. It does
   * not invent another native operation/event sequence. */
  e->retirement = previous;
  e->state = SLCLI_EXTENT_RETIRED;
  return 1;
}
