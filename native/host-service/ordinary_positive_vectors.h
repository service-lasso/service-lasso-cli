#ifndef SLCLI_ORDINARY_POSITIVE_VECTORS_H
#define SLCLI_ORDINARY_POSITIVE_VECTORS_H
#include "service_channel.h"
#if defined(_WIN32) || defined(__linux__)
/* Actual native file/channel fixtures are independently source-admitted inputs.
 * These functions perform real effects when execution is later admitted. They
 * create no issuer/principal, mock native return, grant, profile or store owner.
 * Every supplied original and failed attempt stays retained. */
int slcli_positive_ledger_commit_vector(struct slcli_primary_engine *,
                                        const struct slcli_primary_commit_originals *);
/* Actual attempted native pair required. before is separately retained exact
 * original committed storage, not an expected freshly constructed ledger. */
int slcli_failed_pair_frontier_vector(const struct slcli_primary_engine *,
  const struct slcli_primary_commit_originals *,const struct slcli_ledger_state *,
  uint64_t,uint32_t,uint32_t,const unsigned char[32]);
/* Calls the ordinary pair method and its denied repeat. The original native
 * fixture must independently establish actual pair completion followed by a
 * late failure; this vector does not certify that failure's cause/latency. */
int slcli_late_pair_frontier_vector(struct slcli_primary_engine *,
  const struct slcli_primary_commit_originals *);
/* Independently admitted held stream/root/nonce required. Actual publisher
 * runs once; a repeat must perform no additional native step or clock. */
int slcli_positive_namespace_publish_vector(struct slcli_store_publish *,
  const struct slcli_store_stream *,slcli_input_handle,const unsigned char[16],uint32_t);
/* Existing genuine failed native publisher required, then actual repeat call.
 * This is no qualification of the original failure's cause or latency. */
int slcli_failed_namespace_repeat_vector(struct slcli_store_publish *,
  const struct slcli_store_stream *,slcli_input_handle,const unsigned char[16],uint32_t);
int slcli_positive_archive_copy_vector(struct slcli_archive_copy *,slcli_input_handle,
                                       slcli_input_handle,slcli_input_handle,uint64_t,
                                       const unsigned char[32],slcli_store_capture_sink,void *);
int slcli_positive_service_dispatch_vector(struct slcli_service_request *,struct slcli_primary_engine *,
                                           struct slcli_native_clock *,
                                           const struct slcli_primary_commit_originals *,
                                           uint8_t,uint64_t);
#endif
#endif
