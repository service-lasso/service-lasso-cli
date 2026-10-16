#ifndef SLCLI_PRIMARY_ENGINE_H
#define SLCLI_PRIMARY_ENGINE_H
#include "primary_wire.h"
#include "ledger_commit.h"
#include "native_clock.h"
#include "source_deadline.h"
#if defined(_WIN32) || defined(__linux__)
/* These are actual retained original native-reference rows, from the source
 * owner's separately admitted platform/capture readers. They are not populated
 * from incoming snapshot claims. Source/native correlation and authentic actor
 * acquisition remain external unqualified dependencies; the codec admits none. */
struct slcli_native_reference {
 uint64_t handle,charged,native_rights,version;
 uint32_t owner,source_member,capture_first,capture_count,start_cut,end_cut;
 uint16_t source,alias_of,physical_extent,effect_flags;
 uint8_t kind,state,owner_domain,process_role,producer_slot,consumer_slot,input_slot,applicability;
 uint8_t handle_policy,terminal_edge;
 unsigned char identity[32],observation[32];
 /* Private original constructor owns this observation body and exact source
  * range. It must recheck native identity/rights/captures/extent/alias before
  * publication; equal serialized metadata never supplies its authority. */
 const void *native_original;
 int (*native_recheck)(const struct slcli_native_reference *,
                       const struct slcli_runtime_row_claim *);
};
struct slcli_binding_originals {
  const struct slcli_source_deadline_original *deadline_original;
  struct slcli_slice owner;
  unsigned char primary[32],observer[32],epoch[32],reservation[32];
  struct slcli_native_reference *references;
  const uint32_t *original_ids;
  const uint32_t *original_indices;
  unsigned char primary_terminal_bitmap[16];
  const unsigned char *initial_snapshot;
  uint32_t initial_snapshot_bytes;
  uint32_t count;
  uint64_t deadline,original_limit;
};
struct slcli_primary_engine {
  /* SAME original private-capture store owner; never a second byte budget. */
  struct slcli_capture_store_owner *capture_store;
#if defined(__linux__)
  struct slcli_source_linux *source_linux;
#endif
  struct slcli_ledger_state *ledger;
  /* Separate SAME-arena retained candidate. Committed cells are never changed
   * by an attempted native pair. A failed candidate remains retained forever
   * for this failed engine; no rollback copy erases its actual frontier. */
  struct slcli_ledger_state *candidate;
  uint64_t candidate_base_generation;
  uint32_t candidate_started;
  const struct slcli_binding_originals *binding;
  unsigned char key[32],capability[32],nonce[32],payload_hash[32],snapshot_hash[32],intent_hash[32];
  unsigned char observer_capability[32],observer_nonce[32];
  uint64_t committed_generation,retained_store_bytes;
  /* Exact original pair completion remains reachable even if a later
   * publication/response check fails. Never inferred from a maximum sequence. */
  const struct slcli_ledger_commit *committed_pair_original;
  /* The exact accepted source request remains held in its original arena
   * allocation. Intent rechecks these full bytes and every original native
   * descriptor; a digest by itself is not a current native observation. */
  const struct slcli_frame *snapshot_request_original;
  const unsigned char *snapshot_raw_original;
  uint32_t snapshot_bytes_original;
  uint32_t next,slot,reserved,closing,failed,selected_slot,last_phase;
};
struct slcli_arena;
/* Storage allocation in the original service arena before actor activation.
 * It supplies no binding/key/native identity and performs no enrollment. Both
 * frontiers are retained in the same block even after native-pair failure. */
struct slcli_primary_engine *slcli_primary_engine_allocate(struct slcli_arena *);
/* One fresh original pending commit set for this specific command. Parameters
 * come from the admitted source owner, never from the request body. This is not
 * a constructor or production fixture admission. All resources remain held. */
struct slcli_primary_commit_originals {
  const struct slcli_source_deadline_original *deadline_original;
  /* Genuine original constructor supplies exact retained source block extents. */
  size_t caller_original_bytes,capture_original_bytes;
  struct slcli_ledger_commit *commit;
  slcli_input_handle root,generation_temporary,selector_temporary;
  slcli_store_capture_sink capture_sink;
  slcli_namespace_capture_sink namespace_sink;
  void *capture_original;
  struct slcli_store_stream *artifact_write,*artifact_held,*artifact_reopened;
  struct slcli_store_publish *artifact_publish;
  slcli_input_handle artifact_temporary;
  unsigned char artifact_nonce[16];
};
int slcli_primary_engine_dispatch(struct slcli_primary_engine *,
                                   const struct slcli_frame *,const unsigned char[32],
                                   struct slcli_native_clock *,
                                   const struct slcli_primary_commit_originals *,
                                   struct slcli_frame *);
/* Shared concrete store transaction used by ordinary command/observer owning
 * bodies and direct native vectors. Caller owns serialized cells and retains
 * failure; exposing this internal function grants no source/store authority. */
int slcli_primary_engine_commit_original(struct slcli_primary_engine *,
                                          const struct slcli_primary_commit_originals *);
#endif
#endif
