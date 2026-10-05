#include "inspection.h"
static int fail(struct slcli_inspection *s) { if (s) s->failed = 1; return 0; }
static int nonzero(const unsigned char raw[32]) {
  unsigned char bits = 0; unsigned i;
  for (i = 0; i < 32; ++i) bits |= raw[i];
  return bits != 0;
}
static int digest(struct slcli_cursor *c, const unsigned char expected[32]) {
  const unsigned char *raw;
  return slcli_take(c, 32, &raw) && slcli_equal(raw, expected, 32);
}
int slcli_inspection_frame(struct slcli_inspection *s,
                            const struct slcli_frame *frame,
                            struct slcli_slice *chunk) {
  static const unsigned char domain[] = "SLCLI-INSPECTION-RECORD-1\x00";
  struct slcli_cursor c;
  const unsigned char *raw;
  unsigned char actual[32];
  uint8_t op;
  uint32_t seq, offset, length, count, total, i;
  if (chunk) { chunk->raw = 0; chunk->bytes = 0; }
  if (!s || !chunk || !frame || s->failed || s->ended ||
      frame->length < 5 || frame->length > SLCLI_FRAME_BYTES ||
      !nonzero(s->run) || !nonzero(s->nonce) ||
      !nonzero(s->receipt) || !nonzero(s->catalog)) return fail(s);
  c.raw = frame->body; c.bytes = frame->length; c.offset = 0; c.failed = 0;
  if (!slcli_u8(&c, &op) || !slcli_u32(&c, &seq) || seq != s->sequence)
    return fail(s);
  if (op == 1) {
    if (s->begun || seq || !digest(&c, s->run) || !digest(&c, s->nonce) ||
        !digest(&c, s->receipt) || !digest(&c, s->catalog) ||
        !slcli_take(&c, 32, &raw)) return fail(s);
    for (i = 0; i < 32; ++i) s->digest[i] = raw[i];
    if (!nonzero(s->digest) || !slcli_u32(&c, &s->length) ||
        s->length < 133 || s->length > 768000 ||
        !slcli_u32(&c, &s->count) || !s->count || s->count > 128 ||
        !slcli_u32(&c, &s->total) || s->total > 768000 ||
        !slcli_cursor_end(&c)) return fail(s);
    slcli_sha256_init(&s->record);
    if (!slcli_sha256_update(&s->record, domain, sizeof(domain) - 1)) return fail(s);
    s->begun = 1;
  } else if (op == 2) {
    if (!s->begun || seq > 1024 || !digest(&c, s->run) ||
        !digest(&c, s->digest) || !slcli_u32(&c, &offset) || offset != s->offset ||
        !slcli_u32(&c, &length) || !length || length > 16307 ||
        s->offset > s->length || length > s->length - s->offset ||
        !slcli_take(&c, length, &raw) || !slcli_cursor_end(&c)) return fail(s);
    if (!slcli_sha256_update(&s->record, raw, length)) return fail(s);
    s->offset += length;
    /* Borrowed exact native frame bytes: owning engine must retain/write the
     * ORIGINAL chunk before this native buffer may be reused. No JS alias. */
    chunk->raw = raw; chunk->bytes = length;
  } else if (op == 3) {
    if (!s->begun || s->offset != s->length || !digest(&c, s->run) ||
        !digest(&c, s->digest) || !slcli_u32(&c, &length) || length != s->length ||
        !slcli_u32(&c, &count) || count != s->count ||
        !slcli_u32(&c, &total) || total != s->total || !slcli_cursor_end(&c))
      return fail(s);
    if (!slcli_sha256_finish(&s->record, actual)) return fail(s);
    i = (unsigned)slcli_equal(actual, s->digest, 32);
    slcli_erase(actual, sizeof(actual));
    if (!i) return fail(s);
    s->ended = 1; /* END is not native read-zero, flush, close or FREE. */
  } else return fail(s);
  if (s->sequence == UINT32_MAX) return fail(s);
  ++s->sequence;
  return 1;
}
