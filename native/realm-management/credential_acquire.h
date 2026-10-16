#ifndef SLCLI_MANAGEMENT_CREDENTIAL_ACQUIRE_H
#define SLCLI_MANAGEMENT_CREDENTIAL_ACQUIRE_H
#include "../host-service/source_deadline.h"
#if defined(_WIN32) || defined(__linux__)
/* PRIVATE BQ acquisition storage. Construction requires the original owning
 * serialization and independently admitted BQ/realm/native caller. Acquisition
 * is neither an association producer nor permission to publish a credential.
 * The complete secret destination remains retained on every failed attempt. */
struct slcli_management_credential_acquisition {
  struct slcli_arena *arena;
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin,*predecessor;
  const void *caller_original;
  size_t caller_bytes;
  uint64_t expires,origin_tick,predecessor_tick,args[4],status,returned_bytes;
  struct slcli_native_clock before,after;
  unsigned char secret[64];
  uint32_t prepared,predecessor_bound,started,native_called,result_observed,count_observed;
  uint32_t destination_retained,complete,unresolved;
};
struct slcli_management_credential_acquisition *slcli_management_credential_acquisition_allocate(
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t);
/* Exactly one native RNG call; short/error/expiry results are never retried.
 * Kernel/provider blocking latency remains a separate qualification gap. */
/* Private native factory joins a completed preceding native clock without
 * renewing the original deadline. This cannot admit the producer/caller. */
int slcli_management_credential_acquire_after(struct slcli_management_credential_acquisition *,
  const struct slcli_native_clock *);
int slcli_management_credential_acquire(struct slcli_management_credential_acquisition *);
#endif
#endif
