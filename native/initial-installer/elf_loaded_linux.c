#include "elf_loaded_linux.h"
#include <string.h>
static uint16_t u16(const unsigned char *p){return (uint16_t)((uint16_t)p[0]|((uint16_t)p[1]<<8));}
static uint32_t u32(const unsigned char *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static uint64_t u64(const unsigned char *p){return (uint64_t)u32(p)|((uint64_t)u32(p+4)<<32);}
static int away(const void *a,size_t an,const void *b,size_t bn){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&an<=UINTPTR_MAX-x&&bn<=UINTPTR_MAX-y&&(x+an<=y||y+bn<=x);
}
int slcli_elf64_header_decode_original(const unsigned char *p,size_t n,uint64_t bytes,
 const unsigned char *z,size_t zn,struct slcli_elf64_header_original *out){
 struct slcli_elf64_header_original t;uint16_t count;
 if(n!=64||!away(out,sizeof(*out),p,n)||bytes<64)return 0;
 if(z||zn){if(!z||zn!=64||!away(out,sizeof(*out),z,zn))return 0;}
 if(p[0]!=127||p[1]!='E'||p[2]!='L'||p[3]!='F'||p[4]!=2||p[5]!=1||p[6]!=1||
   u16(p+18)!=62||u32(p+20)!=1||u16(p+52)!=64||u16(p+54)!=56)return 0;
 memset(&t,0,sizeof(t));t.type=u16(p+16);if(t.type!=2&&t.type!=3)return 0;
 t.file_bytes=bytes;t.entry=u64(p+24);t.program_offset=u64(p+32);
 t.section_offset=u64(p+40);t.flags=u32(p+48);count=u16(p+56);
 if(count==65535){
  if(!z||zn!=64||u16(p+58)!=64||!t.section_offset||t.section_offset>bytes||64>bytes-t.section_offset)return 0;
  if(u32(z+4)!=0)return 0;t.program_count=u32(z+44);if(t.program_count<65535)return 0;
 }else {if(z||zn)return 0;t.program_count=count;}
 if(!t.program_count||t.program_offset>bytes||t.program_count>(bytes-t.program_offset)/56)return 0;
 *out=t;return 1;
}
int slcli_elf64_program_decode_original(const struct slcli_elf64_header_original *h,
 uint64_t index,const unsigned char *p,size_t n,struct slcli_elf64_program_original *out){
 struct slcli_elf64_program_original t;uint64_t delta;
 if(n!=56||!away(out,sizeof(*out),p,n)||!away(out,sizeof(*out),h,sizeof(*h)))return 0;
 if(!h->program_count||index>=h->program_count||h->program_offset>h->file_bytes||
   h->program_count>(h->file_bytes-h->program_offset)/56)return 0;
 delta=index*56;memset(&t,0,sizeof(t));t.index=index;t.file_header_offset=h->program_offset+delta;
 t.type=u32(p);t.flags=u32(p+4);t.offset=u64(p+8);t.virtual_address=u64(p+16);
 t.physical_address=u64(p+24);t.file_bytes=u64(p+32);t.memory_bytes=u64(p+40);t.alignment=u64(p+48);
  if(t.type==1){
   if(t.offset>h->file_bytes||t.file_bytes>h->file_bytes-t.offset)return 0;
 }
 if(t.type==1&&t.memory_bytes>UINT64_MAX-t.virtual_address)return 0;
 if(t.type==1&&(t.file_bytes>t.memory_bytes||
   (t.alignment>1&&((t.alignment&(t.alignment-1))||t.offset%t.alignment!=t.virtual_address%t.alignment))))return 0;
 *out=t;return 1;
}