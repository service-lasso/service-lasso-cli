#include "owner_memory.h"

static int fail(struct slcli_memory_owner *o) {
  if (o) o->failed = 1;
  return 0;
}
static int sequence(struct slcli_memory_owner *o, uint64_t *out) {
  if (!o || o->failed || !o->next_sequence || o->next_sequence == UINT64_MAX)
    return fail(o);
  *out = o->next_sequence++;
  return 1;
}
int slcli_memory_prepare(struct slcli_memory_owner *o, uint32_t category,
                         uint64_t bytes, struct slcli_extent_request *r) {
  uint32_t i;
  uint64_t s;
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
  if (!o || !r || r->index >= 256) return fail(o);
  e = &o->extents[r->index];
  if (o->failed || e->state != SLCLI_EXTENT_RESERVED ||
      e->acquisition != r->sequence || !base ||
      e->reserved > UINTPTR_MAX || base > UINTPTR_MAX - (uintptr_t)e->reserved) {
    e->state = SLCLI_EXTENT_RETAINED;
    return fail(o);
  }
  /* Actual successful native return is registered BEFORE subsequent clock,
   * query/capture/reader work can fail. This is not positive geometry proof. */
  e->base = base;
  e->state = SLCLI_EXTENT_PARTIAL_ACQUIRED;
  return 1;
}
int slcli_memory_acquired(struct slcli_memory_owner *o,
                          const struct slcli_extent_request *r, uintptr_t base,
                          uint64_t reserved, uint64_t backed, uint32_t known) {
  struct slcli_extent *e;
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
int slcli_memory_unobserved(struct slcli_memory_owner *o,
                            const struct slcli_extent_request *r) {
  struct slcli_extent *e;
  if (!o || !r || r->index >= 256) return fail(o);
  e = &o->extents[r->index];
  if (e->latest != r->sequence ||
      (e->state != SLCLI_EXTENT_RESERVED &&
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
int slcli_memory_retired(struct slcli_memory_owner *o, uint32_t index,
                         uint64_t previous, uintptr_t base, uint64_t bytes) {
  struct slcli_extent *e;
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
