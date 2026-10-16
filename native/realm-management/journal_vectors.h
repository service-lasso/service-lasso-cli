#ifndef SLCLI_MANAGEMENT_JOURNAL_VECTORS_H
#define SLCLI_MANAGEMENT_JOURNAL_VECTORS_H
#include "signature_vectors.h"
#include "../host-service/ha2_transition.h"
#include "../host-service/ha2_journal_pair.h"
#include "../host-service/native_clock.h"
#if defined(_WIN32) || defined(__linux__)
struct slcli_management_journal_vector {
  struct slcli_arena *arena;
  struct slcli_management_record_vector *record;
  struct slcli_ha2_journal_pair *pair;
  struct slcli_native_clock entry_clock,publication_clock,publication_locked_clock;
  uint32_t publication_lock_observed,publication_lock_acquired;
  uint32_t started,failed,complete;
};
struct slcli_management_journal_vector *slcli_management_journal_vector_allocate(struct slcli_arena *,size_t);
/* Actual native fixture: genuine source-owned before-effect eighteen-field
 * frontier, admitted descriptor and seed/anchor, old high-water raw prefix,
 * two held journals, and native capture persistence owner preexist this call.
 * This composes positive sign/transition/pair IO; it never publishes authority
 * or initializes the authentic BQ native high-water/realm/compound owner. */
int slcli_management_journal_positive_vector(struct slcli_management_journal_vector *,
  const struct slcli_ha2_journal_prefix_originals *,const unsigned char[32],const unsigned char[32],
  const unsigned char *,size_t,const unsigned char *const *,uint32_t,
  slcli_input_handle,slcli_input_handle,uint64_t,slcli_store_capture_sink,void *);
#endif
#endif
