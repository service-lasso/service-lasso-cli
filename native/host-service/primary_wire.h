#ifndef SLCLI_PRIMARY_WIRE_H
#define SLCLI_PRIMARY_WIRE_H
#include "frame.h"
#include "wire.h"
/* Parsed claims are not verified native observations or ledger effects. The
 * owning engine must match every row to retained original classified objects. */
#define SLCLI_RUNTIME_ROW_BYTES 124u
#define SLCLI_RUNTIME_SNAPSHOT_VERSION 3u
#define SLCLI_RUNTIME_MAX_NATIVE_CLASS 22u
#define SLCLI_RUNTIME_SNAPSHOT_BYTES (6u+128u*SLCLI_RUNTIME_ROW_BYTES)
#define SLCLI_RUNTIME_FRAGMENT_ENVELOPE_BYTES 131u
#define SLCLI_RUNTIME_FRAGMENT_PAYLOAD_BYTES (SLCLI_FRAME_BYTES-SLCLI_RUNTIME_FRAGMENT_ENVELOPE_BYTES)
struct slcli_runtime_wire_geometry {
 uint32_t count,snapshot_bytes,terminal_bitmap_bytes,fragment_count;
};
/* Prospective source geometry only: no numeric profile selection, allocation,
 * native authority or widening of the active legacy admission ceiling. */
int slcli_runtime_wire_geometry_derive(uint32_t,struct slcli_runtime_wire_geometry *);
struct slcli_snapshot_fragment_claim {
 uint32_t sequence,whole_bytes,ordinal,fragment_count,offset,chunk_bytes;
 uint64_t revision;
 uint8_t phase;
 const unsigned char *reservation,*digest,*frontier,*chunk;
};
/* Claims only. Owning immutable native pins and one whole assembly/durable
 * transition are required separately before this can reach active dispatch. */
int slcli_snapshot_fragment_claim_parse(const struct slcli_frame *,uint32_t,
 struct slcli_snapshot_fragment_claim *);
int slcli_snapshot_fragment_claim_encode(struct slcli_frame *,uint32_t,
 const struct slcli_snapshot_fragment_claim *);
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
/* Exact prospective original shape; borrowed bytes only, no table allocation
 * or native/source authority. Active public dispatch remains separately gated. */
int slcli_snapshot_claim_parse_exact_shape(const unsigned char *,uint32_t,
 uint32_t,uint64_t,struct slcli_snapshot_claim *);
int slcli_primary_request_parse(const struct slcli_frame *, uint64_t,
                                struct slcli_primary_request *);
#endif
