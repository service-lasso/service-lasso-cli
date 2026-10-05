#include "arena.h"

/* Unexecuted control-retention regression. The actual native qualification
 * caller supplies a genuine live, uninitialized extent and a separate charged
 * scratch buffer covering the complete owner. No invented mapping is used.
 * Whole source/native admission precedes compiling or executing this source. */
int slcli_arena_control_vectors(struct slcli_memory_owner *owner,
                                uint32_t live_extent,
                                unsigned char *saved, size_t saved_bytes) {
  const unsigned char *raw = (const unsigned char *)owner;
  uintptr_t s, o;
  const struct slcli_extent *e;
  size_t i;
  if (!owner || !saved || saved_bytes < sizeof(*owner)) return 0;
  s = (uintptr_t)saved; o = (uintptr_t)owner;
  if (s > UINTPTR_MAX - sizeof(*owner) ||
      o > UINTPTR_MAX - sizeof(*owner) ||
      (s < o + sizeof(*owner) && o < s + sizeof(*owner))) return 0;
  if (owner->failed || owner->count > 256 || live_extent >= owner->count ||
      live_extent >= 256 ||
      owner->extents[live_extent].state != SLCLI_EXTENT_LIVE ||
      owner->extents[live_extent].arena_initialized ||
      !owner->extents[live_extent].base) return 0;
  e = &owner->extents[live_extent];
  if (e->reserved > UINTPTR_MAX ||
      e->base > UINTPTR_MAX - (uintptr_t)e->reserved ||
      (s < e->base + (uintptr_t)e->reserved &&
       e->base < s + sizeof(*owner))) return 0;
  for (i = 0; i < sizeof(*owner); ++i) saved[i] = raw[i];
  if (slcli_arena_init((struct slcli_arena *)owner, owner, live_extent)) return 0;
  for (i = 0; i < sizeof(*owner); ++i) if (saved[i] != raw[i]) return 0;
  if (slcli_arena_init((struct slcli_arena *)(o + sizeof(*owner) - 1),
                       owner, live_extent)) return 0;
  for (i = 0; i < sizeof(*owner); ++i) if (saved[i] != raw[i]) return 0;
  if (slcli_arena_init((struct slcli_arena *)(UINTPTR_MAX -
                       sizeof(struct slcli_arena) + 1), owner, live_extent))
    return 0;
  for (i = 0; i < sizeof(*owner); ++i) if (saved[i] != raw[i]) return 0;
  return 1;
}
