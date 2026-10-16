#ifndef SLCLI_MANAGEMENT_JOURNAL_OWNER_VECTORS_H
#define SLCLI_MANAGEMENT_JOURNAL_OWNER_VECTORS_H
#include "journal_owner.h"
#if defined(_WIN32) || defined(__linux__)
struct slcli_management_committed_original {
  const struct slcli_management_journal_owner *owner;
  const unsigned char *last;
  uint64_t sequence,generation;
  uint32_t state;
  unsigned char hash[32];
};
/* Runs on an already independently admitted qualification fixture thread.
 * Its real stack/native thread/input/runtime references belong the original
 * BQ qualification owner and full case/capacity graph; never an extra free
 * helper, production actor or process birth inferred from the numeric ID. */
struct slcli_management_contention_fixture {
  struct slcli_arena *arena;
  struct slcli_management_journal_owner *owner_original;
  const struct slcli_management_deadline_original *deadline;
  uintptr_t lock_address;
  struct slcli_native_clock *clocks;
  struct slcli_native_clock release_clock;
  size_t capacity,count;
  long actual_thread;
#if defined(_WIN32)
  LONG ready,release;
#else
  atomic_int ready,release;
#endif
  uint32_t started,lock_observed,lock_acquired,completed,expired;
  uint32_t clock_failed,capacity_exhausted,release_observed;
};
struct slcli_management_contention_fixture *slcli_management_contention_fixture_allocate(struct slcli_arena *,size_t);
int slcli_management_publication_contender_original(struct slcli_management_journal_owner *,
  const struct slcli_management_deadline_original *,struct slcli_management_contention_fixture *);
struct slcli_management_expiry_fixture {
  struct slcli_arena *arena;
  struct slcli_native_clock *clocks;
  const struct slcli_management_deadline_original *deadline;
  size_t capacity,count;
  uint32_t started,complete,expired;
};
struct slcli_management_expiry_fixture *slcli_management_expiry_fixture_allocate(struct slcli_arena *,size_t);
/* Private fixture driver calls this after actual pair/capture preservation,
 * before owner publication. It observes REAL original deadline expiry using
 * finite fresh retained native rows; it never edits clock/deadline bytes or
 * reports expiry merely because row capacity was exhausted. */
int slcli_management_publication_expiry_original(const struct slcli_management_deadline_original *,
  struct slcli_management_expiry_fixture *);
enum slcli_management_publication_case { SLCLI_PUBLICATION_REAL_EXPIRY=1,SLCLI_PUBLICATION_REAL_CONTENTION=2 };
struct slcli_management_publication_capture_original {
  struct slcli_management_journal_vector *vector;
  const struct slcli_management_deadline_original *deadline;
  struct slcli_management_expiry_fixture *expiry;
  struct slcli_management_contention_fixture *contender;
  slcli_store_capture_sink persistence;
  void *persistence_original;
  uint32_t source_case,cause_observed;
};
/* PRIVATE source fixture capture adapter preserves genuine raw ABI/body bytes
 * through the independently admitted real persistence sink FIRST. Only after
 * final native observation of journal2's full readback does it cause genuine
 * clock expiry or require readiness from the real admitted contention thread.
 * No fake capture, deadline edit, published field or production hook is used. */
int slcli_management_publication_capture_original(void *,const slcli_input_capture *,
  const unsigned char *,size_t);
/* Real qualification caller binds SAME retained suite origin and its own
 * finite remaining operation deadline within absolute40. No production policy
 * is derived from this qualification fixture. Full authentic source/native
 * owner/frontier/journal/seed/capture inputs still precede the call. */
int slcli_management_journal_qualification_positive_vector(
  struct slcli_management_journal_owner *,struct slcli_management_journal_vector *,
  const struct slcli_ha2_journal_prefix_originals *,const struct slcli_management_deadline_original *,
  const struct slcli_native_clock *,const unsigned char[32],const unsigned char[32],
  const unsigned char *,size_t,slcli_input_handle,slcli_input_handle,slcli_store_capture_sink,void *);
/* Postcondition checks require an actual failed publication with retained
 * native TryAcquire/clock rows and independently retained BEFORE committed
 * source view. Contention must come from the actual original fixture thread;
 * expiry from the SAME original finite deadline, never a synthetic timeout. */
int slcli_management_publication_failure_vector(const struct slcli_management_journal_owner *,
  const struct slcli_management_journal_vector *,const struct slcli_management_committed_original *,
  const struct slcli_management_deadline_original *);
#endif
#endif
