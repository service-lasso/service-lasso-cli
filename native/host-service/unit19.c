#include "unit19.h"
static const uint64_t derive_limits[]={16384,131072,65536,524288,1048576,65536,
  16384,16384,16384,16384,16384,16384};
uint64_t slcli_unit19_derive_member_limit(uint32_t ordinal) {
  return ordinal<12?derive_limits[ordinal]:0;
}
static uint32_t little32(const unsigned char *p) {
  return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
static uint64_t little64(const unsigned char *p) {
  return (uint64_t)little32(p)|((uint64_t)little32(p+4)<<32);
}
static int nonzero32(const unsigned char *p) {
  unsigned char n=0;unsigned i;for(i=0;i<32;i++)n|=p[i];return n!=0;
}
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static void copy32(unsigned char *a,const unsigned char *b) {
  unsigned i;for(i=0;i<32;i++)a[i]=b[i];
}
int slcli_unit19_header(struct slcli_unit19_reader *r,const unsigned char raw[64],
  uint64_t observed_file_bytes,enum slcli_unit19_kind type,uint32_t source,
  const uint64_t *designated_limits,size_t limit_count) {
  static const char magic[][9]={"","SLROLE02","SLQ0U019","SLQ2U019","SLC0U019",
    "SLH2C017","SLQCTL01","SLH2D017","SLSPK001"};
  static const uint32_t counts[]={0,15,3,3,2,2,4,12,2};
  uint32_t n,i;uint64_t maximum=UINT64_C(17179869184),body;
  if(!r||!raw||type<SLCLI_ROLE19||type>SLCLI_HA2_COMPOUND19||
     !away(r,sizeof(*r),raw,64)||r->started||r->failed||r->complete)return 0;
  n=counts[type];body=64+(uint64_t)n*128;
  if(type==SLCLI_CONSUMER_CERT19||type==SLCLI_HA2_CERT19)maximum=33088;
  if(type==SLCLI_HA2_DERIVE19)maximum=1951296;
  if(type==SLCLI_HA2_COMPOUND19)maximum=320+32+16384;
  if(!slcli_equal(raw,(const unsigned char *)magic[type],8)||little32(raw+8)!=1||
     little32(raw+12)!=128||little64(raw+16)!=n||little64(raw+24)!=64||
     little64(raw+32)!=(uint64_t)n*128||little64(raw+40)!=body||
     little64(raw+48)!=observed_file_bytes||little64(raw+56)||
     observed_file_bytes<body||observed_file_bytes>maximum)return 0;
  if(type==SLCLI_CONSUMER0_19||type==SLCLI_CONSUMER2_19||type==SLCLI_HA2_CONTROL19) {
    if(!designated_limits||limit_count!=n||limit_count>SIZE_MAX/sizeof(*designated_limits)||
       !away(r,sizeof(*r),designated_limits,limit_count*sizeof(*designated_limits)))return 0;
    for(i=0;i<n;i++)if(!designated_limits[i]||designated_limits[i]>maximum)return 0;
    if((type==SLCLI_CONSUMER0_19||type==SLCLI_CONSUMER2_19)&&designated_limits[0]>16384)return 0;
    if(type==SLCLI_HA2_CONTROL19)
      for(i=0;i<3;i++)if(designated_limits[i]>16384)return 0;
  } else if(designated_limits||limit_count)return 0;
  r->started=1;r->failed=1;r->type=(uint32_t)type;r->count=n;r->next=0;
  r->source_project=source;r->file_bytes=observed_file_bytes;r->next_offset=body;
  for(i=0;i<n;i++) {
    uint64_t limit=maximum;
    if(type==SLCLI_HA2_DERIVE19)limit=derive_limits[i];
    else if(type==SLCLI_HA2_COMPOUND19)limit=i?16384:32;
    else if(type==SLCLI_CONSUMER_CERT19||type==SLCLI_HA2_CERT19)limit=16384;
    else if(designated_limits)limit=designated_limits[i];
    r->member_limits[i]=limit;
  }
  r->failed=0;return 1;
}
int slcli_unit19_record(struct slcli_unit19_reader *r,const unsigned char raw[128]) {
  struct slcli_unit19_record *member;uint64_t offset,bytes;uint32_t i;
  if(!r||!raw||!away(r,sizeof(*r),raw,128)||!r->started||r->failed||
     r->complete||r->next>=r->count||r->count>15)return 0;
  r->failed=1;offset=little64(raw+96);bytes=little64(raw+104);
  if(!nonzero32(raw)||!nonzero32(raw+32)||!nonzero32(raw+64)||
     slcli_equal(raw,raw+32,32)||
     offset!=r->next_offset||offset>r->file_bytes||bytes>r->file_bytes-offset||
     bytes>r->member_limits[r->next]||little32(raw+112)!=0||
     little32(raw+116)!=r->source_project||little64(raw+120))return 0;
  if(r->type==SLCLI_HA2_COMPOUND19&&
     ((!r->next&&bytes!=32)||(r->next&&!bytes)))return 0;
  for(i=0;i<r->next;i++) {
    if(slcli_equal(raw,r->records[i].association,32)||
       slcli_equal(raw,r->records[i].capture,32)||
       slcli_equal(raw+32,r->records[i].association,32)||
       slcli_equal(raw+32,r->records[i].capture,32))return 0;
  }
  member=&r->records[r->next];copy32(member->association,raw);
  copy32(member->capture,raw+32);copy32(member->digest,raw+64);
  member->offset=offset;member->bytes=bytes;member->kind=0;
  member->source_project=r->source_project;r->next_offset=offset+bytes;r->next++;
  if(r->next==r->count) {
    if(r->next_offset!=r->file_bytes)return 0;
    r->complete=1;
  }
  r->failed=0;return 1;
}
int slcli_unit19_member_matches(const struct slcli_unit19_reader *r,uint32_t ordinal,
  const struct slcli_unit19_record *original) {
  const struct slcli_unit19_record *member;
  if(!r||!original||!away(r,sizeof(*r),original,sizeof(*original))||
     !r->complete||r->failed||ordinal>=r->count||r->count>15)return 0;
  member=&r->records[ordinal];
  return member->offset==original->offset&&member->bytes==original->bytes&&
    member->kind==original->kind&&member->source_project==original->source_project&&
    slcli_equal(member->association,original->association,32)&&
    slcli_equal(member->capture,original->capture,32)&&slcli_equal(member->digest,original->digest,32);
}
static void put32(unsigned char *p,uint32_t n) {
  unsigned i;for(i=0;i<4;i++)p[i]=(unsigned char)(n>>(i*8));
}
static void put64(unsigned char *p,uint64_t n) {
  unsigned i;for(i=0;i<8;i++)p[i]=(unsigned char)(n>>(i*8));
}
int slcli_unit19_encode(struct slcli_unit19_framing *f,enum slcli_unit19_kind type,
  uint32_t source,const struct slcli_unit19_record *members,size_t count,
  const uint64_t *limits,size_t limit_count) {
  static const char magic[][9]={"","SLROLE02","SLQ0U019","SLQ2U019","SLC0U019",
    "SLH2C017","SLQCTL01","SLH2D017","SLSPK001"};
  uint64_t bytes,offset;size_t i,j;
  if(!f||f->started||f->failed||f->complete||type<SLCLI_ROLE19||type>SLCLI_HA2_COMPOUND19||
     !members||!count||count>15||!away(f,sizeof(*f),members,count*sizeof(*members))||
     (limits&&(!limit_count||limit_count>15||!away(f,sizeof(*f),limits,limit_count*sizeof(*limits)))))return 0;
  bytes=64+(uint64_t)count*128;
  for(i=0;i<count;i++) {
    if(members[i].bytes>UINT64_MAX-bytes||members[i].kind||members[i].source_project!=source)return 0;
    bytes+=members[i].bytes;
  }
  f->started=1;f->failed=1;
  for(i=0;i<64;i++)f->header[i]=0;
  for(i=0;i<15;i++)for(j=0;j<128;j++)f->table[i][j]=0;
  for(i=0;i<8;i++)f->header[i]=(unsigned char)magic[type][i];
  put32(f->header+8,1);put32(f->header+12,128);put64(f->header+16,count);
  put64(f->header+24,64);put64(f->header+32,(uint64_t)count*128);
  put64(f->header+40,64+(uint64_t)count*128);put64(f->header+48,bytes);
  if(!slcli_unit19_header(&f->checked,f->header,bytes,type,source,limits,limit_count)||
     f->checked.count!=count)return 0;
  offset=64+(uint64_t)count*128;
  for(i=0;i<count;i++) {
    unsigned char *row=f->table[i];
    if(members[i].offset!=offset)return 0;
    copy32(row,members[i].association);copy32(row+32,members[i].capture);copy32(row+64,members[i].digest);
    put64(row+96,offset);put64(row+104,members[i].bytes);put32(row+116,source);
    if(!slcli_unit19_record(&f->checked,row))return 0;
    offset+=members[i].bytes;
  }
  if(!f->checked.complete||offset!=bytes)return 0;
  for(i=0;i<count;i++)if(!slcli_unit19_member_matches(&f->checked,(uint32_t)i,&members[i]))return 0;
  f->complete=1;f->failed=0;return 1;
}
