#include "wire.h"

static int fail(struct slcli_cursor *c) { if (c) c->failed = 1; return 0; }
int slcli_take(struct slcli_cursor *c, size_t count, const unsigned char **out) {
  if (!c || !out || c->failed || (!c->raw && c->bytes) ||
      c->offset > c->bytes || count > c->bytes - c->offset) return fail(c);
  *out = c->raw ? c->raw + c->offset : c->raw;
  c->offset += count;
  return 1;
}
int slcli_u8(struct slcli_cursor *c, uint8_t *out) {
  const unsigned char *p;
  if (!out || !slcli_take(c, 1, &p)) return fail(c);
  *out = p[0];
  return 1;
}
int slcli_u32(struct slcli_cursor *c, uint32_t *out) {
  const unsigned char *p;
  if (!out || !slcli_take(c, 4, &p)) return fail(c);
  *out = ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
         ((uint32_t)p[2] << 8) | p[3];
  return 1;
}
int slcli_u64(struct slcli_cursor *c, uint64_t *out) {
  const unsigned char *p;
  uint64_t v = 0;
  unsigned i;
  if (!out || !slcli_take(c, 8, &p)) return fail(c);
  for (i = 0; i < 8; ++i) v = (v << 8) | p[i];
  *out = v;
  return 1;
}
int slcli_utf8(const unsigned char *p, size_t bytes, int allow_nul) {
  size_t at = 0;
  if (!p && bytes) return 0;
  while (at < bytes) {
    uint32_t scalar;
    unsigned remaining;
    unsigned char first = p[at++];
    if (first < 0x80) { if (!allow_nul && !first) return 0; continue; }
    if (first >= 0xc2 && first <= 0xdf) { scalar=first&0x1f; remaining=1; }
    else if (first >= 0xe0 && first <= 0xef) { scalar=first&0x0f; remaining=2; }
    else if (first >= 0xf0 && first <= 0xf4) { scalar=first&7; remaining=3; }
    else return 0;
    if (remaining > bytes - at) return 0;
    while (remaining--) {
      unsigned char next = p[at++];
      if ((next & 0xc0) != 0x80) return 0;
      scalar = (scalar << 6) | (next & 0x3f);
    }
    if ((first <= 0xdf && scalar < 0x80) ||
        (first >= 0xe0 && first <= 0xef && scalar < 0x800) ||
        (first >= 0xf0 && scalar < 0x10000) || scalar > 0x10ffff ||
        (scalar >= 0xd800 && scalar <= 0xdfff)) return 0;
  }
  return 1;
}
int slcli_string(struct slcli_cursor *c, uint32_t maximum, int empty,
                  struct slcli_slice *out) {
  const unsigned char *p;
  uint32_t bytes;
  if (!out || !slcli_u32(c, &bytes) || bytes > maximum || (!empty && !bytes) ||
      !slcli_take(c, bytes, &p) || !slcli_utf8(p, bytes, 0)) return fail(c);
  out->raw = p;
  out->bytes = bytes;
  return 1;
}
int slcli_cursor_end(struct slcli_cursor *c) {
  if (!c || c->failed || c->offset != c->bytes) return fail(c);
  return 1;
}
void slcli_put_u32(unsigned char out[4], uint32_t v) {
  unsigned i;
  for (i = 0; i < 4; ++i) out[i] = (unsigned char)(v >> (24 - 8*i));
}
void slcli_put_u64(unsigned char out[8], uint64_t v) {
  unsigned i;
  for (i = 0; i < 8; ++i) out[i] = (unsigned char)(v >> (56 - 8*i));
}
