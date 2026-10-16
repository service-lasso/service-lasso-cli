#include "ha2_c22_consume.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int range_away(struct slcli_arena *a,const struct slcli_unit19_range *r,
  const void *p,size_t n) {
  const struct slcli_input_body *s;
  if(!r||!slcli_arena_live_range(a,r,sizeof(*r))||r->arena!=a||!(s=r->copies)||
    !slcli_arena_live_range(a,s,sizeof(*s))||
    s->capture_capacity>SIZE_MAX/sizeof(*s->captures)||
    !slcli_arena_live_range(a,s->body,s->capacity)||
    !slcli_arena_live_range(a,s->readback,s->readback_capacity)||
    !slcli_arena_live_range(a,s->captures,s->capture_capacity*sizeof(*s->captures)))return 0;
  return away(p,n,r,sizeof(*r))&&away(p,n,s,sizeof(*s))&&
    away(p,n,s->body,s->capacity)&&away(p,n,s->readback,s->readback_capacity)&&
    away(p,n,s->captures,s->capture_capacity*sizeof(*s->captures));
}
static int originals_away(const struct slcli_ha2_c22_consumer *c,const void *p,size_t n) {
  return slcli_arena_live_range(c->arena,p,n)&&away(c,sizeof(*c),p,n)&&
    range_away(c->arena,c->service,p,n)&&range_away(c->arena,c->bq,p,n);
}
static int fresh_storage(const struct slcli_ha2_c22_consumer *c) {
  const struct slcli_unit19_range *r[2]={c->service,c->bq};
  const void *p[11];size_t n[11],i,j,k=1;
  p[0]=c;n[0]=sizeof(*c);
  for(i=0;i<2;i++) {
    const struct slcli_input_body *s;
    /* This also validates every retained output block before its metadata is
     * dereferenced here; c is required disjoint from all such blocks. */
    if(!range_away(c->arena,r[i],c,sizeof(*c)))return 0;
    s=r[i]->copies;
    if(r[i]->started||r[i]->failed||s->started||s->failed||s->complete||
      s->recheck_started||s->consumed||s->capture_count)return 0;
    p[k]=r[i];n[k++]=sizeof(*r[i]);p[k]=s;n[k++]=sizeof(*s);
    p[k]=s->body;n[k++]=s->capacity;
    p[k]=s->readback;n[k++]=s->readback_capacity;
    p[k]=s->captures;n[k++]=s->capture_capacity*sizeof(*s->captures);
  }
  for(i=0;i<11;i++)for(j=0;j<i;j++)if(!away(p[i],n[i],p[j],n[j]))return 0;
  return 1;
}
static int profiles_disjoint(const struct slcli_input_body *a,const struct slcli_input_body *b) {
  const void *left[4]={a,a->body,a->readback,a->captures};
  const void *right[4]={b,b->body,b->readback,b->captures};
  size_t n[4]={sizeof(*a),a->capacity,a->readback_capacity,a->capture_capacity*sizeof(*a->captures)};
  size_t m[4]={sizeof(*b),b->capacity,b->readback_capacity,b->capture_capacity*sizeof(*b->captures)};
  size_t i,j;
  for(i=0;i<4;i++)for(j=0;j<4;j++)if(!away(left[i],n[i],right[j],m[j]))return 0;
  return 1;
}
static int profile_original(const struct slcli_ha2_c22_consumer *c,
  const struct slcli_input_body *p,uint32_t consumed) {
  struct slcli_sha256 h;unsigned char digest[32];
  if(!p||!originals_away(c,p,sizeof(*p))||!p->started||!p->complete||p->failed||
    !p->eof_observed||!p->recheck_started||!p->recheck_complete||
    p->consumed!=consumed||!p->bytes||p->bytes>16384||
    p->bytes>p->capacity||p->bytes>p->readback_capacity||
    p->capture_capacity>SIZE_MAX/sizeof(*p->captures)||
    !originals_away(c,p->body,p->capacity)||
    !originals_away(c,p->readback,p->readback_capacity)||
    !originals_away(c,p->captures,p->capture_capacity*sizeof(*p->captures))||
    !slcli_equal(p->body,p->readback,p->bytes))return 0;
  slcli_sha256_init(&h);
  return slcli_sha256_update(&h,p->body,p->bytes)&&slcli_sha256_finish(&h,digest)&&
    slcli_equal(digest,p->digest,32);
}
static int service_profile_range(const struct slcli_ha2_c22_consumer *c,
  const struct slcli_unit19_range *r) {
  const struct slcli_unit19_reader *u;
  if(!r||!originals_away(c,r,sizeof(*r))||r->arena!=c->arena||!r->started||!r->complete||r->failed||
    r->ordinal!=0||!(u=r->unit)||!originals_away(c,u,sizeof(*u))||
    u->type!=SLCLI_HA2_CONTROL19||!u->started||!u->complete||u->failed||
    u->count!=4||u->next!=4||u->next_offset!=u->file_bytes||
    !profile_original(c,r->copies,2)||r->whole_file_bytes!=u->file_bytes||
    r->copies->bytes!=u->records[0].bytes||
    !slcli_equal(r->copies->digest,u->records[0].digest,32))return 0;
  return 1;
}
struct slcli_ha2_c22_consumer *slcli_ha2_c22_allocate(struct slcli_arena *a,
  size_t service_rows,size_t bq_rows) {
  struct slcli_ha2_c22_consumer *c;
  if(!a)return 0;
  c=slcli_arena_calloc(a,1,sizeof(*c));if(!c)return 0;c->arena=a;
  c->service=slcli_unit19_range_allocate(a,16384,service_rows);
  if(!c->service){c->failed=1;return 0;}
  c->bq=slcli_unit19_range_allocate(a,16384,bq_rows);
  if(!c->bq){c->failed=1;return 0;}return c;
}
int slcli_ha2_c22_consume(struct slcli_ha2_c22_consumer *c,
  const struct slcli_unit19_reader *unit,slcli_input_handle original,
  const struct slcli_unit19_range *service_range,const struct slcli_input_body *bq_p,
  const unsigned char service_source[32],const unsigned char bq_source[32]) {
  if(!c||!c->arena||c->started||c->failed||
    !slcli_arena_live_range(c->arena,c,sizeof(*c))||
    !fresh_storage(c)||
    !originals_away(c,unit,sizeof(*unit))||unit->type!=SLCLI_HA2_CERT19||
    unit->count!=2||!unit->complete||unit->failed||unit->next!=2||
    unit->next_offset!=unit->file_bytes||unit->file_bytes>33088||
    !unit->records[0].bytes||unit->records[0].bytes>16384||
    !unit->records[1].bytes||unit->records[1].bytes>16384||
    !service_profile_range(c,service_range)||!profile_original(c,bq_p,1)||
    !profiles_disjoint(service_range->copies,bq_p)||
    !originals_away(c,service_source,32)||!originals_away(c,bq_source,32))return 0;
  c->started=1;c->failed=1;
  if(!slcli_profile_parse(service_range->copies->body,service_range->copies->bytes,
    &c->service_profile)||!slcli_equal(c->service_profile.digest,service_range->copies->digest,32))return 0;
  /* BOTH full native before/read/whole zero-EOF/after and second-copy passes
   * precede typed decode. All failed raw/native ranges remain in this owner. */
  if(!slcli_unit19_range_read(c->service,unit,0,original)||
    !slcli_unit19_range_read(c->bq,unit,1,original)||
    !slcli_calibration_parse(c->service->copies->body,c->service->copies->bytes,
      service_range->copies->digest,&c->service_c)||
    !slcli_calibration_parse(c->bq->copies->body,c->bq->copies->bytes,
      bq_p->digest,&c->bq_c)||
    c->service_c.domain!=1||c->service_c.maximum!=1048576||c->bq_c.domain!=0||
    !slcli_equal(c->service_c.source,service_source,32)||
    !slcli_equal(c->bq_c.source,bq_source,32)||
    !slcli_equal(c->service_c.raw_digest,unit->records[0].digest,32)||
    !slcli_equal(c->bq_c.raw_digest,unit->records[1].digest,32)||
    !service_profile_range(c,service_range)||!profile_original(c,bq_p,1))return 0;
  c->failed=0;c->complete=1;return 1;
}
#endif
