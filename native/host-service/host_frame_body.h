#ifndef SLCLI_HOST_FRAME_BODY_H
#define SLCLI_HOST_FRAME_BODY_H
#include "frame.h"
/* SAME unchanged host frame MAC over borrowed original body bytes. A fixed
 * short reply does not need an uncharged/full resident16384-byte frame copy. */
int slcli_host_frame_verify_body(uint8_t,const unsigned char[32],const unsigned char[32],
                                  uint32_t,const unsigned char *,uint32_t,const unsigned char[32]);
#endif
