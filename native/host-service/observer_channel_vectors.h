#ifndef SLCLI_OBSERVER_CHANNEL_VECTORS_H
#define SLCLI_OBSERVER_CHANNEL_VECTORS_H
#include "observer_channel.h"
#if defined(_WIN32) || defined(__linux__)
/* Original fixture supplies authentic MAC/native receipt, retained archive
 * expectation and complete source/deadline/capture authority. SOURCE-UNRUN. */
int slcli_observer_ack_positive_original(struct slcli_observer_channel_input *,
  const struct slcli_observer_ack *,struct slcli_observer_ack *);
#endif
#endif
