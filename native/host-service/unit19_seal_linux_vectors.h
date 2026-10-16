#ifndef SLCLI_UNIT19_SEAL_LINUX_VECTORS_H
#define SLCLI_UNIT19_SEAL_LINUX_VECTORS_H
#if defined(__linux__)
#include "unit19_seal_linux.h"
#include "unit19_postseal_linux.h"
/* Actual native positive call on independently admitted fixture originals.
 * This is UNRUN source: no synthetic fd, seal, clock or passing certificate.
 * Genuine native enrollment/readonly acquisition and before-state capture
 * remain obligations of the owning complete fixture driver. */
int slcli_unit19_seal_linux_positive_vector(struct slcli_unit19_seal_linux *,
  struct slcli_unit19_native *,int,uint32_t,
  const struct slcli_source_deadline_original *,const void *,
  slcli_store_capture_sink,void *);
/* Checks an actually observed failed native enable, including unsupported or
 * writable-alias refusal. It does not manufacture the kernel cause or execute
 * the missing enrolled driver. Any preceding creator close remains a fact. */
int slcli_unit19_seal_linux_failed_enable_vector(const struct slcli_unit19_seal_linux *);
/* Genuine separate native full-byte/EOF read after persistent sealing. */
int slcli_unit19_postseal_linux_positive_vector(struct slcli_unit19_postseal_linux *,
  const struct slcli_unit19_seal_linux *,const struct slcli_unit19_framing *,
  const unsigned char *const *,const struct slcli_source_deadline_original *,
  const void *,slcli_store_capture_sink,void *);
/* A separately genuine arena-live later budget cannot renew this operation.
 * Uses fresh preallocated output and real retained original predecessor. */
int slcli_unit19_postseal_linux_no_renewal_vector(struct slcli_unit19_postseal_linux *,
  const struct slcli_unit19_postseal_linux *,
  const struct slcli_unit19_seal_linux *,const struct slcli_unit19_framing *,
  const unsigned char *const *,const struct slcli_source_deadline_original *,
  const void *,slcli_store_capture_sink,void *);
#endif
#endif
