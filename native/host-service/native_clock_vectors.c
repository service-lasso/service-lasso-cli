#include "native_clock_vectors.h"
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_native_clock_positive_original(struct slcli_native_clock *c,
  unsigned char *scratch,size_t scratch_bytes) {
  size_t i;uint32_t calls;
  if(!c||scratch_bytes<sizeof(*c)||!away(c,sizeof(*c),scratch,sizeof(*c)))return 0;
#if defined(_WIN32)
  calls=2;
#elif defined(__linux__)
  calls=1;
#else
  return 0;
#endif
  if(!slcli_native_clock_read(c)||!c->started||!c->observed||!c->complete||!c->tick||
    c->calls_called!=calls||c->calls_returned!=calls)return 0;
  for(i=0;i<sizeof(*c);i++)scratch[i]=((const unsigned char *)c)[i];
  if(slcli_native_clock_read(c))return 0;
  for(i=0;i<sizeof(*c);i++)if(scratch[i]!=((const unsigned char *)c)[i])return 0;
  return 1;
}
int slcli_native_clock_markerless_vectors(void) {
  struct slcli_native_clock c;unsigned char before[sizeof(c)];
  unsigned char *raw=(unsigned char *)&c;size_t i;unsigned field;
  /* Deliberately invalid representation metadata, not native clock originals.
   * Actual clock entry must reject each semantic field before any native call.
   * C padding is not interpreted as a semantic native fact. */
  for(field=0;field<11;field++) {
    for(i=0;i<sizeof(c);i++)raw[i]=0;
    switch(field) {
      case 0:c.args[0]=1;break;case 1:c.args[1]=1;break;
      case 2:c.raw[0]=1;break;case 3:c.raw[1]=1;break;
      case 4:c.status[0]=1;break;case 5:c.status[1]=1;break;
      case 6:c.tick=1;break;case 7:c.observed=1;break;
      case 8:c.complete=1;break;case 9:c.calls_called=1;break;
      default:c.calls_returned=1;break;
    }
    for(i=0;i<sizeof(c);i++)before[i]=raw[i];
    if(slcli_native_clock_read(&c))return 0;
    for(i=0;i<sizeof(c);i++)if(before[i]!=raw[i])return 0;
  }
  return 1;
}