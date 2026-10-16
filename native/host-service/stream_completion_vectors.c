#include "stream_completion_vectors.h"
#if defined(_WIN32)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_stream_completion_positive_original(struct slcli_stream_capture *c,
  const unsigned char *expected,size_t bytes,unsigned char *scratch,size_t scratch_bytes) {
  slcli_input_handle original;unsigned char *buffer;size_t requested,i;
  HANDLE event;
  if(!c||!c->started||c->failed||!c->pending||c->complete||c->completion_observed||
    !c->clock.complete||!bytes||bytes>c->requested||scratch_bytes<sizeof(*c)||
    !away(c,sizeof(*c),expected,bytes)||!away(c,sizeof(*c),scratch,sizeof(*c))||
    !away(c->buffer,c->requested,expected,bytes)||
    !away(c->buffer,c->requested,scratch,sizeof(*c))||
    !away(scratch,sizeof(*c),expected,bytes))return 0;
  original=c->original;buffer=c->buffer;requested=c->requested;event=c->overlapped.hEvent;
  if(!slcli_stream_read_complete(c)||!c->completion_status||!c->completion_observed||
    !c->count_observed||c->returned!=bytes||c->completion_count!=bytes||
    c->pending||!c->complete||c->failed||c->original!=original||c->buffer!=buffer||
    c->requested!=requested||c->overlapped.hEvent!=event||
    !c->completion_clock.complete||c->completion_clock.tick<c->clock.tick||
    !slcli_equal(buffer,expected,bytes))return 0;
  for(i=0;i<sizeof(*c);i++)scratch[i]=((const unsigned char *)c)[i];
  if(slcli_stream_read_complete(c))return 0;
  for(i=0;i<sizeof(*c);i++)if(scratch[i]!=((const unsigned char *)c)[i])return 0;
  return 1;
}
int slcli_stream_completion_clock_failure_retained(const struct slcli_stream_capture *c,
  const unsigned char *expected,size_t bytes) {
  if(!c||!expected||!bytes||bytes>c->requested||
    !away(c,sizeof(*c),expected,bytes)||!away(c->buffer,c->requested,expected,bytes))return 0;
  return c->started&&c->completion_observed&&c->completion_status&&
    c->complete&&!c->pending&&c->failed&&c->count_observed&&
    c->returned==bytes&&c->completion_count==bytes&&
    c->completion_clock.started&&!c->completion_clock.complete&&
    slcli_equal(c->buffer,expected,bytes);
}
#endif