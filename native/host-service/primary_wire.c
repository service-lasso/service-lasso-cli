#include "primary_wire.h"
static int b32(struct slcli_cursor *c, const unsigned char **raw, int required) {
  unsigned char bits = 0;
  unsigned i;
  if (!slcli_take(c, 32, raw)) return 0;
  for (i = 0; i < 32; ++i) bits |= (*raw)[i];
  return !required || bits != 0;
}
int slcli_snapshot_claim_parse(const unsigned char *raw, uint32_t bytes,
                               uint64_t original_limit,
                               struct slcli_snapshot_claim *out) {
  struct slcli_cursor c;
  struct slcli_snapshot_claim result = {0};
  const unsigned char *identity, *observation;
  uint8_t version, kind, state;
  uint32_t i, id, previous = 0, index;
  uint64_t charged;
  if (out) *out = result;
  if (!raw || !out || !original_limit || bytes < 6 || bytes > 6 + 128 * 82)
    return 0;
  c.raw = raw; c.bytes = bytes; c.offset = 0; c.failed = 0;
  if (!slcli_u8(&c, &version) || version != 1 ||
      !slcli_u8(&c, &result.phase) || result.phase > 7 ||
      !slcli_u32(&c, &result.count) || !result.count || result.count > 128)
    return 0;
  for (i = 0; i < result.count; ++i) {
    if (!slcli_u32(&c, &id) || (i && id <= previous) ||
        !slcli_u8(&c, &kind) || kind > 8 || !slcli_u8(&c, &state) || state > 3 ||
        !slcli_u64(&c, &charged) || charged > original_limit - result.charged ||
        !b32(&c, &identity, 1) || !slcli_u32(&c, &index) ||
        !b32(&c, &observation, 1)) return 0;
    if (index == UINT32_MAX) {
      if (kind != 7 && state != 1) return 0;
    } else if (index >= 128 || kind == 7) return 0;
    if (state == 1 && kind != 7 && charged) return 0;
    result.charged += charged; previous = id;
  }
  if (!slcli_cursor_end(&c)) return 0;
  result.raw = raw; result.bytes = bytes;
  *out = result;
  return 1;
}
int slcli_primary_request_parse(const struct slcli_frame *frame,
                                uint64_t original_limit,
                                struct slcli_primary_request *out) {
  struct slcli_cursor c;
  struct slcli_primary_request result = {0};
  const unsigned char *raw;
  uint8_t phase;
  uint32_t bytes;
  if (out) *out = result;
  if (!frame || !out || !original_limit || frame->length < 5 ||
      frame->length > SLCLI_FRAME_BYTES) return 0;
  c.raw = frame->body; c.bytes = frame->length; c.offset = 0; c.failed = 0;
  if (!slcli_u8(&c, &result.operation) || !slcli_u32(&c, &result.sequence)) return 0;
  switch (result.operation) {
  case 1:
    if (result.sequence || !slcli_cursor_end(&c)) return 0;
    break;
  case 3:
    if (!result.sequence || result.sequence > 1024 ||
        !b32(&c, &result.reservation, 1) || !slcli_u8(&c, &phase) || phase > 7 ||
        !slcli_u32(&c, &bytes) || bytes < 6 || bytes > 6 + 128 * 82 ||
        !slcli_take(&c, bytes, &raw) || !slcli_cursor_end(&c) ||
        !slcli_snapshot_claim_parse(raw, bytes, original_limit, &result.snapshot) ||
        result.snapshot.phase != phase) return 0;
    break;
  case 5:
    if (!result.sequence || result.sequence > 1025 || frame->length > 256 ||
        !b32(&c, &result.reservation, 1) || !b32(&c, &result.snapshot_digest, 1) ||
        !b32(&c, &result.observer, 1) || !slcli_u8(&c, &result.expected) ||
        result.expected || !slcli_u64(&c, &result.deadline) ||
        !result.deadline || !slcli_cursor_end(&c)) return 0;
    break;
  default: return 0;
  }
  *out = result;
  return 1;
}
