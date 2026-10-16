#ifndef SLCLI_MANAGEMENT_JOURNAL_OWNER_H
#define SLCLI_MANAGEMENT_JOURNAL_OWNER_H
#include "journal_vectors.h"
#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <stdatomic.h>
_Static_assert(ATOMIC_LONG_LOCK_FREE==2,"BQ marker requires native lock-free thread storage");
_Static_assert(ATOMIC_INT_LOCK_FREE==2,"BQ unresolved gate requires native lock-free storage");
#endif
#if defined(_WIN32) || defined(__linux__)
/* Real private caller's retained remaining finite deadline, never selected
 * from public JSON or restarted for publication. Qualification binds its
 * original absolute40; production keeps its own admitted finite policy. */
struct slcli_management_deadline_original {
  const struct slcli_native_clock *origin;
  const unsigned char *operation_original;
  /* Authentic private caller constructor supplies this exact original tuple;
   * it is never synthesized from the public operation or a new clock. */
  const struct slcli_source_deadline_original *native_original;
  size_t caller_bytes,capture_owner_bytes;
  uint64_t expires;
};
/* PRIVATE SAME BQ source-owned committed frontier. Storage allocation is not
 * BQ admission, a realm constructor or a signing capability. Installed native
 * BQ entry must retain its actual independently admitted realm/compound,
 * source/caller/native eighteen-field frontier and credential borrow before
 * entering this body. No portable request or service route can select it. */
struct slcli_management_journal_owner {
  struct slcli_arena *arena;
#if defined(_WIN32)
  SRWLOCK transition;
  LONG owning_thread;
  LONG publication_unresolved;
#else
  atomic_flag transition;
  atomic_long owning_thread;
  atomic_int publication_unresolved;
#endif
  const unsigned char *records[SLCLI_HA2_JOURNAL_RECORD_LIMIT];
  struct slcli_management_journal_vector *pending;
  const unsigned char *pending_operation;
  size_t pending_operation_bytes;
  const struct slcli_management_deadline_original *pending_deadline;
  uint64_t sequence,generation,pending_sequence,pending_generation,retained_store_bytes;
  uint32_t state,unknown;
  unsigned char committed_hash[32];
};
struct slcli_management_journal_owner *slcli_management_journal_owner_allocate(struct slcli_arena *);
int slcli_management_journal_owner_attempt(struct slcli_management_journal_owner *,
  struct slcli_management_journal_vector *,const struct slcli_ha2_journal_prefix_originals *,
  const struct slcli_management_deadline_original *,
  const unsigned char[32],const unsigned char[32],const unsigned char *,size_t,
  slcli_input_handle,slcli_input_handle,slcli_store_capture_sink,void *);
#endif
#endif
