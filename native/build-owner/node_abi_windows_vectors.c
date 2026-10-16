#include "node_abi_windows.h"
#if defined(_WIN32)
#include <assert.h>
/* STRUCTURAL BYTE FIXTURES ONLY. No genuine linker directory/source identity,
 * held/loaded Node, ABI call or native ROOT is fabricated. Whole native source
 * admission remains mandatory before compiling/running this test harness. */
static const char *const symbols[]={"napi_get_cb_info","napi_is_buffer","napi_get_buffer_info",
 "napi_create_buffer_copy","napi_create_object","napi_set_named_property","napi_create_string_utf8",
 "napi_throw_error","napi_get_undefined","napi_define_properties","napi_get_version"};
static size_t length(const char *s){size_t n=0;while(s[n])n++;return n;}
static void put16(unsigned char *p,uint32_t n){p[0]=(unsigned char)n;p[1]=(unsigned char)(n>>8);}
static void put32(unsigned char *p,uint32_t n){put16(p,n);put16(p+2,n>>16);}
static void size_field(unsigned char *p,size_t n) {
 unsigned char digits[10];size_t i=0,j;do{digits[i++]=(unsigned char)('0'+n%10);n/=10;}while(n);
 for(j=0;j<i;j++)p[j]=digits[i-j-1];
}
static size_t member(unsigned char *p,size_t at,const char *name,size_t bytes) {
 size_t i,n=length(name);for(i=0;i<60;i++)p[at+i]=' ';
 for(i=0;i<n;i++)p[at+i]=(unsigned char)name[i];size_field(p+at+48,bytes);p[at+58]=0x60;p[at+59]=10;
 for(i=0;i<bytes;i++)p[at+60+i]=0;return at+60;
}
static size_t fixture(unsigned char *p,int duplicate) {
 size_t i,j,at=8;const char *signature="!<arch>\n";for(i=0;i<8;i++)p[i]=(unsigned char)signature[i];
 for(i=0;i<2;i++){size_t body=member(p,at,"/",4);at=body+4;}
 for(i=0;i<(duplicate?12u:11u);i++) {
 const char *name=symbols[i%11];size_t n=length(name),bytes=20+n+1+9,body=member(p,at,"node.exe/",bytes);
 put16(p+body+2,0xffff);put16(p+body+6,0x8664);put32(p+body+12,(uint32_t)(bytes-20));put16(p+body+18,4);
 for(j=0;j<n;j++)p[body+20+j]=(unsigned char)name[j];
 for(j=0;j<8;j++)p[body+20+n+1+j]=(unsigned char)"node.exe"[j];
 at=body+bytes;if(at%2)p[at++]=10;
 }return at;
}
int slcli_build_node_abi_windows_archive_vectors_original(void) {
 unsigned char raw[4096];size_t bytes,first;struct slcli_build_node_archive_windows good={0};
 bytes=fixture(raw,0);assert(bytes<sizeof(raw));
 assert(slcli_build_node_archive_windows_parse_original(&good,raw,bytes));
 assert(good.members==13&&good.short_imports==11&&good.symbols[0].imports_found==1&&good.symbols[0].export_rva==0);
 first=good.symbols[0].import_header_offset;
 {struct slcli_build_node_archive_windows bad={0};put16(raw+first+60+6,0x14c);
 assert(!slcli_build_node_archive_windows_parse_original(&bad,raw,bytes));}
 {struct slcli_build_node_archive_windows bad={0};bytes=fixture(raw,0);raw[first+60+20+sizeof("napi_get_cb_info")]='N';
 assert(!slcli_build_node_archive_windows_parse_original(&bad,raw,bytes));}
 {struct slcli_build_node_archive_windows bad={0};bytes=fixture(raw,1);
 assert(!slcli_build_node_archive_windows_parse_original(&bad,raw,bytes));}
 {struct slcli_build_node_archive_windows bad={0};bytes=fixture(raw,0);
 assert(!slcli_build_node_archive_windows_parse_original(&bad,raw,bytes-1));}
 {struct slcli_build_node_archive_windows bad={0};bytes=fixture(raw,0);put16(raw+first+60+18,0x8004);
 assert(!slcli_build_node_archive_windows_parse_original(&bad,raw,bytes));}
 return 1;
}
#endif
