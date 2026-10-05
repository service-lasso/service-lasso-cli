#ifndef SLCLI_PRIMARY_WIRE_H
#define SLCLI_PRIMARY_WIRE_H
#include "frame.h"
#include "wire.h"
/* Parsed claims are not verified native observations or ledger effects. The
 * owning engine must match every row to retained original classified objects. */
struct slcli_snapshot_claim {
  const unsigned char *raw;
  uint32_t bytes, count;
  uint8_t phase;
  uint64_t charged;
};
struct slcli_primary_request {
  uint8_t operation, expected;
  uint32_t sequence;
  const unsigned char *reservation, *snapshot_digest, *observer;
  uint64_t deadline;
  struct slcli_snapshot_claim snapshot;
};
int slcli_snapshot_claim_parse(const unsigned char *, uint32_t, uint64_t,
                               struct slcli_snapshot_claim *);
int slcli_primary_request_parse(const struct slcli_frame *, uint64_t,
                                struct slcli_primary_request *);
#endif
