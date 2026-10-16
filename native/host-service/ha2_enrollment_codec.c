#include "ha2_enrollment_codec.h"
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
static int text(const unsigned char *raw,size_t bytes,struct slcli_json_span s,const char *v) {
  size_t at=s.start,i=0;
  if(s.end>bytes||s.end<at||s.end-at<2||raw[at++]!='"')return 0;
  while(v[i])if(at>=s.end-1||raw[at++]!=(unsigned char)v[i++])return 0;
  return at+1==s.end&&raw[at]=='"';
}
static int hex(const unsigned char *raw,size_t bytes,struct slcli_json_span s,
  unsigned char *out,size_t n,int required) {
  size_t i;unsigned char a,b,any=0;
  if(s.end>bytes||s.end<s.start||s.end-s.start!=2+2*n||
     raw[s.start]!='"'||raw[s.end-1]!='"')return 0;
  for(i=0;i<n;i++) {
    a=raw[s.start+1+2*i];b=raw[s.start+2+2*i];
    if(a>='0'&&a<='9')a-='0';else if(a>='a'&&a<='f')a=a-'a'+10;else return 0;
    if(b>='0'&&b<='9')b-='0';else if(b>='a'&&b<='f')b=b-'a'+10;else return 0;
    out[i]=(unsigned char)(16*a+b);any|=out[i];
  }
  return !required||any;
}
static int ordered(const unsigned char *raw,size_t bytes,struct slcli_json_span root,
  const char *const *keys,const struct slcli_json_span *f,size_t count) {
  size_t at=1,i,j;
  if(root.start||root.end+1!=bytes||raw[0]!='{'||raw[root.end]!='\n')return 0;
  for(i=0;i<count;i++) {
    if(i&&(at>=root.end||raw[at++]!=','))return 0;
    if(at>=root.end||raw[at++]!='"')return 0;
    for(j=0;keys[i][j];j++)if(at>=root.end||raw[at++]!=(unsigned char)keys[i][j])return 0;
    if(at+2>root.end||raw[at++]!='"'||raw[at++]!=':'||f[i].start!=at||
       f[i].end<=at||f[i].end>root.end)return 0;
    at=f[i].end;
  }
  return at+1==root.end&&raw[at]=='}';
}
static int version_spelling(const unsigned char *raw,struct slcli_json_span s,
  const unsigned char *version,size_t n) {
  size_t i,at=s.start+1;
  for(i=0;i<n;i++) {
    if(version[i]=='"'||version[i]=='\\') {
      if(at>=s.end-1||raw[at++]!='\\')return 0;
    }
    if(at>=s.end-1||raw[at++]!=version[i])return 0;
  }
  return at+1==s.end;
}
int slcli_ha2_enrollment_parse(const unsigned char *raw,size_t bytes,struct slcli_ha2_enrollment *out) {
  static const char *const keys[]={"schema","purpose","realm","host","providerSource",
    "startupSource","installerSource","callerSource","storeRootIdentity","profileSha256",
    "grantSha256","keyVersion","credentialOriginal","initialGeneration",
    "managementEntrySourceSha256","anchorSha256","flags","signature"};
  static const unsigned indexes[SLCLI_HE_ASSOCIATIONS]={2,3,4,5,6,7,8,9,10,12,14,15};
  struct slcli_json_span root,f[18];struct slcli_ha2_enrollment result={0};
  struct slcli_sha256 hash;size_t i;uint64_t flags;
  if(!out||!away(out,sizeof(*out),raw,bytes)||
     !slcli_json_validate(raw,bytes,SLCLI_HA2_ENROLLMENT_LIMIT,&root)||
     !slcli_json_object(raw,bytes,root,keys,18,f)||!ordered(raw,bytes,root,keys,f,18)||
     !text(raw,bytes,f[0],"service-lasso.ha2-realm-enrollment.v1")||
     !text(raw,bytes,f[13],"1")||!slcli_json_unsigned(raw,bytes,f[16],0,1,&flags)||flags||
     !hex(raw,bytes,f[17],result.signature,64,0)||
     !slcli_json_text(raw,bytes,f[11],result.key_version,64,0,&result.key_version_bytes)||
     !result.key_version_bytes||!version_spelling(raw,f[11],result.key_version,result.key_version_bytes))return 0;
  if(text(raw,bytes,f[1],"qualification"))result.purpose=1;
  else if(text(raw,bytes,f[1],"production"))result.purpose=2;else return 0;
  for(i=0;i<SLCLI_HE_ASSOCIATIONS;i++)if(!hex(raw,bytes,f[indexes[i]],result.associations[i],32,1))return 0;
  result.prefix_bytes=f[16].end;result.raw=raw;result.bytes=bytes;
  slcli_sha256_init(&hash);
  if(!slcli_sha256_update(&hash,raw,bytes)||!slcli_sha256_finish(&hash,result.raw_digest))return 0;
  *out=result;return 1;
}
size_t slcli_ha2_enrollment_preimage(unsigned char *out,size_t capacity,
  const unsigned char *prefix,size_t bytes) {
  static const unsigned char domain[]="ServiceLassoHA2RealmEnrollment1";
  size_t i,n;
  if(!bytes||bytes>SLCLI_HA2_ENROLLMENT_LIMIT)return 0;
  n=sizeof(domain)+4+bytes;
  if(capacity<n||!away(out,capacity,prefix,bytes))return 0;
  for(i=0;i<sizeof(domain);i++)out[i]=domain[i];
  out[sizeof(domain)]=(unsigned char)(bytes>>24);out[sizeof(domain)+1]=(unsigned char)(bytes>>16);
  out[sizeof(domain)+2]=(unsigned char)(bytes>>8);out[sizeof(domain)+3]=(unsigned char)bytes;
  for(i=0;i<bytes;i++)out[sizeof(domain)+4+i]=prefix[i];return n;
}
