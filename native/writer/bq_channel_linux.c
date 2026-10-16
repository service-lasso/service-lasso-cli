#include "bq_channel_linux.h"
#if defined(__linux__)
#include "../host-service/native_syscall_linux.h"
#include <sys/socket.h>
static int separate(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int zero(const void *p,size_t bytes) {
 const unsigned char *b=p;size_t i;for(i=0;i<bytes;i++)if(b[i])return 0;return 1;
}
static int before(struct slcli_bq_channel_linux_original *c,unsigned i) {
 c->next=i+1;
 c->before_result[i]=slcli_native_clock_read(&c->before[i]);c->before_returned[i]=1;
 return c->before_result[i]&&c->before[i].tick>=c->previous_tick&&
  c->before[i].tick<c->expires&&c->deadline->origin==c->origin&&
  c->origin->tick==c->origin_tick&&c->deadline->expires==c->expires&&
  slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)&&
  c->prior->complete&&c->prior->tick==c->prior_tick;
}
static int after(struct slcli_bq_channel_linux_original *c,unsigned i) {
 c->after_result[i]=slcli_native_clock_read(&c->after[i]);c->after_returned[i]=1;
 if(!c->after_result[i]||c->after[i].tick<c->before[i].tick||
  c->after[i].tick>=c->expires||c->deadline->origin!=c->origin||
  c->origin->tick!=c->origin_tick||c->deadline->expires!=c->expires||
  !slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller))return 0;
 c->previous_tick=c->after[i].tick;return c->returned[i]&&c->status[i]==0;
}
static int raw_original(const struct slcli_bq_channel_linux_original *c) {
 unsigned i;uint64_t previous;
 if(!c||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||!c->started||c->next!=3||
  !slcli_arena_live_block(c->arena,c->caller,c->caller_bytes)||
  !slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)||
  !c->prior||!slcli_arena_live_range(c->arena,c->prior,sizeof(*c->prior))||
  !c->prior->complete||c->prior->tick!=c->prior_tick||c->prior_tick<c->origin_tick||
  c->deadline->origin!=c->origin||c->origin->tick!=c->origin_tick||c->deadline->expires!=c->expires||
  c->endpoints[0]<0||c->endpoints[1]<0||c->endpoints[0]==c->endpoints[1]||c->pass_credentials!=1||
  c->args[0][0]!=AF_UNIX||c->args[0][1]!=(SOCK_SEQPACKET|SOCK_CLOEXEC)||
  c->args[0][2]!=0||c->args[0][3]!=(uintptr_t)c->endpoints||c->args[0][4]||c->args[0][5])return 0;
 previous=c->prior_tick;
 for(i=0;i<3;i++) {
  if(!c->called[i]||!c->returned[i]||c->status[i]||
   !c->before_returned[i]||!c->before_result[i]||!c->before[i].complete||
   !c->after_returned[i]||!c->after_result[i]||!c->after[i].complete||
   c->before[i].tick<previous||c->after[i].tick<c->before[i].tick||c->after[i].tick>=c->expires)return 0;
  if(i&&(c->args[i][0]!=(uint64_t)c->endpoints[i-1]||c->args[i][1]!=SOL_SOCKET||
   c->args[i][2]!=SO_PASSCRED||c->args[i][3]!=(uintptr_t)&c->pass_credentials||
   c->args[i][4]!=sizeof(c->pass_credentials)||c->args[i][5]))return 0;
  previous=c->after[i].tick;
 }
 return 1;
}
int slcli_bq_channel_linux_matches_original(const struct slcli_bq_channel_linux_original *c) {
 return raw_original(c)&&c->complete&&!c->failed&&c->validation_returned&&c->validation_result&&
  c->validation_clock_returned&&c->validation_clock_result&&c->validation_after.complete&&
  c->validation_after.tick>=c->after[2].tick&&c->validation_after.tick<c->expires&&
  c->previous_tick==c->validation_after.tick;
}
int slcli_bq_channel_linux_create_preowned_original(
 struct slcli_bq_channel_linux_original *c,struct slcli_arena *a,
 const struct slcli_source_deadline_original *d,const void *caller,size_t bytes,
 const struct slcli_native_clock *prior) {
 unsigned i;
 if(!c||!a||!bytes||!slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !slcli_arena_live_block(a,caller,bytes)||!slcli_source_deadline_original_matches(a,d,caller)||
  !prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  prior->tick<d->origin->tick||prior->tick>=d->expires||
  !separate(c,sizeof(*c),caller,bytes)||!separate(c,sizeof(*c),prior,sizeof(*prior))||
  !slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
 c->started=1;c->failed=1;c->arena=a;c->deadline=d;c->caller=caller;c->caller_bytes=bytes;
 c->prior=prior;c->prior_tick=prior->tick;c->previous_tick=prior->tick;
 c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
 c->endpoints[0]=-1;c->endpoints[1]=-1;c->pass_credentials=1;
 c->args[0][0]=AF_UNIX;c->args[0][1]=SOCK_SEQPACKET|SOCK_CLOEXEC;
 c->args[0][2]=0;c->args[0][3]=(uintptr_t)c->endpoints;
 if(!before(c,0))return 0;c->called[0]=1;
 c->status[0]=slcli_kernel_call(__NR_socketpair,(long)c->args[0][0],(long)c->args[0][1],0,
  (long)c->endpoints,0,0);
 c->returned[0]=1;if(!after(c,0)||c->endpoints[0]<0||c->endpoints[1]<0||
  c->endpoints[0]==c->endpoints[1])return 0;
 for(i=1;i<3;i++) {
  c->args[i][0]=(uint64_t)c->endpoints[i-1];c->args[i][1]=SOL_SOCKET;c->args[i][2]=SO_PASSCRED;
  c->args[i][3]=(uintptr_t)&c->pass_credentials;c->args[i][4]=sizeof(c->pass_credentials);
  if(!before(c,i))return 0;c->called[i]=1;
  c->status[i]=slcli_kernel_call(__NR_setsockopt,(long)c->args[i][0],SOL_SOCKET,SO_PASSCRED,
   (long)&c->pass_credentials,sizeof(c->pass_credentials),0);
  c->returned[i]=1;if(!after(c,i))return 0;
 }
 c->validation_result=raw_original(c);c->validation_returned=1;
 c->validation_clock_result=slcli_native_clock_read(&c->validation_after);c->validation_clock_returned=1;
 if(!c->validation_result||!c->validation_clock_result||c->validation_after.tick<c->after[2].tick||
  c->validation_after.tick>=c->expires||c->prior->tick!=c->prior_tick||!c->prior->complete||
  c->deadline->origin!=c->origin||c->origin->tick!=c->origin_tick||c->deadline->expires!=c->expires)return 0;
 c->previous_tick=c->validation_after.tick;c->complete=1;c->failed=0;return 1;
}
#endif
