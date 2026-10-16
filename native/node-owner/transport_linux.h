#ifndef SLCLI_NODE_TRANSPORT_LINUX_H
#define SLCLI_NODE_TRANSPORT_LINUX_H
#include "bootstrap_transport.h"
#include "../host-service/source_deadline.h"
#include "../host-service/preentry_backing_linux.h"
#if defined(__linux__)
/* Distinct immutable ingress object, never the writable Node runtime memfd.
 * The containing native parent owns this complete book and arena lifetime.
 * Native object/source/private-rights qualification is independent of this
 * fixed producer's byte/seal correspondence. No close/retry is implicit. */
struct slcli_node_transport_linux {
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin,*predecessor;
  const struct slcli_native_clock *publication_predecessor;
  const void *caller;
  const struct slcli_node_bootstrap_transport *transport_original;
  uint64_t expires,origin_tick,predecessor_tick,physical_bytes;
  size_t caller_bytes,compared;
  unsigned char *encoded,*readback;
  struct slcli_extent_request request;
  struct slcli_native_clock constructor_before[4];
  struct slcli_preentry_backing_call calls[7];
  struct stat identity_original;
  int descriptor,encode_result,prepare_result,backing_result,retained_result;
  uint32_t started,encode_returned,prepare_returned,backing_returned;
  uint32_t retained_returned,returned,terminal,bytes_equal,seals_observed;
  uint32_t object_prepared,publication_attempted;
};
/* Complete producer control-book + two actual arena block charges. Caller,
 * input transport, transitive stack/code and physical ingress remain separate
 * terms in the genuine containing-parent profile; this is no fit assertion. */
size_t slcli_node_transport_linux_control_bytes(void);
int slcli_node_transport_linux_create(struct slcli_node_transport_linux *,
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,
  size_t,const struct slcli_native_clock *);
/* The existing launcher inherits descriptor after create, retains actual birth,
 * then publishes once before release. No future birth is encoded prebirth. */
int slcli_node_transport_linux_publish(struct slcli_node_transport_linux *,
  struct slcli_arena *,const struct slcli_native_clock *,const struct slcli_node_bootstrap_transport *);
#endif
#endif
