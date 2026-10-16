#include "primary_receipt_vectors.h"
#if defined(_WIN32) || defined(__linux__)
static uint64_t retained_tick(const struct slcli_primary_receipt *c) {
  uint64_t tick=0;size_t i;for(i=50;i<58;i++)tick=(tick<<8)|c->raw[i];return tick;
}
static int prepared(const struct slcli_primary_receipt *c) {
  return c&&!c->failed&&!c->consumed&&c->complete&&!c->pending_original&&
    c->bytes==90&&c->retention&&c->retention->sealed&&!c->retention->failed&&
    c->retention->count==90&&!c->consume_clock.started&&!c->accepted_tick;
}
int slcli_primary_receipt_positive_original(struct slcli_primary_receipt *c,
  const struct slcli_primary_intent_original *o) {
  uint64_t tick;uint32_t bytes;struct slcli_evidence_file *retention;
  if(!prepared(c)||!o)return 0;tick=retained_tick(c);bytes=c->bytes;retention=c->retention;
  if(!slcli_primary_receipt_consume(c,o)||!c->consumed||c->failed||
     !c->consume_clock.started||!c->consume_clock.observed||!c->consume_clock.complete||
     !tick||c->accepted_tick!=tick||tick>c->consume_clock.tick||
     c->consume_clock.tick>=o->deadline||c->bytes!=bytes||c->retention!=retention)return 0;
  /* A repeat must not obtain a fresh clock, consume another receipt or clear
   * retained lineage. Same completed native observation remains in place. */
  {struct slcli_native_clock before=c->consume_clock;
   if(slcli_primary_receipt_consume(c,o)||c->accepted_tick!=tick||c->failed||
      !slcli_equal((const unsigned char *)&before,(const unsigned char *)&c->consume_clock,sizeof(before)))return 0;}
  return 1;
}
int slcli_primary_receipt_future_original(struct slcli_primary_receipt *c,
  const struct slcli_primary_intent_original *o) {
  uint64_t tick;uint32_t bytes;struct slcli_evidence_file *retention;
  if(!prepared(c)||!o)return 0;tick=retained_tick(c);bytes=c->bytes;retention=c->retention;
  /* Fixture supplies an actual authenticated retained ACK whose sender tick
   * exceeds the subsequently observed consumer frontier. It must establish
   * valid MAC/body/intent lineage independently; this postcondition alone is
   * not proof that future time was the sole rejection cause. */
  if(slcli_primary_receipt_consume(c,o)||!c->consumed||!c->failed||
     !c->consume_clock.started||!c->consume_clock.observed||!c->consume_clock.complete||
     !tick||tick>=o->deadline||tick<=c->consume_clock.tick||
     c->consume_clock.tick>=o->deadline||c->accepted_tick||
     c->bytes!=bytes||c->retention!=retention||!retention->sealed)return 0;
  {struct slcli_native_clock before=c->consume_clock;
   if(slcli_primary_receipt_consume(c,o)||!c->failed||c->accepted_tick||
      !slcli_equal((const unsigned char *)&before,(const unsigned char *)&c->consume_clock,sizeof(before)))return 0;}
  return 1;
}
#endif