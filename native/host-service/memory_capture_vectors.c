#include "platform_memory.h"

/* Unexecuted original-row preservation regression. Calling these actual
 * platform entry points with an already used capture must return before ANY
 * native acquisition/free/query/clock effect, regardless of completion state.
 * Whole source/native admission precedes compiling or executing this source. */
static struct slcli_memory_capture original;
static unsigned char saved[sizeof(original)];
static int unchanged(void) {
  const unsigned char *raw = (const unsigned char *)&original;
  size_t i;
  for (i = 0; i < sizeof(original); ++i) if (raw[i] != saved[i]) return 0;
  return 1;
}
int slcli_memory_capture_vectors(struct slcli_memory_owner *owner,
                                 uint32_t original_live_extent) {
  unsigned char *raw = (unsigned char *)&original;
  size_t i;
  uint32_t operation, observed;
  /* The full native qualification fixture supplies its genuine already live
   * original owner/extent. No fabricated address or test-owned numeric handle
   * is passed to a native release, even if the rejection guard regresses. */
  if (!owner || owner->failed || original_live_extent >= owner->count ||
      original_live_extent >= 256 ||
      owner->extents[original_live_extent].state != SLCLI_EXTENT_LIVE ||
      !owner->extents[original_live_extent].base) return 0;
  for (operation = 1; operation <= 13; ++operation) {
    for (observed = 0; observed <= 1; ++observed) {
      for (i = 0; i < sizeof(original); ++i) raw[i] = (unsigned char)(i + 1);
      original.operation = operation;
      original.observed = observed;
      for (i = 0; i < sizeof(original); ++i) saved[i] = raw[i];
      if (slcli_platform_acquire(owner, SLCLI_RUNTIME, 4096, &original) ||
          !unchanged()) return 0;
      if (slcli_platform_release(owner, original_live_extent, &original) ||
          !unchanged()) return 0;
    }
  }
  /* Overlapping owner/capture control must be rejected without dereferencing
   * an invented capture row or overwriting original owner metadata. */
  if (slcli_memory_capture_fresh(owner,
      (const struct slcli_memory_capture *)owner)) return 0;
  if (slcli_memory_capture_fresh((const struct slcli_memory_owner *)
      (UINTPTR_MAX - sizeof(*owner) + 1), &original)) return 0;
  if (slcli_memory_capture_fresh(owner, (const struct slcli_memory_capture *)
      (UINTPTR_MAX - sizeof(original) + 1))) return 0;
  return 1;
}
