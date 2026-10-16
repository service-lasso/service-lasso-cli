#if defined(__linux__)
#include "unit19_postseal_linux.h"
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
struct slcli_unit19_postseal_linux *slcli_unit19_postseal_linux_allocate(struct slcli_arena *a,
  size_t rows) {
  struct slcli_unit19_postseal_linux *s;
  if(!a||!rows||rows==SIZE_MAX||rows+1>SIZE_MAX/sizeof(struct slcli_native_clock))return 0;
  s=slcli_arena_calloc(a,1,sizeof(*s));if(!s)return 0;s->arena=a;s->failed=1;
  s->read=slcli_store_unit19_allocate(a,rows);
  s->clocks=slcli_arena_calloc(a,rows+1,sizeof(*s->clocks));
  s->clock_capacity=rows+1;if(s->read&&s->clocks)s->failed=0;return s;
}
static int bounded(struct slcli_unit19_postseal_linux *s) {
  struct slcli_native_clock *c;
  if(s->clock_count>=s->clock_capacity){s->failed=1;return 0;}
  c=&s->clocks[s->clock_count++];
  if(s->deadline->expires!=s->expires||s->deadline->origin!=s->origin||
     s->deadline->caller_original!=s->caller_original||s->origin->tick!=s->origin_tick||
     !slcli_native_clock_read(c)||c->tick<s->previous_tick||c->tick>=s->expires) {
    s->failed=1;return 0;
  }
  s->previous_tick=c->tick;return 1;
}
static int transfer(void *original,const slcli_input_capture *c,const unsigned char *raw,size_t bytes) {
  struct slcli_unit19_postseal_linux *s=original;int ok;
  ok=s->sink(s->capture_original,c,raw,bytes);
  if(!c->clock_observed||c->clock_status||!c->tick||
     c->tick<s->previous_tick||c->tick>=s->expires)ok=0;
  else s->previous_tick=c->tick;
  /* The real raw read body goes to its original sink even on a later clock
   * failure. Fresh post-transfer clock bounds every next streamed effect. */
  if(!bounded(s)||!ok){s->failed=1;return 0;}return 1;
}
static int immutable(struct slcli_unit19_postseal_linux *s,const void *p,size_t n) {
  struct slcli_store_stream *r=s->read;
  return p&&n&&slcli_arena_live_range(s->arena,p,n)&&
    away(s,sizeof(*s),p,n)&&away(r,sizeof(*r),p,n)&&
    away(r->captures,r->capture_capacity*sizeof(*r->captures),p,n)&&
    away(r->unit_chunk,16384,p,n)&&away(r->unit_readback,16385,p,n)&&
    away(s->clocks,s->clock_capacity*sizeof(*s->clocks),p,n);
}
int slcli_unit19_postseal_linux_original(struct slcli_unit19_postseal_linux *s,
  const struct slcli_unit19_seal_linux *seal,const struct slcli_unit19_framing *f,
  const unsigned char *const *members,const struct slcli_source_deadline_original *d,
  const void *caller,slcli_store_capture_sink sink,void *capture_original) {
  struct slcli_store_stream *r;const struct slcli_unit19_native *u;uint32_t i;
  const void *blocks[6];size_t lengths[6];unsigned j,k;
  if(!s||s->started||s->failed||!s->arena||!(r=s->read)||!s->clocks||
     !slcli_arena_live_range(s->arena,s,sizeof(*s))||
     !slcli_arena_live_range(s->arena,r,sizeof(*r))||
     !r->capture_capacity||r->capture_capacity>SIZE_MAX/sizeof(*r->captures)||
     !s->clock_capacity||s->clock_capacity>SIZE_MAX/sizeof(*s->clocks)||
     r->unit_geometry!=1||r->attempted||r->failed||!sink||!capture_original)return 0;
  blocks[0]=s;lengths[0]=sizeof(*s);blocks[1]=r;lengths[1]=sizeof(*r);
  blocks[2]=r->captures;lengths[2]=r->capture_capacity*sizeof(*r->captures);
  blocks[3]=r->unit_chunk;lengths[3]=16384;blocks[4]=r->unit_readback;lengths[4]=16385;
  blocks[5]=s->clocks;lengths[5]=s->clock_capacity*sizeof(*s->clocks);
  for(j=0;j<6;j++) {
    if(!slcli_arena_live_range(s->arena,blocks[j],lengths[j]))return 0;
    for(k=0;k<j;k++)if(!away(blocks[j],lengths[j],blocks[k],lengths[k]))return 0;
    if(!slcli_source_deadline_away(d,blocks[j],lengths[j]))return 0;
    if(!away(blocks[j],lengths[j],caller,1)||
       !away(blocks[j],lengths[j],capture_original,1))return 0;
  }
  if(!slcli_source_deadline_original_matches(s->arena,d,caller)||
     !immutable(s,seal,sizeof(*seal))||!seal->sealed||seal->failed||
     !(u=seal->prepared)||!immutable(s,u,sizeof(*u))||!u->prepared||u->failed||
     !u->creator_closed||!immutable(s,seal->verity,sizeof(*seal->verity))||
     !seal->verity->verified||seal->verity->failed||seal->verity->row_count!=6||
     !seal->verity->seal_applied||seal->verity->original!=seal->readonly_original||
     seal->verity->deadline!=d||seal->verity->caller_original!=caller||
     seal->verity->original_origin!=d->origin||seal->verity->expires!=d->expires||
     seal->verity->origin_tick!=d->origin->tick||
     seal->verity->previous_tick<seal->verity->origin_tick||
     seal->verity->previous_tick>=seal->verity->expires||
     !immutable(s,f,sizeof(*f))||!f->complete||f->failed||
     f->checked.count!=u->parsed.count||!u->parsed.count||u->parsed.count>15||
     f->checked.file_bytes!=u->parsed.file_bytes||
     !immutable(s,members,u->parsed.count*sizeof(*members)))return 0;
  for(j=0;j<2;j++) {
    const struct slcli_store_stream *prior=j?u->held:u->write;size_t n;
    if(!immutable(s,prior,sizeof(*prior))||!prior->capture_capacity||
       prior->capture_capacity>SIZE_MAX/sizeof(*prior->captures)||
       prior->capture_count>prior->capture_capacity||!prior->held_verified)return 0;
    n=prior->capture_capacity*sizeof(*prior->captures);
    if(!immutable(s,prior->captures,n)||!immutable(s,prior->unit_chunk,16384)||
       !immutable(s,prior->unit_readback,16385))return 0;
  }
  for(i=0;i<u->parsed.count;i++) {
    if(u->parsed.records[i].bytes>SIZE_MAX||
       !slcli_unit19_member_matches(&f->checked,i,&u->parsed.records[i])||
       (u->parsed.records[i].bytes&&!immutable(s,members[i],(size_t)u->parsed.records[i].bytes)))return 0;
  }
  s->started=1;s->failed=1;s->deadline=d;s->origin=d->origin;s->caller_original=caller;
  s->seal_original=seal;s->framing_original=f;s->members_original=members;
  s->sink=sink;s->capture_original=capture_original;s->expires=d->expires;
  s->origin_tick=d->origin->tick;s->previous_tick=seal->verity->previous_tick;
  if(!bounded(s)||!slcli_store_unit19_begin(r,seal->readonly_original,1,&u->parsed,transfer,s)||
     !slcli_input_original_same(&seal->verity->rows[5],&r->captures[0],u->parsed.file_bytes)||
     !slcli_store_stream_append(r,f->header,64)||
     !slcli_store_stream_append(r,&f->table[0][0],u->parsed.count*128))return 0;
  for(i=0;i<u->parsed.count;i++)if(u->parsed.records[i].bytes&&
    !slcli_store_stream_append(r,members[i],(size_t)u->parsed.records[i].bytes))return 0;
  if(!slcli_store_stream_finish(r)||!r->held_verified||
     !slcli_input_original_same(&seal->verity->rows[5],
       &r->captures[r->capture_count-1],u->parsed.file_bytes)||
     !slcli_equal(r->digest,u->write->digest,32))return 0;
  s->complete=1;s->failed=0;return 1;
}
#endif
