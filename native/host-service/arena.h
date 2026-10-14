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
void *slcli_arena_alloc(struct slcli_arena *, size_t);
void *slcli_arena_calloc(struct slcli_arena *, size_t, size_t);
void *slcli_arena_realloc(struct slcli_arena *, void *, size_t);
int slcli_arena_free(struct slcli_arena *, void *);
#endif
