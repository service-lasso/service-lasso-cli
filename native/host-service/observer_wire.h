#ifndef SLCLI_OBSERVER_WIRE_H
#define SLCLI_OBSERVER_WIRE_H
#include "frame.h"
#include "wire.h"
struct slcli_observer_ack {
  const unsigned char *reservation,*receipt,*archive;
  uint64_t generation;
};
struct slcli_observer_terminal {
  const unsigned char *reservation,*observer,*intent,*closure,*ledger;
  uint64_t generation,tick;
  uint8_t slot,disposition;
};
int slcli_observer_ack_parse(const unsigned char *,size_t,struct slcli_observer_ack *);
int slcli_observer_terminal_parse(const unsigned char *,size_t,struct slcli_observer_terminal *);
int slcli_observer_terminal_encode(unsigned char[183],const struct slcli_observer_terminal *);
#endif
