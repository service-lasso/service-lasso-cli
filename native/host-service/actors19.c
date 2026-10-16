#include "actors19.h"
#include "grant.h"
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int text_equal(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
  const char *expected) {
  unsigned char text[96];size_t n,i;
  if(!slcli_json_text(raw,bytes,span,text,sizeof(text),0,&n))return 0;
  for(i=0;i<n;i++)if(!expected[i]||text[i]!=(unsigned char)expected[i])return 0;
  return !expected[n];
}
static int hex_text(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
  unsigned char *out,size_t width) {
  size_t n,i;
  if(!slcli_json_text(raw,bytes,span,out,width,0,&n)||n!=width)return 0;
  for(i=0;i<n;i++)if(!((out[i]>='0'&&out[i]<='9')||(out[i]>='a'&&out[i]<='f')))return 0;
  return 1;
}
static int digest(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
  unsigned char out[32]) {
  unsigned char text[64],any=0;size_t i;
  if(!hex_text(raw,bytes,span,text,64))return 0;
  for(i=0;i<32;i++) {
    unsigned char a=text[2*i],b=text[2*i+1];
    a=a<='9'?a-'0':a-'a'+10;b=b<='9'?b-'0':b-'a'+10;
    out[i]=(unsigned char)(a*16+b);any|=out[i];
  }
  return any!=0;
}
int slcli_actors19_parse(const unsigned char *raw,size_t bytes,struct slcli_actors19 *out) {
  static const char *const root_keys[]={"schema","platform","actors"};
  static const char *const actor_keys[]={"role","sourceCommit","sourceAssociationSha256",
    "imageSha256","ownerIdentity","entrySourceSha256"};
  static const char *const roles[]={"bootstrap-authority","issuer","installer","input-provider",
    "service","native-primary","confined-writer","observer","sea","node-parent",
    "native-facade-sea","native-facade-library","library-transport","ledger-provider","public-facade"};
  struct slcli_actors19 result={0};struct slcli_json_span root,fields[3],actors[15],member[6];
  struct slcli_sha256 hash;unsigned char owner[184];size_t count,n,i;
  if(!out||!away(out,sizeof(*out),raw,bytes)||
     !slcli_json_validate(raw,bytes,16384,&root)||
     !slcli_json_object(raw,bytes,root,root_keys,3,fields)||
     !text_equal(raw,bytes,fields[0],"service-lasso-original-input-actors.v2")||
     !slcli_json_array(raw,bytes,fields[2],actors,15,&count)||count!=15)return 0;
#if defined(_WIN32)
  if(!text_equal(raw,bytes,fields[1],"win32"))return 0;
#elif defined(__linux__)
  if(!text_equal(raw,bytes,fields[1],"linux"))return 0;
#else
  return 0;
#endif
  for(i=0;i<15;i++) {
    struct slcli_actor19 *a=&result.actors[i];
    if(!slcli_json_object(raw,bytes,actors[i],actor_keys,6,member)||
       !text_equal(raw,bytes,member[0],roles[i])||
       !hex_text(raw,bytes,member[1],a->commit,40)||
       !digest(raw,bytes,member[2],a->association)||!digest(raw,bytes,member[3],a->image)||
       !slcli_json_text(raw,bytes,member[4],owner,sizeof(owner),0,&n)||!slcli_grant_owner(owner,n)||
       !digest(raw,bytes,member[5],a->entry))return 0;
    a->owner=member[4];
  }
  slcli_sha256_init(&hash);
  if(!slcli_sha256_update(&hash,raw,bytes)||!slcli_sha256_finish(&hash,result.digest))return 0;
  result.raw=raw;result.bytes=bytes;*out=result;return 1;
}
int slcli_actors19_role_unit_matches(const struct slcli_actors19 *a,const struct slcli_unit19_reader *r) {
  unsigned i;
  if(!a||!a->raw||!a->bytes||!r||!r->complete||r->failed||r->type!=SLCLI_ROLE19||
     r->count!=15||r->next!=15||!away(a,sizeof(*a),r,sizeof(*r)))return 0;
  for(i=0;i<15;i++)if(!slcli_equal(a->actors[i].association,r->records[i].digest,32))return 0;
  return 1;
}
int slcli_actors19_facade_profile_matches(const struct slcli_actors19 *a,const struct slcli_profile *p) {
  unsigned char image[32],binding_image[32];
  if(!a||!a->raw||!p||!p->raw||!away(a,sizeof(*a),p,sizeof(*p))||
     !digest(p->raw,p->raw_bytes,p->images[4],image)||
     !digest(p->raw,p->raw_bytes,p->image_bindings[3][4],binding_image))return 0;
  return slcli_equal(image,binding_image,32)&&slcli_equal(image,a->actors[14].image,32);
}
