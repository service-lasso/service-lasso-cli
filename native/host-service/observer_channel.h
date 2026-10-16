#ifndef SLCLI_OBSERVER_CHANNEL_H
#define SLCLI_OBSERVER_CHANNEL_H
#include "service_channel.h"
#include "observer_wire.h"
#if defined(_WIN32) || defined(__linux__)
/* Original observer receives terminal183; original service receives ACK109.
 * Separately admitted original channel/source/capability/retention ownership is
 * mandatory. This owning IO body does not construct it or grant archive reuse. */
struct slcli_observer_channel_input {
  unsigned char raw[219],nonce[32],capability[32],observer[32];
  slcli_input_handle original;
  struct slcli_evidence_file *retention;
  slcli_channel_capture_sink capture_sink;
  void *capture_original;
  struct slcli_stream_capture *pending_original;
  struct slcli_native_clock consume_clock;
  uint64_t deadline,previous_tick;
  uint32_t operation,bytes,complete,consumed,failed;
};
int slcli_observer_channel_read(struct slcli_observer_channel_input *,
  struct slcli_stream_capture *,slcli_input_handle);
#if defined(_WIN32)
int slcli_observer_channel_complete(struct slcli_observer_channel_input *,struct slcli_stream_capture *);
#endif
/* Parsed outputs borrow SAME retained native bytes. They are grammar and MAC
 * observations only: raw digest equality cannot admit a native observer,
 * verify the complete original archive or stand in for physical closure. */
int slcli_observer_channel_terminal(struct slcli_observer_channel_input *,struct slcli_observer_terminal *);
int slcli_observer_channel_ack(struct slcli_observer_channel_input *,struct slcli_observer_ack *);
#endif
#endif
