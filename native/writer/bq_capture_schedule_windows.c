#include "bq_capture_schedule_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int zero(const void *p,size_t n){
 size_t i;const unsigned char *b=p;for(i=0;i<n;i++)if(b[i])return 0;return 1;
}
static int shape(const struct slcli_bq_capture_schedule_windows *s){
 const struct slcli_extent *e;
 if(!s||!s->started||!s->arena||!slcli_arena_live_block(s->arena,s,sizeof(*s))||
  !s->count||s->count>(SIZE_MAX-2)/2||s->count>SIZE_MAX/sizeof(*s->plan)||
  s->target_count!=2+2*s->count||s->target_count>SIZE_MAX/sizeof(*s->targets)||
  s->next>s->target_count||!s->file_bytes||s->file_bytes>INT64_MAX||
  !slcli_arena_live_block(s->arena,s->backing,sizeof(*s->backing))||
  !slcli_arena_live_block(s->arena,s->plan,s->count*sizeof(*s->plan))||
  !slcli_arena_live_block(s->arena,s->targets,s->target_count*sizeof(*s->targets))||
  !slcli_arena_live_block(s->arena,s->owner,sizeof(*s->owner))||
  !slcli_memory_owner_writable(s->owner)||s->owner->failed||
  s->request.index>=s->owner->count||s->request.index>=256||!s->request.sequence)return 0;
 e=&s->owner->extents[s->request.index];
 return e->reserved==s->file_bytes&&e->category==SLCLI_IO&&
  e->acquisition==s->request.sequence&&e->latest==s->request.sequence&&
  !e->base&&!e->arena_initialized&&!e->retirement&&
  (e->state==SLCLI_EXTENT_RESERVED||e->state==SLCLI_EXTENT_BACKING_ACQUIRED);
}
int slcli_bq_capture_schedule_windows_matches_original(
 const struct slcli_bq_capture_schedule_windows *s){
 size_t i;if(!shape(s)||!s->prepared||s->failed)return 0;
 for(i=0;i<s->target_count;i++){
  const struct slcli_bq_capture_target_windows *t=&s->targets[i];
  const void *container=i<2?(const void *)s->backing:(const void *)s->plan[(i-2)/2].capture;
  size_t bytes=i<2?sizeof(*s->backing):sizeof(*s->plan[0].capture);
  void *output;
  if(!slcli_arena_live_block(s->arena,container,bytes))return 0;
  output=i<2?(void *)&s->backing->sections[i]:(void *)&s->plan[(i-2)/2].capture->views[(i-2)%2];
  if(t->containing!=container||
   t->containing_bytes!=bytes||t->output!=output||
   t->output_offset!=(size_t)((uintptr_t)output-(uintptr_t)container)||
   t->output_offset>bytes||sizeof(void *)>bytes-t->output_offset||
   t->kind!=(i<2?22u:9u)||t->taken!=(uint32_t)(i<s->next)||
   t->file_offset!=(i<2?0:s->plan[(i-2)/2].file_offset)||
   t->bytes!=(i<2?s->file_bytes:s->plan[(i-2)/2].bytes))return 0;
 }
 return 1;
}
int slcli_bq_capture_schedule_windows_prepare_original(
 struct slcli_bq_capture_schedule_windows *s,struct slcli_arena *a,
 struct slcli_writer_capture_backing_windows *b,
 const struct slcli_writer_capture_view_plan_windows *plan,size_t count,
 struct slcli_bq_capture_target_windows *targets,size_t target_count,
 struct slcli_memory_owner *owner,const struct slcli_extent_request *request,uint64_t file_bytes){
 const void *blocks[5];size_t sizes[5],i,j;uint64_t total=0;
 if(!a||!s||!b||!plan||!targets||!owner||!request||!count||count>(SIZE_MAX-2)/2||
  count>SIZE_MAX/sizeof(*plan)||target_count!=2+2*count||
  target_count>SIZE_MAX/sizeof(*targets)||!file_bytes||file_bytes>INT64_MAX||
  !slcli_arena_live_range(a,request,sizeof(*request)))return 0;
 blocks[0]=s;sizes[0]=sizeof(*s);blocks[1]=b;sizes[1]=sizeof(*b);
 blocks[2]=plan;sizes[2]=count*sizeof(*plan);blocks[3]=targets;sizes[3]=target_count*sizeof(*targets);
 blocks[4]=owner;sizes[4]=sizeof(*owner);
 for(i=0;i<5;i++){
  if(!slcli_arena_live_block(a,blocks[i],sizes[i]))return 0;
  for(j=0;j<i;j++)if(!apart(blocks[i],sizes[i],blocks[j],sizes[j]))return 0;
 }
 if(!zero(s,sizeof(*s))||!zero(b,sizeof(*b))||!zero(targets,sizes[3])||
  !apart(s,sizeof(*s),request,sizeof(*request))||!apart(targets,sizes[3],request,sizeof(*request)))return 0;
 for(i=0;i<count;i++){
  const struct slcli_writer_capture_extent_windows *c=plan[i].capture;
  if(!c||!slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
   !plan[i].bytes||plan[i].bytes>=file_bytes||plan[i].file_offset>=file_bytes||
   plan[i].bytes>file_bytes-plan[i].file_offset||total>file_bytes-plan[i].bytes)return 0;
  for(j=0;j<5;j++)if(!apart(c,sizeof(*c),blocks[j],sizes[j]))return 0;
  for(j=0;j<i;j++)if(!apart(c,sizeof(*c),plan[j].capture,sizeof(*c))||
   (plan[i].file_offset<plan[j].file_offset+plan[j].bytes&&
    plan[j].file_offset<plan[i].file_offset+plan[i].bytes))return 0;
  total+=plan[i].bytes;
 }
 if(total>=file_bytes||request->index>=owner->count||request->index>=256||
  owner->extents[request->index].state!=SLCLI_EXTENT_RESERVED)return 0;
 s->started=1;s->failed=1;s->arena=a;s->backing=b;s->plan=plan;s->targets=targets;
 s->owner=owner;s->request=*request;s->count=count;s->target_count=target_count;s->file_bytes=file_bytes;
 if(!shape(s))return 0;
 for(i=0;i<target_count;i++){
  struct slcli_bq_capture_target_windows *t=&targets[i];
  t->containing=i<2?(const void *)b:(const void *)plan[(i-2)/2].capture;
  t->containing_bytes=i<2?sizeof(*b):sizeof(*plan[0].capture);
  t->output=i<2?(void *)&b->sections[i]:(void *)&plan[(i-2)/2].capture->views[(i-2)%2];
  t->output_offset=(size_t)((uintptr_t)t->output-(uintptr_t)t->containing);
  t->kind=i<2?22:9;t->file_offset=i<2?0:plan[(i-2)/2].file_offset;
  t->bytes=i<2?file_bytes:plan[(i-2)/2].bytes;
 }
 s->prepared=1;s->failed=0;return slcli_bq_capture_schedule_windows_matches_original(s);
}
int slcli_bq_capture_schedule_windows_take_original(
 struct slcli_bq_capture_schedule_windows *s,size_t ordinal,const void *container,size_t bytes,void *output){
 struct slcli_bq_capture_target_windows *t;
 if(!slcli_bq_capture_schedule_windows_matches_original(s)||ordinal!=s->next||ordinal>=s->target_count)return 0;
 t=&s->targets[ordinal];
 if(t->containing!=container||t->containing_bytes!=bytes||t->output!=output)return 0;
 t->taken=1;s->next=ordinal+1;return 1;
}
#endif
