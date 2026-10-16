#ifndef SLCLI_LEDGER_STATE_H
#define SLCLI_LEDGER_STATE_H
#include "crypto.h"
#include "wire.h"
#define SLCLI_LEDGER_LIMIT 262144u
#define SLCLI_STORE_LIMIT 4194304u
#define SLCLI_BINDINGS 16u
enum slcli_slot_state { SLCLI_FREE, SLCLI_RESERVED, SLCLI_LIVE,
  SLCLI_CLOSE_PENDING, SLCLI_RETAINED, SLCLI_QUARANTINED };
/* Decoded source-owned cells, not an actor/source/key admission. Common profile,
 * epoch and key-version originals are retained once; native owner text remains
 * borrowed from the original admitted user catalogue. Serial owner only. */
struct slcli_ledger_slot {
  unsigned char reservation[32],primary[32],resource[32],closure[32];
  unsigned char observer[32],intent[32],observation[32];
  struct slcli_slice owner;
  uint64_t generation,created,retained,intent_generation,intent_tick,deadline;
  uint32_t state,observation_bytes;
  uint32_t closure_present,intent_present,observation_present;
};
struct slcli_terminal_cell {
  unsigned char reservation[32],intent[32],closure[32],ledger[32];
  unsigned char receipt[32],receipt_ref[32],ack[32];
  uint64_t generation;
  uint32_t occupied,disposition,intent_present,ready,ack_present;
};
struct slcli_ledger_state {
  unsigned char host[32],epoch[32],profile[32],previous[32];
  struct slcli_slice key_version;
  uint64_t generation;
  uint32_t previous_present;
  struct slcli_ledger_slot slots[SLCLI_BINDINGS];
  struct slcli_terminal_cell terminal[SLCLI_BINDINGS];
};
/* The sink receives actual canonical bytes in source order. It must preserve
 * its original precharged body/capture/native effects; successful serializer
 * completion does not establish flush, EOF, store admission or custody. */
typedef int (*slcli_ledger_sink)(void *,const unsigned char *,size_t);
int slcli_ledger_state_valid(const struct slcli_ledger_state *);
int slcli_ledger_payload(const struct slcli_ledger_state *,slcli_ledger_sink,
                          void *,uint32_t *,unsigned char[32]);
int slcli_ledger_frame(const struct slcli_ledger_state *,const unsigned char[32],
                        slcli_ledger_sink,void *,uint32_t *,unsigned char[32]);
int slcli_ledger_selector(unsigned char[74],uint8_t,uint64_t,
                           const unsigned char[32],const unsigned char[32]);
int slcli_ledger_selector_read(const unsigned char[74],const unsigned char[32],
                                uint8_t *,uint64_t *,unsigned char[32]);
#endif
