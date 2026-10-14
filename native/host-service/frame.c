#include "frame.h"
#include "wire.h"
struct domain { const unsigned char *raw; size_t bytes; };
#define DOMAIN(value) {(const unsigned char *)(value), sizeof(value) - 1}
static const struct domain domains[] = {
  DOMAIN("SLCLI-HOST-ADMISSION-2\x00"),
  DOMAIN("SLCLI-HOST-LAUNCH-3\x00"),
  DOMAIN("SLCLI-INSPECTION-SEA-1\x00"),
  DOMAIN("SLCLI-INSPECTION-PRIMARY-1\x00"),
  DOMAIN("SLCLI-INSPECTION-LIBRARY-1\x00")
};
static int nonzero(const unsigned char value[32]) {
  unsigned char result = 0;
  unsigned i;
  if (!value) return 0;
  for (i = 0; i < 32; ++i) result |= value[i];
  return result != 0;
}
int slcli_frame_mac(enum slcli_frame_domain domain, uint8_t direction,
                     const unsigned char nonce[32], const unsigned char key[32],
                     uint32_t sequence, const struct slcli_frame *frame,
                     unsigned char out[32]) {
  struct slcli_hmac256 h;
  unsigned char seq[4], length[4];
  struct slcli_cursor cursor;
  uint8_t operation;
  uint32_t original_sequence;
  int ok;
  if ((unsigned)domain >= sizeof(domains) / sizeof(domains[0]) || direction > 1 ||
      !out || !frame || frame->length < 5 || frame->length > SLCLI_FRAME_BYTES ||
      !nonzero(nonce) || !nonzero(key)) return 0;
  cursor.raw = frame->body; cursor.bytes = frame->length;
  cursor.offset = 0; cursor.failed = 0;
  if (!slcli_u8(&cursor, &operation) || !slcli_u32(&cursor, &original_sequence) ||
      original_sequence != sequence) return 0;
  /* Operation phase/cap is independently closed by the native command reader;
   * this primitive cannot authorize an otherwise unknown authenticated opcode. */
  (void)operation;
  slcli_put_u32(seq, sequence); slcli_put_u32(length, frame->length);
  ok = slcli_hmac256_init(&h, key) &&
       slcli_hmac256_update(&h, domains[domain].raw, domains[domain].bytes) &&
       slcli_hmac256_update(&h, &direction, 1) &&
       slcli_hmac256_update(&h, nonce, 32) &&
       slcli_hmac256_update(&h, seq, 4) &&
       slcli_hmac256_update(&h, length, 4) &&
       slcli_hmac256_update(&h, frame->body, frame->length) &&
       slcli_hmac256_finish(&h, out);
  slcli_erase(&h, sizeof(h));
  if (!ok) slcli_erase(out, 32);
  return ok;
}
int slcli_frame_verify(enum slcli_frame_domain domain, uint8_t direction,
                        const unsigned char nonce[32], const unsigned char key[32],
                        uint32_t sequence, const struct slcli_frame *frame,
                        const unsigned char actual[32]) {
  unsigned char expected[32];
  int ok;
  if (!actual) return 0;
  ok = slcli_frame_mac(domain, direction, nonce, key, sequence, frame, expected);
  if (ok) ok = slcli_equal(expected, actual, 32);
  slcli_erase(expected, sizeof(expected));
  return ok;
}
