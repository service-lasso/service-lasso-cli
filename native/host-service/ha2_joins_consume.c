#include "ha2_joins_consume.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
struct slcli_ha2_joins_consumer *slcli_ha2_joins_consumer_allocate(struct slcli_arena *a,size_t rows) {
  struct slcli_ha2_joins_consumer *c;
  if(!a)return 0;c=slcli_arena_calloc(a,1,sizeof(*c));if(!c)return 0;c->arena=a;
  c->range=slcli_unit19_derivation_range_allocate(a,1,rows);
  if(!c->range||c->range->failed)c->failed=1;return c;
}
static int original(struct slcli_ha2_joins_consumer *c,const void *p,size_t n) {
  struct slcli_unit19_range *r=c->range;struct slcli_input_body *s=r->copies;
  return p&&n&&slcli_arena_live_range(c->arena,p,n)&&away(c,sizeof(*c),p,n)&&
    away(r,sizeof(*r),p,n)&&away(s,sizeof(*s),p,n)&&away(s->body,s->capacity,p,n)&&
    away(s->readback,s->readback_capacity,p,n)&&
    away(s->captures,s->capture_capacity*sizeof(*s->captures),p,n);
}
static int storage(struct slcli_ha2_joins_consumer *c) {
  struct slcli_unit19_range *r=c->range;struct slcli_input_body *s;
  const void *p[6];size_t n[6],i,j;
  if(!r||!slcli_arena_live_range(c->arena,r,sizeof(*r))||r->arena!=c->arena||
     r->started||r->failed||!(s=r->copies)||!slcli_arena_live_range(c->arena,s,sizeof(*s))||
     s->started||s->failed||s->complete||s->consumed||s->capture_count||s->recheck_started||
     s->capture_capacity>SIZE_MAX/sizeof(*s->captures))return 0;
  p[0]=c;n[0]=sizeof(*c);p[1]=r;n[1]=sizeof(*r);p[2]=s;n[2]=sizeof(*s);
  p[3]=s->body;n[3]=s->capacity;p[4]=s->readback;n[4]=s->readback_capacity;
  p[5]=s->captures;n[5]=s->capture_capacity*sizeof(*s->captures);
  for(i=0;i<6;i++) {
    if(!slcli_arena_live_range(c->arena,p[i],n[i]))return 0;
    for(j=0;j<i;j++)if(!away(p[i],n[i],p[j],n[j]))return 0;
  }
  return 1;
}
int slcli_ha2_joins_consume(struct slcli_ha2_joins_consumer *c,
  const struct slcli_ha2_target_controls *controls,const struct slcli_unit19_reader *unit,
  slcli_input_handle handle,const struct slcli_unit19_reader *trial,
  const struct slcli_unit19_reader *certificates) {
  size_t i;struct slcli_input_body *b;
  static const uint32_t ordinals[5]={0,6,7,8,11};
  const struct slcli_ha2_derivation_contract *contract;
  if(!c||!c->arena||c->started||c->failed||
     !slcli_arena_live_range(c->arena,c,sizeof(*c))||!storage(c)||
     !original(c,controls,sizeof(*controls))||controls->arena!=c->arena||
     !controls->controls_decoded||controls->failed||
     !original(c,unit,sizeof(*unit))||!original(c,trial,sizeof(*trial))||
     !original(c,certificates,sizeof(*certificates))||
     unit->type!=SLCLI_HA2_DERIVE19||!unit->complete||unit->failed||unit->count!=12)return 0;
  for(i=0;i<5;i++) {
    const struct slcli_unit19_range *r=controls->ranges[i];const struct slcli_input_body *s;
    if(!original(c,r,sizeof(*r))||!r->complete||r->failed||r->unit!=unit||
       r->ordinal!=ordinals[i]||r->whole_file_bytes!=unit->file_bytes||!(s=r->copies)||
       !original(c,s,sizeof(*s))||!s->complete||!s->recheck_complete||s->consumed!=2||
       s->capture_capacity>SIZE_MAX/sizeof(*s->captures)||
       !original(c,s->body,s->capacity)||!original(c,s->readback,s->readback_capacity)||
       !original(c,s->captures,s->capture_capacity*sizeof(*s->captures)))return 0;
  }
  c->started=1;c->failed=1;
  if(!slcli_unit19_range_read(c->range,unit,1,handle))return 0;
  b=c->range->copies;contract=&controls->contract;
  if(!slcli_ha2_joins_parse(b->body,b->bytes,&c->joins)||
     !slcli_equal(c->joins.raw_digest,unit->records[1].digest,32)||
     !slcli_ha2_joins_unit_members(&c->joins,unit,trial,certificates)||
     !slcli_ha2_ref_same(&c->joins.entries[0].trial,&contract->refs[SLCLI_H2_SOURCE])||
     !slcli_ha2_ref_same(&c->joins.entries[9].trial,&contract->refs[SLCLI_H2_BQ])||
     !slcli_ha2_ref_same(&c->joins.entries[10].trial,&contract->refs[SLCLI_H2_SERVICE])||
     !slcli_ha2_ref_same(&c->joins.entries[18].trial,&contract->refs[SLCLI_H2_TRIAL_P])||
     !slcli_ha2_ref_same(&c->joins.entries[18].target,&contract->refs[SLCLI_H2_TARGET_P])||
     !slcli_ha2_ref_same(&c->joins.entries[23].trial,&contract->refs[SLCLI_H2_TRIAL_C])||
     !slcli_ha2_ref_same(&c->joins.entries[23].target,&contract->refs[SLCLI_H2_TARGET_C]))return 0;
  c->joins_decoded=1;c->failed=0;return 1;
}
int slcli_ha2_joins_positive_original(struct slcli_ha2_joins_consumer *c,
  const struct slcli_ha2_target_controls *controls,const struct slcli_unit19_reader *unit,
  slcli_input_handle handle,const struct slcli_unit19_reader *trial,
  const struct slcli_unit19_reader *certificates) {
  struct slcli_input_body *b;
  if(!slcli_ha2_joins_consume(c,controls,unit,handle,trial,certificates))return 0;
  b=c->range->copies;
  return c->joins_decoded&&!c->failed&&c->range->complete&&!c->range->failed&&
    b->complete&&b->recheck_complete&&b->eof_observed&&b->consumed==2&&
    b->capture_count>=6&&c->joins.raw==b->body&&c->joins.bytes==b->bytes&&
    slcli_equal(b->body,b->readback,b->bytes)&&slcli_equal(b->digest,c->joins.raw_digest,32);
}
#endif
