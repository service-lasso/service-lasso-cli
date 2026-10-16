#include "observer_channel.h"
#include "host_frame_body.h"
#if defined(_WIN32) || defined(__linux__)
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static uint32_t body_bytes(const struct slcli_observer_channel_input *c) {
  return c->operation==9?183:c->operation==10?109:0;
}
static int ready(const struct slcli_observer_channel_input *c,const struct slcli_stream_capture *r) {
  uint32_t body;if(!c||!r||!(body=body_bytes(c)))return 0;
  return !c->failed&&!c->complete&&c->bytes<body+36&&c->deadline&&c->retention&&
    c->retention->started&&!c->retention->failed&&!c->retention->sealed&&
    c->retention->count==c->bytes&&c->retention->original!=c->original&&
    c->capture_sink&&c->capture_original&&separate(c,sizeof(*c),r,sizeof(*r))&&
    separate(c,sizeof(*c),c->retention,sizeof(*c->retention))&&
    separate(r,sizeof(*r),c->retention,sizeof(*c->retention));
}
static size_t needed(const struct slcli_observer_channel_input *c) {
  return c->bytes<4?4-c->bytes:body_bytes(c)+36-c->bytes;
}
static int accept(struct slcli_observer_channel_input *c,struct slcli_stream_capture *r) {
  size_t n=r->count_observed&&r->returned<=r->requested?r->returned:0;
  uint32_t body=body_bytes(c);uint64_t tick;
  if(!ready(c,r)||r->original!=c->original||r->buffer!=c->raw+c->bytes||r->requested!=needed(c))return 0;
  if(c->pending_original&&c->pending_original!=r)goto failed;
  if(r->pending)c->pending_original=r;
  if(!slcli_stream_transfer_original(r,c->capture_sink,c->capture_original,r->buffer,n))goto failed;
  if(c->pending_original&&c->pending_original!=r)goto failed;
  if(r->pending)c->pending_original=r; /* Retain native custody even if late. */
  /* Raw native results/body reach custody before fallible timing gates. The
   * SAME pending row's completion must follow its retained initial clock. */
  if(!r->clock.complete||!r->clock.tick)goto failed;
  tick=r->clock.tick;
#if defined(_WIN32)
  if(r->completion_observed) {
    if(!r->completion_clock.complete||r->completion_clock.tick<tick)goto failed;
    tick=r->completion_clock.tick;
  }
#endif
  if(tick<c->previous_tick)goto failed;
  c->previous_tick=tick; /* Genuine late return is retained, never erased. */
  if(tick>=c->deadline)goto failed;
  if(r->pending){if(r->failed)goto failed;return 1;}
  if(c->pending_original&&c->pending_original!=r)goto failed;c->pending_original=0;
  if(!r->complete||r->failed||!r->count_observed||!n||n>needed(c)||
    !slcli_evidence_append(c->retention,c->raw+c->bytes,n))goto failed;
  c->bytes+=(uint32_t)n;
  if(c->bytes>=4&&(c->raw[0]||c->raw[1]||c->raw[2]||c->raw[3]!=body))goto failed;
  if(c->bytes==body+36){if(!slcli_evidence_flush_readback(c->retention))goto failed;c->complete=1;}
  return 1;
failed:c->failed=1;return 0;
}
int slcli_observer_channel_read(struct slcli_observer_channel_input *c,struct slcli_stream_capture *r,
  slcli_input_handle event) {
  int ok;if(!ready(c,r)||r->started||c->pending_original)return 0;
  ok=slcli_stream_read_original(r,c->original,c->raw+c->bytes,needed(c),event);
  if(!r->started){c->failed=1;return 0;}if(!accept(c,r))return 0;return ok;
}
#if defined(_WIN32)
int slcli_observer_channel_complete(struct slcli_observer_channel_input *c,struct slcli_stream_capture *r) {
  int ok;
  if(c&&r&&c->failed&&body_bytes(c)&&c->bytes<body_bytes(c)+36&&
     c->pending_original==r&&r->started&&r->pending&&
     r->original==c->original&&r->buffer==c->raw+c->bytes&&r->requested==needed(c)) {
    size_t n;
    slcli_stream_read_complete(r);
    n=r->count_observed&&r->returned<=r->requested?r->returned:0;
    slcli_stream_transfer_original(r,c->capture_sink,c->capture_original,r->buffer,n);
    return 0; /* Custody completion cannot resume failed ACK/archive admission. */
  }
  if(!ready(c,r)||c->pending_original!=r||!r->pending)return 0;
  ok=slcli_stream_read_complete(r);if(!accept(c,r))return 0;return ok;
}
#endif
static int consume(struct slcli_observer_channel_input *c,unsigned operation,void *out,size_t out_bytes) {
  uint32_t body;if(!c||!out||c->operation!=operation||!(body=body_bytes(c))||
    c->failed||!c->complete||c->consumed||c->pending_original||!c->deadline||
    c->bytes!=body+36||!c->retention||c->retention->failed||!c->retention->sealed||
    c->retention->count!=c->bytes||!separate(c,sizeof(*c),out,out_bytes)||
    !separate(c->retention,sizeof(*c->retention),out,out_bytes)||
    c->retention->capture_capacity>SIZE_MAX/sizeof(*c->retention->captures)||
    !separate(c->retention->captures,c->retention->capture_capacity*sizeof(*c->retention->captures),out,out_bytes))return 0;
  c->consumed=1;
  if(!slcli_evidence_recheck_readback(c->retention)||!slcli_native_clock_read(&c->consume_clock)||
    c->consume_clock.tick<c->previous_tick||c->consume_clock.tick>=c->deadline||
    !slcli_host_frame_verify_body(operation==9?1:0,c->nonce,c->capability,1,c->raw+4,body,c->raw+4+body)) {
    c->failed=1;return 0;
  }
  return 1;
}
int slcli_observer_channel_terminal(struct slcli_observer_channel_input *c,struct slcli_observer_terminal *out) {
  if(!consume(c,9,out,sizeof(*out)))return 0;
  if(!slcli_observer_terminal_parse(c->raw+4,183,out)||
    !slcli_equal(out->observer,c->observer,32)||out->tick>=c->deadline||
    out->tick>c->consume_clock.tick){c->failed=1;return 0;}return 1;
}
int slcli_observer_channel_ack(struct slcli_observer_channel_input *c,struct slcli_observer_ack *out) {
  if(!consume(c,10,out,sizeof(*out)))return 0;
  if(!slcli_observer_ack_parse(c->raw+4,109,out)){c->failed=1;return 0;}return 1;
}
#endif
