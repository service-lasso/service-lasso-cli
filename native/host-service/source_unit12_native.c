#include "source_unit12_native.h"
#if defined(__linux__)
#include <linux/fcntl.h>
#include <time.h>
#endif
#if defined(_WIN32) || defined(__linux__)
static int separate(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int zero(const void *p,size_t n){size_t i;const unsigned char *v=p;for(i=0;i<n;i++)if(v[i])return 0;return 1;}
static int add_charge(size_t *total,size_t requested){
 size_t n=slcli_arena_allocation_bytes(requested);if(!n||n>SIZE_MAX-*total)return 0;*total+=n;return 1;
}
static int read_geometry(const struct slcli_input_body *s,size_t bytes){
#if defined(_WIN32)
 size_t count;
 if(!s->source_native_read_maximum||s->source_native_read_maximum>bytes)return 0;
 count=bytes/s->source_native_read_maximum+(bytes%s->source_native_read_maximum!=0);
 return count<=(SIZE_MAX-6)/2&&s->capture_capacity>=2*(count+3);
#else
 return !s->source_native_read_maximum&&s->capture_capacity>=8;
#endif
}
static slcli_input_capture *next(struct slcli_input_body *s){
 if(s->capture_count>=s->capture_capacity)return 0;return &s->captures[s->capture_count++];
}
static int observe(struct slcli_input_body *s,slcli_input_capture *p){
 int ok;if(!p||!slcli_input_deadline_before(s,p))return 0;
#if defined(_WIN32)
 ok=slcli_windows_source_observe(s->original,p);
#else
 ok=slcli_linux_file_observe(s->original,p);
#endif
 if(!slcli_input_deadline_after(s,p,0,0,ok))return 0;
#if defined(__linux__)
 return (p->flags_status&O_ACCMODE)==O_RDONLY&&!(p->flags_status&O_PATH)&&(p->descriptor_status&FD_CLOEXEC);
#else
 return 1;
#endif
}
static int read_native(struct slcli_input_body *s,unsigned char *out,size_t bytes,uint64_t offset,
 slcli_input_capture *p,uint64_t *received){
 int ok,observed;if(!p||!slcli_input_deadline_before(s,p))return 0;
#if defined(_WIN32)
 ok=slcli_windows_file_read(s->original,out,(DWORD)bytes,offset,p);observed=p->bytes_observed!=0;
#else
 ok=slcli_linux_file_read(s->original,out,bytes,offset,p);observed=p->returned_bytes_observed!=0;
#endif
 if(!slcli_input_deadline_after(s,p,out,bytes,ok)||!observed)return 0;
 *received=p->returned_bytes;return *received<=bytes;
}
static int pass(struct slcli_source_unit12_native *c,struct slcli_source_unit12_native_range *r,unsigned char *out){
 struct slcli_input_body *s=r->body;slcli_input_capture *before,*after,*p;uint64_t at=0,n;size_t requested;
 before=next(s);if(!observe(s,before)||
  !slcli_input_original_same(&c->ranges[0].body->captures[0],before,c->file_bytes))return 0;
 while(at<r->bytes){p=next(s);
  requested=(size_t)(r->bytes-at);
#if defined(_WIN32)
  if(!s->source_native_read_maximum)return 0;
  if(requested>s->source_native_read_maximum)requested=s->source_native_read_maximum;
#endif
  if(!read_native(s,out+(size_t)at,requested,r->offset+at,p,&n)||!n)return 0;at+=n;
 }
 p=next(s);if(!read_native(s,out+(size_t)r->bytes,1,c->file_bytes,p,&n)||n)return 0;
 after=next(s);return observe(s,after)&&slcli_input_original_same(before,after,c->file_bytes);
}
static int range_read(struct slcli_source_unit12_native *c,struct slcli_source_unit12_native_range *r){
 struct slcli_input_body *s=r->body;struct slcli_sha256 h;
 if(!slcli_input_deadline_ready(s)||s->started||s->capture_count||s->consumed)return 0;
 s->started=1;s->recheck_started=1;s->consumed=2;s->failed=1;s->original=c->original;
 if(!pass(c,r,s->body))return 0;s->bytes=(size_t)r->bytes;s->eof_observed=1;
 if(!pass(c,r,s->readback)||!slcli_equal(s->body,s->readback,s->bytes))return 0;
 slcli_sha256_init(&h);
 if(!slcli_sha256_update(&h,s->body,s->bytes)||!slcli_sha256_finish(&h,s->digest))return 0;
 s->complete=1;s->recheck_complete=1;s->failed=0;return 1;
}
int slcli_source_unit12_native_table_prepare_original(struct slcli_source_unit12_native *c,
 struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,uint64_t file_bytes,
 struct slcli_input_body *const *bodies,struct slcli_source_unit12_native_range *ranges,size_t count){
 size_t i,j,n,context,alignment;struct slcli_input_body *s;
 if(!c||!a||!slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||count<2||
  count>SIZE_MAX/sizeof(*ranges)||count>SIZE_MAX/sizeof(*bodies)||
  !slcli_arena_live_block(a,ranges,count*sizeof(*ranges))||
  !slcli_arena_live_block(a,bodies,count*sizeof(*bodies))||
  !separate(c,sizeof(*c),ranges,count*sizeof(*ranges))||!separate(c,sizeof(*c),bodies,count*sizeof(*bodies))||
  !separate(ranges,count*sizeof(*ranges),bodies,count*sizeof(*bodies))||
  !slcli_source_deadline_original_matches(a,d,caller)||!caller_bytes||
  !slcli_arena_live_block(a,caller,caller_bytes)||!prior||
  !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||prior->tick<d->origin->tick||prior->tick>=d->expires||
  !separate(c,sizeof(*c),caller,caller_bytes)||!separate(c,sizeof(*c),prior,sizeof(*prior))||
  !separate(ranges,count*sizeof(*ranges),caller,caller_bytes)||
  !separate(ranges,count*sizeof(*ranges),prior,sizeof(*prior))||
  !separate(bodies,count*sizeof(*bodies),caller,caller_bytes)||
  !separate(bodies,count*sizeof(*bodies),prior,sizeof(*prior))||
  !slcli_source_deadline_away(d,c,sizeof(*c))||
  !slcli_source_deadline_away(d,ranges,count*sizeof(*ranges))||
  !slcli_source_deadline_away(d,bodies,count*sizeof(*bodies))||file_bytes>UINT64_C(17179869184)||
  count-1>(UINT64_MAX-64)/128||file_bytes<64+(uint64_t)(count-1)*128)return 0;
 c->started=1;c->failed=1;c->arena=a;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->prepare_prior_original=prior;c->origin_original=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
 c->file_bytes=file_bytes;c->bodies_original=bodies;c->ranges=ranges;c->range_count=count;
 if(!add_charge(&c->table_storage_charge,sizeof(*c))||
  !add_charge(&c->table_storage_charge,count*sizeof(*ranges))||
  !add_charge(&c->table_storage_charge,count*sizeof(*bodies)))return 0;
 if(!zero(ranges,count*sizeof(*ranges)))return 0;
 for(i=0;i<count;i++){
  s=bodies[i];n=i?128:64;
  if(!s||!slcli_arena_live_range(a,s,sizeof(*s))||!slcli_input_deadline_ready(s)||
   s->started||s->complete||s->failed||s->capture_count||s->consumed||s->recheck_started||
   s->capacity!=n+1||s->readback_capacity!=n+1||!read_geometry(s,n)||
   s->deadline_owner->arena!=a||s->deadline_owner->deadline!=d||
   s->deadline_owner->caller_original!=caller||s->deadline_owner->caller_bytes!=caller_bytes||
   !separate(c,sizeof(*c),s,s->deadline_owner->body_storage_bytes)||
   !separate(ranges,count*sizeof(*ranges),s,s->deadline_owner->body_storage_bytes)||
   !separate(bodies,count*sizeof(*bodies),s,s->deadline_owner->body_storage_bytes))return 0;
  for(j=0;j<i;j++)if(s==bodies[j])return 0;
  alignment=_Alignof(struct slcli_native_clock);context=sizeof(*s->deadline_owner);
  if(context>SIZE_MAX-(alignment-1))return 0;
  context=(context+alignment-1)/alignment*alignment;
  if(s->capture_capacity>(SIZE_MAX-context)/sizeof(struct slcli_native_clock))return 0;
  n=context+s->capture_capacity*sizeof(struct slcli_native_clock);
  if(s->deadline_owner->before!=(struct slcli_native_clock *)((unsigned char *)s->deadline_owner+context)||
   !slcli_arena_live_block(a,s->deadline_owner,n)||
   !separate(s->deadline_owner,n,c,sizeof(*c))||
   !separate(s->deadline_owner,n,ranges,count*sizeof(*ranges))||
   !separate(s->deadline_owner,n,bodies,count*sizeof(*bodies))||
   !separate(s->deadline_owner,n,s,s->deadline_owner->body_storage_bytes))return 0;
  for(j=0;j<i;j++){
   const struct slcli_input_body *p=ranges[j].body;
   if(!separate(s,s->deadline_owner->body_storage_bytes,p,p->deadline_owner->body_storage_bytes)||
    !separate(s,s->deadline_owner->body_storage_bytes,p->deadline_owner,ranges[j].deadline_storage_bytes)||
    !separate(s->deadline_owner,n,p,p->deadline_owner->body_storage_bytes)||
    !separate(s->deadline_owner,n,p->deadline_owner,ranges[j].deadline_storage_bytes))return 0;
  }
  if(
   !add_charge(&c->table_storage_charge,s->deadline_owner->body_storage_bytes)||
   !add_charge(&c->table_storage_charge,n))return 0;
  ranges[i].deadline_storage_bytes=n;
  c->preflighted_bodies=i+1;
  n=i?128:64;
  ranges[i].body=s;ranges[i].offset=i?64+(uint64_t)(i-1)*128:0;ranges[i].bytes=n;
 }
 /* Every term above is an actual distinct live allocation in this owner.
  * Other source/retention/image/native storage remains outside this subtotal. */
 if(!a->owner||c->table_storage_charge>a->owner->charged)return 0;
 c->prepare_after_result=slcli_native_clock_read(&c->prepare_after);c->prepare_after_returned=1;
 if(!c->prepare_after_result||c->prepare_after.tick<prior->tick||c->prepare_after.tick>=d->expires||
  !slcli_source_deadline_original_matches(a,d,caller)||d->origin!=c->origin_original||
  c->origin_original->tick!=c->origin_tick||d->expires!=c->expires)return 0;
 c->prepared=1;c->failed=0;return 1;
}
int slcli_source_unit12_native_table_prepared_matches_original(const struct slcli_source_unit12_native *c){
 size_t i,j,n,alignment,context,total=0;const struct slcli_input_body *s;
 if(!c||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||!c->started||!c->prepared||c->failed||
  c->read_started||c->attempted||c->completed||c->table_observed||c->range_count<2||
  c->read_prior_original||!zero(&c->before,sizeof(c->before))||!zero(&c->reader,sizeof(c->reader))||
  c->range_count>SIZE_MAX/sizeof(*c->ranges)||c->range_count>SIZE_MAX/sizeof(*c->bodies_original)||
  c->file_bytes>UINT64_C(17179869184)||c->range_count-1>(UINT64_MAX-64)/128||
  c->file_bytes<64+(uint64_t)(c->range_count-1)*128||
  !slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)||
  c->deadline->origin!=c->origin_original||c->origin_original->tick!=c->origin_tick||c->deadline->expires!=c->expires||
  !slcli_arena_live_block(c->arena,c->caller,c->caller_bytes)||
  !slcli_arena_live_range(c->arena,c->prepare_prior_original,sizeof(*c->prepare_prior_original))||
  !c->prepare_prior_original->complete||c->prepare_prior_original->tick<c->origin_tick||
  c->prepare_after_returned!=1||!c->prepare_after_result||!c->prepare_after.complete||
  c->prepare_after.tick<c->prepare_prior_original->tick||c->prepare_after.tick>=c->expires||
  !slcli_arena_live_block(c->arena,c->ranges,c->range_count*sizeof(*c->ranges))||
  !slcli_arena_live_block(c->arena,c->bodies_original,c->range_count*sizeof(*c->bodies_original))||
  !separate(c,sizeof(*c),c->ranges,c->range_count*sizeof(*c->ranges))||
  !separate(c,sizeof(*c),c->bodies_original,c->range_count*sizeof(*c->bodies_original))||
  !separate(c->ranges,c->range_count*sizeof(*c->ranges),c->bodies_original,c->range_count*sizeof(*c->bodies_original))||
  !separate(c,sizeof(*c),c->caller,c->caller_bytes)||
  !separate(c->ranges,c->range_count*sizeof(*c->ranges),c->caller,c->caller_bytes)||
  !separate(c->bodies_original,c->range_count*sizeof(*c->bodies_original),c->caller,c->caller_bytes)||
  !separate(c,sizeof(*c),c->prepare_prior_original,sizeof(*c->prepare_prior_original))||
  !separate(c->ranges,c->range_count*sizeof(*c->ranges),c->prepare_prior_original,sizeof(*c->prepare_prior_original))||
  !separate(c->bodies_original,c->range_count*sizeof(*c->bodies_original),c->prepare_prior_original,sizeof(*c->prepare_prior_original))||
  !slcli_source_deadline_away(c->deadline,c,sizeof(*c))||
  !slcli_source_deadline_away(c->deadline,c->ranges,c->range_count*sizeof(*c->ranges))||
  !slcli_source_deadline_away(c->deadline,c->bodies_original,c->range_count*sizeof(*c->bodies_original))||
  c->preflighted_bodies!=c->range_count||
  !add_charge(&total,sizeof(*c))||!add_charge(&total,c->range_count*sizeof(*c->ranges))||
  !add_charge(&total,c->range_count*sizeof(*c->bodies_original)))return 0;
 for(i=0;i<c->range_count;i++){
  const struct slcli_source_unit12_native_range *r=&c->ranges[i];s=c->bodies_original[i];n=i?128:64;
  if(!s||!slcli_arena_live_range(c->arena,s,sizeof(*s))||!slcli_input_deadline_ready(s)||
   s->started||s->complete||s->failed||s->capture_count||s->consumed||s->recheck_started||
   s->capacity!=n+1||s->readback_capacity!=n+1||!read_geometry(s,n)||
   s->deadline_owner->arena!=c->arena||s->deadline_owner->deadline!=c->deadline||
   s->deadline_owner->caller_original!=c->caller||s->deadline_owner->caller_bytes!=c->caller_bytes||
   r->body!=s||r->offset!=(i?64+(uint64_t)(i-1)*128:0)||r->bytes!=n||
   r->read_returned||r->after_returned||r->decode_returned||r->member_decoded||
   r->read_result||r->after_result||r->decode_result||!zero(&r->after,sizeof(r->after))||
   !zero(&r->member,sizeof(r->member)))return 0;
  alignment=_Alignof(struct slcli_native_clock);context=sizeof(*s->deadline_owner);
  if(context>SIZE_MAX-(alignment-1))return 0;context=(context+alignment-1)/alignment*alignment;
  if(s->capture_capacity>(SIZE_MAX-context)/sizeof(struct slcli_native_clock))return 0;
  n=context+s->capture_capacity*sizeof(struct slcli_native_clock);
  if(r->deadline_storage_bytes!=n||s->deadline_owner->before!=(struct slcli_native_clock *)((unsigned char *)s->deadline_owner+context)||
   !slcli_arena_live_block(c->arena,s->deadline_owner,n)||
   !separate(s->deadline_owner,n,c,sizeof(*c))||!separate(s->deadline_owner,n,c->ranges,c->range_count*sizeof(*c->ranges))||
   !separate(s->deadline_owner,n,c->bodies_original,c->range_count*sizeof(*c->bodies_original))||
   !separate(s->deadline_owner,n,s,s->deadline_owner->body_storage_bytes)||
   !separate(c,sizeof(*c),s,s->deadline_owner->body_storage_bytes)||
   !separate(c->ranges,c->range_count*sizeof(*c->ranges),s,s->deadline_owner->body_storage_bytes)||
   !separate(c->bodies_original,c->range_count*sizeof(*c->bodies_original),s,s->deadline_owner->body_storage_bytes)||
   !add_charge(&total,s->deadline_owner->body_storage_bytes)||!add_charge(&total,n))return 0;
  for(j=0;j<i;j++){
   const struct slcli_input_body *p=c->ranges[j].body;
   if(!separate(s,s->deadline_owner->body_storage_bytes,p,p->deadline_owner->body_storage_bytes)||
    !separate(s,s->deadline_owner->body_storage_bytes,p->deadline_owner,c->ranges[j].deadline_storage_bytes)||
    !separate(s->deadline_owner,n,p,p->deadline_owner->body_storage_bytes)||
    !separate(s->deadline_owner,n,p->deadline_owner,c->ranges[j].deadline_storage_bytes))return 0;
  }
 }
 return c->arena->owner&&total==c->table_storage_charge&&total<=c->arena->owner->charged;
}
int slcli_source_unit12_native_table_read_prepared_original(struct slcli_source_unit12_native *c,
 slcli_input_handle original,const struct slcli_native_clock *prior){
 size_t i,count;struct slcli_source_unit12_native_range *ranges;
 const struct slcli_native_clock *previous;const struct slcli_source_deadline_original *d;uint64_t file_bytes;
 if(!slcli_source_unit12_native_table_prepared_matches_original(c))return 0;
 if(!prior||!slcli_arena_live_range(c->arena,prior,sizeof(*prior))||!prior->complete||
  prior->tick<c->prepare_after.tick||prior->tick>=c->expires||
  !separate(prior,sizeof(*prior),c,sizeof(*c))||
  !separate(prior,sizeof(*prior),c->ranges,c->range_count*sizeof(*c->ranges))||
  !separate(prior,sizeof(*prior),c->bodies_original,c->range_count*sizeof(*c->bodies_original)))return 0;
 for(i=0;i<c->range_count;i++)if(
  !separate(prior,sizeof(*prior),c->ranges[i].body,c->ranges[i].body->deadline_owner->body_storage_bytes)||
  !separate(prior,sizeof(*prior),c->ranges[i].body->deadline_owner,c->ranges[i].deadline_storage_bytes))return 0;
 /* The predecessor must survive every mutable output, including this owner. */
 c->read_started=1;c->failed=1;c->original=original;c->read_prior_original=prior;
 d=c->deadline;file_bytes=c->file_bytes;count=c->range_count;ranges=c->ranges;
 c->before_result=slcli_native_clock_read(&c->before);c->before_returned=1;
 if(!c->before_result||c->before.tick<prior->tick||c->before.tick>=c->expires||
  !slcli_source_deadline_original_matches(c->arena,d,c->caller)||d->origin!=c->origin_original||
  c->origin_original->tick!=c->origin_tick||d->expires!=c->expires)return 0;
 previous=&c->before;
 for(i=0;i<count;i++){
  struct slcli_source_unit12_native_range *r=&ranges[i];c->attempted=i+1;
  if(!slcli_input_deadline_precede(r->body->deadline_owner,previous))return 0;
  r->read_result=range_read(c,r);r->read_returned=1;if(!r->read_result)return 0;
  r->decode_result=i?slcli_source_unit12_record(&c->reader,r->body->body):
   slcli_source_unit12_header(&c->reader,r->body->body,file_bytes);r->decode_returned=1;
  if(!r->decode_result||c->reader.count!=count-1)return 0;
  if(i){r->member=c->reader.previous;r->member_decoded=1;}
  r->after_result=slcli_native_clock_read(&r->after);r->after_returned=1;
  if(!r->after_result||r->after.tick<r->body->deadline_owner->previous_tick||r->after.tick>=d->expires)return 0;
  previous=&r->after;c->completed=i+1;
 }
 if(!c->reader.table_complete||c->reader.failed)return 0;
 c->table_observed=1;c->failed=0;return 1;
}
static int clock_original(const struct slcli_native_clock *v){
 uint64_t whole,fraction;
 if(!v||!v->started||!v->observed||!v->complete||!v->tick)return 0;
#if defined(_WIN32)
 if(v->calls_called!=2||v->calls_returned!=2||!v->status[0]||!v->status[1]||
  !v->args[0]||!v->args[1]||v->args[0]==v->args[1]||
  (int64_t)v->raw[0]<=0||(int64_t)v->raw[1]<=0)return 0;
 whole=v->raw[0]/v->raw[1];fraction=v->raw[0]%v->raw[1];
 if(whole>UINT64_MAX/1000000000||fraction>UINT64_MAX/1000000000)return 0;
 whole*=1000000000;fraction=fraction*1000000000/v->raw[1];
#else
 if(v->calls_called!=1||v->calls_returned!=1||v->status[0]||v->status[1]||
  v->args[0]!=CLOCK_MONOTONIC||!v->args[1]||(int64_t)v->raw[0]<0||v->raw[1]>=1000000000||
  v->raw[0]>UINT64_MAX/1000000000)return 0;
 whole=v->raw[0]*1000000000;fraction=v->raw[1];
#endif
 return fraction<=UINT64_MAX-whole&&v->tick==whole+fraction;
}
static int record_same(const struct slcli_source_unit12_record *a,const struct slcli_source_unit12_record *b){
 return slcli_equal(a->association,b->association,32)&&slcli_equal(a->capture,b->capture,32)&&
  slcli_equal(a->digest,b->digest,32)&&a->offset==b->offset&&a->bytes==b->bytes&&
  a->index==b->index&&a->kind==b->kind&&a->source_project==b->source_project;
}
static int row_original(const struct slcli_source_unit12_native *c,const struct slcli_input_body *s,
 size_t index,uint64_t *tick,int kind,const unsigned char *out,size_t requested,uint64_t offset){
 const slcli_input_capture *p;const struct slcli_native_clock *before;
 if(index>=s->capture_count)return 0;p=&s->captures[index];before=&s->deadline_owner->before[index];
 if(!clock_original(before)||before->tick<*tick||before->tick>=c->expires||
  p->tick<before->tick||p->tick>=c->expires)return 0;
 if(kind==1){
#if defined(_WIN32)
  if(!slcli_windows_source_observation_matches_original(c->original,p))return 0;
#else
  if(!slcli_linux_file_observation_matches_original(c->original,p,S_IFREG))return 0;
#endif
  if(!slcli_input_original_same(&c->ranges[0].body->captures[0],p,c->file_bytes))return 0;
 }else{
#if defined(_WIN32)
  if(!slcli_windows_file_read_matches_original(c->original,out,(DWORD)requested,offset,p))return 0;
#else
  if(!slcli_linux_file_read_matches_original(c->original,out,requested,offset,p))return 0;
#endif
  if(kind==2?!p->returned_bytes:p->returned_bytes!=0)return 0;
 }
 *tick=p->tick;return 1;
}
static int range_original(const struct slcli_source_unit12_native *c,
 const struct slcli_source_unit12_native_range *r,const struct slcli_native_clock *prior){
 const struct slcli_input_body *s=r->body;size_t pass,index=0,at,requested;uint64_t tick=prior->tick;
 struct slcli_sha256 hash;unsigned char digest[32];
 if(!slcli_input_deadline_ready(s)||s->original!=c->original||!s->started||!s->complete||
  !s->eof_observed||s->failed||!s->recheck_started||!s->recheck_complete||s->consumed!=2||
  s->bytes!=r->bytes||s->capacity!=r->bytes+1||s->readback_capacity!=r->bytes+1||
  !read_geometry(s,(size_t)r->bytes)||s->capture_count<8||s->capture_count>s->capture_capacity||
  s->deadline_owner->arena!=c->arena||s->deadline_owner->deadline!=c->deadline||
  s->deadline_owner->caller_original!=c->caller||s->deadline_owner->caller_bytes!=c->caller_bytes||
  s->deadline_owner->origin!=c->origin_original||s->deadline_owner->origin_tick!=c->origin_tick||
  s->deadline_owner->expires!=c->expires||!s->deadline_owner->predecessor_advanced||
  s->deadline_owner->predecessor!=prior||s->deadline_owner->predecessor_tick!=prior->tick)return 0;
 for(pass=0;pass<2;pass++){
  const unsigned char *out=pass?s->readback:s->body;at=0;
  if(!row_original(c,s,index++,&tick,1,0,0,0))return 0;
  while(at<r->bytes){
   requested=(size_t)r->bytes-at;
#if defined(_WIN32)
   if(!s->source_native_read_maximum||s->source_native_read_maximum>r->bytes)return 0;
   if(requested>s->source_native_read_maximum)requested=s->source_native_read_maximum;
#endif
   if(!row_original(c,s,index,&tick,2,out+at,requested,r->offset+at))return 0;
   at+=(size_t)s->captures[index++].returned_bytes;
  }
  if(!row_original(c,s,index++,&tick,3,out+r->bytes,1,c->file_bytes)||
   !row_original(c,s,index++,&tick,1,0,0,0))return 0;
 }
 if(index!=s->capture_count||s->deadline_owner->previous_tick!=tick||
  !slcli_equal(s->body,s->readback,s->bytes)||r->read_returned!=1||!r->read_result||
  r->decode_returned!=1||!r->decode_result||r->after_returned!=1||!r->after_result||
  !clock_original(&r->after)||r->after.tick<tick||r->after.tick>=c->expires)return 0;
 slcli_sha256_init(&hash);return slcli_sha256_update(&hash,s->body,s->bytes)&&
  slcli_sha256_finish(&hash,digest)&&slcli_equal(digest,s->digest,32);
}
int slcli_source_unit12_native_matches_original(const struct slcli_source_unit12_native *c){
 struct slcli_source_unit12_reader reader={0};const struct slcli_native_clock *prior;
 size_t i,total=0,context,alignment,n;int decoded;
 if(!c||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||!c->started||!c->prepared||
  !c->read_started||c->failed||!c->table_observed||c->range_count<2||
  c->attempted!=c->range_count||c->completed!=c->range_count||c->preflighted_bodies!=c->range_count||
  c->range_count-1>(UINT64_MAX-64)/128||c->file_bytes>UINT64_C(17179869184)||
  c->file_bytes<64+(uint64_t)(c->range_count-1)*128||
  c->range_count>SIZE_MAX/sizeof(*c->ranges)||c->range_count>SIZE_MAX/sizeof(*c->bodies_original)||
  !slcli_arena_live_block(c->arena,c->ranges,c->range_count*sizeof(*c->ranges))||
  !slcli_arena_live_block(c->arena,c->bodies_original,c->range_count*sizeof(*c->bodies_original))||
  !slcli_arena_live_block(c->arena,c->caller,c->caller_bytes)||
  !slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)||
  c->deadline->origin!=c->origin_original||c->origin_original->tick!=c->origin_tick||
  c->deadline->expires!=c->expires||!c->read_prior_original||!c->prepare_prior_original||
  !slcli_arena_live_range(c->arena,c->prepare_prior_original,sizeof(*c->prepare_prior_original))||
  !clock_original(c->prepare_prior_original)||c->prepare_prior_original->tick<c->origin_tick||
  !slcli_arena_live_range(c->arena,c->read_prior_original,sizeof(*c->read_prior_original))||
  !clock_original(c->read_prior_original)||c->before_returned!=1||!c->before_result||
  !clock_original(&c->before)||c->before.tick<c->read_prior_original->tick||c->before.tick>=c->expires||
  c->prepare_after_returned!=1||!c->prepare_after_result||!clock_original(&c->prepare_after)||
  c->prepare_after.tick<c->prepare_prior_original->tick||c->prepare_after.tick>=c->expires||
  c->read_prior_original->tick<c->prepare_after.tick||
  !add_charge(&total,sizeof(*c))||!add_charge(&total,c->range_count*sizeof(*c->ranges))||
  !add_charge(&total,c->range_count*sizeof(*c->bodies_original)))return 0;
 prior=&c->before;
 for(i=0;i<c->range_count;i++){
  const struct slcli_source_unit12_native_range *r=&c->ranges[i];
  if(r->body!=c->bodies_original[i]||!r->body||!slcli_arena_live_range(c->arena,r->body,sizeof(*r->body))||
   r->offset!=(i?64+(uint64_t)(i-1)*128:0)||r->bytes!=(i?128:64)||!range_original(c,r,prior)||
   !add_charge(&total,r->body->deadline_owner->body_storage_bytes)||!add_charge(&total,r->deadline_storage_bytes))return 0;
  alignment=_Alignof(struct slcli_native_clock);context=sizeof(*r->body->deadline_owner);
  if(context>SIZE_MAX-(alignment-1))return 0;context=(context+alignment-1)/alignment*alignment;
  if(r->body->capture_capacity>(SIZE_MAX-context)/sizeof(struct slcli_native_clock))return 0;
  n=context+r->body->capture_capacity*sizeof(struct slcli_native_clock);
  if(r->deadline_storage_bytes!=n)return 0;
  decoded=i?slcli_source_unit12_record(&reader,r->body->body):
   slcli_source_unit12_header(&reader,r->body->body,c->file_bytes);
  if(!decoded||reader.count!=c->range_count-1||
   (i?(r->member_decoded!=1||!record_same(&r->member,&reader.previous)):
    (r->member_decoded||!zero(&r->member,sizeof(r->member)))))return 0;
  prior=&r->after;
 }
 return c->arena->owner&&total==c->table_storage_charge&&total<=c->arena->owner->charged&&
  c->reader.started==reader.started&&c->reader.failed==reader.failed&&c->reader.count==reader.count&&
  c->reader.file_bytes==reader.file_bytes&&c->reader.body_offset==reader.body_offset&&
  c->reader.next_offset==reader.next_offset&&c->reader.next==reader.next&&c->reader.attempted==reader.attempted&&
  c->reader.table_complete==reader.table_complete&&reader.table_complete&&
  record_same(&c->reader.previous,&reader.previous)&&slcli_equal(c->reader.header,reader.header,64)&&
  slcli_equal(c->reader.attempted_record,reader.attempted_record,128);
}
#endif
