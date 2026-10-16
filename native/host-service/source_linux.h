#ifndef SLCLI_SOURCE_LINUX_H
#define SLCLI_SOURCE_LINUX_H
#include "primary_engine.h"
#include "arena.h"
#include "shared_workspace.h"
#include "source_operation.h"
#if defined(__linux__)
#include <stdatomic.h>
#if ATOMIC_LONG_LOCK_FREE != 2
#error Selected Linux source transitions require always lock-free native long atomics
#endif
_Static_assert(ATOMIC_INT_LOCK_FREE==2,"Source unresolved marker must be lock-free");
/* SAME service arena storage. This transition owner is neither a credential
 * nor a constructor for authenticated actors or native source observations. */
struct slcli_source_linux {
  struct slcli_arena *arena;
  struct slcli_primary_engine *primary;
  struct slcli_shared_workspace *workspace;
  atomic_flag transition;
  atomic_long owning_thread;
  atomic_int transition_unresolved;
  struct slcli_source_transition_attempt *attempts[3],*retained_attempts;
  struct slcli_source_transition_attempt *transaction_attempt;
  const struct slcli_source_deadline_original *deadline_original;
  uint32_t lock_depth,operation[3],failed;
  const void *transaction_original,*pending_native_original;
  const void *continued_original;
  long continued_thread;
  uint64_t expected_generation;
  uint64_t observed_clock_frontier;
  uint32_t expected_sequence;
};
struct slcli_source_linux *slcli_source_linux_allocate(struct slcli_arena *,
  struct slcli_primary_engine *,struct slcli_shared_workspace *);
struct slcli_source_linux *slcli_source_linux_enter(struct slcli_primary_engine *,
  uint32_t,const void *,const void *);
int slcli_source_linux_pending(struct slcli_source_linux *,const void *);
int slcli_source_linux_continue(struct slcli_source_linux *,const void *,int);
int slcli_source_linux_leave(struct slcli_source_linux *,int);
#endif
#endif
