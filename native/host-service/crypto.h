#ifndef SLCLI_CRYPTO_H
#define SLCLI_CRYPTO_H
#include <stddef.h>
#include <stdint.h>
struct slcli_sha256 {
  uint32_t state[8];
  unsigned char block[64];
  uint64_t bytes;
  size_t used;
  uint32_t finished, failed;
};
struct slcli_hmac256 { struct slcli_sha256 inner; unsigned char outer[64]; };
void slcli_erase(void *, size_t);
int slcli_equal(const unsigned char *, const unsigned char *, size_t);
void slcli_sha256_init(struct slcli_sha256 *);
int slcli_sha256_update(struct slcli_sha256 *, const void *, size_t);
int slcli_sha256_finish(struct slcli_sha256 *, unsigned char[32]);
int slcli_hmac256_init(struct slcli_hmac256 *, const unsigned char[32]);
int slcli_hmac256_update(struct slcli_hmac256 *, const void *, size_t);
int slcli_hmac256_finish(struct slcli_hmac256 *, unsigned char[32]);
#endif
