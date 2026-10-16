#ifndef SLCLI_OWNER_MEMORY_H
#define SLCLI_OWNER_MEMORY_H
#include <stddef.h>
#include <stdint.h>
struct slcli_extent_request;
struct slcli_arena;

/* Physical extents belong to their original process/image owner. This table
 * records native acquisition; it neither authenticates the owner nor allocates
 * a second quota. Bootstrap image/stack extents enter this same ledger. */
enum slcli_memory_category { SLCLI_RUNTIME, SLCLI_STACK, SLCLI_IO };
enum slcli_extent_state { SLCLI_EXTENT_UNUSED, SLCLI_EXTENT_RESERVED,
  SLCLI_EXTENT_PARTIAL_ACQUIRED, SLCLI_EXTENT_LIVE, SLCLI_EXTENT_RELEASE_PENDING,
  SLCLI_EXTENT_RETAINED, SLCLI_EXTENT_RETIRED, SLCLI_EXTENT_BACKING_ACQUIRED };
struct slcli_extent {
  uintptr_t base;
  uint64_t reserved, backed, acquisition, latest, retirement;
  /* arena_initialized:0 before adoption; otherwise1+allocation_floor/16.
   * Ordinary fresh arenas retain value1. Prebirth wire still requires0. */
  uint32_t category, state, backed_observed, arena_initialized;
};
struct slcli_memory_owner {
  uint64_t limit, charged, next_sequence;
  uint64_t category_peak[3], category_live[3], peak;
  uint8_t original_birth[32], original_image[32], original_owner[32], original_source[32];
  uint32_t failed, count;
  uint64_t native_page_size;
  uintptr_t native_page_geometry[2];
  /* Serialized source custody, not actor authority. A frozen parent ledger is
   * immutable predecessor evidence; only its genuine child continuation writes. */
  uint32_t handoff_state;
  /* Native-only immutable BQ attachment. Genuine initialized/zero prebirth
   * constructors alone establish these bytes; no field is a wire capability.
   * Publish before carving the caller or initializing its native lock. Partial
   * attachment is absorbing and denies every general mutation entry. */
  struct slcli_memory_owner *bq_owner_original;
  struct slcli_arena *bq_arena_original;
  void *bq_caller_original;
  uint64_t bq_acquisition_original;
  uint32_t bq_extent_original,bq_attachment_state;
  struct slcli_extent extents[256];
};
enum slcli_memory_handoff_state { SLCLI_OWNER_LOCAL, SLCLI_OWNER_FROZEN,
  SLCLI_OWNER_CHILD };
int slcli_memory_owner_writable(const struct slcli_memory_owner *);
/* Initialized native owner only. This is mutation eligibility, not source,
 * caller, native lock, physical ownership or admission authentication. */
int slcli_memory_owner_unattached(const struct slcli_memory_owner *);
int slcli_memory_owner_mutable(const struct slcli_memory_owner *);
/* Freeze only a precharged unmapped target ledger. Complete native source,
 * child identity, private transport and storage admission remain independent. */
int slcli_memory_owner_freeze(struct slcli_memory_owner *);
int slcli_memory_owner_prebirth_shape(const struct slcli_memory_owner *);
int slcli_memory_owner_frozen(const struct slcli_memory_owner *);
int slcli_memory_backing_request_matches(const struct slcli_memory_owner *,
  uint32_t,uint64_t,uint64_t);
/* Pure local reconstruction after independent complete transport/native join;
 * no new reservation, charge, quota, sequence or ownership grant is created. */
int slcli_memory_owner_continue(struct slcli_memory_owner *,
                                 const struct slcli_memory_owner *);
/* Structural SAME prefix lineage before any postbirth prepare/arena bind.
 * Native image/stack/TLS/loader/transport observations remain independent. */
int slcli_memory_owner_prefix_matches(const struct slcli_memory_owner *,
                                     const struct slcli_memory_owner *);
/* Structural relation for an independently acquired exact runtime mapping.
 * This observes LIVE; it never promotes frozen transport or grants authority. */
int slcli_memory_owner_mapped_runtime_prefix_matches(
 const struct slcli_memory_owner *,const struct slcli_memory_owner *,
 const struct slcli_extent_request *,uintptr_t,uint64_t);
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
/* Actual section backing exists before any process view; SAME reservation stays charged. */
int slcli_memory_backing_acquired(struct slcli_memory_owner *,
                                  const struct slcli_extent_request *);
int slcli_memory_backing_mapped(struct slcli_memory_owner *,
                                const struct slcli_extent_request *, uintptr_t);
int slcli_memory_unobserved(struct slcli_memory_owner *,
                            const struct slcli_extent_request *);
int slcli_memory_prepare_release(struct slcli_memory_owner *, uint32_t,
                                 struct slcli_extent_request *, uint64_t *);
/* Fixed parent transport caller only, after its actual successful unmap.
 * Remove current view geometry while backing/readonly aliases stay charged.
 * Original view/result remain in native capture; no retirement/remap grant. */
int slcli_memory_backing_view_unmapped(struct slcli_memory_owner *,
  const struct slcli_extent_request *,uint64_t,uintptr_t,uint64_t);
int slcli_memory_retired(struct slcli_memory_owner *, uint32_t, uint64_t,
                         uintptr_t, uint64_t);
#if defined(__linux__)
int slcli_linux_bootstrap_pages(struct slcli_memory_owner *, uint32_t,
                                uintptr_t, uintptr_t[2]);
#endif
#endif
