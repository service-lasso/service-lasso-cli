#ifndef SLCLI_MANAGEMENT_CREDENTIAL_PERSIST_H
#define SLCLI_MANAGEMENT_CREDENTIAL_PERSIST_H
#include "credential_acquire.h"
#include "../host-service/store_body.h"
#if defined(_WIN32) || defined(__linux__)
/* PRIVATE mutable-file preparation only. The independently admitted original
 * native creator supplies an empty held file and capture owner. This body does
 * not create paths, designate a realm, seal bytes or issue an association. */
struct slcli_management_credential_persistence {
  struct slcli_arena *arena;
  struct slcli_management_credential_acquisition *acquisition;
  struct slcli_store_stream *write,*first,*second;
  slcli_input_handle original;
  slcli_store_capture_sink capture_sink;
  void *capture_original;
  size_t capture_original_bytes;
  struct slcli_native_clock clocks[6],before[3][68];
  slcli_input_capture *pending_capture;
  struct slcli_store_stream *pending_stream;
  size_t before_used[3];
  uint64_t previous_tick;
  /* write_verified means the full stream completed flush AND final held
   * validation. A false value never denies an earlier native write/flush effect;
   * every actual failed/completed effect remains in write->captures. */
  uint32_t prepared,original_bound,started,unresolved,write_verified,first_verified,second_verified,complete;
};
/* Prepare all three finite full short-IO-capacity streams BEFORE RNG effects.
 * Each68-row allocation and its private buffers remain charged on failure. */
struct slcli_management_credential_persistence *slcli_management_credential_persistence_allocate(
  struct slcli_arena *,struct slcli_management_credential_acquisition *,
  slcli_input_handle,slcli_store_capture_sink,void *,size_t);
/* Private construction storage before native birth. No native handle or effect
 * is admitted until the owning factory binds its actual create-only result. */
struct slcli_management_credential_persistence *slcli_management_credential_persistence_prepare(
  struct slcli_arena *,struct slcli_management_credential_acquisition *,
  slcli_store_capture_sink,void *,size_t);
/* Read-only exact pre/native/post row chain; alone is not native exercise. */
int slcli_management_credential_persistence_completed_lineage(const struct slcli_management_credential_persistence *);
int slcli_management_credential_persist(struct slcli_management_credential_persistence *);
#endif
#endif
