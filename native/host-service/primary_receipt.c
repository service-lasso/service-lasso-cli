#include "primary_receipt.h"
#include "host_frame_body.h"
#if defined(_WIN32) || defined(__linux__)
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int ready(const struct slcli_primary_receipt *c,const struct slcli_stream_capture *r) {
  return c&&r&&!c->failed&&!c->complete&&c->bytes<90&&c->retention&&
    c->retention->started&&!c->retention->failed&&!c->retention->sealed&&c->retention->count==c->bytes&&
    c->retention->original!=c->original&&c->capture_sink&&c->capture_original&&
    separate(c,sizeof(*c),r,sizeof(*r))&&
    separate(c,sizeof(*c),c->retention,sizeof(*c->retention))&&
    separate(r,sizeof(*r),c->retention,sizeof(*c->retention));
}
static size_t needed(const struct slcli_primary_receipt *c) {
  return c->bytes<4?4-c->bytes:90-c->bytes;
}
static int accept(struct slcli_primary_receipt *c,struct slcli_stream_capture *r) {
  size_t n=r->count_observed&&r->returned<=r->requested?r->returned:0;
  if(!ready(c,r)||r->original!=c->original||r->buffer!=c->raw+c->bytes||r->requested!=needed(c))return 0;
  if(!c->capture_sink(c->capture_original,r,r->buffer,n))goto failed;
  if(r->pending){c->pending_original=r;if(r->failed)goto failed;return 1;}
  if(c->pending_original&&c->pending_original!=r)goto failed;c->pending_original=0;
  if(!r->complete||r->failed||!r->count_observed||!n||n>needed(c)||
     !slcli_evidence_append(c->retention,c->raw+c->bytes,n))goto failed;
  c->bytes+=(uint32_t)n;
  if(c->bytes>=4&&(c->raw[0]||c->raw[1]||c->raw[2]||c->raw[3]!=54))goto failed;
  if(c->bytes==90){if(!slcli_evidence_flush_readback(c->retention))goto failed;c->complete=1;}
  return 1;
failed:c->failed=1;return 0;
}
int slcli_primary_receipt_read(struct slcli_primary_receipt *c,struct slcli_stream_capture *r,slcli_input_handle event) {
  int ok;if(!ready(c,r)||r->started||c->pending_original)return 0;
  ok=slcli_stream_read_original(r,c->original,c->raw+c->bytes,needed(c),event);
  if(!r->started){c->failed=1;return 0;}if(!accept(c,r))return 0;return ok;
}
#if defined(_WIN32)
int slcli_primary_receipt_complete(struct slcli_primary_receipt *c,struct slcli_stream_capture *r) {
  int ok;if(!ready(c,r)||c->pending_original!=r||!r->pending)return 0;
  ok=slcli_stream_read_complete(r);if(!accept(c,r))return 0;return ok;
}
#endif
int slcli_primary_receipt_consume(struct slcli_primary_receipt *c,const struct slcli_primary_intent_original *o) {
  struct slcli_cursor cursor;uint8_t op,status;uint32_t seq;
  uint64_t generation,tick;const unsigned char *intent;
  if(!c||!o||c->failed||!c->complete||c->consumed||c->pending_original||
     !c->retention||!c->retention->sealed||c->retention->failed||c->bytes!=90||c->retention->count!=90||
     !o->generation||!o->sequence||o->sequence>1025||!o->deadline||
     !separate(c,sizeof(*c),o,sizeof(*o))||
     !slcli_equal(c->primary,o->primary,32)||
     !slcli_equal(c->nonce,o->nonce,32)||!slcli_equal(c->capability,o->capability,32))return 0;
  c->consumed=1;
  if(!slcli_native_clock_read(&c->consume_clock)||c->consume_clock.tick>=o->deadline)goto failed;
  if(!slcli_host_frame_verify_body(1,c->nonce,c->capability,o->sequence,c->raw+4,54,c->raw+58))goto failed;
  cursor.raw=c->raw+4;cursor.bytes=54;cursor.offset=0;cursor.failed=0;
  if(!slcli_u8(&cursor,&op)||op!=6||!slcli_u32(&cursor,&seq)||seq!=o->sequence||
     !slcli_u8(&cursor,&status)||status||!slcli_u64(&cursor,&generation)||
     !slcli_take(&cursor,32,&intent)||!slcli_u64(&cursor,&tick)||!slcli_cursor_end(&cursor)||
     generation!=o->generation||!tick||tick>=o->deadline||tick>c->consume_clock.tick||!slcli_equal(intent,o->intent,32))goto failed;
  c->accepted_tick=tick;
  return 1;
failed:c->failed=1;return 0;
}
#endif
