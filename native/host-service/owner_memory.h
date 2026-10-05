#ifndef SLCLI_OWNER_MEMORY_H
#define SLCLI_OWNER_MEMORY_H
#include <stddef.h>
#include <stdint.h>

/* Physical extents belong to their original process/image owner. This table
 * records native acquisition; it neither authenticates the owner nor allocates
 * a second quota. Bootstrap image/stack extents enter this same ledger. */
enum slcli_memory_category { SLCLI_RUNTIME, SLCLI_STACK, SLCLI_IO };
enum slcli_extent_state { SLCLI_EXTENT_UNUSED, SLCLI_EXTENT_RESERVED,
  SLCLI_EXTENT_PARTIAL_ACQUIRED, SLCLI_EXTENT_LIVE, SLCLI_EXTENT_RELEASE_PENDING,
  SLCLI_EXTENT_RETAINED, SLCLI_EXTENT_RETIRED };
struct slcli_extent {
  uintptr_t base;
  uint64_t reserved, backed, acquisition, latest, retirement;
  uint32_t category, state, backed_observed;
};
struct slcli_memory_owner {
  uint64_t limit, charged, next_sequence;
  uint64_t category_peak[3], category_live[3], peak;
  uint8_t original_birth[32], original_image[32], original_owner[32], original_source[32];
  uint32_t failed, count;
  uint64_t native_page_size;
  uintptr_t native_page_geometry[2];
  struct slcli_extent extents[256];
};
_Static_assert(sizeof(struct slcli_extent) == 64,
               "Actual native extent stride must match reviewed storage");
struct slcli_extent_request { uint32_t index; uint64_t sequence; };

/* Calls are serialized by the native owner's actual loop/lock. No borrowed
 * asynchronous callback may modify or recycle a table cell. */
int slcli_memory_prepare(struct slcli_memory_owner *, uint32_t, uint64_t,
                         struct slcli_extent_request *);
int slcli_memory_acquired(struct slcli_memory_owner *,
                          const struct slcli_extent_request *, uintptr_t,
                          uint64_t, uint64_t, uint32_t);
int slcli_memory_partial_acquired(struct slcli_memory_owner *,
                                  const struct slcli_extent_request *, uintptr_t);
int slcli_memory_unobserved(struct slcli_memory_owner *,
                            const struct slcli_extent_request *);
int slcli_memory_prepare_release(struct slcli_memory_owner *, uint32_t,
                                 struct slcli_extent_request *, uint64_t *);
int slcli_memory_retired(struct slcli_memory_owner *, uint32_t, uint64_t,
                         uintptr_t, uint64_t);
#if defined(__linux__)
int slcli_linux_bootstrap_pages(struct slcli_memory_owner *, uint32_t,
                                uintptr_t, uintptr_t[2]);
#endif
#endif
