#ifndef SLCLI_FRAME_H
#define SLCLI_FRAME_H
#include "crypto.h"
#define SLCLI_FRAME_BYTES 16384u
enum slcli_frame_domain { SLCLI_HOST, SLCLI_LAUNCH, SLCLI_INSPECTION_SEA,
  SLCLI_INSPECTION_PRIMARY, SLCLI_INSPECTION_LIBRARY };
struct slcli_frame { unsigned char body[SLCLI_FRAME_BYTES]; uint32_t length; };
/* Internal fixed cryptographic framing, never a peer/key admission constructor.
 * Native transport must retain original read/write frames and own these buffers
 * inside its charged original binding before command dispatch. */
int slcli_frame_mac(enum slcli_frame_domain, uint8_t, const unsigned char[32],
                     const unsigned char[32], uint32_t,
                     const struct slcli_frame *, unsigned char[32]);
int slcli_frame_verify(enum slcli_frame_domain, uint8_t, const unsigned char[32],
                        const unsigned char[32], uint32_t,
                        const struct slcli_frame *, const unsigned char[32]);
#endif
