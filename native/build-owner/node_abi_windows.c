#include "node_abi_windows.h"
#if defined(_WIN32)
static const uint64_t sizes[SLCLI_BUILD_NODE_INPUTS]={86997320,2790574,10370,31770,7603,1825,4243};
static const char *const hashes[SLCLI_BUILD_NODE_INPUTS]={
 "0d0f5e39f9f3d9587bc19f73eab3c2c9c4903fd02d6dbf9c853dd81b3d95fad4",
 "ac15f1e9d7c8279353723a77f6319967f1a41c06026521094a8234c2e6fbe052",
 "2d4560831e525b47b060ec8a0864ab73993df9e20215ce9f0fe7b24cd31af32a",
 "582e64ead4d5fdf663e2bd934f2fc97e8e155a7740d28995830393d19d089705",
 "05e3d86e24c814f94a8b52b6a70cec11d7e91c810008253553528637e3a15b2f",
 "088766b3cf1f0cd9b292c361ad30f7a04e7cc6db35a20ca52c572c1adf9635e7",
 "21f88237adb8b6823e2fb64fdc868c50fa785c8c2c5a19f6e7498401b1a963ff"};
/* Fixed ordinary Node-API8 C seed surface. Names are byte constraints only;
 * these rows contain RVAs, never callable function pointers or load grants. */
static const char *const names[SLCLI_BUILD_NODE_API_IMPORTS]={"napi_get_cb_info","napi_is_buffer","napi_get_buffer_info",
 "napi_create_buffer_copy","napi_create_object","napi_set_named_property","napi_create_string_utf8",
 "napi_throw_error","napi_get_undefined","napi_define_properties","napi_get_version"};
