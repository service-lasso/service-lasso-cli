#include "selector_windows.h"
#if defined(_WIN32)
static int separate(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_node_selector_windows_encode(WCHAR *out,size_t n,HANDLE transport,HANDLE parent,
 const void *base,uint64_t bytes) {
  static const WCHAR name[]=SLCLI_NODE_SELECTOR_NAME,hex[]=L"0123456789abcdef";
  size_t i,j,k=0;uint64_t a=(uintptr_t)transport,b=(uintptr_t)parent;
  uint64_t values[4]={a,b,(uintptr_t)base,bytes};
  if(!out||n!=SLCLI_NODE_SELECTOR_ENTRY_CHARS||!a||!b||a==b||
     transport==INVALID_HANDLE_VALUE||parent==INVALID_HANDLE_VALUE||!base||!bytes||
     bytes>UINTPTR_MAX||(uintptr_t)base>UINTPTR_MAX-(uintptr_t)bytes)return 0;
  for(i=0;i+1<sizeof(name)/sizeof(*name);i++)out[k++]=name[i];
  out[k++]=L'=';
  for(j=0;j<4;j++) {
    if(j)out[k++]=L':';
    for(i=0;i<16;i++)out[k++]=hex[(values[j]>>(60u-4u*i))&15u];
  }
  out[k++]=0;return k==n;
}
int slcli_node_selector_windows_decode(const WCHAR *in,size_t n,HANDLE *transport,HANDLE *parent,
 void **base,uint64_t *bytes) {
  size_t i,j;uint64_t values[4]={0,0,0,0};WCHAR ch;unsigned digit;
  if(!in||n!=SLCLI_NODE_SELECTOR_VALUE_CHARS+1u||!transport||!parent||transport==parent||
     !base||!bytes||(void *)base==(void *)transport||(void *)base==(void *)parent||
     (void *)bytes==(void *)transport||(void *)bytes==(void *)parent||(void *)bytes==(void *)base||
     !separate(transport,sizeof(*transport),parent,sizeof(*parent))||
     !separate(transport,sizeof(*transport),base,sizeof(*base))||
     !separate(transport,sizeof(*transport),bytes,sizeof(*bytes))||
     !separate(parent,sizeof(*parent),base,sizeof(*base))||
     !separate(parent,sizeof(*parent),bytes,sizeof(*bytes))||!separate(base,sizeof(*base),bytes,sizeof(*bytes))||
     in[16]!=L':'||in[33]!=L':'||in[50]!=L':'||in[67])return 0;
  for(j=0;j<4;j++)for(i=0;i<16;i++) {
    ch=in[j*17+i];
    if(ch>=L'0'&&ch<=L'9')digit=ch-L'0';
    else if(ch>=L'a'&&ch<=L'f')digit=ch-L'a'+10u;else return 0;
    values[j]=(values[j]<<4)|digit;
  }
  if(!values[0]||!values[1]||values[0]==values[1]||values[0]>UINTPTR_MAX||values[1]>UINTPTR_MAX||
     (HANDLE)(uintptr_t)values[0]==INVALID_HANDLE_VALUE||(HANDLE)(uintptr_t)values[1]==INVALID_HANDLE_VALUE||
     !values[2]||!values[3]||values[2]>UINTPTR_MAX||values[3]>UINTPTR_MAX||
     values[2]>UINTPTR_MAX-values[3])return 0;
  *transport=(HANDLE)(uintptr_t)values[0];*parent=(HANDLE)(uintptr_t)values[1];
  *base=(void *)(uintptr_t)values[2];*bytes=values[3];return 1;
}
#endif
