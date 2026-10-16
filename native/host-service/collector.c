#include "collector.h"
#if defined(_WIN32) || defined(__linux__)
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int ready(struct slcli_collector *c,const struct slcli_stream_capture *r) {
  return c&&r&&!c->failed&&!c->eof&&c->data_file&&c->native_file&&
    c->data_file!=c->native_file&&c->data_file->original!=c->native_file->original&&
    c->data_file->original!=c->original&&c->native_file->original!=c->original&&
    c->native_sink&&c->native_original&&c->maximum&&c->count<=c->maximum&&c->deadline&&
    separate(c,sizeof(*c),r,sizeof(*r))&&
    separate(c,sizeof(*c),c->data_file,sizeof(*c->data_file))&&
    separate(c,sizeof(*c),c->native_file,sizeof(*c->native_file));
}
static int accept(struct slcli_collector *c,struct slcli_stream_capture *r) {
  struct slcli_primitive p={0};unsigned char length[4];uint64_t tick;size_t bytes;unsigned i;
  if(!ready(c,r)||r->original!=c->original||r->buffer!=c->buffer||
     r->requested!=sizeof(c->buffer))return 0;
  bytes=r->count_observed&&r->returned<=sizeof(c->buffer)?r->returned:0;
  if(c->pending_original&&c->pending_original!=r){c->failed=1;return 0;}
  if(r->pending)c->pending_original=r;
  if(!slcli_stream_transfer_original(r,c->native_sink,c->native_original,c->buffer,bytes)){c->failed=1;return 0;}
  if(r->pending){c->pending_original=r;if(r->failed){c->failed=1;return 0;}return 1;}
  if(c->pending_original&&c->pending_original!=r){c->failed=1;return 0;}
  c->pending_original=0;
  tick=r->clock.complete?r->clock.tick:0;
#if defined(_WIN32)
  if(r->completion_observed)tick=r->completion_clock.complete?r->completion_clock.tick:0;
#endif
  if(!tick||tick>=c->deadline){c->failed=1;return 0;}
  p.kind=r->complete&&!r->failed&&r->count_observed&&!r->returned?SLCLI_EOF:SLCLI_READ;
#if defined(_WIN32)
  p.operation=SLCLI_WIN_READ;
#else
  p.operation=SLCLI_LINUX_READ;
#endif
  p.tick=tick;p.outcome=r->complete&&!r->failed?0:2;
  for(i=0;i<32;i++){p.identity[i]=p.related[i]=c->identity[i];p.source[i]=c->source[i];}
  p.data=r->count_observed?c->buffer:0;p.bytes=bytes;
  c->primitive_bytes=(uint32_t)slcli_primitive_into(c->primitive,sizeof(c->primitive),&p);
  if(!c->primitive_bytes){c->failed=1;return 0;}slcli_put_u32(length,c->primitive_bytes);
  if(!slcli_evidence_append(c->native_file,length,4)||
     !slcli_evidence_append(c->native_file,c->primitive,c->primitive_bytes)) {c->failed=1;return 0;}
  if(!r->complete||r->failed||!r->count_observed){c->failed=1;return 0;}
  if(!bytes) {
    struct slcli_sha256 h;
    if(!r->eof||!slcli_evidence_flush_readback(c->data_file)||
       !slcli_evidence_flush_readback(c->native_file)){c->failed=1;return 0;}
    slcli_sha256_init(&h);
    if(!slcli_sha256_update(&h,c->primitive,c->primitive_bytes)||
       !slcli_sha256_finish(&h,c->eof_hash)){c->failed=1;return 0;}
    c->eof_original=r;c->eof=1;return 1;
  }
  if(bytes>c->maximum-c->count||!slcli_evidence_append(c->data_file,c->buffer,bytes)||
     (c->forward&&(!c->forward_original||!c->forward(c->forward_original,c->buffer,bytes)))) {
    c->failed=1;return 0;
  }
  c->count+=bytes;return 1;
}
int slcli_collector_recheck_eof(struct slcli_collector *c) {
  struct slcli_sha256 h;unsigned char digest[32];const struct slcli_stream_capture *r;
  if(!c||c->failed||!c->eof||c->pending_original||!(r=c->eof_original)||
    !r->started||!r->observed||!r->complete||r->pending||r->failed||!r->eof||
    !r->count_observed||r->returned||r->original!=c->original||r->buffer!=c->buffer||
    !c->primitive_bytes||c->primitive_bytes>sizeof(c->primitive)||!c->data_file||!c->native_file||
    c->data_file==c->native_file||!c->data_file->sealed||!c->native_file->sealed||
    c->data_file->count!=c->count||!c->native_file->count)return 0;
  c->failed=1;slcli_sha256_init(&h);
  if(!slcli_sha256_update(&h,c->primitive,c->primitive_bytes)||!slcli_sha256_finish(&h,digest)||
    !slcli_equal(digest,c->eof_hash,32)||!slcli_evidence_recheck_readback(c->native_file)||
    !slcli_evidence_recheck_readback(c->data_file))return 0;
  c->failed=0;return 1;
}
int slcli_collector_read(struct slcli_collector *c,struct slcli_stream_capture *r,slcli_input_handle event) {
  int ok;
  /* Pending OVERLAPPED input owns this buffer until its SAME actual completion
   * and capture transfer. A second read may not overwrite the borrowed bytes. */
  if(!ready(c,r)||r->started||c->pending_original||c->reads>=1025)return 0;c->reads++;
  ok=slcli_stream_read_original(r,c->original,c->buffer,sizeof(c->buffer),event);
  if(!r->started){c->failed=1;return 0;}
  if(!accept(c,r))return 0;
  return ok;
}
#if defined(_WIN32)
int slcli_collector_complete(struct slcli_collector *c,struct slcli_stream_capture *r) {
  int ok;
  if(c&&r&&c->failed&&c->pending_original==r&&r->started&&r->pending&&
     r->original==c->original&&r->buffer==c->buffer&&r->requested==sizeof(c->buffer)) {
    size_t bytes;
    slcli_stream_read_complete(r);
    bytes=r->count_observed&&r->returned<=r->requested?r->returned:0;
    slcli_stream_transfer_original(r,c->native_sink,c->native_original,c->buffer,bytes);
    return 0; /* Original pending bytes stay retained; no EOF/FREE publication. */
  }
  if(!ready(c,r)||c->pending_original!=r||r->original!=c->original||
     r->buffer!=c->buffer||!r->pending)return 0;
  ok=slcli_stream_read_complete(r);
  if(!accept(c,r))return 0;return ok;
}
#endif
#endif
