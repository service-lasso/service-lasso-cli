#ifndef SLCLI_ROOT_FOUNDATION_H
#define SLCLI_ROOT_FOUNDATION_H
#include "../realm-management/foundation_roots.h"
#if defined(_WIN32)
#include "../realm-management/foundation_create_windows.h"
typedef struct slcli_management_foundation_windows_creation slcli_root_creator;
#elif defined(__linux__)
#include "../realm-management/foundation_create_linux.h"
typedef struct slcli_management_foundation_linux_creation slcli_root_creator;
#endif
#if defined(_WIN32) || defined(__linux__)
/* PRIVATE native ROOT_FOUNDATION custody composition. Each prepared creator
 * already requires authentic independently designated caller68/101..103,
 * namespace/ancestor/ACL/all-alias authority and qualified filesystem inputs.
 * This body does not manufacture those inputs, an association or a grant file.
 * All five creators and the whole observer preexist the first native birth. */
struct slcli_root_foundation {
  struct slcli_arena *arena;
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin;
  const void *caller;
  size_t caller_bytes;
  slcli_root_creator *const *original_creators;
  slcli_root_creator *creators[5];
  slcli_input_handle *birth_cells[5];
  const void *producers[5];
  size_t producer_bytes[5];
  struct slcli_management_foundation_roots *roots;
  struct slcli_native_clock clocks[5];
  uint64_t expires,origin_tick,previous_tick;
  uint32_t prepared,started,attempted,completed,complete,unresolved;
};
struct slcli_root_foundation *slcli_root_foundation_allocate(struct slcli_arena *,
  slcli_root_creator *const[5]);
/* Success is twice-held distinct native directory custody, not BQ/source
 * qualification, credential authority or permission to construct P/G. */
int slcli_root_foundation_create(struct slcli_root_foundation *);
#endif
#endif
