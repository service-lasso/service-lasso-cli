#ifndef SLCLI_MEMORY_CAPTURE_VECTORS_H
#define SLCLI_MEMORY_CAPTURE_VECTORS_H
#include "platform_memory.h"
int slcli_memory_capture_vectors(struct slcli_memory_owner *,uint32_t);
/* Actual native owner/acquisition caller. Constructor, source/native authority,
 * original deadline/capture archival and every scratch/stack extent remain
 * independent required qualification. SOURCE-UNRUN. */
int slcli_memory_acquire_positive_original(struct slcli_memory_owner *,uint32_t,
  uint64_t,struct slcli_memory_capture *,unsigned char *,size_t);
#endif