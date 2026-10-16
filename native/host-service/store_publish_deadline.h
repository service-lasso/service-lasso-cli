#ifndef SLCLI_STORE_PUBLISH_DEADLINE_H
#define SLCLI_STORE_PUBLISH_DEADLINE_H
#include "store_publish.h"
#include "store_deadline.h"
#if defined(_WIN32) || defined(__linux__)
/* Private original raw namespace custody, never an authority constructor.
 * Sixteen rows are two clocks for each actual Linux eight-effect publisher;
 * Windows uses its actual five-effect sequence. All storage remains charged. */
struct slcli_store_publish_deadline_owner {
  struct slcli_arena *arena;
  void *container_original;
  size_t container_bytes;
  struct slcli_store_publish *publish_original;
  struct slcli_store_deadline_owner *stream_original;
  slcli_namespace_capture_sink sink;
  void *capture_original;
  size_t capture_original_bytes;
  struct slcli_native_clock clocks[16];
  uint32_t kinds[8],count,pending,prepared,failed;
};
struct slcli_ledger_commit;
int slcli_store_publish_deadline_bind_ledger(struct slcli_ledger_commit *,
  slcli_namespace_capture_sink);
int slcli_store_publish_deadline_bind_artifact(struct slcli_store_publish *,
  struct slcli_store_deadline_owner *,slcli_namespace_capture_sink);
int slcli_store_publish_deadline_before(struct slcli_store_publish *,uint32_t);
int slcli_store_publish_deadline_after(struct slcli_store_publish *,int);
/* Absorbing source failure preserves every completed/pending native row. */
void slcli_store_publish_deadline_fail(struct slcli_store_publish *);
int slcli_store_publish_deadline_ready(const struct slcli_store_publish *);
#endif
#endif