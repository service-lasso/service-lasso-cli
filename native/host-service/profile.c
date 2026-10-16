#include "profile.h"
#include "crypto.h"
#include "primary_wire.h"
static int text(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
                  size_t maximum,unsigned char *decoded,size_t *count) {
  return slcli_json_text(raw,bytes,span,decoded,maximum,0,count)&&*count;
}
static int bounded(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
                     size_t maximum) {
  unsigned char decoded[4096]; size_t count;
  return maximum<=sizeof(decoded)&&text(raw,bytes,span,maximum,decoded,&count);
}
static int equal_text(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
                        const char *expected) {
  unsigned char decoded[96]; size_t count,i;
  if(!text(raw,bytes,span,sizeof(decoded),decoded,&count)) return 0;
  for(i=0;i<count;++i) if(!expected[i]||decoded[i]!=(unsigned char)expected[i]) return 0;
  return !expected[count];
}
static int hex(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
                 size_t width,unsigned char *decoded) {
  size_t count,i;
  if(width>64||!text(raw,bytes,span,width,decoded,&count)||count!=width) return 0;
  for(i=0;i<count;++i) if(!((decoded[i]>='0'&&decoded[i]<='9')||
     (decoded[i]>='a'&&decoded[i]<='f'))) return 0;
  return 1;
}
static int digest(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
                    size_t width) {
  unsigned char decoded[64]; return hex(raw,bytes,span,width,decoded);
}
static int equal_digest(const unsigned char *raw,size_t bytes,
                          struct slcli_json_span a,struct slcli_json_span b) {
  unsigned char left[64],right[64];
  return hex(raw,bytes,a,64,left)&&hex(raw,bytes,b,64,right)&&slcli_equal(left,right,64);
}
static int number(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
                    uint64_t maximum,uint64_t *out) {
  return slcli_json_unsigned(raw,bytes,span,maximum,0,out);
}
static int exact_number(const unsigned char *raw,size_t bytes,
                          struct slcli_json_span span,uint64_t expected) {
  uint64_t value; return number(raw,bytes,span,expected,&value)&&value==expected;
}
static int endpoint(const unsigned char *raw,size_t bytes,
                       struct slcli_json_span *fields) {
  unsigned char name[4096]; size_t count,i,start;
  if(!text(raw,bytes,fields[1],sizeof(name),name,&count)) return 0;
#if defined(_WIN32)
  static const unsigned char prefix[]="\\\\.\\pipe\\";
  if(!equal_text(raw,bytes,fields[0],"named-pipe")||count<=sizeof(prefix)-1) return 0;
  for(i=0;i<sizeof(prefix)-1;++i) if(name[i]!=prefix[i]) return 0;
  for(;i<count;++i) if(name[i]=='\\') return 0;
  (void)start;
#elif defined(__linux__)
  if(!equal_text(raw,bytes,fields[0],"unix-socket")||name[0]!='/') return 0;
  start=1;
  for(i=1;i<=count;++i) {
    if(i<count&&name[i]=='\\') return 0;
    if(i<count&&name[i]!='/') continue;
    if(i==start||(i-start==1&&name[start]=='.')||
       (i-start==2&&name[start]=='.'&&name[start+1]=='.')) return 0;
    start=i+1;
  }
#else
  (void)i;(void)start;return 0;
#endif
  return 1;
}
int slcli_profile_parse(const unsigned char *raw,size_t bytes,struct slcli_profile *out) {
  static const char *const root_keys[]={"schema","platform","serviceSource","endpoint","owner","store","clientImages","control","retention","launch"};
  static const char *const service_keys[]={"repository","commit","imageSha256","sourceSha256"};
  static const char *const endpoint_keys[]={"kind","name"};
  static const char *const owner_keys[]={"nativeId"};
  static const char *const store_keys[]={"provider","rootIdentity","keyVersion","maximumBytes"};
  static const char *const images_keys[]={"primarySha256","observerSha256","launcherSha256","libraryFacadeSourceSha256","facadeSha256"};
  static const char *const control_keys[]={"maximumBindings","maximumReferencesPerBinding","maximumServiceBytes"};
  static const char *const retention_keys[]={"provider","rootIdentity","maximumBytes","maximumRecordBytes","minimumDays"};
  static const char *const launch_keys[]={"endpoint","primaryImageBinding","observerImageBinding","launcherImageBinding","libraryTransportBinding","maximumRequestBytes","maximumResultBytes","maximumStdoutBytes","maximumStderrBytes","maximumAggregateCaptureBytes","clientCopies","facadeImageBinding"};
  static const char *const image_keys[]={"repository","commit","blob","rawSha256","imageSha256"};
  static const char *const library_keys[]={"repository","commit","blob","rawSha256","nodeImageSha256","packageSourceSha256","inheritedChannelKind"};
  static const char *const copies_keys[]={"maximumBytes","maximumCopySets","release"};
  struct slcli_json_span root; struct slcli_sha256 hash;
  uintptr_t a=(uintptr_t)out,b=(uintptr_t)raw;
  unsigned char owner[184],first[4096],second[4096]; size_t count,n,m,i;
  uint64_t value,runtime_references;
  struct slcli_runtime_wire_geometry runtime_geometry;
  if(!out||!raw||a>UINTPTR_MAX-sizeof(*out)||b>UINTPTR_MAX-bytes||
     (a<b+bytes&&b<a+sizeof(*out))) return 0;
  slcli_erase(out,sizeof(*out));
  if(!slcli_json_validate(raw,bytes,16384,&root)||
     !slcli_json_object(raw,bytes,root,root_keys,10,out->root)||
     !equal_text(raw,bytes,out->root[0],"service-lasso.cli-host-profile.v5")) return 0;
#if defined(_WIN32)
  if(!equal_text(raw,bytes,out->root[1],"win32")) return 0;
#elif defined(__linux__)
  if(!equal_text(raw,bytes,out->root[1],"linux")) return 0;
#else
  return 0;
#endif
  if(!slcli_json_object(raw,bytes,out->root[2],service_keys,4,out->service)||
     !slcli_json_object(raw,bytes,out->root[3],endpoint_keys,2,out->endpoint)||
     !slcli_json_object(raw,bytes,out->root[4],owner_keys,1,out->owner)||
     !slcli_json_object(raw,bytes,out->root[5],store_keys,4,out->store)||
     !slcli_json_object(raw,bytes,out->root[6],images_keys,5,out->images)||
     !slcli_json_object(raw,bytes,out->root[7],control_keys,3,out->control)||
     !slcli_json_object(raw,bytes,out->root[8],retention_keys,5,out->retention)||
     !slcli_json_object(raw,bytes,out->root[9],launch_keys,12,out->launch)||
     !bounded(raw,bytes,out->service[0],256)||!digest(raw,bytes,out->service[1],40)||
     !digest(raw,bytes,out->service[2],64)||!digest(raw,bytes,out->service[3],64)||
     !endpoint(raw,bytes,out->endpoint)||!text(raw,bytes,out->owner[0],184,owner,&count)) return 0;
#if defined(__linux__)
  value=0;
  for(i=0;i<count;++i) {
    if(owner[i]<'0'||owner[i]>'9'||value>(UINT32_MAX-(owner[i]-'0'))/10) return 0;
    value=value*10+owner[i]-'0';
  }
#else
  if(count<4||owner[0]!='S'||owner[1]!='-'||owner[2]!='1'||owner[3]!='-') return 0;
  (void)value;
#endif
  if(!bounded(raw,bytes,out->store[0],64)||!digest(raw,bytes,out->store[1],64)||
     !bounded(raw,bytes,out->store[2],64)||!exact_number(raw,bytes,out->store[3],4194304)||
     !exact_number(raw,bytes,out->control[0],16)||
     !number(raw,bytes,out->control[1],UINT16_MAX-1u,&runtime_references)||!runtime_references||
     !slcli_runtime_wire_geometry_derive((uint32_t)runtime_references,&runtime_geometry)||
     !exact_number(raw,bytes,out->control[2],1048576)||
     !bounded(raw,bytes,out->retention[0],64)||!digest(raw,bytes,out->retention[1],64)||
     !number(raw,bytes,out->retention[2],UINT64_C(9007199254740991),&out->retention_maximum)||
     !number(raw,bytes,out->retention[3],out->retention_maximum,&out->record_maximum)||
     !exact_number(raw,bytes,out->retention[4],90)||
     !slcli_json_object(raw,bytes,out->launch[0],endpoint_keys,2,out->launch_endpoint)||
     !endpoint(raw,bytes,out->launch_endpoint)||
     !text(raw,bytes,out->endpoint[1],sizeof(first),first,&n)||
     !text(raw,bytes,out->launch_endpoint[1],sizeof(second),second,&m)||
     (n==m&&slcli_equal(first,second,n))) return 0;
  for(i=0;i<3;++i) {
    if(!slcli_json_object(raw,bytes,out->launch[i+1],image_keys,5,out->image_bindings[i])||
       !bounded(raw,bytes,out->image_bindings[i][0],256)||
       !digest(raw,bytes,out->image_bindings[i][1],40)||
       !digest(raw,bytes,out->image_bindings[i][2],40)||
       !digest(raw,bytes,out->image_bindings[i][3],64)||
       !equal_digest(raw,bytes,out->image_bindings[i][4],out->images[i])) return 0;
  }
  /* Ordinal4 is the independently installed public facade79/82, never the
   * installer/launcher ordinal3 or embedded/loaded library facade source. */
  if(!slcli_json_object(raw,bytes,out->launch[11],image_keys,5,out->image_bindings[3])||
     !bounded(raw,bytes,out->image_bindings[3][0],256)||
     !digest(raw,bytes,out->image_bindings[3][1],40)||
     !digest(raw,bytes,out->image_bindings[3][2],40)||
     !digest(raw,bytes,out->image_bindings[3][3],64)||
     !equal_digest(raw,bytes,out->image_bindings[3][4],out->images[4]))return 0;
  {
    unsigned char facade[64],source[64];unsigned char nz_image=0,nz_source=0;
    if(!hex(raw,bytes,out->images[4],64,facade)||
       !hex(raw,bytes,out->image_bindings[3][3],64,source))return 0;
    for(i=0;i<64;i++){nz_image|=(unsigned char)(facade[i]!='0');nz_source|=(unsigned char)(source[i]!='0');}
    if(!nz_image||!nz_source)return 0;
  }
  if(!digest(raw,bytes,out->images[3],64)||
     !slcli_json_object(raw,bytes,out->launch[4],library_keys,7,out->library)||
     !bounded(raw,bytes,out->library[0],256)||!digest(raw,bytes,out->library[1],40)||
     !digest(raw,bytes,out->library[2],40)||!digest(raw,bytes,out->library[3],64)||
     !digest(raw,bytes,out->library[4],64)||!digest(raw,bytes,out->library[5],64)||
     !bounded(raw,bytes,out->library[6],64)||
     !slcli_json_object(raw,bytes,out->launch[10],copies_keys,3,out->copies)||
     !exact_number(raw,bytes,out->copies[0],8388608)||!exact_number(raw,bytes,out->copies[1],128)||
     !equal_text(raw,bytes,out->copies[2],"original-parent-exit")||
     !number(raw,bytes,out->launch[5],16384,&out->request_maximum)||
     !number(raw,bytes,out->launch[6],16384,&out->result_maximum)||
     !number(raw,bytes,out->launch[9],out->record_maximum,&out->capture_maximum)||
     !number(raw,bytes,out->launch[7],out->capture_maximum,&out->stdout_maximum)||
     !number(raw,bytes,out->launch[8],out->capture_maximum,&out->stderr_maximum)) return 0;
  slcli_sha256_init(&hash);
  if(!slcli_sha256_update(&hash,raw,bytes)||!slcli_sha256_finish(&hash,out->digest)) return 0;
  out->runtime_references=(uint32_t)runtime_references;
  out->raw=raw;out->raw_bytes=bytes;
  return 1;
}
