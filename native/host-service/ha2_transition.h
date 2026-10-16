#ifndef SLCLI_HA2_TRANSITION_H
#define SLCLI_HA2_TRANSITION_H
#include "ha2_operation.h"
#include "ha2_journal_codec.h"
/* Full raw sequence/event comparison. This is deliberately separate from the
 * native before-effect snapshot's eighteen independently admitted originals,
 * pair durability, anchor verification and SAME high-water publication. A
 * successful comparison alone permits no lease, borrow, activation or revoke. */
int slcli_ha2_transition_compare(const unsigned char *,size_t,
  const unsigned char *,size_t,const unsigned char *,size_t);
#endif
