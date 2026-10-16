#include "channel_write_vectors.h"
#if defined(_WIN32)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
/* Precharged native fixture supplies SAME genuinely pending original operation,
 * authentic endpoint/body/source/clock/deadline/capture ownership and scratch.
 * Actual adapter is invoked, SOURCE-UNRUN; nothing creates a native original. */
int slcli_channel_write_completion_positive_original(struct slcli_channel_write_capture *c,
  size_t bytes,unsigned char *scratch,size_t scratch_bytes) {
  slcli_input_handle original;const unsigned char *body;size_t requested,i;HANDLE event;
  if(!c||!c->started||c->failed||!c->pending||c->complete||c->completion_observed||
    !c->clock.complete||!bytes||bytes>c->requested||scratch_bytes<sizeof(*c)||
    !away(c,sizeof(*c),scratch,sizeof(*c))||
    !away(c->body,c->requested,scratch,sizeof(*c)))return 0;
  original=c->original;body=c->body;requested=c->requested;event=c->overlapped.hEvent;
  if(!slcli_channel_write_complete(c)||!c->completion_status||!c->completion_observed||
    !c->count_observed||c->returned!=bytes||c->completion_count!=bytes||
    c->pending||!c->complete||c->failed||c->original!=original||c->body!=body||
    c->requested!=requested||c->overlapped.hEvent!=event||
    !c->completion_clock.complete||c->completion_clock.tick<c->clock.tick)return 0;
  for(i=0;i<sizeof(*c);i++)scratch[i]=((const unsigned char *)c)[i];
  if(slcli_channel_write_complete(c))return 0;
  for(i=0;i<sizeof(*c);i++)if(scratch[i]!=((const unsigned char *)c)[i])return 0;
  return 1;
}
int slcli_channel_write_clock_failure_retained(const struct slcli_channel_write_capture *c,size_t bytes) {
  return c&&bytes&&bytes<=c->requested&&c->started&&c->completion_observed&&
    c->completion_status&&c->complete&&!c->pending&&c->failed&&c->count_observed&&
    c->returned==bytes&&c->completion_count==bytes&&
    c->completion_clock.started&&!c->completion_clock.complete;
}
#endif
#if defined(_WIN32) || defined(__linux__)
int slcli_channel_write_observed_zero_retained(const struct slcli_channel_write_capture *c) {
  if(!c||!c->started||!c->requested||!c->complete||c->pending||!c->failed||
    !c->count_observed||c->returned)return 0;
#if defined(_WIN32)
  return (c->completion_observed&&c->completion_status&&!c->completion_count)||
    (!c->completion_observed&&c->observed&&c->initial_status&&!c->initial_count);
#else
  return c->flags_observed&&c->observed&&c->status==0;
#endif
}
#endif