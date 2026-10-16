#ifndef SLCLI_HA2_OPERATION_H
#define SLCLI_HA2_OPERATION_H
#include "json.h"
enum slcli_ha2_event {
  SLCLI_HA2_INSTALL_PREPARE=1,SLCLI_HA2_ACTIVATE,SLCLI_HA2_LEASE_BIRTH,
  SLCLI_HA2_LEDGER_BIRTH,SLCLI_HA2_BORROW_OPEN,SLCLI_HA2_BORROW_CLOSE,
  SLCLI_HA2_ARCHIVE,SLCLI_HA2_NEXT_START_PREPARE,SLCLI_HA2_REVOKE_BEGIN,
  SLCLI_HA2_REVOKE_COMPLETE,SLCLI_HA2_QUARANTINE
};
struct slcli_ha2_operation {
  const unsigned char *raw;
  size_t bytes;
  uint64_t sequence,generation;
  uint32_t event;
  unsigned char caller[32],source[32],birth[32],frontier[32],prior_request[32],prior_record[32];
};
/* Private operation-range representation only. The independently held source
 * original selects this range/event before construction; incoming public JSON
 * must never be passed to this decoder as a source-owned operation. The native
 * snapshot, original admission and authentic high-water remain separate gates. */
int slcli_ha2_operation_parse(const unsigned char *,size_t,struct slcli_ha2_operation *);
#endif
