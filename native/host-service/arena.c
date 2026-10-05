#include "arena.h"

struct block { uint64_t bytes, requested, next, state; };
_Static_assert(sizeof(struct block) == 32, "Reviewed in-arena header stride");
enum { BLOCK_FREE = 1, BLOCK_LIVE = 2 };
static int deny(struct slcli_arena *a) {
  if (a && a->owner) a->owner->failed = 1;
  return 0;
}
static struct slcli_extent *extent(struct slcli_arena *a) {
  struct slcli_extent *e;
  if (!a || !a->owner || a->owner->failed || a->owner->count > 256 ||
      a->extent >= 256 || a->extent >= a->owner->count)
    return 0;
  e = &a->owner->extents[a->extent];
  if (e->state != SLCLI_EXTENT_LIVE || !e->arena_initialized ||
      e->acquisition != a->acquisition ||
      !e->base || e->reserved > SIZE_MAX || e->base % 16 ||
      e->reserved < sizeof(struct block) + 16) return 0;
  return e;
}
static struct block *block_at(struct slcli_extent *e, uint64_t offset) {
  struct block *b;
  if (offset % 16 || offset > e->reserved - sizeof(struct block)) return 0;
  b = (struct block *)(e->base + (uintptr_t)offset);
  if (b->bytes % 16 || b->bytes < sizeof(struct block) + 16 ||
      b->bytes > e->reserved - offset ||
      (b->state != BLOCK_FREE && b->state != BLOCK_LIVE) ||
      (b->state == BLOCK_FREE && b->requested) ||
      (b->state == BLOCK_LIVE && !b->requested) ||
      b->requested > b->bytes - sizeof(struct block) ||
      (!b->next && offset + b->bytes != e->reserved) ||
      (b->next && (b->next != offset + b->bytes ||
                   b->next > e->reserved - sizeof(struct block)))) return 0;
  return b;
}
int slcli_arena_init(struct slcli_arena *a, struct slcli_memory_owner *o,
                      uint32_t index) {
  struct slcli_extent *e;
  struct block *b;
  if (!a || !o || o->failed || o->count > 256 || index >= 256 ||
      index >= o->count) return 0;
  e = &o->extents[index];
  if (e->state != SLCLI_EXTENT_LIVE || e->arena_initialized ||
      e->category == SLCLI_STACK ||
      !e->base || e->base % 16 ||
      e->reserved > SIZE_MAX || e->reserved % 16 || e->reserved < 48)
    return 0;
  /* Initial stack and owner/control descriptors never become payload arenas.
   * Their storage has its own original charged extent; no header may overwrite
   * the ledger whose identity permits this operation. */
  if (((uintptr_t)o >= e->base && (uintptr_t)o < e->base + e->reserved) ||
      ((uintptr_t)a >= e->base && (uintptr_t)a < e->base + e->reserved) ||
      ((uintptr_t)o < e->base && e->base - (uintptr_t)o < sizeof(*o)) ||
      ((uintptr_t)a < e->base && e->base - (uintptr_t)a < sizeof(*a))) return 0;
  a->owner = o; a->extent = index; a->acquisition = e->acquisition;
  b = (struct block *)e->base;
  b->bytes = e->reserved; b->requested = 0; b->next = 0;
  b->state = BLOCK_FREE;
  e->arena_initialized = 1;
  return 1;
}
void *slcli_arena_alloc(struct slcli_arena *a, size_t requested) {
  struct slcli_extent *e = extent(a);
  uint64_t offset = 0, need;
  struct block *b, *tail;
  if (!requested) return 0;
  if (!e || requested > UINT64_MAX - 47) {
    deny(a); return 0;
  }
  need = ((uint64_t)requested + 15) & ~(uint64_t)15;
  need += sizeof(struct block);
  for (;;) {
    b = block_at(e, offset);
    if (!b) { deny(a); return 0; }
    if (b->state == BLOCK_FREE && b->bytes >= need) {
      if (b->bytes - need >= 48) {
        tail = (struct block *)(e->base + (uintptr_t)(offset + need));
        tail->bytes = b->bytes - need; tail->requested = 0;
        tail->next = b->next; tail->state = BLOCK_FREE;
        b->next = offset + need; b->bytes = need;
      }
      b->requested = requested; b->state = BLOCK_LIVE;
      return (void *)(e->base + (uintptr_t)offset + sizeof(struct block));
    }
    if (!b->next) return 0; /* Capacity exhaustion is a real allocation failure. */
    offset = b->next;
  }
}
int slcli_arena_free(struct slcli_arena *a, void *value) {
  struct slcli_extent *e = extent(a);
  uint64_t offset = 0;
  struct block *b, *previous = 0, *next;
  uintptr_t address = (uintptr_t)value;
  if (!value) return 1;
  if (!e || address < e->base + sizeof(struct block) ||
      address >= e->base + (uintptr_t)e->reserved) return deny(a);
  for (;;) {
    b = block_at(e, offset);
    if (!b) return deny(a);
    if (e->base + (uintptr_t)offset + sizeof(struct block) == address) break;
    if (!b->next) return deny(a);
    previous = b; offset = b->next;
  }
  if (b->state != BLOCK_LIVE) return deny(a);
  /* Erase requested bytes before reuse; physical extent charge never changes. */
  { volatile unsigned char *p = value; uint64_t n = b->requested;
    while (n--) *p++ = 0; }
  b->requested = 0; b->state = BLOCK_FREE;
  if (b->next) {
    next = block_at(e, b->next);
    if (!next) return deny(a);
    if (next->state == BLOCK_FREE) { b->bytes += next->bytes; b->next = next->next; }
  }
  if (previous && previous->state == BLOCK_FREE) {
    previous->bytes += b->bytes; previous->next = b->next;
  }
  return 1;
}
void *slcli_arena_calloc(struct slcli_arena *a, size_t count, size_t size) {
  unsigned char *p;
  size_t bytes, i;
  if (size && count > SIZE_MAX / size) return 0;
  bytes = count * size;
  p = slcli_arena_alloc(a, bytes);
  if (p) for (i = 0; i < bytes; ++i) p[i] = 0;
  return p;
}
void *slcli_arena_realloc(struct slcli_arena *a, void *value, size_t bytes) {
  struct slcli_extent *e;
  struct block *b;
  uint64_t offset = 0, original, i;
  unsigned char *replacement;
  if (!value) return slcli_arena_alloc(a, bytes);
  if (!bytes) { slcli_arena_free(a, value); return 0; }
  e = extent(a);
  if (!e) { deny(a); return 0; }
  for (;;) {
    b = block_at(e, offset);
    if (!b) { deny(a); return 0; }
    if (e->base + (uintptr_t)offset + sizeof(struct block) == (uintptr_t)value)
      break;
    if (!b->next) { deny(a); return 0; }
    offset = b->next;
  }
  if (b->state != BLOCK_LIVE) { deny(a); return 0; }
  original = b->requested;
  /* Old and new native blocks coexist until copying and original release. On
   * capacity failure the caller still owns the untouched original allocation. */
  replacement = slcli_arena_alloc(a, bytes);
  if (!replacement) return 0;
  for (i = 0; i < original && i < bytes; ++i)
    replacement[i] = ((const unsigned char *)value)[i];
  if (!slcli_arena_free(a, value)) return 0;
  return replacement;
}
