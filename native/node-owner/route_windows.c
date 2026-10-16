#include "route_windows.h"
#if defined(_WIN32)
#include "capture_layout_windows.h"
/* External export plus explicit linker inclusion retains the actual object in
 * the executable. Initial source bytes are zero, never a positive default. */
#pragma section(".slroute",read,write)
__declspec(allocate(".slroute")) __declspec(dllexport)
volatile unsigned char slcli_node_route_cell_v1[SLCLI_NODE_ROUTE_CELL_BYTES]={0};
static int separate(const void *a,size_t n,const volatile void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int valid(const struct slcli_node_route_windows *r) {
 uintptr_t a=(uintptr_t)r->transport,b=(uintptr_t)r->parent,p=(uintptr_t)r->base;
 return a&&b&&a!=b&&r->transport!=INVALID_HANDLE_VALUE&&r->parent!=INVALID_HANDLE_VALUE&&
  p&&r->bytes&&r->bytes<=UINTPTR_MAX&&p<=UINTPTR_MAX-(uintptr_t)r->bytes&&
  r->capture_bytes&&r->capture_bytes<r->bytes&&r->threads&&r->events>=5&&
  r->regions&&r->modules&&r->name_units&&r->page_bytes&&r->granularity;
}
int slcli_node_route_windows_encode(unsigned char *out,size_t n,const struct slcli_node_route_windows *r) {
 size_t i,j;uint64_t v[14];struct slcli_node_capture_layout_windows shape;
 if(!out||n!=SLCLI_NODE_ROUTE_CELL_BYTES||!r||!separate(out,n,r,sizeof(*r))||!valid(r)||
  !slcli_node_capture_layout_windows_route_shape_original(r,&shape))return 0;
 v[0]=UINT64_C(0x31544c43494c4353);v[1]=1;v[2]=(uintptr_t)r->transport;v[3]=(uintptr_t)r->parent;
 v[4]=(uintptr_t)r->base;v[5]=r->bytes;v[6]=r->capture_bytes;v[7]=r->threads;
 v[8]=r->events;v[9]=r->regions;v[10]=r->modules;v[11]=r->name_units;
 v[12]=r->page_bytes;v[13]=r->granularity;
 for(i=0;i<14;i++)for(j=0;j<8;j++)out[i*8+j]=(unsigned char)(v[i]>>(j*8));
 for(i=112;i<n;i++)out[i]=0;return 1;
}
int slcli_node_route_windows_decode(const volatile unsigned char *in,size_t n,struct slcli_node_route_windows *out) {
 size_t i,j;uint64_t v[14]={0};struct slcli_node_route_windows r;
 struct slcli_node_capture_layout_windows shape;
 if(!in||n!=SLCLI_NODE_ROUTE_CELL_BYTES||!out||!separate(out,sizeof(*out),in,n))return 0;
 for(i=0;i<14;i++)for(j=0;j<8;j++)v[i]|=(uint64_t)in[i*8+j]<<(j*8);
 for(i=112;i<n;i++)if(in[i])return 0;
 if(v[0]!=UINT64_C(0x31544c43494c4353)||v[1]!=1||v[2]>UINTPTR_MAX||v[3]>UINTPTR_MAX||v[4]>UINTPTR_MAX)return 0;
 r.transport=(HANDLE)(uintptr_t)v[2];r.parent=(HANDLE)(uintptr_t)v[3];r.base=(void *)(uintptr_t)v[4];
 r.bytes=v[5];r.capture_bytes=v[6];r.threads=v[7];r.events=v[8];r.regions=v[9];r.modules=v[10];r.name_units=v[11];
 r.page_bytes=v[12];r.granularity=v[13];
 if(!valid(&r)||!slcli_node_capture_layout_windows_route_shape_original(&r,&shape))return 0;
 /* Publish only after full syntax validation. Suspension orders the outside
  * writer before entry; this function does not itself prove that relation. */
 *out=r;return 1;
}
int slcli_node_route_windows_read_original(struct slcli_node_route_windows *out) {
 return slcli_node_route_windows_decode(slcli_node_route_cell_v1,SLCLI_NODE_ROUTE_CELL_BYTES,out);
}
#endif
