#include "unit19_native.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
struct slcli_unit19_native *slcli_unit19_native_allocate(struct slcli_arena *arena,
  size_t write_rows,size_t held_rows) {
  struct slcli_unit19_native *u;
  if(!arena||!arena->owner)return 0;
  u=slcli_arena_calloc(arena,1,sizeof(*u));if(!u)return 0;
  u->arena=arena;u->failed=1;
  u->write=slcli_store_unit19_allocate(arena,write_rows);
  u->held=slcli_store_unit19_allocate(arena,held_rows);
  if(u->write&&u->held)u->failed=0;
  return u;
}
int slcli_unit19_native_deadline_bind(struct slcli_unit19_native *u,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  const struct slcli_native_clock *prior,slcli_store_capture_sink sink,void *owner,size_t owner_bytes) {
  struct slcli_store_stream *s[2];
  if(!u||!u->arena||!slcli_arena_live_block(u->arena,u,sizeof(*u))||
     u->started||u->failed||u->prepared||u->deadline_owner)return 0;
  s[0]=u->write;s[1]=u->held;
  u->deadline_owner=slcli_store_deadline_bind(u->arena,s,2,d,caller,caller_bytes,prior,sink,owner,owner_bytes);
  if(!u->deadline_owner)return 0;
  if(!slcli_store_deadline_away(u->deadline_owner,u,sizeof(*u))||
     !slcli_store_deadline_ready(u->deadline_owner)){u->failed=1;return 0;}return 1;
}
static int immutable(struct slcli_unit19_native *u,const void *p,size_t n) {
  struct slcli_store_stream *s[2]={u->write,u->held};unsigned i;
  if(!p||!n||!slcli_arena_live_range(u->arena,p,n)||!away(u,sizeof(*u),p,n)||
     (u->deadline_owner&&!slcli_store_deadline_away(u->deadline_owner,p,n)))return 0;
  for(i=0;i<2;i++)if(!s[i]||s[i]->capture_capacity>SIZE_MAX/sizeof(*s[i]->captures)||
    s[i]->unit_geometry!=1||
    !slcli_arena_live_range(u->arena,s[i],sizeof(*s[i]))||
    !slcli_arena_live_range(u->arena,s[i]->captures,s[i]->capture_capacity*sizeof(*s[i]->captures))||
    !slcli_arena_live_range(u->arena,s[i]->unit_chunk,16384)||
    !slcli_arena_live_range(u->arena,s[i]->unit_readback,16385)||
    !away(s[i],sizeof(*s[i]),p,n)||
    !away(s[i]->captures,s[i]->capture_capacity*sizeof(*s[i]->captures),p,n)||
    !away(s[i]->unit_chunk,16384,p,n)||!away(s[i]->unit_readback,16385,p,n))return 0;
  return 1;
}
static int streams(struct slcli_unit19_native *u) {
  struct slcli_store_stream *a=u->write,*b=u->held;size_t n,m;
  if(!a||!b||!a->capture_capacity||!b->capture_capacity||
     a->unit_geometry!=1||b->unit_geometry!=1||
     a->capture_capacity>SIZE_MAX/sizeof(*a->captures)||
     b->capture_capacity>SIZE_MAX/sizeof(*b->captures))return 0;
  n=a->capture_capacity*sizeof(*a->captures);m=b->capture_capacity*sizeof(*b->captures);
  {
    const void *ranges[9]={u,a,a->captures,a->unit_chunk,a->unit_readback,
      b,b->captures,b->unit_chunk,b->unit_readback};
    size_t lengths[9]={sizeof(*u),sizeof(*a),n,16384,16385,sizeof(*b),m,16384,16385};
    unsigned i,j;
    for(i=0;i<9;i++) {
      if(!slcli_arena_live_range(u->arena,ranges[i],lengths[i]))return 0;
      for(j=0;j<i;j++)if(!away(ranges[i],lengths[i],ranges[j],lengths[j]))return 0;
    }
  }
  return slcli_arena_live_range(u->arena,a,sizeof(*a))&&
    slcli_arena_live_range(u->arena,b,sizeof(*b))&&
    slcli_arena_live_range(u->arena,a->captures,n)&&slcli_arena_live_range(u->arena,b->captures,m)&&
    away(u,sizeof(*u),a,sizeof(*a))&&away(u,sizeof(*u),b,sizeof(*b))&&
    away(u,sizeof(*u),a->captures,n)&&away(u,sizeof(*u),b->captures,m)&&
    away(a,sizeof(*a),b,sizeof(*b))&&away(a,sizeof(*a),b->captures,m)&&
    away(a->captures,n,b,sizeof(*b))&&away(a->captures,n,b->captures,m)&&
    !a->attempted&&!b->attempted&&!a->failed&&!b->failed&&
    (!u->deadline_owner||slcli_store_deadline_ready(u->deadline_owner));
}
static int emit(struct slcli_unit19_native *u,struct slcli_store_stream *s,
  const unsigned char *header,const unsigned char *table,const unsigned char *const *members) {
  uint32_t i;
  if(!slcli_store_stream_append(s,header,64)||
     !slcli_store_stream_append(s,table,(size_t)u->parsed.count*128))return 0;
  for(i=0;i<u->parsed.count;i++)if(u->parsed.records[i].bytes&&
    !slcli_store_stream_append(s,members[i],(size_t)u->parsed.records[i].bytes))return 0;
  return slcli_store_stream_finish(s);
}
int slcli_unit19_native_framing_storage(struct slcli_unit19_native *u,
  const struct slcli_unit19_framing *f,const unsigned char *const *members,
  const struct slcli_unit19_record *records,size_t count,const uint64_t *limits,size_t limit_count) {
  size_t i;
  if(!u||!u->arena||u->started||u->failed||!streams(u)||!immutable(u,f,sizeof(*f))||
     !count||count>15||!immutable(u,members,count*sizeof(*members))||
     !immutable(u,records,count*sizeof(*records))||
     !away(f,sizeof(*f),members,count*sizeof(*members))||
     !away(f,sizeof(*f),records,count*sizeof(*records))||
     (limits&&(!limit_count||limit_count>15||!immutable(u,limits,limit_count*sizeof(*limits))||
       !away(f,sizeof(*f),limits,limit_count*sizeof(*limits)))))return 0;
  for(i=0;i<count;i++)if(records[i].bytes>SIZE_MAX||
    (records[i].bytes&&(!immutable(u,members[i],(size_t)records[i].bytes)||
      !away(f,sizeof(*f),members[i],(size_t)records[i].bytes))))return 0;
  return 1;
}
int slcli_unit19_native_prepare(struct slcli_unit19_native *u,enum slcli_unit19_kind type,
  uint32_t source,const unsigned char header[64],const unsigned char *table,size_t table_bytes,
  const unsigned char *const *members,const struct slcli_unit19_record *originals,
  const uint64_t *limits,size_t limit_count,slcli_input_handle file,
  slcli_store_capture_sink sink,void *sink_original,uint64_t prior_store_bytes) {
  uint64_t bytes=0;uint32_t i,count;unsigned char digest[32];
  if(!u||!u->arena||u->started||u->failed||!u->write||!u->held||!sink||!sink_original||
     !slcli_arena_live_range(u->arena,u,sizeof(*u))||!streams(u)||!immutable(u,header,64)||
     !table_bytes||table_bytes%128||table_bytes>15*128||!immutable(u,table,table_bytes))return 0;
  count=(uint32_t)(table_bytes/128);
  if(!immutable(u,members,count*sizeof(*members))||
     !immutable(u,originals,count*sizeof(*originals))||
     (limits&&(!limit_count||limit_count>15||!immutable(u,limits,limit_count*sizeof(*limits)))))return 0;
  for(i=0;i<8;i++)bytes|=(uint64_t)header[48+i]<<(i*8);
  u->started=1;u->failed=1;
  if(!slcli_unit19_header(&u->parsed,header,bytes,type,source,limits,limit_count)||
     u->parsed.count!=count)return 0;
  for(i=0;i<count;i++)if(!slcli_unit19_record(&u->parsed,table+(size_t)i*128))return 0;
  for(i=0;i<count;i++) {
    if(!slcli_unit19_member_matches(&u->parsed,i,&originals[i])||
       u->parsed.records[i].bytes>SIZE_MAX||
       (u->parsed.records[i].bytes&&!immutable(u,members[i],(size_t)u->parsed.records[i].bytes)))return 0;
    slcli_sha256_init(&u->member_hash);
    if(!slcli_sha256_update(&u->member_hash,members[i],(size_t)u->parsed.records[i].bytes)||
       !slcli_sha256_finish(&u->member_hash,digest)||
       !slcli_equal(digest,u->parsed.records[i].digest,32))return 0;
  }
  if(prior_store_bytes>4194304||bytes>4194304-prior_store_bytes)return 0;
  u->retained_store_bytes=prior_store_bytes+bytes; /* Retain even partial native output. */
  if(!slcli_store_unit19_begin(u->write,file,0,&u->parsed,sink,sink_original)||
     !emit(u,u->write,header,table,members)||
     !slcli_store_unit19_begin(u->held,file,1,&u->parsed,sink,sink_original)||
     !emit(u,u->held,header,table,members)||
     !slcli_store_original_same(&u->write->captures[0],&u->held->captures[0])||
     !slcli_store_original_same(&u->held->captures[0],
       &u->held->captures[u->held->capture_count-1])||
     !slcli_store_original_same(&u->write->captures[0],
       &u->held->captures[u->held->capture_count-1])||
     !slcli_equal(u->write->digest,u->held->digest,32)||
     (u->deadline_owner&&!slcli_store_deadline_ready(u->deadline_owner)))return 0;
  u->prepared=1;u->failed=0;return 1;
}
#endif
