#include "product_route_image_windows.h"
#if defined(_WIN32)
static uint16_t u16(const unsigned char *p){return (uint16_t)((uint16_t)p[0]|((uint16_t)p[1]<<8));}
static uint32_t u32(const unsigned char *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static int range(const struct slcli_product_windows_route_image *c,uint32_t rva,uint64_t bytes,uint32_t *raw,uint32_t *which) {
 const unsigned char *s;uint32_t i,va,n,file,at,delta;unsigned found=0;
 if(!bytes||rva>c->image_bytes||bytes>c->image_bytes-rva)return 0;
 for(i=0;i<c->section_count;i++) {
 s=c->file_original->body_original+c->pe_offset+24+c->optional_bytes+(size_t)i*40;
 va=u32(s+12);n=u32(s+8);file=u32(s+16);at=u32(s+20);
 if(rva<va)continue;delta=rva-va;
 if(bytes>n||delta>n-bytes||bytes>file||delta>file-bytes)continue;
 if(at>UINT32_MAX-delta||at+delta>c->file_original->used_bytes||bytes>c->file_original->used_bytes-(at+delta))return 0;
 *raw=at+delta;*which=i;found++;
 }
 return found==1;
}
static const unsigned char *export_at(const struct slcli_product_windows_route_image *c,uint32_t raw,uint32_t rva,uint64_t bytes) {
 uint64_t delta;if(rva<c->export_rva)return NULL;delta=rva-c->export_rva;
 if(delta>c->export_bytes||bytes>c->export_bytes-delta)return NULL;
 return c->file_original->body_original+raw+(size_t)delta;
}
int slcli_product_windows_route_image_original(struct slcli_product_windows_route_image *out,
 const struct slcli_build_held_file_windows *file) {
 struct slcli_product_windows_route_image c={0};const unsigned char *b,*nt,*s,*exp,*names,*ordinals,*functions,*name;
 uint64_t total;uint32_t i,j,k,raw,section,previous=0,previous_end=0,va,n,file_bytes,file_at,headers,names_count,functions_count,index,name_rva,rva,flags,other_at,other_bytes;
 unsigned found=0;static const unsigned char wanted[]="slcli_product_route_cell_v1";
 static const unsigned char section_name[8]={'.','s','l','p','r','o','u','t'};
 if(!out||!slcli_build_held_file_windows_output_away_original(file,out,sizeof(*out)))return 0;
 total=file->used_bytes;b=file->body_original;if(total<64||total>SIZE_MAX||b[0]!='M'||b[1]!='Z')return 0;
 c.file_original=file;c.pe_offset=u32(b+60);
 if(c.pe_offset<64||c.pe_offset>total||total-c.pe_offset<24)return 0;nt=b+c.pe_offset;
 if(u32(nt)!=0x00004550||u16(nt+4)!=IMAGE_FILE_MACHINE_AMD64)return 0;
 c.section_count=u16(nt+6);c.optional_bytes=u16(nt+20);
 if(!c.section_count||c.optional_bytes<240||c.optional_bytes>total-c.pe_offset-24||u16(nt+24)!=IMAGE_NT_OPTIONAL_HDR64_MAGIC)return 0;
 c.image_bytes=u32(nt+24+56);headers=u32(nt+24+60);
 if(!c.image_bytes||headers>total||headers>c.image_bytes||u32(nt+24+108)<1||
 (uint64_t)c.section_count*40>total-c.pe_offset-24-c.optional_bytes||
 (uint64_t)c.pe_offset+24+c.optional_bytes+(uint64_t)c.section_count*40>headers)return 0;
 c.export_rva=u32(nt+24+112);c.export_bytes=u32(nt+24+116);
 if(c.export_bytes<40||c.export_rva>c.image_bytes||c.export_bytes>c.image_bytes-c.export_rva)return 0;
 for(i=0;i<c.section_count;i++) {
 s=nt+24+c.optional_bytes+(size_t)i*40;va=u32(s+12);n=u32(s+8);file_bytes=u32(s+16);file_at=u32(s+20);
 if(va<headers||!n||va>c.image_bytes||n>c.image_bytes-va||
 (i&&(va<=previous||va<previous_end))||file_at>total||file_bytes>total-file_at||
 (file_bytes&&file_at<headers))return 0;
 for(k=0;k<i&&file_bytes;k++) {
 const unsigned char *other=nt+24+c.optional_bytes+(size_t)k*40;
 other_at=u32(other+20);other_bytes=u32(other+16);
 if(other_bytes&&(uint64_t)file_at<(uint64_t)other_at+other_bytes&&
 (uint64_t)other_at<(uint64_t)file_at+file_bytes)return 0;
 }
 previous=va;previous_end=va+n;
 }
 if(!range(&c,c.export_rva,c.export_bytes,&raw,&section))return 0;exp=b+raw;
 functions_count=u32(exp+20);names_count=u32(exp+24);
 functions=export_at(&c,raw,u32(exp+28),(uint64_t)functions_count*4);
 names=export_at(&c,raw,u32(exp+32),(uint64_t)names_count*4);
 ordinals=export_at(&c,raw,u32(exp+36),(uint64_t)names_count*2);
 if(!functions_count||!names_count||!functions||!names||!ordinals)return 0;
 for(i=0;i<names_count;i++) {
 name_rva=u32(names+(size_t)i*4);name=export_at(&c,raw,name_rva,1);index=u16(ordinals+(size_t)i*2);
 if(!name||index>=functions_count)return 0;
 for(j=0;j<c.export_bytes-(name_rva-c.export_rva)&&name[j];j++){}
 if(j==c.export_bytes-(name_rva-c.export_rva))return 0;
 if(j!=sizeof(wanted)-1)continue;for(j=0;j<sizeof(wanted);j++)if(name[j]!=wanted[j])break;
 if(j!=sizeof(wanted))continue;
 rva=u32(functions+(size_t)index*4);
 if(++found!=1||(rva>=c.export_rva&&rva-c.export_rva<c.export_bytes)||!range(&c,rva,32,&c.route_raw,&section))return 0;
 s=nt+24+c.optional_bytes+(size_t)section*40;flags=u32(s+36);
 for(j=0;j<8;j++)if(s[j]!=section_name[j])return 0;
 if(!(flags&IMAGE_SCN_MEM_READ)||!(flags&IMAGE_SCN_MEM_WRITE)||(flags&IMAGE_SCN_MEM_EXECUTE)||!(flags&IMAGE_SCN_CNT_INITIALIZED_DATA))return 0;
 for(j=0;j<32;j++)if(b[(size_t)c.route_raw+j])return 0;c.route_rva=rva;
 }
 if(found!=1)return 0;*out=c;return 1;
}
#endif
