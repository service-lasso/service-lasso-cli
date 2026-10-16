#include "ha2_joins.h"
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
int slcli_ha2_ref_same(const struct slcli_ha2_ref *a,const struct slcli_ha2_ref *b) {
  return a&&b&&a->slot==b->slot&&a->offset==b->offset&&a->bytes==b->bytes&&
    slcli_equal(a->association,b->association,32)&&slcli_equal(a->capture,b->capture,32)&&
    slcli_equal(a->digest,b->digest,32)&&slcli_equal(a->source,b->source,32)&&
    slcli_equal(a->caller,b->caller,32)&&slcli_equal(a->birth,b->birth,32);
}
static int born(const struct slcli_ha2_ref *r) {
  return r->slot!=1&&r->slot!=4&&r->slot!=5&&r->slot!=76;
}
int slcli_ha2_joins_parse(const unsigned char *raw,size_t bytes,struct slcli_ha2_joins *out) {
  static const char *const keys[]={"schema","version","entries"};
  static const char *const row_keys[]={"role","trial","target","relation"};
  static const char *const roles[]={"source-unit","source-commit","entry","image",
    "compiler","linker","SDK","CRT","API","bq-birth","service-birth","bq-caller",
    "service-caller","store-root","retention-root","source-root","credential","realm",
    "profile","grant","qualification","cases","capacity","certificate","trial-lease",
    "trial-archive","layer1"};
  struct slcli_json_span root,f[3],rows[27],fields[4];
  struct slcli_ha2_joins result={0};struct slcli_sha256 hash;
  size_t count,i;uint64_t version;
  if(!out||!away(raw,bytes,out,sizeof(*out))||!bytes||bytes>131072||
     !slcli_json_validate(raw,bytes,262144,&root)||
     !slcli_json_object(raw,bytes,root,keys,3,f)||
     !slcli_json_canonical_ascii_object(raw,bytes,root,keys,f,3,1)||
     !text(raw,bytes,f[0],"service-lasso.ha2-derivation-joins.v1")||
     !slcli_json_unsigned(raw,bytes,f[1],1,0,&version)||version!=1||
     !slcli_json_array(raw,bytes,f[2],rows,27,&count)||count!=27)return 0;
  for(i=0;i<27;i++) {
    struct slcli_ha2_join *j=&result.entries[i];
    if(!slcli_json_object(raw,bytes,rows[i],row_keys,4,fields)||
       !slcli_json_canonical_ascii_object(raw,bytes,rows[i],row_keys,fields,4,0)||
       !text(raw,bytes,fields[0],roles[i])||
       !slcli_ha2_ref_parse(raw,bytes,fields[1],&j->trial)||
       !slcli_ha2_ref_parse(raw,bytes,fields[2],&j->target)||
       !born(&j->trial)||!born(&j->target))return 0;
    if(text(raw,bytes,fields[3],"same-original"))j->relation=SLCLI_H2_SAME;
    else if(text(raw,bytes,fields[3],"distinct-original"))j->relation=SLCLI_H2_DISTINCT;
    else if(text(raw,bytes,fields[3],"distinct-admitted-entry-same-source"))j->relation=SLCLI_H2_DISTINCT_ENTRY;
    else if(text(raw,bytes,fields[3],"trial-retained-no-production-alias"))j->relation=SLCLI_H2_TRIAL_RETAINED;
    else return 0;
    if(i<16||i>=25) {
      if(j->relation==SLCLI_H2_SAME) {
        if(!slcli_ha2_ref_same(&j->trial,&j->target))return 0;
      } else if(j->relation==SLCLI_H2_DISTINCT_ENTRY&&(i==2||i==11||i==12)) {
        if(slcli_equal(j->trial.association,j->target.association,32)||
           !slcli_equal(j->trial.source,j->target.source,32))return 0;
        /* Both actual designated entries and full allowlist/source closure
         * must still independently resolve; this predicate supplies none. */
      } else return 0;
    } else if(i==24) {
      if(j->relation!=SLCLI_H2_TRIAL_RETAINED||j->trial.slot!=24||
         !slcli_ha2_ref_same(&j->trial,&j->target))return 0;
    } else if(j->relation!=SLCLI_H2_DISTINCT||
              slcli_equal(j->trial.association,j->target.association,32))return 0;
  }
  if(result.entries[0].trial.slot!=12||result.entries[9].trial.slot!=41||
     result.entries[10].trial.slot!=45||result.entries[13].trial.slot!=21||
     result.entries[14].trial.slot!=98||result.entries[15].trial.slot!=97||
     result.entries[16].trial.slot!=16||result.entries[16].target.slot!=7||
     result.entries[17].trial.slot!=15||result.entries[17].target.slot!=6)return 0;
  for(i=18;i<=23;i++)if(result.entries[i].target.slot!=26||
    result.entries[i].trial.slot!=(i==21?25:i==23?22:23))return 0;
  for(i=25;i<27;i++)if(result.entries[i].trial.slot<106||
    result.entries[i].trial.slot>108)return 0;
  slcli_sha256_init(&hash);
  if(!slcli_sha256_update(&hash,raw,bytes)||!slcli_sha256_finish(&hash,result.raw_digest))return 0;
  result.raw=raw;result.bytes=bytes;*out=result;return 1;
}
int slcli_ha2_joins_unit_members(const struct slcli_ha2_joins *j,
  const struct slcli_unit19_reader *target,const struct slcli_unit19_reader *trial,
  const struct slcli_unit19_reader *certificates) {
  if(!j||!target||!trial||!certificates||target->type!=SLCLI_HA2_DERIVE19||
     target->count!=12||trial->type!=SLCLI_HA2_CONTROL19||trial->count!=4||
     certificates->type!=SLCLI_HA2_CERT19||certificates->count!=2)return 0;
  return slcli_ha2_ref_member_matches(&j->entries[18].trial,trial,23,0)&&
    slcli_ha2_ref_member_matches(&j->entries[19].trial,trial,23,1)&&
    slcli_ha2_ref_member_matches(&j->entries[20].trial,trial,23,2)&&
    slcli_ha2_ref_member_matches(&j->entries[22].trial,trial,23,3)&&
    slcli_ha2_ref_member_matches(&j->entries[23].trial,certificates,22,0)&&
    slcli_ha2_ref_member_matches(&j->entries[18].target,target,26,6)&&
    slcli_ha2_ref_member_matches(&j->entries[19].target,target,26,7)&&
    slcli_ha2_ref_member_matches(&j->entries[20].target,target,26,8)&&
    slcli_ha2_ref_member_matches(&j->entries[21].target,target,26,9)&&
    slcli_ha2_ref_member_matches(&j->entries[22].target,target,26,10)&&
    slcli_ha2_ref_member_matches(&j->entries[23].target,target,26,11);
}
