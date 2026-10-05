#include "calibration.h"
#include "crypto.h"
static int nonzero(const unsigned char *value,size_t bytes) {
  unsigned char any=0; size_t i; for(i=0;i<bytes;++i) any|=value[i]; return any!=0;
}
static int text_equal(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
                        const char *expected) {
  unsigned char decoded[96]; size_t count,i;
  if(!slcli_json_text(raw,bytes,span,decoded,sizeof(decoded),0,&count)) return 0;
  for(i=0;i<count;++i) if(!expected[i]||decoded[i]!=(unsigned char)expected[i]) return 0;
  return !expected[count];
}
static int digest(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
                    unsigned char out[32]) {
  unsigned char text[64]; size_t count,i; uint32_t high,low;
  if(!slcli_json_text(raw,bytes,span,text,sizeof(text),0,&count)||count!=64) return 0;
  for(i=0;i<32;++i) {
    unsigned char a=text[i*2],b=text[i*2+1];
    if(a>='0'&&a<='9') high=a-'0'; else if(a>='a'&&a<='f') high=a-'a'+10; else return 0;
    if(b>='0'&&b<='9') low=b-'0'; else if(b>='a'&&b<='f') low=b-'a'+10; else return 0;
    out[i]=(unsigned char)((high<<4)|low);
  }
  return 1;
}
static int integer(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
                     uint64_t maximum,int zero,uint64_t *out) {
  return slcli_json_unsigned(raw,bytes,span,maximum,zero,out);
}
static int name_valid(const unsigned char *name,size_t bytes) {
  size_t i;
  if(!bytes||bytes>240||(bytes==1&&name[0]=='.')||
     (bytes==2&&name[0]=='.'&&name[1]=='.')) return 0;
  for(i=0;i<bytes;++i) if(!((name[i]>='a'&&name[i]<='z')||
     (name[i]>='A'&&name[i]<='Z')||(name[i]>='0'&&name[i]<='9')||
     name[i]=='.'||name[i]=='_'||name[i]=='-')) return 0;
  return 1;
}
int slcli_qualification_parse(const unsigned char *raw,size_t bytes,
                               const unsigned char profile[32],struct slcli_qualification *out) {
  static const char *const keys[]={"schema","profileDigest","sourceDigest","imageSha256",
    "platform","domain","maximumBytes","physicalPageBytes","maximumObjects",
    "caseSetDigest","capacityPlanDigest","entrySourceDigest"};
  struct slcli_json_span root,fields[12]; struct slcli_qualification result={0};
  uint64_t domain,objects;
  if(out) slcli_erase(out,sizeof(*out));
  if(!out||!profile||!nonzero(profile,32)||!slcli_json_validate(raw,bytes,16384,&root)||
     !slcli_json_object(raw,bytes,root,keys,12,fields)||
     !text_equal(raw,bytes,fields[0],"service-lasso-native-runtime-qualification.v1")||
     !digest(raw,bytes,fields[1],result.profile)||!slcli_equal(profile,result.profile,32)||
     !digest(raw,bytes,fields[2],result.source)||!nonzero(result.source,32)||
     !digest(raw,bytes,fields[3],result.image)||!nonzero(result.image,32)||
     !integer(raw,bytes,fields[5],2,1,&domain)||(domain!=0&&domain!=2)||
     !integer(raw,bytes,fields[6],UINT64_C(34359738368),0,&result.maximum)||
     result.maximum!=UINT64_C(34359738368)||
     !integer(raw,bytes,fields[7],result.maximum,0,&result.page)||
     (result.page&(result.page-1))||result.maximum%result.page||
     !integer(raw,bytes,fields[8],256,0,&objects)||objects!=256||
     !digest(raw,bytes,fields[9],result.cases)||!nonzero(result.cases,32)||
     !digest(raw,bytes,fields[10],result.capacity)||!nonzero(result.capacity,32)||
     !digest(raw,bytes,fields[11],result.entry)||!nonzero(result.entry,32)) return 0;
#if defined(__linux__)
  if(!text_equal(raw,bytes,fields[4],"linux")) return 0;
#elif defined(_WIN32)
  if(!text_equal(raw,bytes,fields[4],"win32")) return 0;
#else
  return 0;
#endif
  result.domain=(uint32_t)domain; result.objects=(uint32_t)objects;
  *out=result; return 1;
}
int slcli_calibration_parse(const unsigned char *raw,size_t bytes,
                             const unsigned char profile[32],struct slcli_calibration *out) {
  static const char *const keys[]={"schema","profileDigest","sourceDigest","domain",
    "maximumBytes","nativeObjectBytes","runtimeBaseBytes","stackReservationBytes",
    "ioReservationBytes","physicalPageBytes","maximumObjects","rawEvidence","sourceOwnedPeakBytes"};
  static const char *const evidence_keys[]={"name","sha256","size","sourceDigest"};
  struct slcli_json_span root,fields[13],entries[128],item[4];
  struct slcli_sha256 hash;
  uint64_t domain,objects,total;
  size_t count,i,j,name_bytes;
  if(!out) return 0;
  slcli_erase(out,sizeof(*out));
  if(!profile||!nonzero(profile,32)||!slcli_json_validate(raw,bytes,16384,&root)||
     !slcli_json_object(raw,bytes,root,keys,13,fields)||
     !text_equal(raw,bytes,fields[0],"service-lasso-native-allocation-calibration.v1")||
     !digest(raw,bytes,fields[1],out->profile)||!slcli_equal(profile,out->profile,32)||
     !digest(raw,bytes,fields[2],out->source)||!nonzero(out->source,32)||
     !integer(raw,bytes,fields[3],4,1,&domain)||domain==3||
     !integer(raw,bytes,fields[4],UINT64_C(9007199254740991),0,&out->maximum)||
     (domain==1&&out->maximum!=1048576)||
     !integer(raw,bytes,fields[5],out->maximum,0,&out->metadata)||
     !integer(raw,bytes,fields[6],out->maximum,0,&out->runtime)||
     !integer(raw,bytes,fields[7],out->maximum,0,&out->stack)||
     !integer(raw,bytes,fields[8],out->maximum,0,&out->io)||
     !integer(raw,bytes,fields[9],out->maximum,0,&out->page)||(out->page&(out->page-1))||
     !integer(raw,bytes,fields[10],256,0,&objects)||
     !integer(raw,bytes,fields[12],out->maximum,0,&out->peak)||
     !slcli_json_array(raw,bytes,fields[11],entries,128,&count)||!count) goto failed;
  if(out->stack>out->maximum-out->runtime) goto failed;
  total=out->runtime+out->stack;
  if(out->io>out->maximum-total) goto failed;
  total+=out->io; if(out->peak>total) goto failed;
  for(i=0;i<count;++i) {
    struct slcli_evidence_reference *e=&out->evidence[i];
    if(!slcli_json_object(raw,bytes,entries[i],evidence_keys,4,item)||
       !slcli_json_text(raw,bytes,item[0],e->name,sizeof(e->name),0,&name_bytes)||
       !name_valid(e->name,name_bytes)||!digest(raw,bytes,item[1],e->digest)||
       !integer(raw,bytes,item[2],8388608,0,&e->bytes)||
       !digest(raw,bytes,item[3],e->source)||!slcli_equal(e->source,out->source,32)) goto failed;
    e->name_bytes=(uint32_t)name_bytes;
    for(j=0;j<i;++j) if(e->name_bytes==out->evidence[j].name_bytes&&
      slcli_equal(e->name,out->evidence[j].name,e->name_bytes)) goto failed;
  }
  slcli_sha256_init(&hash);
  if(!slcli_sha256_update(&hash,raw,bytes)||!slcli_sha256_finish(&hash,out->raw_digest)) goto failed;
  out->domain=(uint32_t)domain; out->objects=(uint32_t)objects;
  out->evidence_count=(uint32_t)count; out->raw=raw; out->raw_bytes=bytes;
  /* Category facts include image/metadata ownership; metadata is not added as
   * another physical pool. Q32 is never substituted for measured production B. */
  return 1;
failed:
  slcli_erase(out,sizeof(*out)); return 0;
}
