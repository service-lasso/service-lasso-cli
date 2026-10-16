#include "observer_wire.h"
#include "ledger_state.h"
static int nonzero(const unsigned char *p) {
  unsigned char n=0;unsigned i;if(!p)return 0;for(i=0;i<32;i++)n|=p[i];return n!=0;
}
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_observer_ack_parse(const unsigned char *raw,size_t n,struct slcli_observer_ack *out) {
  struct slcli_cursor c;struct slcli_observer_ack r={0};uint8_t op;uint32_t sequence;
  if(!raw||n!=109||!out||!separate(raw,n,out,sizeof(*out)))return 0;
  c.raw=raw;c.bytes=n;c.offset=0;c.failed=0;
  if(!slcli_u8(&c,&op)||op!=10||!slcli_u32(&c,&sequence)||sequence!=1||
     !slcli_take(&c,32,&r.reservation)||!slcli_u64(&c,&r.generation)||!r.generation||
     !slcli_take(&c,32,&r.receipt)||!slcli_take(&c,32,&r.archive)||
     !slcli_cursor_end(&c)||!nonzero(r.reservation)||!nonzero(r.receipt)||!nonzero(r.archive))return 0;
  *out=r;return 1;
}
static int valid(const struct slcli_observer_terminal *r) {
  return r&&nonzero(r->reservation)&&nonzero(r->observer)&&r->intent&&
    nonzero(r->closure)&&nonzero(r->ledger)&&r->generation&&r->tick&&
    r->slot<SLCLI_BINDINGS&&r->disposition<=2&&(r->disposition||nonzero(r->intent));
}
int slcli_observer_terminal_parse(const unsigned char *raw,size_t n,
                                   struct slcli_observer_terminal *out) {
  struct slcli_cursor c;struct slcli_observer_terminal r={0};uint8_t op;uint32_t sequence;
  if(!raw||n!=183||!out||!separate(raw,n,out,sizeof(*out)))return 0;
  c.raw=raw;c.bytes=n;c.offset=0;c.failed=0;
  if(!slcli_u8(&c,&op)||op!=9||!slcli_u32(&c,&sequence)||sequence!=1||
     !slcli_take(&c,32,&r.reservation)||!slcli_u8(&c,&r.slot)||
     !slcli_take(&c,32,&r.observer)||!slcli_take(&c,32,&r.intent)||
     !slcli_u64(&c,&r.generation)||!slcli_take(&c,32,&r.closure)||
     !slcli_take(&c,32,&r.ledger)||!slcli_u64(&c,&r.tick)||
     !slcli_u8(&c,&r.disposition)||!slcli_cursor_end(&c)||!valid(&r))return 0;
  *out=r;return 1;
}
int slcli_observer_terminal_encode(unsigned char out[183],const struct slcli_observer_terminal *r) {
  const unsigned char *parts[5];size_t offsets[5]={5,38,70,110,142};unsigned i,j;
  if(!out||!valid(r)||!separate(out,183,r,sizeof(*r)))return 0;
  parts[0]=r->reservation;parts[1]=r->observer;parts[2]=r->intent;parts[3]=r->closure;parts[4]=r->ledger;
  for(i=0;i<5;i++)if(!separate(out,183,parts[i],32))return 0;
  out[0]=9;slcli_put_u32(out+1,1);out[37]=r->slot;
  for(i=0;i<5;i++)for(j=0;j<32;j++)out[offsets[i]+j]=parts[i][j];
  slcli_put_u64(out+102,r->generation);slcli_put_u64(out+174,r->tick);out[182]=r->disposition;return 1;
}
