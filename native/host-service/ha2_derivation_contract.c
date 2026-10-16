#include "ha2_derivation_contract.h"
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int text_is(const unsigned char *raw,size_t bytes,struct slcli_json_span s,const char *v) {
  size_t i=0,at=s.start;
  if(s.end>bytes||at>=s.end||s.end-at<2||raw[at++]!='"')return 0;
  while(v[i])if(at>=s.end-1||raw[at++]!=(unsigned char)v[i++])return 0;
  return at+1==s.end&&raw[at]=='"';
}
static int hex(const unsigned char *raw,size_t bytes,struct slcli_json_span s,
  unsigned char *out,size_t count,int require_nonzero) {
  size_t i;unsigned char any=0,a,b;
  if(s.end>bytes||s.end<s.start||s.end-s.start!=2+count*2||
    raw[s.start]!='"'||raw[s.end-1]!='"')return 0;
  for(i=0;i<count;i++) {
    a=raw[s.start+1+2*i];b=raw[s.start+2+2*i];
    if(a>='0'&&a<='9')a-='0';else if(a>='a'&&a<='f')a=a-'a'+10;else return 0;
    if(b>='0'&&b<='9')b-='0';else if(b>='a'&&b<='f')b=b-'a'+10;else return 0;
    out[i]=(unsigned char)(16*a+b);any|=out[i];
  }
  return !require_nonzero||any;
}
static int decimal(const unsigned char *raw,size_t bytes,struct slcli_json_span s,uint64_t *out) {
  uint64_t value=0,d;size_t i,n;
  if(s.end>bytes||s.end<s.start||s.end-s.start<3||s.end-s.start>22||
    raw[s.start]!='"'||raw[s.end-1]!='"')return 0;
  n=s.end-s.start-2;if(n>1&&raw[s.start+1]=='0')return 0;
  for(i=s.start+1;i<s.end-1;i++) {
    if(raw[i]<'0'||raw[i]>'9')return 0;d=raw[i]-'0';
    if(value>(UINT64_MAX-d)/10)return 0;value=value*10+d;
  }
  *out=value;return 1;
}
int slcli_ha2_ref_parse(const unsigned char *raw,size_t bytes,struct slcli_json_span root,
  struct slcli_ha2_ref *out) {
  static const char *const keys[]={"slot","associationId","nativeCaptureAssociationId",
    "offset","bytes","rawSha256","sourceAssociationId","callerAssociationId","birthAssociationId"};
  struct slcli_json_span f[9];struct slcli_ha2_ref r={0};uint64_t slot;
  if(!out||!away(out,sizeof(*out),raw,bytes)||
    !slcli_json_object(raw,bytes,root,keys,9,f)||
    !slcli_json_canonical_ascii_object(raw,bytes,root,keys,f,9,0)||
    !slcli_json_unsigned(raw,bytes,f[0],128,0,&slot)||!slot||
    !hex(raw,bytes,f[1],r.association,32,1)||!hex(raw,bytes,f[2],r.capture,32,1)||
    !decimal(raw,bytes,f[3],&r.offset)||!decimal(raw,bytes,f[4],&r.bytes)||
    r.bytes>UINT64_MAX-r.offset||!hex(raw,bytes,f[5],r.digest,32,1)||
    !hex(raw,bytes,f[6],r.source,32,1)||!hex(raw,bytes,f[7],r.caller,32,1)||
    !hex(raw,bytes,f[8],r.birth,32,1))return 0;
  r.slot=(uint32_t)slot;*out=r;return 1;
}
static int slots(const unsigned char *raw,size_t bytes,struct slcli_json_span s,
  const uint32_t *wanted,size_t count) {
  struct slcli_json_span rows[10];size_t n,i;uint64_t value;
  if(!slcli_json_array(raw,bytes,s,rows,10,&n)||n!=count)return 0;
  for(i=0;i<count;i++)if(!slcli_json_unsigned(raw,bytes,rows[i],128,0,&value)||value!=wanted[i])return 0;
  return 1;
}
int slcli_ha2_derivation_contract_parse(const unsigned char *raw,size_t bytes,
  struct slcli_ha2_derivation_contract *out) {
  static const char *const keys[]={"schema","version","mode","platform","trialSlots",
    "productionSlots","bqBirth","serviceBirth","sourceUnit","sourceCommit","entrySource",
    "image","compilerApi","trialProfile","targetProfile","trialCertificate","targetCertificate",
    "caseMap","envelope","closure","activationRule","flags"};
  static const uint32_t trial[]={14,15,16,17,18,19,20,23,24,25};
  static const uint32_t production[]={5,6,7,8,9,10,11,4,76};
  static const uint32_t indexes[]={6,7,8,10,11,12,13,14,15,16,17,18,19};
  struct slcli_json_span root,f[22];struct slcli_ha2_derivation_contract r={0};
  struct slcli_sha256 h;uint64_t version,flags;size_t i;
  if(!out||!away(out,sizeof(*out),raw,bytes)||!slcli_json_validate(raw,bytes,16384,&root)||
    root.start!=0||root.end+1!=bytes||raw[root.end]!='\n'||
    !slcli_json_object(raw,bytes,root,keys,22,f)||
    !slcli_json_canonical_ascii_object(raw,bytes,root,keys,f,22,1)||
    !text_is(raw,bytes,f[0],"service-lasso.ha2-derivation.v1")||
    !slcli_json_unsigned(raw,bytes,f[1],1,0,&version)||version!=1||
    !text_is(raw,bytes,f[2],"source-closed-target-bytes")||
    !slots(raw,bytes,f[4],trial,10)||!slots(raw,bytes,f[5],production,9)||
    !hex(raw,bytes,f[9],r.source_commit,20,0)||
    !text_is(raw,bytes,f[20],"whole-u4-original-equality-before-install")||
    !slcli_json_unsigned(raw,bytes,f[21],0,1,&flags)||flags)return 0;
#if defined(_WIN32)
  if(!text_is(raw,bytes,f[3],"win32"))return 0;
#elif defined(__linux__)
  if(!text_is(raw,bytes,f[3],"linux"))return 0;
#else
  return 0;
#endif
  for(i=0;i<SLCLI_H2_REF_COUNT;i++) {
    if(!slcli_ha2_ref_parse(raw,bytes,f[indexes[i]],&r.refs[i]))return 0;
    /* These role plans are permitted in productionSlots only. Their actual
     * originals do not exist at unit26 construction, so no Ref may claim one. */
    if(r.refs[i].slot==1||r.refs[i].slot==4||r.refs[i].slot==5||r.refs[i].slot==76)return 0;
  }
  if(r.refs[SLCLI_H2_BQ].slot!=41||r.refs[SLCLI_H2_SERVICE].slot!=45||
    r.refs[SLCLI_H2_SOURCE].slot!=12||r.refs[SLCLI_H2_TRIAL_P].slot!=23||
    r.refs[SLCLI_H2_TARGET_P].slot!=26||r.refs[SLCLI_H2_TRIAL_C].slot!=22||
    r.refs[SLCLI_H2_TARGET_C].slot!=26)return 0;
  slcli_sha256_init(&h);
  if(!slcli_sha256_update(&h,raw,bytes)||!slcli_sha256_finish(&h,r.raw_digest))return 0;
  r.raw=raw;r.bytes=bytes;*out=r;return 1;
}
int slcli_ha2_ref_member_matches(const struct slcli_ha2_ref *ref,
  const struct slcli_unit19_reader *unit,uint32_t slot,uint32_t ordinal) {
  const struct slcli_unit19_record *r;
  if(!ref||!unit||!unit->started||!unit->complete||unit->failed||
    unit->next!=unit->count||unit->next_offset!=unit->file_bytes||
    ordinal>=unit->count||unit->count>15||ref->slot!=slot)return 0;
  r=&unit->records[ordinal];
  return ref->offset==r->offset&&ref->bytes==r->bytes&&
    slcli_equal(ref->association,r->association,32)&&slcli_equal(ref->capture,r->capture,32)&&
    slcli_equal(ref->digest,r->digest,32);
}
