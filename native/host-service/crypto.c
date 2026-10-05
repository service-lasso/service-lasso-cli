#include "crypto.h"

/* C11 port of the retained source-owned library-client SHA256/HMAC primitive.
 * Digest/MAC comparison supplies integrity inside admitted originals; it never
 * creates provider/key/profile/image/source admission. All state is fixed. */
void slcli_erase(void *value, size_t bytes) {
  volatile unsigned char *p = (volatile unsigned char *)value;
  while (bytes--) *p++ = 0;
}
int slcli_equal(const unsigned char *a, const unsigned char *b, size_t count) {
  unsigned char difference = 0;
  size_t i;
  if ((!a || !b) && count) return 0;
  for (i = 0; i < count; ++i) difference |= a[i] ^ b[i];
  return difference == 0;
}
static uint32_t rotate(uint32_t v, unsigned bits) {
  return (v >> bits) | (v << (32 - bits));
}
static void compress(struct slcli_sha256 *s, const unsigned char block[64]) {
  static const uint32_t k[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
  };
  uint32_t w[64], a, b, c, d, e, f, g, h;
  unsigned i;
  for (i = 0; i < 16; ++i)
    w[i] = ((uint32_t)block[4*i] << 24) | ((uint32_t)block[4*i+1] << 16) |
           ((uint32_t)block[4*i+2] << 8) | block[4*i+3];
  for (i = 16; i < 64; ++i) {
    uint32_t x = w[i-15], y = w[i-2];
    w[i] = w[i-16] + (rotate(x,7)^rotate(x,18)^(x>>3)) + w[i-7] +
           (rotate(y,17)^rotate(y,19)^(y>>10));
  }
  a=s->state[0]; b=s->state[1]; c=s->state[2]; d=s->state[3];
  e=s->state[4]; f=s->state[5]; g=s->state[6]; h=s->state[7];
  for (i = 0; i < 64; ++i) {
    uint32_t one = h + (rotate(e,6)^rotate(e,11)^rotate(e,25)) +
                   ((e&f)^((~e)&g)) + k[i] + w[i];
    uint32_t two = (rotate(a,2)^rotate(a,13)^rotate(a,22)) +
                   ((a&b)^(a&c)^(b&c));
    h=g; g=f; f=e; e=d+one; d=c; c=b; b=a; a=one+two;
  }
  s->state[0]+=a; s->state[1]+=b; s->state[2]+=c; s->state[3]+=d;
  s->state[4]+=e; s->state[5]+=f; s->state[6]+=g; s->state[7]+=h;
  slcli_erase(w, sizeof(w));
}
void slcli_sha256_init(struct slcli_sha256 *s) {
  if (!s) return;
  slcli_erase(s, sizeof(*s));
  s->state[0]=0x6a09e667; s->state[1]=0xbb67ae85;
  s->state[2]=0x3c6ef372; s->state[3]=0xa54ff53a;
  s->state[4]=0x510e527f; s->state[5]=0x9b05688c;
  s->state[6]=0x1f83d9ab; s->state[7]=0x5be0cd19;
}
int slcli_sha256_update(struct slcli_sha256 *s, const void *input, size_t count) {
  const unsigned char *p = (const unsigned char *)input;
  if (!s || s->failed || s->finished || s->used >= sizeof(s->block) || (!input && count) ||
      s->bytes > UINT64_MAX/8 || count > UINT64_MAX/8 - s->bytes) {
    if (s) s->failed = 1;
    return 0;
  }
  s->bytes += count;
  while (count--) {
    s->block[s->used++] = *p++;
    if (s->used == 64) { compress(s, s->block); s->used = 0; }
  }
  return 1;
}
int slcli_sha256_finish(struct slcli_sha256 *s, unsigned char out[32]) {
  uint64_t bits;
  unsigned i, j;
  if (!s || !out || s->failed || s->finished || s->used >= 64) return 0;
  bits = s->bytes * 8;
  s->block[s->used++] = 0x80;
  if (s->used > 56) {
    while (s->used < 64) s->block[s->used++] = 0;
    compress(s, s->block);
    s->used = 0;
  }
  while (s->used < 56) s->block[s->used++] = 0;
  for (i = 0; i < 8; ++i) s->block[56+i] = (unsigned char)(bits >> (56-8*i));
  compress(s, s->block);
  for (i = 0; i < 8; ++i) for (j = 0; j < 4; ++j)
    out[4*i+j] = (unsigned char)(s->state[i] >> (24-8*j));
  slcli_erase(s->block, sizeof(s->block));
  slcli_erase(s->state, sizeof(s->state));
  s->finished = 1;
  return 1;
}
int slcli_hmac256_init(struct slcli_hmac256 *h, const unsigned char key[32]) {
  unsigned char pad[64];
  unsigned i;
  if (!h || !key) return 0;
  slcli_sha256_init(&h->inner);
  for (i = 0; i < 64; ++i) {
    unsigned char v = i < 32 ? key[i] : 0;
    pad[i] = v ^ 0x36;
    h->outer[i] = v ^ 0x5c;
  }
  i = (unsigned)slcli_sha256_update(&h->inner, pad, sizeof(pad));
  slcli_erase(pad, sizeof(pad));
  return (int)i;
}
int slcli_hmac256_update(struct slcli_hmac256 *h, const void *p, size_t n) {
  return h && slcli_sha256_update(&h->inner, p, n);
}
int slcli_hmac256_finish(struct slcli_hmac256 *h, unsigned char out[32]) {
  struct slcli_sha256 outer;
  unsigned char inside[32];
  int ok;
  if (!h || !out) return 0;
  if (!slcli_sha256_finish(&h->inner, inside)) {
    slcli_erase(h->outer, sizeof(h->outer));
    return 0;
  }
  slcli_sha256_init(&outer);
  ok = slcli_sha256_update(&outer, h->outer, sizeof(h->outer)) &&
       slcli_sha256_update(&outer, inside, sizeof(inside)) &&
       slcli_sha256_finish(&outer, out);
  slcli_erase(inside, sizeof(inside));
  slcli_erase(h->outer, sizeof(h->outer));
  slcli_erase(&outer, sizeof(outer));
  return ok;
}
