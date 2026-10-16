#include "host_frame_body.h"
#include "wire.h"
int slcli_host_frame_verify_body(uint8_t direction,const unsigned char nonce[32],const unsigned char key[32],
                                  uint32_t sequence,const unsigned char *raw,uint32_t bytes,const unsigned char mac[32]) {
  static const unsigned char domain[]="SLCLI-HOST-ADMISSION-2";
  struct slcli_hmac256 h;struct slcli_cursor c;unsigned char seq[4],length[4],expected[32];
  unsigned char n=0,k=0;uint8_t operation;uint32_t original;unsigned i;int ok;
  if(direction>1||!nonce||!key||!raw||!mac||bytes<5||bytes>SLCLI_FRAME_BYTES)return 0;
  for(i=0;i<32;i++){n|=nonce[i];k|=key[i];}if(!n||!k)return 0;
  c.raw=raw;c.bytes=bytes;c.offset=0;c.failed=0;
  if(!slcli_u8(&c,&operation)||!slcli_u32(&c,&original)||original!=sequence)return 0;
  (void)operation;slcli_put_u32(seq,sequence);slcli_put_u32(length,bytes);
  ok=slcli_hmac256_init(&h,key)&&slcli_hmac256_update(&h,domain,sizeof(domain))&&
    slcli_hmac256_update(&h,&direction,1)&&slcli_hmac256_update(&h,nonce,32)&&
    slcli_hmac256_update(&h,seq,4)&&slcli_hmac256_update(&h,length,4)&&
    slcli_hmac256_update(&h,raw,bytes)&&slcli_hmac256_finish(&h,expected);
  if(ok)ok=slcli_equal(expected,mac,32);slcli_erase(&h,sizeof(h));slcli_erase(expected,sizeof(expected));return ok;
}
