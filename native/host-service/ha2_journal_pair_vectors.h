#ifndef SLCLI_HA2_JOURNAL_PAIR_VECTORS_H
#define SLCLI_HA2_JOURNAL_PAIR_VECTORS_H
#include "ha2_journal_pair.h"
#if defined(_WIN32) || defined(__linux__)
/* Actual admitted native prefix fixtures for the339th,340th and1024th
 * records. Separately created/authenticated prefixes and original pair/capture
 * owner are supplied; source code never fabricates completed native IO. */
int slcli_ha2_journal_boundary_positive_vector(struct slcli_ha2_journal_pair *,
  const unsigned char *const *,uint32_t,const unsigned char[772],
  slcli_input_handle,slcli_input_handle,uint64_t,slcli_store_capture_sink,void *);
/* Real boundary body under exact independently supplied original deadline;
 * no actor/capture/source authority is produced by this UNRUN driver. */
int slcli_ha2_journal_bound_boundary_positive_vector(struct slcli_ha2_journal_pair *,
  const unsigned char *const *,uint32_t,const unsigned char[772],
  slcli_input_handle,slcli_input_handle,uint64_t,slcli_store_capture_sink,void *,
  const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,size_t);
int slcli_ha2_journal_exhaustion_vector(struct slcli_ha2_journal_pair *,
  const unsigned char *const *,const unsigned char[772],
  slcli_input_handle,slcli_input_handle,uint64_t,slcli_store_capture_sink,void *);
#endif
#endif
