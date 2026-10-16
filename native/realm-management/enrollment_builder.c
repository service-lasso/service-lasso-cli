#include "enrollment_builder.h"
#include "../host-service/wire.h"
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
struct slcli_management_enrollment_builder *slcli_management_enrollment_builder_allocate(struct slcli_arena *a) {
  struct slcli_management_enrollment_builder *b;
  if(!a)return 0;b=slcli_arena_calloc(a,1,sizeof(*b));if(!b)return 0;b->arena=a;b->failed=1;
  b->signer=slcli_management_enrollment_signature_allocate(a);if(b->signer)b->failed=0;return b;
}
static int immutable(struct slcli_management_enrollment_builder *b,const void *p,size_t n) {
  return p&&n&&slcli_arena_live_range(b->arena,p,n)&&away(b,sizeof(*b),p,n)&&
    away(b->signer,sizeof(*b->signer),p,n);
}
static int outside_credential(const struct slcli_input_body *c,const void *p,size_t n) {
  return away(c->body,c->capacity,p,n)&&away(c->readback,c->readback_capacity,p,n);
}
static int byte(struct slcli_management_enrollment_builder *b,unsigned char value) {
  if(b->bytes>=sizeof(b->raw))return 0;b->raw[b->bytes++]=value;return 1;
}
static int literal(struct slcli_management_enrollment_builder *b,const char *s) {
  while(*s)if(!byte(b,(unsigned char)*s++))return 0;return 1;
}
static int hex(struct slcli_management_enrollment_builder *b,const unsigned char *raw,size_t n) {
  static const char digits[]="0123456789abcdef";size_t i;
  if(!byte(b,'"'))return 0;
  for(i=0;i<n;i++)if(!byte(b,digits[raw[i]>>4])||!byte(b,digits[raw[i]&15]))return 0;
  return byte(b,'"');
}
static int version(struct slcli_management_enrollment_builder *b,const unsigned char *s,size_t n) {
  size_t i;if(!byte(b,'"'))return 0;
  for(i=0;i<n;i++) {
    if(s[i]=='"'||s[i]=='\\')if(!byte(b,'\\'))return 0;
    if(!byte(b,s[i]))return 0;
  }
  return byte(b,'"');
}
int slcli_management_enrollment_build(struct slcli_management_enrollment_builder *b,
  const struct slcli_management_enrollment_originals *o) {
  static const char *const names[SLCLI_HE_ASSOCIATIONS]={"realm","host","providerSource",
    "startupSource","installerSource","callerSource","storeRootIdentity","profileSha256",
    "grantSha256","credentialOriginal","managementEntrySourceSha256","anchorSha256"};
  size_t i,j;unsigned char any;const struct slcli_input_body *credential;
  if(!b||b->started||b->failed||b->bytes||b->prefix_bytes||b->complete||!b->arena||!b->signer||
     !slcli_arena_live_range(b->arena,b,sizeof(*b))||
     !slcli_arena_live_range(b->arena,b->signer,sizeof(*b->signer))||
     !away(b,sizeof(*b),b->signer,sizeof(*b->signer))||b->signer->arena!=b->arena||
     b->signer->started||b->signer->failed||!immutable(b,o,sizeof(*o))||
     !immutable(b,o->credential,sizeof(*o->credential))||
     !o->credential->complete||o->credential->failed||o->credential->arena!=b->arena||
     !(credential=o->credential->original)||!immutable(b,credential,sizeof(*credential))||
     !credential->complete||credential->failed||!credential->recheck_complete||
     !credential->eof_observed||credential->consumed!=1||credential->bytes!=64||
     credential->capacity<65||credential->readback_capacity<65||
     credential->capture_count<8||credential->capture_count>credential->capture_capacity||
     credential->capture_capacity>SIZE_MAX/sizeof(*credential->captures)||
     !immutable(b,credential->body,credential->capacity)||
     !immutable(b,credential->readback,credential->readback_capacity)||
     !immutable(b,credential->captures,credential->capture_capacity*sizeof(*credential->captures))||
     !slcli_equal(credential->body,credential->readback,64)||o->seed!=credential->body||
     o->credential->seed!=credential->body||o->credential->ledger!=credential->body+32||
     !slcli_equal(o->credential->raw_digest,credential->digest,32)||
     !outside_credential(credential,o,sizeof(*o))||
     (o->purpose!=1&&o->purpose!=2)||!o->key_version_bytes||o->key_version_bytes>64||
     !immutable(b,o->key_version,o->key_version_bytes)||
     !outside_credential(credential,o->key_version,o->key_version_bytes)||
     !slcli_utf8(o->key_version,o->key_version_bytes,0)||
     !immutable(b,o->seed,32)||!immutable(b,o->anchor,32)||
     !outside_credential(credential,o->anchor,32)||
     !away(o->seed,32,o->anchor,32)||
     !o->profile_bytes||o->profile_bytes>16384||!o->grant_bytes||o->grant_bytes>16384||
     !immutable(b,o->profile,o->profile_bytes)||!immutable(b,o->grant,o->grant_bytes)||
     !outside_credential(credential,o->profile,o->profile_bytes)||
     !outside_credential(credential,o->grant,o->grant_bytes)||
     !away(o->seed,32,o->profile,o->profile_bytes)||!away(o->seed,32,o->grant,o->grant_bytes))return 0;
  for(i=0;i<o->key_version_bytes;i++)if(o->key_version[i]<32||o->key_version[i]==127)return 0;
  for(i=0;i<SLCLI_HE_ASSOCIATIONS;i++) {
    if(!immutable(b,o->associations[i],32)||
       !outside_credential(credential,o->associations[i],32))return 0;
    any=0;for(j=0;j<32;j++)any|=o->associations[i][j];if(!any)return 0;
  }
  b->started=1;b->failed=1;b->originals=o;
  if(!slcli_profile_parse(o->profile,o->profile_bytes,&b->profile)||
     !slcli_grant_parse(o->grant,o->grant_bytes,b->profile.digest,&b->grant)||
     !slcli_equal(b->profile.digest,o->associations[SLCLI_HE_PROFILE],32)||
     !slcli_equal(b->grant.digest,o->associations[SLCLI_HE_GRANT],32))return 0;
  slcli_sha256_init(&b->hash);
  if(!slcli_sha256_update(&b->hash,o->anchor,32)||!slcli_sha256_finish(&b->hash,b->anchor_hash)||
     !slcli_equal(b->anchor_hash,o->associations[SLCLI_HE_ANCHOR],32))return 0;
  if(!literal(b,"{\"schema\":\"service-lasso.ha2-realm-enrollment.v1\",\"purpose\":")||
     !literal(b,o->purpose==1?"\"qualification\"":"\"production\""))return 0;
  for(i=0;i<SLCLI_HE_ASSOCIATIONS;i++) {
    if(i==SLCLI_HE_CREDENTIAL) {
      if(!literal(b,",\"keyVersion\":")||!version(b,o->key_version,o->key_version_bytes))return 0;
    }
    if(i==SLCLI_HE_MANAGEMENT_ENTRY)if(!literal(b,",\"initialGeneration\":\"1\""))return 0;
    if(!literal(b,",\"")||!literal(b,names[i])||!literal(b,"\":")||
       !hex(b,o->associations[i],32))return 0;
  }
  if(!literal(b,",\"flags\":0"))return 0;b->prefix_bytes=b->bytes;
  if(!slcli_management_enrollment_signature_original(b->signer,o->seed,o->anchor,b->raw,b->prefix_bytes)||
     !literal(b,",\"signature\":")||!hex(b,b->signer->signature,64)||!literal(b,"}\n")||
     !slcli_ha2_enrollment_parse(b->raw,b->bytes,&b->decoded)||
     b->decoded.prefix_bytes!=b->prefix_bytes||b->decoded.purpose!=o->purpose||
     b->decoded.key_version_bytes!=o->key_version_bytes||
     !slcli_equal(b->decoded.key_version,o->key_version,o->key_version_bytes)||
     !slcli_equal(b->decoded.signature,b->signer->signature,64))return 0;
  for(i=0;i<SLCLI_HE_ASSOCIATIONS;i++)if(!slcli_equal(b->decoded.associations[i],o->associations[i],32))return 0;
  b->complete=1;b->failed=0;return 1;
}
int slcli_management_enrollment_positive_original(struct slcli_management_enrollment_builder *b,
  const struct slcli_management_enrollment_originals *o) {
  size_t i;unsigned char wiped=0;
  if(!slcli_management_enrollment_build(b,o))return 0;
  for(i=0;i<32;i++)wiped|=b->signer->temporary_seed[i];
  return b->complete&&!b->failed&&b->signer->signed_complete&&!b->signer->failed&&!wiped&&
    b->signer->preimage_bytes==SLCLI_HA2_ENROLLMENT_DOMAIN_BYTES+4+b->prefix_bytes&&
    b->decoded.raw==b->raw&&b->decoded.bytes==b->bytes;
}
