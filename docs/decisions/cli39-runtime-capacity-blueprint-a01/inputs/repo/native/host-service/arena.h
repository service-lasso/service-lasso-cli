#ifndef SLCLI_ARENA_H
#define SLCLI_ARENA_H
#include "owner_memory.h"
/* Headers, alignment and free space remain inside the fully charged original
 * native extent. Calls require the same owning native serialization as ledger
 * mutations. This allocator never acquires or retires physical storage. */
struct slcli_arena {
  struct slcli_memory_owner *owner;
  uint32_t extent;
  uint64_t acquisition;
};
int slcli_arena_init(struct slcli_arena *, struct slcli_memory_owner *, uint32_t);
/* Exact existing per-block logical header/alignment charge; zero on overflow.
 * Physical native page rounding/whole-owner accounting remains independent. */
size_t slcli_arena_allocation_bytes(size_t);
void *slcli_arena_alloc(struct slcli_arena *, size_t);
void *slcli_arena_calloc(struct slcli_arena *, size_t, size_t);
void *slcli_arena_realloc(struct slcli_arena *, void *, size_t);
int slcli_arena_free(struct slcli_arena *, void *);
/* Actual live allocator block containment; no allocation, source authority,
 * physical ownership admission or process-total fit is inferred here. */
int slcli_arena_live_range(struct slcli_arena *,const void *,size_t);
/* Exact current logical allocation start AND original requested bytes. This
 * read-only query preserves the general contained-range API above and grants
 * neither native retirement nor source/actor ownership authority. */
int slcli_arena_live_block(struct slcli_arena *,const void *,size_t);
/* SAME logical block start/original requested geometry under owning native
 * serialization. Capacity failure preserves the old block; physical extent
 * charge never changes. No relocation or acquisition occurs. */
int slcli_arena_resize_in_place(struct slcli_arena *,void *,size_t,size_t);
#endif
