#ifndef SLCLI_PRIMARY_RECEIPT_H
#define SLCLI_PRIMARY_RECEIPT_H
#include "service_channel.h"
#if defined(_WIN32) || defined(__linux__)
/* Actual owning PRIMARY reader, not the service's output-delivery marker.
 * Original source/birth/channel/capture/retention constructor is independently
 * admitted and remains unqualified. This body creates no actor or handle lease. */
struct slcli_primary_receipt {
  unsigned char raw[90],primary[32],nonce[32],capability[32];
  slcli_input_handle original;
  struct slcli_evidence_file *retention;
  slcli_channel_capture_sink capture_sink;
  void *capture_original;
  struct slcli_stream_capture *pending_original;
  uint32_t bytes,complete,failed,consumed;
  uint64_t accepted_tick;
  struct slcli_native_clock consume_clock;
};
/* SAME actual owning primary conversation state: original CLOSE_INTENT body
 * digest, last accepted generation/sequence and original absolute deadline.
 * These are internal source-owner inputs, not public request metadata or a
 * pointer into service memory across a process boundary. */
struct slcli_primary_intent_original {
  unsigned char primary[32],nonce[32],capability[32],intent[32];
  uint64_t generation,deadline;
  uint32_t sequence;
};
int slcli_primary_receipt_read(struct slcli_primary_receipt *,struct slcli_stream_capture *,slcli_input_handle);
#if defined(_WIN32)
int slcli_primary_receipt_complete(struct slcli_primary_receipt *,struct slcli_stream_capture *);
#endif
/* Consumes the completed original native read and its retained full readback.
 * MAC/body equality does not independently authenticate primary provenance. */
int slcli_primary_receipt_consume(struct slcli_primary_receipt *,const struct slcli_primary_intent_original *);
#endif
#endif
