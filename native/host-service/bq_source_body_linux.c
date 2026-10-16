/* SPDX-License-Identifier: Apache-2.0 */
#include "bq_source_body_linux.h"
#if defined(__linux__)
#include <limits.h>
static int zero(const void *p,size_t n){const unsigned char *b=p;size_t i;for(i=0;i<n;++i)if(b[i])return 0;return 1;}
static int apart(const void *a,size_t n,const void *b,size_t m){uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&n<=UINTPTR_MAX-x&&m<=UINTPTR_MAX-y&&(x+n<=y||y+m<=x);}
static int identity(const struct slcli_linux_file_capture *a,const struct slcli_linux_file_capture *b){
 const struct statx *x=&a->identity,*y=&b->identity;
 return x->stx_ino==y->stx_ino&&x->stx_size==y->stx_size&&x->stx_dev_major==y->stx_dev_major&&
  x->stx_dev_minor==y->stx_dev_minor&&x->stx_mode==y->stx_mode&&x->stx_uid==y->stx_uid&&
  x->stx_gid==y->stx_gid&&x->stx_nlink==y->stx_nlink&&x->stx_mtime.tv_sec==y->stx_mtime.tv_sec&&
  x->stx_mtime.tv_nsec==y->stx_mtime.tv_nsec&&x->stx_ctime.tv_sec==y->stx_ctime.tv_sec&&
  x->stx_ctime.tv_nsec==y->stx_ctime.tv_nsec;
}
static int same(const struct slcli_bq_source_body_linux *c){
 return c&&c->arena&&slcli_arena_live_block(c->arena,c,sizeof(*c))&&c->started&&
  slcli_arena_live_block(c->arena,c->caller,c->caller_bytes)&&
  slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)&&
  c->deadline->origin==c->origin&&c->origin->tick==c->origin_tick&&c->deadline->expires==c->expires&&
  slcli_arena_live_range(c->arena,c->prior,sizeof(*c->prior))&&c->prior->complete&&
  c->prior->tick==c->prior_tick&&c->prior_tick>=c->origin_tick&&c->prior_tick<c->expires&&
  slcli_bq_source_readable_linux_matches_original(c->source)&&c->source->arena==c->arena&&
  c->source->deadline==c->deadline&&c->source->caller==c->caller&&c->source->finish.tick<=c->prior_tick&&
  c->expected<SIZE_MAX&&c->expected<INT64_MAX&&c->source->readable.identity.stx_size==c->expected&&
  slcli_arena_live_block(c->arena,c->body,c->expected+1)&&
  slcli_arena_live_block(c->arena,c->readback,c->expected+1)&&c->capacity&&
  c->capacity<=SIZE_MAX/sizeof(*c->rows)&&c->attempted<=c->capacity&&
  slcli_arena_live_block(c->arena,c->rows,c->capacity*sizeof(*c->rows))&&
  slcli_arena_live_range(c->arena,c->expected_digest_original,32);
}
static int row_original(const struct slcli_bq_source_body_linux *c,
 const struct slcli_bq_source_body_linux_row *r,size_t pass,size_t kind,size_t offset,size_t requested){
 const unsigned char *output=pass?c->readback:c->body;
 if(!r->attempted||!r->called||!r->returned||!r->result||r->pass!=pass||r->kind!=kind||
  r->offset!=offset||r->requested!=requested||!r->before_returned||!r->after_returned||
  !r->before_result||!r->after_result||!r->before.complete||!r->after.complete||
  r->after.tick<r->before.tick||r->after.tick>=c->expires||r->native.tick<r->before.tick||
  r->native.tick>r->after.tick)return 0;
 if(kind==1)return !offset&&!requested&&
  slcli_linux_file_observation_matches_original(c->source->descriptor,&r->native,S_IFREG)&&
  identity(&r->native,&c->source->readable);
 return slcli_linux_file_read_matches_original(c->source->descriptor,output+offset,requested,offset,&r->native)&&
  (kind==2?r->native.returned_bytes>0:r->native.returned_bytes==0);
}
static int whole(const struct slcli_bq_source_body_linux *c){
 size_t pass,i,offset,requested;uint64_t tick;struct slcli_sha256 hash;unsigned char digest[32];
 if(!same(c)||!c->pass_complete[0]||!c->pass_complete[1]||c->pass_first[0]||
  c->pass_first[1]!=c->pass_end[0]||c->pass_end[1]!=c->attempted)return 0;
 tick=c->prior_tick;
 for(pass=0;pass<2;++pass){
  if(c->pass_end[pass]>c->attempted||c->pass_end[pass]-c->pass_first[pass]<3)return 0;
  i=c->pass_first[pass];offset=0;
  if(!row_original(c,&c->rows[i],pass,1,0,0)||c->rows[i].before.tick<tick)return 0;
  tick=c->rows[i++].after.tick;
  while(offset<c->expected){
   if(i>=c->pass_end[pass])return 0;requested=c->expected-offset;if(requested>16384)requested=16384;
   if(!row_original(c,&c->rows[i],pass,2,offset,requested)||c->rows[i].before.tick<tick)return 0;
   offset+=(size_t)c->rows[i].native.returned_bytes;tick=c->rows[i++].after.tick;
  }
  if(i>=c->pass_end[pass]||!row_original(c,&c->rows[i],pass,3,c->expected,1)||c->rows[i].before.tick<tick)return 0;
  tick=c->rows[i++].after.tick;
  if(i>=c->pass_end[pass]||!row_original(c,&c->rows[i],pass,1,0,0)||c->rows[i].before.tick<tick||
   i+1!=c->pass_end[pass]||c->offset[pass]!=c->expected||!c->hash_finish_result[pass])return 0;
  tick=c->rows[i].after.tick;
  slcli_sha256_init(&hash);
  if(!slcli_sha256_update(&hash,pass?c->readback:c->body,c->expected)||
   !slcli_sha256_finish(&hash,digest)||!slcli_equal(digest,c->digest[pass],32)||
   !slcli_equal(digest,c->expected_digest_original,32))return 0;
 }
 return tick==c->previous_tick&&slcli_equal(c->body,c->readback,c->expected);
}
int slcli_bq_source_body_linux_matches_original(const struct slcli_bq_source_body_linux *c){
 return whole(c)&&c->observed&&!c->failed&&c->finish_returned&&c->finish_result&&c->finish.complete&&
  c->finish.tick>=c->previous_tick&&c->finish.tick<c->expires;
}
static int operation(struct slcli_bq_source_body_linux *c,size_t pass,uint32_t kind,size_t offset,size_t requested){
 struct slcli_bq_source_body_linux_row *r;unsigned char *output=pass?c->readback:c->body;
 if(!same(c)||c->attempted==c->capacity)return 0;
 r=&c->rows[c->attempted++];r->attempted=1;r->pass=(uint32_t)pass;r->kind=kind;r->offset=offset;r->requested=requested;
 r->before_result=slcli_native_clock_read(&r->before);r->before_returned=1;
 if(!same(c)||!r->before_result||r->before.tick<c->previous_tick||r->before.tick>=c->expires)return 0;
 r->called=1;
 r->result=kind==1?slcli_linux_file_observe(c->source->descriptor,&r->native):
  slcli_linux_file_read(c->source->descriptor,output+offset,requested,offset,&r->native);
 r->returned=1;r->after_result=slcli_native_clock_read(&r->after);r->after_returned=1;
 if(!same(c)||!row_original(c,r,pass,kind,offset,requested))return 0;
 c->previous_tick=r->after.tick;return 1;
}
int slcli_bq_source_body_linux_acquire_original(struct slcli_bq_source_body_linux *c,
 struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_bq_source_readable_linux *source,const struct slcli_native_clock *prior,
 unsigned char *body,unsigned char *readback,size_t expected,const unsigned char digest[32],
 struct slcli_bq_source_body_linux_row *rows,size_t capacity){
 const void *targets[4],*borrowed[20];size_t sizes[4],bytes[20],i,j,pass,requested,minimum;
 if(!c||!a||!d||!caller||!prior||!source||!digest||expected==SIZE_MAX||expected>=INT64_MAX||
  !capacity||capacity>SIZE_MAX/sizeof(*rows)||!slcli_source_deadline_original_matches(a,d,caller)||
  !slcli_arena_live_block(a,caller,caller_bytes)||!slcli_arena_live_range(a,prior,sizeof(*prior))||
  !prior->complete||prior->tick<d->origin->tick||prior->tick>=d->expires||
  !slcli_bq_source_readable_linux_matches_original(source)||source->arena!=a||source->deadline!=d||
  source->caller!=caller||source->finish.tick>prior->tick||source->readable.identity.stx_size!=expected||
  !slcli_arena_live_range(a,digest,32))return 0;
 minimum=expected/16384+(expected%16384!=0);
 if(minimum>SIZE_MAX/2-3||capacity<2*(minimum+3))return 0;
 targets[0]=c;sizes[0]=sizeof(*c);targets[1]=body;sizes[1]=expected+1;
 targets[2]=readback;sizes[2]=expected+1;targets[3]=rows;sizes[3]=capacity*sizeof(*rows);
 borrowed[0]=source;bytes[0]=sizeof(*source);borrowed[1]=caller;bytes[1]=caller_bytes;
 borrowed[2]=prior;bytes[2]=sizeof(*prior);borrowed[3]=digest;bytes[3]=32;
 borrowed[4]=a;bytes[4]=sizeof(*a);borrowed[5]=d;bytes[5]=sizeof(*d);
 borrowed[6]=d->origin;bytes[6]=sizeof(*d->origin);
 borrowed[7]=source->source;bytes[7]=sizeof(*source->source);
 borrowed[8]=source->proc_directory;bytes[8]=sizeof(*source->proc_directory);
 borrowed[9]=source->process;bytes[9]=sizeof(*source->process);
 borrowed[10]=source->source->components;bytes[10]=source->source->count*sizeof(*source->source->components);
 borrowed[11]=source->source->names;bytes[11]=source->source->path_bytes+2;
 borrowed[12]=source->source->path;bytes[12]=source->source->path_bytes;
 borrowed[13]=source->proc_directory->components;bytes[13]=source->proc_directory->count*sizeof(*source->proc_directory->components);
 borrowed[14]=source->proc_directory->names;bytes[14]=source->proc_directory->path_bytes+2;
 borrowed[15]=source->proc_directory->path;bytes[15]=source->proc_directory->path_bytes;
 borrowed[16]=source->prior;bytes[16]=sizeof(*source->prior);
 borrowed[17]=source->source->prior;bytes[17]=sizeof(*source->source->prior);
 borrowed[18]=source->proc_directory->prior;bytes[18]=sizeof(*source->proc_directory->prior);
 borrowed[19]=source->process->prior;bytes[19]=sizeof(*source->process->prior);
 for(i=0;i<4;++i){
  if(!slcli_arena_live_block(a,targets[i],sizes[i])||!zero(targets[i],sizes[i])||
   !slcli_source_deadline_away(d,targets[i],sizes[i]))return 0;
  for(j=0;j<20;++j)if(!apart(targets[i],sizes[i],borrowed[j],bytes[j]))return 0;
  for(j=0;j<i;++j)if(!apart(targets[i],sizes[i],targets[j],sizes[j]))return 0;
 }
 c->started=1;c->failed=1;c->arena=a;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->source=source;c->prior=prior;c->prior_tick=prior->tick;c->origin=d->origin;c->origin_tick=d->origin->tick;
 c->expires=d->expires;c->previous_tick=prior->tick;c->body=body;c->readback=readback;c->expected=expected;
 c->expected_digest_original=digest;c->rows=rows;c->capacity=capacity;
 for(pass=0;pass<2;++pass){
  c->pass_first[pass]=c->attempted;slcli_sha256_init(&c->hashes[pass]);
  if(!operation(c,pass,1,0,0))return 0;
  while(c->offset[pass]<expected){
   requested=expected-c->offset[pass];if(requested>16384)requested=16384;
   if(!operation(c,pass,2,c->offset[pass],requested))return 0;
   c->hash_update_result[pass]=slcli_sha256_update(&c->hashes[pass],(pass?readback:body)+c->offset[pass],
    (size_t)c->rows[c->attempted-1].native.returned_bytes);
   if(!c->hash_update_result[pass])return 0;
   c->offset[pass]+=(size_t)c->rows[c->attempted-1].native.returned_bytes;
  }
  if(!operation(c,pass,3,expected,1)||!operation(c,pass,1,0,0))return 0;
  c->hash_finish_result[pass]=slcli_sha256_finish(&c->hashes[pass],c->digest[pass]);
  if(!c->hash_finish_result[pass]||!slcli_equal(c->digest[pass],digest,32))return 0;
  c->pass_end[pass]=c->attempted;c->pass_complete[pass]=1;
 }
 if(!whole(c))return 0;
 c->finish_result=slcli_native_clock_read(&c->finish);c->finish_returned=1;
 if(!same(c)||!c->finish_result||c->finish.tick<c->previous_tick||c->finish.tick>=c->expires)return 0;
 c->failed=0;c->observed=1;return 1;
}
#endif
