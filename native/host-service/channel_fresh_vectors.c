#include "channel_fresh_vectors.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int plan(struct slcli_stream_capture *r,struct slcli_channel_write_capture *w,
  unsigned char *buffer,const unsigned char *body,size_t n,unsigned char *saved,size_t bytes) {
  const void *p[5];size_t lengths[5],i,j;
  p[0]=r;p[1]=w;p[2]=buffer;p[3]=body;p[4]=saved;
  lengths[0]=sizeof(*r);lengths[1]=sizeof(*w);lengths[2]=n;lengths[3]=n;
  lengths[4]=sizeof(*r)>sizeof(*w)?sizeof(*r):sizeof(*w);
  if(!n||n>16384||bytes<lengths[4])return 0;
  for(i=0;i<5;i++)for(j=i+1;j<5;j++)if(!away(p[i],lengths[i],p[j],lengths[j]))return 0;
  return 1;
}
static int zeroed(const void *p,size_t n) {
  const unsigned char *r=p;size_t i;for(i=0;i<n;i++)if(r[i])return 0;return 1;
}
static void copy(unsigned char *d,const void *p,size_t n) {
  const unsigned char *s=p;size_t i;for(i=0;i<n;i++)d[i]=s[i];
}
static int equal(const unsigned char *saved,const void *p,size_t n) {
  const unsigned char *s=p;size_t i;for(i=0;i<n;i++)if(saved[i]!=s[i])return 0;return 1;
}
int slcli_channel_fresh_positive_original(struct slcli_stream_capture *r,
  struct slcli_channel_write_capture *w,slcli_input_handle input,slcli_input_handle output,
  slcli_input_handle read_event,slcli_input_handle write_event,unsigned char *buffer,const unsigned char *body,size_t n,
  unsigned char *saved,size_t bytes) {
  if(!plan(r,w,buffer,body,n,saved,bytes)||!zeroed(r,sizeof(*r))||!zeroed(w,sizeof(*w)))return 0;
#if defined(_WIN32)
  if(read_event==write_event)return 0; /* Pending operations own distinct events. */
#endif
  /* Independent native fixture supplies input with available bytes and output
   * with capacity; Windows may genuinely return pending. Never claim delivered
   * bytes from pending, and retain either original if a later assertion fails. */
  if(!slcli_stream_read_original(r,input,buffer,n,read_event)||!r->started||!r->observed||
    !r->clock.complete||r->failed||r->original!=input||r->buffer!=buffer||r->requested!=n||
    (!r->pending&&(!r->complete||!r->count_observed||!r->returned||r->returned>n)))return 0;
  copy(saved,r,sizeof(*r));
  if(slcli_stream_read_original(r,input,buffer,n,read_event)||!equal(saved,r,sizeof(*r)))return 0;
  if(!slcli_channel_write_original(w,output,body,n,write_event)||!w->started||!w->observed||
    !w->clock.complete||w->failed||w->original!=output||w->body!=body||w->requested!=n||
    (!w->pending&&(!w->complete||!w->count_observed||!w->returned||w->returned>n)))return 0;
  copy(saved,w,sizeof(*w));
  return !slcli_channel_write_original(w,output,body,n,write_event)&&equal(saved,w,sizeof(*w));
}
int slcli_channel_markerless_vectors_original(struct slcli_stream_capture *r,
  struct slcli_channel_write_capture *w,slcli_input_handle input,slcli_input_handle output,
  slcli_input_handle read_event,slcli_input_handle write_event,unsigned char *buffer,const unsigned char *body,size_t n,
  unsigned char *saved,size_t bytes) {
  size_t i,j;unsigned char *raw;
  if(!plan(r,w,buffer,body,n,saved,bytes)||!zeroed(r,sizeof(*r))||!zeroed(w,sizeof(*w)))return 0;
  /* Isolated representation fixture rows only, never a pending native row.
   * Exercise every byte outside started, including padding, without inventing
   * native successful output. Genuine supplied endpoints remain unused. */
  raw=(unsigned char *)r;
  for(i=0;i<sizeof(*r);i++) {
    if(i>=offsetof(struct slcli_stream_capture,started)&&i<offsetof(struct slcli_stream_capture,started)+sizeof(r->started))continue;
    for(j=0;j<sizeof(*r);j++)raw[j]=0;raw[i]=1;copy(saved,r,sizeof(*r));
    if(slcli_stream_read_original(r,input,buffer,n,read_event)||!equal(saved,r,sizeof(*r)))return 0;
  }
  raw=(unsigned char *)w;
  for(i=0;i<sizeof(*w);i++) {
    if(i>=offsetof(struct slcli_channel_write_capture,started)&&i<offsetof(struct slcli_channel_write_capture,started)+sizeof(w->started))continue;
    for(j=0;j<sizeof(*w);j++)raw[j]=0;raw[i]=1;copy(saved,w,sizeof(*w));
    if(slcli_channel_write_original(w,output,body,n,write_event)||!equal(saved,w,sizeof(*w)))return 0;
  }
  return 1;
}
#endif
