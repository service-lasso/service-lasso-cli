#ifndef SLCLI_MANAGEMENT_CREDENTIAL_ORIGINAL_H
#define SLCLI_MANAGEMENT_CREDENTIAL_ORIGINAL_H
#include "../host-service/input_body.h"
#if defined(_WIN32) || defined(__linux__)
/* PRIVATE BQ view only. Native readonly/seal/source/provider/realm and actor
 * admission preexist; actual two-pass raw64 consumption does not grant them.
 * No service/public constructor or signing-seed handoff is supplied here. */
struct slcli_management_credential {
  struct slcli_arena *arena;
  struct slcli_input_body *original;
  const unsigned char *seed,*ledger;
  unsigned char raw_digest[32];
  uint32_t started,failed,complete;
};
struct slcli_management_credential *slcli_management_credential_allocate(struct slcli_arena *);
int slcli_management_credential_consume(struct slcli_management_credential *,struct slcli_input_body *);
#endif
#endif
