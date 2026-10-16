#ifndef SLCLI_SOURCE_WINDOWS_H
#define SLCLI_SOURCE_WINDOWS_H
#include "capture_windows.h"
#include "source_operation.h"
#if defined(_WIN32)
/* Actual storage/serialized native owner, not an actor enrolment. Original
 * source authority remains separately required; no dispatch/admission API exists. */
struct slcli_source_windows {
  struct slcli_arena *arena;
  struct slcli_capture_store_owner *store;
  struct slcli_shared_workspace *workspace;
  struct slcli_peer_path_workspace *path;
  SRWLOCK lock;
  volatile LONG owning_thread;
  volatile LONG transition_unresolved;
  struct slcli_source_transition_attempt *attempts[3],*retained_attempts;
  struct slcli_source_transition_attempt *transaction_attempt;
  const struct slcli_source_deadline_original *deadline_original;
  uint32_t lock_depth;
  uint32_t operation[3];
  /* Short-lock pending marker. Original request/capture/control storage remains
   * separately retained in SAME arena; a suspended native operation keeps this
   * exact root/row/buffer owner exclusive without holding SRW across IO. */
  const void *transaction_original,*pending_native_original;
  const void *continued_original;
  LONG continued_thread;
  uint64_t expected_generation;
  uint64_t observed_clock_frontier;
  uint32_t expected_sequence;
  struct slcli_capture_windows *graphs[16];
  const struct slcli_capture_windows_originals *originals[16];
  uint32_t count,active,failed;
};
struct slcli_source_windows *slcli_source_windows_allocate(
  struct slcli_capture_store_owner *);
/* SRW protects transitions only. The immutable original marker excludes every
 * conflicting transaction through native IO and nested ordinary calls. Native
 * wait/read/write/flush/readback executes AFTER the SRW lock is released. */
struct slcli_source_windows *slcli_source_windows_enter(struct slcli_primary_engine *,uint32_t,
  const void *,const void *);
int slcli_source_windows_pending(struct slcli_source_windows *,const void *);
int slcli_source_windows_continue(struct slcli_source_windows *,const void *,int);
/* Custody of a previously submitted SAME native row only; never effect/reuse admission. */
int slcli_source_windows_pending_custody(struct slcli_source_windows *,const void *,const void *);
int slcli_source_windows_held(struct slcli_source_windows *);
int slcli_source_windows_graph_held(struct slcli_source_windows *,
  const struct slcli_capture_windows *,const struct slcli_capture_windows_originals *);
int slcli_source_windows_leave(struct slcli_source_windows *,int);
/* SAME retained authentic process/channel/image/birth source inputs must be
 * supplied in actual LIVE owning storage; binding.reference rows are not inputs.
 * Positive native/raw association still cannot authorize service admission. */
int slcli_source_windows_capture(struct slcli_source_windows *,
  const struct slcli_capture_windows_originals *,size_t,size_t,size_t,size_t);
#endif
#endif
