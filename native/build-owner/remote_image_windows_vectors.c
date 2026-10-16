#include "remote_image_windows.h"
#if defined(_WIN32)
#include <assert.h>
/* LABELLED STRUCTURAL FIXTURES. These bytes are not an executable/tool/source
 * qualification. They exercise the actual pure geometry helper used before
 * native capture. Execution remains behind whole-source/ROOT admission. */
static void put16(unsigned char *p,uint32_t value){p[0]=(unsigned char)value;p[1]=(unsigned char)(value>>8);}
static void put32(unsigned char *p,uint32_t value){put16(p,value);put16(p+2,value>>16);}
static void fixture(unsigned char *p) {
 size_t i;for(i=0;i<1024;i++)p[i]=0;p[0]='M';p[1]='Z';put32(p+60,64);
 put32(p+64,0x00004550);put16(p+68,0x8664);put16(p+70,1);put16(p+84,240);
 put16(p+88,0x20b);put32(p+120,4096);put32(p+124,512);put32(p+144,8192);put32(p+148,512);put32(p+196,16);
 put32(p+328+8,512);put32(p+328+12,4096);put32(p+328+16,512);put32(p+328+20,512);
}
int slcli_build_remote_image_windows_geometry_vectors_original(void) {
 unsigned char raw[1024];struct slcli_build_pe64_geometry_windows g={0};
 fixture(raw);assert(slcli_build_pe64_geometry_windows_original(&g,raw,sizeof(raw),4096));
 assert(g.image_bytes==8192&&g.headers_bytes==512&&g.section_table_offset==328&&g.section_count==1&&g.capacity==2);
 /* No row, process, handle, prepared owner, mapping or continuation is returned. */
 {struct slcli_build_pe64_geometry_windows bad={0};fixture(raw);put32(raw+328+20,900);
 assert(!slcli_build_pe64_geometry_windows_original(&bad,raw,sizeof(raw),4096));}
 {struct slcli_build_pe64_geometry_windows bad={0};fixture(raw);put32(raw+328+12,8192);
 assert(!slcli_build_pe64_geometry_windows_original(&bad,raw,sizeof(raw),4096));}
 {struct slcli_build_pe64_geometry_windows bad={0};fixture(raw);put16(raw+70,97);
 assert(!slcli_build_pe64_geometry_windows_original(&bad,raw,sizeof(raw),4096));}
 {struct slcli_build_pe64_geometry_windows bad={0};fixture(raw);put16(raw+68,0x14c);
 assert(!slcli_build_pe64_geometry_windows_original(&bad,raw,sizeof(raw),4096));}
 {struct slcli_build_pe64_geometry_windows bad={0};fixture(raw);put32(raw+144,8191);
 assert(!slcli_build_pe64_geometry_windows_original(&bad,raw,sizeof(raw),4096));}
 {struct slcli_build_pe64_geometry_windows bad={0};fixture(raw);
 assert(!slcli_build_pe64_geometry_windows_original(&bad,raw,128,4096));}
 {struct slcli_build_pe64_geometry_windows bad={0};fixture(raw);
 assert(!slcli_build_pe64_geometry_windows_original(&bad,raw,sizeof(raw),3));}
 return 1;
}
#endif
