#include "bq_source_namespace_linux.h"
#if defined(__linux__)
#include "native_syscall_linux.h"
#include <linux/fcntl.h>
#include <limits.h>
static int zero(const void *p,size_t n){const unsigned char *b=p;size_t i;for(i=0;i<n;++i)if(b[i])return 0;return 1;}
static int apart(const void *a,size_t n,const void *b,size_t m){uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);}
static int shape(const char *p,size_t n,size_t *out){
 size_t i,start=1,count=1;
 if(!p||n<2||n>SIZE_MAX-2||p[0]!='/'||p[n-1])return 0;
 if(n==2){*out=1;return 1;}
 for(i=1;i<n;++i){
  if(!p[i]&&i!=n-1)return 0;
  if(p[i]=='/'||i==n-1){size_t bytes=i-start;
   if(!bytes||(bytes==1&&p[start]=='.')||(bytes==2&&p[start]=='.'&&p[start+1]=='.')||count==SIZE_MAX)return 0;
   ++count;start=i+1;
  }
 }
 *out=count;return 1;
}
static int same(const struct slcli_bq_source_namespace_linux *c){
 size_t i,count;
 if(!c||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||!c->started||
  !slcli_arena_live_block(c->arena,c->caller,c->caller_bytes)||
  !slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)||
  c->deadline->origin!=c->origin||c->origin->tick!=c->origin_tick||c->deadline->expires!=c->expires||
  !slcli_arena_live_range(c->arena,c->prior,sizeof(*c->prior))||!c->prior->complete||
  c->prior->tick!=c->prior_tick||c->prior_tick<c->origin_tick||c->prior_tick>=c->expires||
  !slcli_arena_live_range(c->arena,c->path,c->path_bytes)||!shape(c->path,c->path_bytes,&count)||
  count!=c->count||count>SIZE_MAX/sizeof(*c->components)||
  (c->leaf_kind!=S_IFREG&&c->leaf_kind!=S_IFDIR)||(count==1&&c->leaf_kind!=S_IFDIR)||
  !slcli_arena_live_block(c->arena,c->names,c->path_bytes+2)||
  !slcli_arena_live_block(c->arena,c->components,count*sizeof(*c->components)))return 0;
 for(i=0;i<c->path_bytes;++i)if(c->names[i]!=(c->path[i]=='/'?0:c->path[i]))return 0;
 return c->names[c->path_bytes]=='/'&&!c->names[c->path_bytes+1];
}
static int tick(struct slcli_bq_source_namespace_linux *c,struct slcli_native_clock *out){
 if(!slcli_native_clock_read(out)||!same(c)||!slcli_native_clock_linux_matches_original(out)||out->tick<c->previous_tick||out->tick>=c->expires)return 0;
 c->previous_tick=out->tick;return 1;
}
static int rows_original(const struct slcli_bq_source_namespace_linux *c){
 size_t i,j,start=1,end;uint64_t prior;
 if(!same(c)||c->attempted!=c->count||c->completed!=c->count)return 0;
 prior=c->prior_tick;
 for(i=0;i<c->count;++i){
  const struct slcli_bq_source_namespace_component_linux *r=&c->components[i];
  size_t offset,length;long flags=O_PATH|O_CLOEXEC|O_NOFOLLOW;
  if(i+1<c->count||c->leaf_kind==S_IFDIR)flags|=O_DIRECTORY;
  if(!i){offset=c->path_bytes;length=1;}
  else{end=start;while(end<c->path_bytes-1&&c->path[end]!='/')++end;offset=start;length=end-start;start=end+1;}
  if(!r->attempted||!r->called||!r->returned||r->status<0||r->status>INT_MAX||r->descriptor!=r->status||
   r->name_offset!=offset||r->name_bytes!=length||r->args[0]!=(i?c->components[i-1].descriptor:AT_FDCWD)||
   r->args[1]!=(long)(c->names+offset)||r->args[2]!=flags||
   !r->before_returned||!r->after_returned||!r->finish_returned||
   !r->before_result||!r->after_result||!r->finish_result||
   !slcli_native_clock_linux_matches_original(&r->before)||!slcli_native_clock_linux_matches_original(&r->after)||
   !slcli_native_clock_linux_matches_original(&r->finish)||
   r->before.tick<prior||r->after.tick<r->before.tick||r->finish.tick<r->after.tick||
   r->finish.tick>=c->expires||!r->observe_returned||!r->observe_result||
   !slcli_linux_path_observation_matches_original(r->descriptor,&r->observation,i+1<c->count?S_IFDIR:c->leaf_kind)||
   r->observation.tick<r->after.tick||r->finish.tick<r->observation.tick)return 0;
  for(j=3;j<6;++j)if(r->args[j])return 0;
  prior=r->finish.tick;
 }
 return c->previous_tick==prior;
}
int slcli_bq_source_namespace_linux_matches_original(const struct slcli_bq_source_namespace_linux *c){
 return rows_original(c)&&!c->failed&&c->observed&&c->finish_returned&&c->finish_result&&
  slcli_native_clock_linux_matches_original(&c->finish)&&c->finish.tick>=c->previous_tick&&c->finish.tick<c->expires;
}
static int acquire_kind(struct slcli_bq_source_namespace_linux *c,
 struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const char *path,size_t path_bytes,char *names,
 struct slcli_bq_source_namespace_component_linux *components,size_t count,uint32_t kind){
 const void *mutable[3],*borrowed[5];size_t ms[3],bs[5],i,j,actual,start=1,end;long flags;
 if(!a||!c||!d||!caller||!caller_bytes||!prior||!path||!names||!components||
  (kind!=S_IFREG&&kind!=S_IFDIR)||
  !slcli_arena_live_range(a,path,path_bytes)||!shape(path,path_bytes,&actual)||actual!=count||
  (count==1&&kind!=S_IFDIR)||
  count>SIZE_MAX/sizeof(*components)||!slcli_source_deadline_original_matches(a,d,caller)||
  !slcli_arena_live_block(a,caller,caller_bytes)||!slcli_arena_live_range(a,prior,sizeof(*prior))||
  !prior->complete||prior->tick<d->origin->tick||prior->tick>=d->expires)return 0;
 mutable[0]=c;ms[0]=sizeof(*c);mutable[1]=names;ms[1]=path_bytes+2;
 mutable[2]=components;ms[2]=count*sizeof(*components);
 borrowed[0]=path;bs[0]=path_bytes;borrowed[1]=caller;bs[1]=caller_bytes;
 borrowed[2]=prior;bs[2]=sizeof(*prior);borrowed[3]=a;bs[3]=sizeof(*a);
 borrowed[4]=d;bs[4]=sizeof(*d);
 for(i=0;i<3;++i){
  if(!slcli_arena_live_block(a,mutable[i],ms[i])||!zero(mutable[i],ms[i])||
   !slcli_source_deadline_away(d,mutable[i],ms[i]))return 0;
  for(j=0;j<5;++j)if(!apart(mutable[i],ms[i],borrowed[j],bs[j]))return 0;
  for(j=0;j<i;++j)if(!apart(mutable[i],ms[i],mutable[j],ms[j]))return 0;
 }
 c->started=1;c->failed=1;c->arena=a;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->prior=prior;c->prior_tick=prior->tick;c->origin=d->origin;c->origin_tick=d->origin->tick;
 c->expires=d->expires;c->previous_tick=prior->tick;c->path=path;c->path_bytes=path_bytes;
 c->names=names;c->components=components;c->count=count;c->leaf_kind=kind;
 for(i=0;i<path_bytes;++i)names[i]=path[i]=='/'?0:path[i];names[path_bytes]='/';
 /* Consume the entire finite target roster before the first native open. */
 for(i=0;i<count;++i){struct slcli_bq_source_namespace_component_linux *r=&components[i];
  r->attempted=1;r->descriptor=-1;
  if(!i){r->name_offset=path_bytes;r->name_bytes=1;}
  else{end=start;while(end<path_bytes-1&&path[end]!='/')++end;r->name_offset=start;r->name_bytes=end-start;start=end+1;}
 }
 for(i=0;i<count;++i){struct slcli_bq_source_namespace_component_linux *r=&components[i];
  c->attempted=i+1;r->before_result=tick(c,&r->before);r->before_returned=1;if(!r->before_result)return 0;
  flags=O_PATH|O_CLOEXEC|O_NOFOLLOW;if(i+1<count||kind==S_IFDIR)flags|=O_DIRECTORY;
  r->args[0]=i?components[i-1].descriptor:AT_FDCWD;r->args[1]=(long)(names+r->name_offset);r->args[2]=flags;
  r->called=1;r->status=slcli_kernel_call(__NR_openat,r->args[0],r->args[1],flags,0,0,0);r->returned=1;
  if(r->status>=0&&r->status<=INT_MAX)r->descriptor=(int)r->status;
  r->after_result=tick(c,&r->after);r->after_returned=1;
  if(!r->after_result||r->status<0||r->status>INT_MAX)return 0;
  r->observe_result=slcli_linux_path_source_observe(r->descriptor,&r->observation,i+1<count?S_IFDIR:kind);
  r->observe_returned=1;
  if(r->observation.clock_observed&&r->observation.tick>=c->previous_tick)c->previous_tick=r->observation.tick;
  r->finish_result=tick(c,&r->finish);r->finish_returned=1;
  if(!r->observe_result||!r->finish_result)return 0;
  ++c->completed;
 }
 if(!rows_original(c))return 0;
 c->finish_result=slcli_native_clock_read(&c->finish);c->finish_returned=1;
 if(!c->finish_result||!same(c)||!slcli_native_clock_linux_matches_original(&c->finish)||c->finish.tick<c->previous_tick||c->finish.tick>=c->expires)return 0;
 c->failed=0;c->observed=1;return 1;
}
int slcli_bq_source_namespace_linux_acquire_original(struct slcli_bq_source_namespace_linux *c,
 struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const char *path,size_t path_bytes,char *names,
 struct slcli_bq_source_namespace_component_linux *components,size_t count){
 return acquire_kind(c,a,d,caller,caller_bytes,prior,path,path_bytes,names,components,count,S_IFREG);
}
int slcli_bq_source_namespace_linux_acquire_directory_original(struct slcli_bq_source_namespace_linux *c,
 struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const char *path,size_t path_bytes,char *names,
 struct slcli_bq_source_namespace_component_linux *components,size_t count){
 return acquire_kind(c,a,d,caller,caller_bytes,prior,path,path_bytes,names,components,count,S_IFDIR);
}
#endif
