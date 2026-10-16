#include "ha2_target_controls.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int storage(struct slcli_ha2_target_controls *c,const void **p,size_t *n) {
  size_t i,j,k=1;p[0]=c;n[0]=sizeof(*c);
  for(i=0;i<5;i++) {
    struct slcli_unit19_range *r=c->ranges[i];struct slcli_input_body *s;
    if(!r||!slcli_arena_live_range(c->arena,r,sizeof(*r))||r->arena!=c->arena||
      r->started||r->failed||!(s=r->copies)||
      !slcli_arena_live_range(c->arena,s,sizeof(*s))||s->started||s->failed||
      s->complete||s->consumed||s->recheck_started||s->capture_count||
      s->capture_capacity>SIZE_MAX/sizeof(*s->captures))return 0;
    p[k]=r;n[k++]=sizeof(*r);p[k]=s;n[k++]=sizeof(*s);
    p[k]=s->body;n[k++]=s->capacity;p[k]=s->readback;n[k++]=s->readback_capacity;
    p[k]=s->captures;n[k++]=s->capture_capacity*sizeof(*s->captures);
  }
  for(i=0;i<26;i++) {
    if(!slcli_arena_live_range(c->arena,p[i],n[i]))return 0;
    for(j=0;j<i;j++)if(!away(p[i],n[i],p[j],n[j]))return 0;
  }
  return 1;
}
static int original(struct slcli_ha2_target_controls *c,const void **p,const size_t *n,
  const void *raw,size_t bytes) {
  size_t i;if(!slcli_arena_live_range(c->arena,raw,bytes))return 0;
  for(i=0;i<26;i++)if(!away(p[i],n[i],raw,bytes))return 0;return 1;
}
struct slcli_ha2_target_controls *slcli_ha2_target_controls_allocate(struct slcli_arena *a,
  const size_t rows[5]) {
  struct slcli_ha2_target_controls *c;size_t i;
  if(!a||!rows)return 0;c=slcli_arena_calloc(a,1,sizeof(*c));if(!c)return 0;c->arena=a;
  for(i=0;i<5;i++) {
    c->ranges[i]=slcli_unit19_range_allocate(a,16384,rows[i]);
    if(!c->ranges[i]||c->ranges[i]->failed){c->failed=1;return 0;}
  }
  return c;
}
int slcli_ha2_target_controls_consume(struct slcli_ha2_target_controls *c,
  const struct slcli_unit19_reader *unit,slcli_input_handle handle,
  const struct slcli_unit19_reader *trial,const struct slcli_unit19_reader *certificates,
  const unsigned char source[32]) {
  static const uint32_t ordinal[]={0,6,7,8,11};
  const void *p[26];size_t n[26],i;struct slcli_input_body *b[5];
  if(!c||!c->arena||c->started||c->failed||
    !slcli_arena_live_range(c->arena,c,sizeof(*c))||!storage(c,p,n)||
    !original(c,p,n,unit,sizeof(*unit))||unit->type!=SLCLI_HA2_DERIVE19||
    !unit->complete||unit->failed||unit->count!=12||unit->file_bytes>1951296||
    !original(c,p,n,trial,sizeof(*trial))||trial->type!=SLCLI_HA2_CONTROL19||
    !trial->complete||trial->failed||trial->count!=4||
    !original(c,p,n,certificates,sizeof(*certificates))||
    certificates->type!=SLCLI_HA2_CERT19||!certificates->complete||certificates->failed||
    certificates->count!=2||!original(c,p,n,source,32))return 0;
  c->started=1;c->failed=1;
  for(i=0;i<5;i++) {
    if(!slcli_unit19_range_read(c->ranges[i],unit,ordinal[i],handle))return 0;
    b[i]=c->ranges[i]->copies;
  }
  if(!slcli_ha2_derivation_contract_parse(b[0]->body,b[0]->bytes,&c->contract)||
    !slcli_equal(c->contract.raw_digest,unit->records[0].digest,32)||
    !slcli_ha2_ref_member_matches(&c->contract.refs[SLCLI_H2_TRIAL_P],trial,23,0)||
    !slcli_ha2_ref_member_matches(&c->contract.refs[SLCLI_H2_TRIAL_C],certificates,22,0)||
    !slcli_ha2_ref_member_matches(&c->contract.refs[SLCLI_H2_TARGET_P],unit,26,6)||
    !slcli_ha2_ref_member_matches(&c->contract.refs[SLCLI_H2_TARGET_C],unit,26,11)||
    !slcli_ha2_ref_member_matches(&c->contract.refs[SLCLI_H2_CLOSURE],unit,26,3)||
    !slcli_ha2_ref_member_matches(&c->contract.refs[SLCLI_H2_ENVELOPE],unit,26,4)||
    !slcli_profile_parse(b[1]->body,b[1]->bytes,&c->profile)||
    !slcli_equal(c->profile.digest,unit->records[6].digest,32)||
    !slcli_grant_parse(b[2]->body,b[2]->bytes,c->profile.digest,&c->grant)||
    !slcli_equal(c->grant.digest,unit->records[7].digest,32)||
    !slcli_ha2_service_qualification_parse(b[3]->body,b[3]->bytes,c->profile.digest,
      &c->qualification)||!slcli_equal(c->qualification.source,source,32)||
    !slcli_equal(c->qualification.cases,unit->records[9].digest,32)||
    !slcli_equal(c->qualification.capacity,unit->records[10].digest,32)||
    !slcli_calibration_parse(b[4]->body,b[4]->bytes,c->profile.digest,&c->candidate)||
    c->candidate.domain!=1||c->candidate.maximum!=1048576||
    c->candidate.page!=c->qualification.page||
    c->candidate.objects>c->qualification.objects||
    !slcli_equal(c->candidate.source,source,32)||
    !slcli_equal(c->candidate.raw_digest,unit->records[11].digest,32))return 0;
  /* Deliberately a decoded INACTIVE target view. Table joins neither read the
   * large relation/envelope members nor certify their secret-sensitive source.
   * Actual native U4 equality/caller/source binding must remain a later gate. */
  c->failed=0;c->controls_decoded=1;return 1;
}
#endif
