/* SPDX-License-Identifier: Apache-2.0 */
#include "source_association_index.h"
static uint32_t u32(const unsigned char *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static uint64_t u64(const unsigned char *p){return (uint64_t)u32(p)|(uint64_t)u32(p+4)<<32;}
static int zero(const unsigned char *p,size_t n){size_t i;for(i=0;i<n;i++)if(p[i])return 0;return 1;}
static void copy(unsigned char *out,const unsigned char *in,size_t n){size_t i;for(i=0;i<n;i++)out[i]=in[i];}
static int compare(const unsigned char *a,const unsigned char *b,size_t n){size_t i;
 for(i=0;i<n;i++){if(a[i]<b[i])return -1;if(a[i]>b[i])return 1;}return 0;}
static int apart(const void *a,size_t n,const void *b,size_t m){uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);}
int slcli_source_association_header(struct slcli_source_association_reader *r,
 const unsigned char raw[128],uint64_t file_bytes,uint64_t expected_count){
 static const unsigned char magic[8]={'S','L','A','I','D','X','0','1'};
 uint64_t count,table,names,bytes;
 if(!r||!raw||!apart(r,sizeof(*r),raw,128)||!zero((const unsigned char *)r,sizeof(*r)))return 0;
 r->started=1;r->failed=1;copy(r->header,raw,128);
 if(compare(raw,magic,8)||u32(raw+8)!=1||u32(raw+12)!=320||!zero(raw+64,64))return 0;
 count=u64(raw+16);if(!count||count!=expected_count||count>(UINT64_MAX-128)/320)return 0;
 table=count*320;names=128+table;bytes=u64(raw+48);
 if(u64(raw+24)!=128||u64(raw+32)!=table||u64(raw+40)!=names||
  bytes<count||bytes>UINT64_MAX-names||u64(raw+56)!=names+bytes||file_bytes!=names+bytes)return 0;
 r->count=count;r->file_bytes=file_bytes;r->names_offset=names;r->names_bytes=bytes;
 r->next_name=names;r->failed=0;return 1;
}
static int mode(uint32_t kind,uint32_t value){
 if(kind==0||kind==4)return value==0;
 if(kind==1)return value==0||value==0160000;
 if(kind==2)return value==0||value==0040000;
 return value==0||value==0100644||value==0100755||value==0120000;
}
int slcli_source_association_record(struct slcli_source_association_reader *r,
 const unsigned char raw[320]){
 struct slcli_source_association_record v={0};
 if(!r||!raw||!apart(r,sizeof(*r),raw,320)||!r->started||r->failed||r->table_complete||r->next>=r->count)return 0;
 r->failed=1;r->attempted++;copy(r->attempted_record,raw,320);
 copy(v.association,raw,32);copy(v.capture,raw+32,32);copy(v.digest,raw+64,32);
 copy(v.git,raw+96,20);copy(v.revision,raw+116,20);copy(v.root,raw+136,20);
 v.kind=u32(raw+156);v.project=u32(raw+160);v.mode=u32(raw+164);
 v.name_offset=u64(raw+168);v.name_bytes=u64(raw+176);v.body_bytes=u64(raw+184);v.index=r->next;
 copy(v.provider,raw+192,32);copy(v.publication,raw+224,32);copy(v.provenance,raw+256,32);
 if(!zero(raw+288,32)||v.kind>4||!mode(v.kind,v.mode)||
  zero(v.association,32)||zero(v.capture,32)||zero(v.provider,32)||zero(v.publication,32)||zero(v.provenance,32)||
  !v.name_bytes||v.name_offset!=r->next_name||v.name_offset>r->file_bytes||v.name_bytes>r->file_bytes-v.name_offset)return 0;
 if(v.kind==0){if(!zero(v.git,20)||!zero(v.revision,20)||!zero(v.root,20))return 0;}
 else if(zero(v.git,20)||zero(v.revision,20)||zero(v.root,20))return 0;
 if(r->next&&(v.project<r->previous.project||
  (v.project==r->previous.project&&compare(v.association,r->previous.association,32)<=0)))return 0;
 if(r->next+1==r->count&&v.name_offset+v.name_bytes!=r->file_bytes)return 0;
 r->previous=v;r->next_name=v.name_offset+v.name_bytes;r->next++;
 if(r->next==r->count)r->table_complete=1;
 r->failed=0;return 1;
}
int slcli_source_association_names(struct slcli_source_association_reader *r,uint64_t offset,size_t bytes){
 if(!r||!r->started||r->failed||!r->table_complete||r->names_complete)return 0;
 r->failed=1;
 if(!bytes||bytes>16384||r->names_consumed>r->names_bytes||
  offset!=r->names_offset+r->names_consumed||(uint64_t)bytes>r->names_bytes-r->names_consumed)return 0;
 r->names_consumed+=(uint64_t)bytes;if(r->names_consumed==r->names_bytes)r->names_complete=1;
 r->failed=0;return 1;
}
