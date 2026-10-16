#include "product_route_windows.h"
#if defined(_WIN32)
#pragma section(".slprout",read,write)
__declspec(allocate(".slprout")) __declspec(dllexport)
volatile unsigned char slcli_product_route_cell_v1[SLCLI_PRODUCT_ROUTE_BYTES]={0};
static int apart(const void *a,size_t n,const volatile void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);
}
static int route(const struct slcli_product_windows_entry_route *r) {
 return r&&r->readonly_section&&r->parent_process&&
 r->readonly_section!=INVALID_HANDLE_VALUE&&r->parent_process!=INVALID_HANDLE_VALUE&&
 r->readonly_section!=r->parent_process;
}
int slcli_product_windows_route_encode(unsigned char *out,size_t bytes,
 const struct slcli_product_windows_entry_route *r) {
 uint64_t v[4];size_t i,j;
 if(bytes!=SLCLI_PRODUCT_ROUTE_BYTES||!apart(out,bytes,r,sizeof(*r))||!route(r))return 0;
 v[0]=UINT64_C(0x31545250494c4353);v[1]=1;
 v[2]=(uintptr_t)r->readonly_section;v[3]=(uintptr_t)r->parent_process;
 for(i=0;i<4;i++)for(j=0;j<8;j++)out[8*i+j]=(unsigned char)(v[i]>>(8*j));return 1;
}
int slcli_product_windows_route_decode(struct slcli_product_windows_entry_route *out,
 const volatile unsigned char *raw,size_t bytes) {
 uint64_t v[4]={0};struct slcli_product_windows_entry_route r;size_t i,j;
 if(bytes!=SLCLI_PRODUCT_ROUTE_BYTES||!apart(out,sizeof(*out),raw,bytes))return 0;
 for(i=0;i<4;i++)for(j=0;j<8;j++)v[i]|=(uint64_t)raw[8*i+j]<<(8*j);
 if(v[0]!=UINT64_C(0x31545250494c4353)||v[1]!=1||v[2]>UINTPTR_MAX||v[3]>UINTPTR_MAX)return 0;
 r.readonly_section=(HANDLE)(uintptr_t)v[2];r.parent_process=(HANDLE)(uintptr_t)v[3];
 if(!route(&r))return 0;out->readonly_section=r.readonly_section;out->parent_process=r.parent_process;return 1;
}
#endif
