#ifndef SLCLI_INSPECTION_H
#define SLCLI_INSPECTION_H
#include "frame.h"
#include "wire.h"
/* Streaming validation preserves full selected record/chunk bounds. Actual
 * native source capture and genuine EOF remain separate owning obligations. */
struct slcli_inspection {
  unsigned char run[32], nonce[32], receipt[32], catalog[32], digest[32];
  uint32_t length, count, total, offset, sequence, begun, ended, failed;
  struct slcli_sha256 record;
};
int slcli_inspection_frame(struct slcli_inspection *, const struct slcli_frame *,
                            struct slcli_slice *);
#endif