static int zero(const void *p,size_t n){const unsigned char *s=p;size_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;}
static uint32_t le16(const unsigned char *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8);}
static uint32_t le32(const unsigned char *p){return le16(p)|(le16(p+2)<<16);}
static int apart(const void *a,size_t n,const void *b,size_t m){uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);}
static int equal(const unsigned char *p,size_t n,const char *text) {
 size_t i;for(i=0;i<n;i++)if(!text[i]||p[i]!=(unsigned char)text[i])return 0;return text[n]==0;
}
static unsigned hex(char c){return c>='0'&&c<='9'?(unsigned)(c-'0'):(unsigned)(c-'a'+10);}
static int pinned(const struct slcli_build_windows_member *m,size_t slot) {
 size_t i;if(m->bytes!=sizes[slot])return 0;
 for(i=0;i<32;i++)if(m->digest[i]!=(hex(hashes[slot][i*2])*16+hex(hashes[slot][i*2+1])))return 0;return 1;
}
int slcli_build_node_abi_windows_source_output_away_original(const struct slcli_build_node_abi_windows_source *s,const void *out,size_t n) {
 size_t i,j;if(!apart(out,n,s,sizeof(*s)))return 0;
 for(i=0;i<SLCLI_BUILD_NODE_INPUTS;i++) {
 const struct slcli_build_windows_member *m=&s->members[i];
 if(!m->leaf.original||!m->leaf.units||m->leaf.units>SIZE_MAX/sizeof(WCHAR)||!m->ancestors_original||
 !m->ancestor_count||m->ancestor_count>SIZE_MAX/sizeof(*m->ancestors_original)||
 m->ancestor_count>SIZE_MAX/sizeof(struct slcli_build_windows_open)||
 !m->read_capacity||m->read_capacity>SIZE_MAX/sizeof(struct slcli_windows_file_capture)||
 !apart(out,n,m->leaf.original,m->leaf.units*sizeof(WCHAR))||
 !apart(out,n,m->ancestors_original,m->ancestor_count*sizeof(*m->ancestors_original)))return 0;
 for(j=0;j<m->ancestor_count;j++)if(!m->ancestors_original[j].original||!m->ancestors_original[j].units||
 m->ancestors_original[j].units>SIZE_MAX/sizeof(WCHAR)||
 !apart(out,n,m->ancestors_original[j].original,m->ancestors_original[j].units*sizeof(WCHAR)))return 0;
 }return 1;
}
static int decimal(const unsigned char *p,size_t *value) {
 size_t i,result=0;int digit=0,space=0;
 for(i=0;i<10;i++){if(p[i]==' '){space=1;continue;}
 if(space||p[i]<'0'||p[i]>'9'||result>(SIZE_MAX-(p[i]-'0'))/10)return 0;
 result=result*10+(p[i]-'0');digit=1;}if(!digit)return 0;*value=result;return 1;
}
int slcli_build_node_archive_windows_parse_original(struct slcli_build_node_archive_windows *c,const unsigned char *p,size_t n) {
 size_t i;
 if(!c||!p||n<8||n>sizes[SLCLI_BUILD_NODE_LIBRARY]||!apart(c,sizeof(*c),p,n)||!zero(c,sizeof(*c))||
 !equal(p,8,"!<arch>\n"))return 0;c->cursor=8;
 while(c->cursor<n) {
 size_t offset=c->cursor,bytes,name_end,dll_end;const unsigned char *h,*body;uint32_t type;
 if(n-offset<60)return 0;h=p+offset;
 if(h[58]!=0x60||h[59]!=10||!decimal(h+48,&bytes)||bytes>n-offset-60)return 0;
 body=h+60;c->members++;
 if(c->members<=2) {
 if(h[0]!='/'||h[1]!=' ')return 0; // two complete held linker member bodies
 }else if(bytes>=20&&le16(body)==0&&le16(body+2)==0xffff) {
 if(le16(body+4)!=0||le16(body+6)!=0x8664||le32(body+12)!=bytes-20)return 0;
 type=le16(body+18);if((type>>5)||(type&3)>2||((type>>2)&7)>3)return 0;
 for(name_end=20;name_end<bytes&&body[name_end];name_end++);if(name_end==20||name_end>=bytes)return 0;
 for(dll_end=name_end+1;dll_end<bytes&&body[dll_end];dll_end++);
 if(dll_end!=bytes-1||!equal(body+name_end+1,dll_end-name_end-1,"node.exe"))return 0;
 c->short_imports++;
 for(i=0;i<SLCLI_BUILD_NODE_API_IMPORTS;i++)if(equal(body+20,name_end-20,names[i])) {
 if((type&3)!=0||((type>>2)&7)!=1||c->symbols[i].imports_found)return 0;
 c->symbols[i].imports_found=1;c->symbols[i].import_header_offset=offset;
 }
 }else {
 /* Source-pinned long COFF descriptor objects remain complete held bytes;
  * validate machine and bounded section table, never interpret them as imports. */
 if(bytes<20||le16(body)!=0x8664||!le16(body+2)||le16(body+16)!=0||
 le16(body+2)>(bytes-20)/40)return 0;
 }
 c->cursor=offset+60+bytes;
 if(c->cursor%2){if(c->cursor>=n||p[c->cursor]!=10)return 0;c->cursor++;}
 }
 if(c->cursor!=n||c->members<2)return 0;
 for(i=0;i<SLCLI_BUILD_NODE_API_IMPORTS;i++)if(c->symbols[i].imports_found!=1)return 0;return 1;
}
static const unsigned char *rva(const struct slcli_build_node_abi_windows *c,uint32_t address,size_t bytes) {
 const unsigned char *p=c->inputs[SLCLI_BUILD_NODE_IMAGE].body_original;size_t i;
 if(address>c->image_geometry.image_bytes||bytes>c->image_geometry.image_bytes-address)return NULL;
 if(address<c->image_geometry.headers_bytes&&bytes<=c->image_geometry.headers_bytes-address)return p+address;
 for(i=0;i<c->image_geometry.section_count;i++) {
 const unsigned char *s=p+c->image_geometry.section_table_offset+i*40;
 uint64_t va=le32(s+12),raw=le32(s+16),from=le32(s+20);
 if(address>=va&&address-va<=raw&&bytes<=raw-(address-va))return p+(size_t)(from+address-va);
 }return NULL;
}
static int exports(struct slcli_build_node_abi_windows *c) {
 const unsigned char *p=c->inputs[SLCLI_BUILD_NODE_IMAGE].body_original,*directory,*functions,*name_table,*ordinals;
 size_t optional=(size_t)le32(p+60)+24,i,j;uint32_t export_rva,export_bytes,function_count,name_count;
 if(le32(p+optional+108)<1)return 0;export_rva=le32(p+optional+112);export_bytes=le32(p+optional+116);
 if(export_bytes<40||export_rva>c->image_geometry.image_bytes||export_bytes>c->image_geometry.image_bytes-export_rva)return 0;
 directory=rva(c,export_rva,40);if(!directory)return 0;
 function_count=le32(directory+20);name_count=le32(directory+24);
 if(!function_count||!name_count||function_count>sizes[0]/4||name_count>sizes[0]/4)return 0;
 functions=rva(c,le32(directory+28),(size_t)function_count*4);
 name_table=rva(c,le32(directory+32),(size_t)name_count*4);ordinals=rva(c,le32(directory+36),(size_t)name_count*2);
 if(!functions||!name_table||!ordinals)return 0;
 for(i=0;i<name_count;i++) {
 uint32_t address=le32(name_table+i*4),ordinal=le16(ordinals+i*2),function;const unsigned char *name;size_t length;
 if(ordinal>=function_count)return 0;
 for(length=0;length<4096;length++){if(address>UINT32_MAX-(uint32_t)length)return 0;
 name=rva(c,address+((uint32_t)length),1);if(!name)return 0;if(!*name)break;}
 if(!length||length==4096)return 0;name=rva(c,address,length+1);if(!name)return 0;c->export_names_checked++;
 for(j=0;j<SLCLI_BUILD_NODE_API_IMPORTS;j++)if(equal(name,length,names[j])) {
 size_t k;int executable=0;if(c->archive_original.symbols[j].exports_found)return 0;function=le32(functions+(size_t)ordinal*4);
 if(!function||!rva(c,function,1)||(function>=export_rva&&function-export_rva<export_bytes))return 0;
 for(k=0;k<c->image_geometry.section_count;k++) {
 const unsigned char *s=p+c->image_geometry.section_table_offset+k*40;uint32_t va=le32(s+12),raw=le32(s+16);
 if(function>=va&&function-va<raw&&(le32(s+36)&0x20000000))executable=1;
 }
 if(!executable)return 0;c->archive_original.symbols[j].exports_found=1;c->archive_original.symbols[j].export_rva=function;
 c->archive_original.symbols[j].export_name_offset=(size_t)(name-p);
 }
 }
 for(i=0;i<SLCLI_BUILD_NODE_API_IMPORTS;i++)if(c->archive_original.symbols[i].exports_found!=1)return 0;return 1;
}
static int deny(struct slcli_build_node_abi_windows *c,struct slcli_build_bootstrap_windows *b){c->failed=1;b->failed=1;b->memory_original.failed=1;return 0;}
int slcli_build_node_abi_windows_capture_original(struct slcli_build_node_abi_windows *c,
 struct slcli_build_bootstrap_windows *b,const struct slcli_build_node_abi_windows_source *s) {
 size_t i;uint64_t prior;
 if(!c||!b||!s||!b->completed||b->failed||b->memory_original.failed||
 !slcli_build_principal_windows_issuing_relation_original(b->effective_original,b->primary_original)||
 !slcli_arena_live_block(&b->arena_original,c,sizeof(*c))||!zero(c,sizeof(*c))||
 !apart(c,sizeof(*c),b,sizeof(*b))||!slcli_build_node_abi_windows_source_output_away_original(s,c,sizeof(*c))||
 !slcli_build_held_file_windows_output_away_original(b->compiler_original,c,sizeof(*c)))return 0;
 for(i=0;i<SLCLI_BUILD_NODE_INPUTS;i++)if(!pinned(&s->members[i],i))return 0;
 c->source_original=s;c->bootstrap_original=b;c->started=1;c->failed=1;
 /* Whole finite output roster precedes the first seven-file native effect. */
 for(i=0;i<SLCLI_BUILD_NODE_INPUTS;i++) {
 const struct slcli_build_windows_member *m=&s->members[i];struct slcli_build_node_input_windows *row=&c->inputs[i];
 row->ancestors_original=slcli_arena_calloc(&b->arena_original,m->ancestor_count,sizeof(*row->ancestors_original));
 row->reads_original=slcli_arena_calloc(&b->arena_original,m->read_capacity,sizeof(*row->reads_original));
 row->body_original=slcli_arena_calloc(&b->arena_original,(size_t)m->bytes,1);
 if(!row->ancestors_original||!row->reads_original||!row->body_original)return deny(c,b);
 }
 prior=b->prior_tick;
 for(i=0;i<SLCLI_BUILD_NODE_INPUTS;i++) {
 struct slcli_build_node_input_windows *row=&c->inputs[i];
 if(!slcli_build_held_file_windows_capture_original(&row->held_original,b->effective_original,&s->members[i],
 row->ancestors_original,row->reads_original,row->body_original)||
 row->ancestors_original[0].call.before.tick<prior||row->held_original.finish.tick<prior)return deny(c,b);
 prior=row->held_original.finish.tick;c->inputs_acquired++;
 }
 if(!slcli_native_clock_read(&c->before_parse)||c->before_parse.tick<prior||c->before_parse.tick>=b->source_original->expires||
 !slcli_build_pe64_geometry_windows_original(&c->image_geometry,c->inputs[0].body_original,(size_t)sizes[0],b->pages_original.dwPageSize)||
 !slcli_build_node_archive_windows_parse_original(&c->archive_original,c->inputs[1].body_original,(size_t)sizes[1])||
 c->archive_original.members!=9257||c->archive_original.short_imports!=9252||
 !exports(c)||!slcli_native_clock_read(&c->after_parse)||c->after_parse.tick<c->before_parse.tick||
 c->after_parse.tick>=b->source_original->expires)return deny(c,b);
 c->byte_tuple_corresponds=1;c->failed=0;b->prior_tick=c->after_parse.tick;return 1;
}
#endif
