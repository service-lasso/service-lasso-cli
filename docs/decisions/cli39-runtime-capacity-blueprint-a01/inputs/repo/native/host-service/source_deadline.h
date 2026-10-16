#ifndef SLCLI_SOURCE_DEADLINE_H
#define SLCLI_SOURCE_DEADLINE_H
#include "arena.h"
#include "native_clock.h"
/* Retained private source caller budget, never supplied by incoming JSON or
 * renewed by nested dispatch/commit/IO completion. Authentic caller/native
 * origin and full operation lifetime admission remain independent gates. */
struct slcli_source_deadline_original {
  const struct slcli_native_clock *origin;
  const void *caller_original;
  uint64_t expires;
};
struct slcli_primary_engine;
struct slcli_primary_commit_originals;
struct slcli_ledger_commit;
struct slcli_ledger_state;
/* The real once-only pair result is separate from metadata publication. No
 * maximum generation or successful generic leave can manufacture progress. */
struct slcli_source_ledger_progress {
  const struct slcli_primary_engine *primary_original;
  const struct slcli_ledger_commit *before_pair,*pair_original;
  const struct slcli_primary_commit_originals *commit_original;
  const struct slcli_primary_commit_originals *commit_before;
  const struct slcli_primary_commit_originals *returned_commit_original;
  const struct slcli_ledger_state *candidate_original;
  uint64_t base_generation,successor_generation;
  uint32_t base_slot,successor_slot,started,pair_returned,qualified;
  unsigned char base_hash[32],successor_hash[32];
  int pair_result;
};
/* One charged native metadata attempt. Fresh raw rows are never reset/reused;
 * its retained predecessor chain survives all transaction/lock failures. */
struct slcli_source_transition_attempt {
  struct slcli_source_transition_attempt *prior;
  const struct slcli_source_deadline_original *deadline;
  const void *original;
  const void *root_original;
  const struct slcli_native_clock *original_origin;
  struct slcli_native_clock entry,leave,leave_locked;
  uint64_t original_expires,original_origin_tick;
  uint32_t operation,completed,unresolved;
  /* The owning body return precedes every fallible leave-clock/publication
   * action. Publication is a distinct original result, never a replacement
   * for a native/body failure. Each fresh attempt is retained in prior. */
  int effect_result,publication_result;
  uint32_t effect_returned,publication_returned;
  struct slcli_source_ledger_progress ledger_progress;
};
int slcli_source_deadline_original_matches(struct slcli_arena *,
  const struct slcli_source_deadline_original *,const void *);
int slcli_source_deadline_away(const struct slcli_source_deadline_original *,const void *,size_t);
struct slcli_source_transition_attempt *slcli_source_transition_begin(struct slcli_arena *,
  const struct slcli_source_deadline_original *,const void *,const void *,uint32_t,
  struct slcli_source_transition_attempt *);
int slcli_source_transition_before_leave(struct slcli_source_transition_attempt *);
/* Fresh original native clock capture before the single nonwaiting metadata
 * acquisition. The locked predicate below performs metadata checks only. */
int slcli_source_transition_publication_clock(struct slcli_source_transition_attempt *);
int slcli_source_transition_locked_leave(struct slcli_source_transition_attempt *);
int slcli_source_ledger_begin(struct slcli_source_transition_attempt *,const struct slcli_primary_engine *);
int slcli_source_ledger_prepare(struct slcli_arena *,struct slcli_source_transition_attempt *,
  const struct slcli_primary_engine *,const struct slcli_primary_commit_originals *);
int slcli_source_ledger_record(struct slcli_source_transition_attempt *,const struct slcli_primary_engine *,
  const struct slcli_primary_commit_originals *,const struct slcli_ledger_commit *,int);
int slcli_source_ledger_unchanged(const struct slcli_source_transition_attempt *,const struct slcli_primary_engine *);
#endif
