/* SPDX-License-Identifier: Apache-2.0 */
#include "bq_provider_entry.h"
#include <limits.h>
typedef char slcli_provider_native_int32[(sizeof(int)==4&&INT_MAX==2147483647)?1:-1];
static int apart(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&n<=UINTPTR_MAX-x&&m<=UINTPTR_MAX-y&&(x+n<=y||y+m<=x);
}
int slcli_bq_provider_endpoint_encode(char *out,size_t capacity,size_t *written,int fd){
 char reversed[10];unsigned value;size_t n=0,i;
 if(fd<0||capacity!=SLCLI_BQ_PROVIDER_ENDPOINT_TEXT_BYTES||
  !apart(out,capacity,written,sizeof(*written)))return 0;
 value=(unsigned)fd;
 do{reversed[n++]=(char)('0'+value%10);value/=10;}while(value);
 for(i=0;i<n;++i)out[i]=reversed[n-1-i];
 out[n]=0;*written=n+1;return 1;
}
int slcli_bq_provider_endpoint_decode(int *out,const char *text,size_t bytes){
 unsigned value=0,digit;size_t i;
 if(bytes<2||bytes>SLCLI_BQ_PROVIDER_ENDPOINT_TEXT_BYTES||
  !apart(out,sizeof(*out),text,bytes)||text[bytes-1]||
  (bytes>2&&text[0]=='0'))return 0;
 for(i=0;i+1<bytes;++i){
  if(text[i]<'0'||text[i]>'9')return 0;
  digit=(unsigned)(text[i]-'0');
  if(value>((unsigned)INT_MAX-digit)/10)return 0;
  value=value*10+digit;
 }
 *out=(int)value;return 1;
}
int slcli_bq_provider_count_encode(char *out,size_t capacity,size_t *written,uint64_t value){
 char reversed[20];size_t n=0,i;
 if(!value||capacity!=SLCLI_BQ_PROVIDER_COUNT_TEXT_BYTES||
  !apart(out,capacity,written,sizeof(*written)))return 0;
 do{reversed[n++]=(char)('0'+value%10);value/=10;}while(value);
 for(i=0;i<n;++i)out[i]=reversed[n-1-i];
 out[n]=0;*written=n+1;return 1;
}
int slcli_bq_provider_count_decode(uint64_t *out,const char *text,size_t bytes){
 uint64_t value=0,digit;size_t i;
 if(bytes<2||bytes>SLCLI_BQ_PROVIDER_COUNT_TEXT_BYTES||
  !apart(out,sizeof(*out),text,bytes)||text[bytes-1]||text[0]=='0')return 0;
 for(i=0;i+1<bytes;++i){
  if(text[i]<'0'||text[i]>'9')return 0;
  digit=(uint64_t)(text[i]-'0');
  if(value>(UINT64_MAX-digit)/10)return 0;
  value=value*10+digit;
 }
 *out=value;return 1;
}
