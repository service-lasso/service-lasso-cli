#ifndef SLCLI_PRIMARY_RECEIPT_VECTORS_H
#define SLCLI_PRIMARY_RECEIPT_VECTORS_H
#include "primary_receipt.h"
#if defined(_WIN32) || defined(__linux__)
/* Genuine native reader/retention/frame constructors and independently admitted
 * original intent supplied by fixture owner. No synthetic authenticated frame
 * or clock. These actual source callers are UNRUN before whole admission. */
int slcli_primary_receipt_positive_original(struct slcli_primary_receipt *,const struct slcli_primary_intent_original *);
int slcli_primary_receipt_future_original(struct slcli_primary_receipt *,const struct slcli_primary_intent_original *);
#endif
#endif