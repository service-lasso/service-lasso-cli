#ifndef SLCLI_LEDGER_COMMIT_H
#define SLCLI_LEDGER_COMMIT_H
#include "store_publish.h"
#include "store_deadline.h"
#if defined(_WIN32) || defined(__linux__)
/* Source-owned pending commit retains every original temporary/readback object
 * and all native captures on every outcome. No key/profile/source/store lease
 * is constructed here. Admitted original caller/provider/key/root and immutable
 * decoded cells remain independently supplied, and this route is unqualified. */
struct slcli_ledger_commit {
  struct slcli_store_deadline_owner *deadline_owner;
  struct slcli_store_stream *generation_write,*generation_held,*generation_reopened;
  struct slcli_store_stream *selector_write,*selector_held,*selector_reopened;
  struct slcli_store_publish generation_publish,selector_publish;
  unsigned char generation_nonce[16],selector_nonce[16],payload_hash[32],selector[74];
  uint64_t committed_generation,retained_store_bytes;
  /* Written only by the actual pair body after both complete original
   * durable readbacks. A later deadline/publication failure cannot undo the
   * selector's native successor. This fact alone grants no response or FREE. */
  uint32_t durable_pair_complete;
  int caller_result;
  uint32_t caller_attempted,caller_returned;
  uint32_t started,failed,complete,selected_slot;
};
/* Source-only fresh preparation before effects, not installed admission. */
int slcli_ledger_commit_deadline_bind(struct slcli_arena *,struct slcli_ledger_commit *,
  const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,slcli_store_capture_sink,void *,size_t);
int slcli_ledger_commit_original(struct slcli_ledger_commit *,
                                  const struct slcli_ledger_state *,
                                  const unsigned char[32],slcli_input_handle,
                                  slcli_input_handle,slcli_input_handle,
                                  uint8_t,uint64_t,slcli_store_capture_sink,void *);
#endif
#endif
