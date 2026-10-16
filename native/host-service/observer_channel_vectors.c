#include "observer_channel_vectors.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_observer_ack_positive_original(struct slcli_observer_channel_input *c,
  const struct slcli_observer_ack *expected,struct slcli_observer_ack *out) {
  uint64_t prior,generation;const unsigned char *reservation,*receipt,*archive;
  if(!c||!expected||!out||c->operation!=10||c->failed||!c->complete||c->consumed||
    !c->previous_tick||c->previous_tick>=c->deadline||
    !away(c,sizeof(*c),expected,sizeof(*expected))||
    !away(c,sizeof(*c),out,sizeof(*out))||!away(expected,sizeof(*expected),out,sizeof(*out))||
    !away(c,sizeof(*c),expected->reservation,32)||!away(c,sizeof(*c),expected->receipt,32)||
    !away(c,sizeof(*c),expected->archive,32)||!away(out,sizeof(*out),expected->reservation,32)||
    !away(out,sizeof(*out),expected->receipt,32)||!away(out,sizeof(*out),expected->archive,32))return 0;
  prior=c->previous_tick;
  /* Invoke actual native consume clock and retained evidence readback before
   * checking independently supplied original archive/receipt expectations. */
  if(!slcli_observer_channel_ack(c,out)||c->failed||!c->consumed||
    !c->consume_clock.complete||c->consume_clock.tick<prior||
    c->consume_clock.tick>=c->deadline||out->generation!=expected->generation||
    !slcli_equal(out->reservation,expected->reservation,32)||
    !slcli_equal(out->receipt,expected->receipt,32)||!slcli_equal(out->archive,expected->archive,32))return 0;
  generation=out->generation;reservation=out->reservation;receipt=out->receipt;archive=out->archive;
  /* Consumed original cannot cause another clock/evidence read or overwrite
   * its borrowed parsed output. Archive equality alone is not authority. */
  return !slcli_observer_channel_ack(c,out)&&out->generation==generation&&
    out->reservation==reservation&&out->receipt==receipt&&out->archive==archive&&
    c->previous_tick==prior;
}
#endif
