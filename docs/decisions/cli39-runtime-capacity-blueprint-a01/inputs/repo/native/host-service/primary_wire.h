#ifndef SLCLI_PRIMARY_WIRE_H
#define SLCLI_PRIMARY_WIRE_H
#include "frame.h"
#include "wire.h"
/* Parsed claims are not verified native observations or ledger effects. The
 * owning engine must match every row to retained original classified objects. */
#define SLCLI_RUNTIME_ROW_BYTES 124u
#define SLCLI_RUNTIME_SNAPSHOT_BYTES (6u+128u*SLCLI_RUNTIME_ROW_BYTES)
struct slcli_runtime_row_claim {
 uint32_t id,handle_index,source_member,capture_first,capture_count,start_cut,end_cut;
 uint64_t charged,native_rights;
 uint16_t alias_of,physical_extent,effect_flags;
 uint8_t kind,state,owner_domain,process_role,producer_slot,consumer_slot,input_slot,applicability;
 const unsigned char *identity,*observation;
};
int slcli_runtime_row_claim_parse(const unsigned char *,uint32_t,uint32_t,
                                 struct slcli_runtime_row_claim *);
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
