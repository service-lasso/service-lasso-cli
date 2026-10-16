#include "ha2_journal_codec.h"
static uint32_t le32(const unsigned char *p) {
  return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
static uint64_t le64(const unsigned char *p) {
  return (uint64_t)le32(p)|((uint64_t)le32(p+4)<<32);
}
static void put_le32(unsigned char *p,uint32_t value) {
  unsigned i;for(i=0;i<4;i++)p[i]=(unsigned char)(value>>(i*8));
}
static void put_le64(unsigned char *p,uint64_t value) {
  put_le32(p,(uint32_t)value);put_le32(p+4,(uint32_t)(value>>32));
}
static int nonzero(const unsigned char *p,size_t bytes) {
  unsigned char n=0;size_t i;for(i=0;i<bytes;i++)n|=p[i];return n!=0;
}
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int prefix(const unsigned char *raw,size_t bytes,struct slcli_ha2_journal_record *out) {
  struct slcli_ha2_journal_record record={0};unsigned i;
  static const unsigned char magic[]="SLPKJ001";
  if(!raw||bytes!=SLCLI_HA2_JOURNAL_SIGNED_BYTES||!out||
     !away(out,sizeof(*out),raw,bytes)||!slcli_equal(raw,magic,8)||
     le32(raw+8)!=1||le32(raw+12)!=1||le32(raw+24)!=740||le32(raw+28)||le32(raw+36))return 0;
  record.raw=raw;record.sequence=le64(raw+16);record.state=le32(raw+32);
  record.generation=le64(raw+40);record.prior_offset=le64(raw+48);record.prior_bytes=le64(raw+56);
  if(!record.sequence||record.sequence>1024||!record.generation||
     record.state<SLCLI_HA2_PREPARED||record.state>SLCLI_HA2_QUARANTINED)return 0;
  for(i=0;i<18;i++)record.associations[i]=raw+64+i*32;
  /* All13 already-born foundation/request/process fields remain nonzero.
   * Lease/borrow/ledger/archive zero applicability is phase-specific and is
   * deliberately NOT granted by this representation reader. */
  for(i=0;i<13;i++)if(!nonzero(record.associations[i],32))return 0;
  if(record.sequence==1) {
    if(record.generation!=1||record.state!=SLCLI_HA2_PREPARED||
       record.prior_offset||record.prior_bytes||nonzero(record.associations[17],32))return 0;
  } else if(record.prior_bytes!=SLCLI_HA2_JOURNAL_BYTES||
      record.prior_offset!=(record.sequence-2)*SLCLI_HA2_JOURNAL_BYTES||
      !nonzero(record.associations[17],32))return 0;
  record.key_version_bytes=le32(raw+640);record.key_version=raw+644;
  if(!record.key_version_bytes||record.key_version_bytes>64||
     !slcli_utf8(record.key_version,record.key_version_bytes,0))return 0;
  for(i=0;i<record.key_version_bytes;i++)
    if(record.key_version[i]<32||record.key_version[i]==127)return 0;
  for(i=record.key_version_bytes;i<64;i++)if(record.key_version[i])return 0;
  *out=record;return 1;
}
int slcli_ha2_journal_record_parse(const unsigned char *raw,size_t bytes,
                                    struct slcli_ha2_journal_record *out) {
  struct slcli_ha2_journal_record record;
  if(!raw||!out||bytes!=SLCLI_HA2_JOURNAL_BYTES||
     !away(out,sizeof(*out),raw,bytes)||
     !prefix(raw,SLCLI_HA2_JOURNAL_SIGNED_BYTES,&record)||
     !nonzero(raw+SLCLI_HA2_JOURNAL_SIGNED_BYTES,64))return 0;
  record.signature=raw+SLCLI_HA2_JOURNAL_SIGNED_BYTES;*out=record;return 1;
}
int slcli_ha2_journal_prefix_original(unsigned char *raw,size_t bytes,
  const struct slcli_ha2_journal_prefix_originals *o) {
  static const unsigned char magic[]="SLPKJ001";
  struct slcli_ha2_journal_record record;size_t i;
  if(!raw||bytes!=SLCLI_HA2_JOURNAL_SIGNED_BYTES||!o||
     !away(raw,bytes,o,sizeof(*o))||
     !away(raw,bytes,o->associations,18*32)||
     !away(raw,bytes,o->key_version,o->key_version_bytes)||
     !o->sequence||o->sequence>1024||!o->generation||
     o->state<SLCLI_HA2_PREPARED||o->state>SLCLI_HA2_QUARANTINED||
     !o->key_version_bytes||o->key_version_bytes>64||
     !slcli_utf8(o->key_version,o->key_version_bytes,0))return 0;
  for(i=0;i<13;i++)if(!nonzero(o->associations[i],32))return 0;
  for(i=0;i<o->key_version_bytes;i++)if(o->key_version[i]<32||o->key_version[i]==127)return 0;
  if(o->sequence==1) {
    if(o->generation!=1||o->state!=SLCLI_HA2_PREPARED||
       nonzero(o->associations[17],32))return 0;
  } else if(!nonzero(o->associations[17],32))return 0;
  for(i=0;i<bytes;i++)raw[i]=0;
  for(i=0;i<8;i++)raw[i]=magic[i];
  put_le32(raw+8,1);put_le32(raw+12,1);put_le64(raw+16,o->sequence);
  put_le32(raw+24,740);put_le32(raw+32,o->state);put_le64(raw+40,o->generation);
  if(o->sequence>1) {
    put_le64(raw+48,(o->sequence-2)*SLCLI_HA2_JOURNAL_BYTES);
    put_le64(raw+56,SLCLI_HA2_JOURNAL_BYTES);
  }
  for(i=0;i<18*32;i++)raw[64+i]=((const unsigned char *)o->associations)[i];
  put_le32(raw+640,(uint32_t)o->key_version_bytes);
  for(i=0;i<o->key_version_bytes;i++)raw[644+i]=o->key_version[i];
  return prefix(raw,bytes,&record);
}
size_t slcli_ha2_journal_signature_preimage(unsigned char *output,size_t capacity,
                                            const unsigned char *raw,size_t bytes) {
  static const unsigned char domain[]=SLCLI_HA2_JOURNAL_SIGNATURE_DOMAIN;
  struct slcli_ha2_journal_record record;size_t i,n=SLCLI_HA2_JOURNAL_PREIMAGE_BYTES;
  if(!output||capacity<n||!away(output,n,raw,bytes)||
     !prefix(raw,bytes,&record))return 0;
  for(i=0;i<sizeof(domain);i++)output[i]=domain[i]; /* Includes exact NUL. */
  slcli_put_u32(output+sizeof(domain),SLCLI_HA2_JOURNAL_SIGNED_BYTES);
  for(i=0;i<SLCLI_HA2_JOURNAL_SIGNED_BYTES;i++)output[sizeof(domain)+4+i]=raw[i];
  return n;
}
