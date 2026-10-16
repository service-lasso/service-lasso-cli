/* SPDX-License-Identifier: Apache-2.0 */
#include "bq_process_reference_linux.h"
#if defined(__linux__)
#include "../host-service/native_syscall_linux.h"
#include <linux/fcntl.h>
#include <limits.h>
static int apart(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&n<=UINTPTR_MAX-x&&m<=UINTPTR_MAX-y&&(x+n<=y||y+m<=x);
}
static int zero(const void *p,size_t n){
 const unsigned char *b=p;size_t i;for(i=0;i<n;++i)if(b[i])return 0;return 1;
}
static long number(unsigned i){
 const long operations[8]={__NR_getpid,__NR_gettid,__NR_getuid,__NR_geteuid,
  __NR_getgid,__NR_getegid,__NR_pidfd_open,__NR_fcntl};
 return operations[i];
}
static int same(const struct slcli_bq_process_reference_linux_original *c){
 return c&&c->arena&&slcli_arena_live_block(c->arena,c,sizeof(*c))&&
  c->started&&slcli_arena_live_block(c->arena,c->caller,c->caller_bytes)&&
  slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)&&
  c->deadline->origin==c->origin&&c->origin->tick==c->origin_tick&&c->deadline->expires==c->expires&&
  c->prior&&slcli_arena_live_range(c->arena,c->prior,sizeof(*c->prior))&&
  c->prior->complete&&c->prior->tick==c->prior_tick;
}
static int raw(const struct slcli_bq_process_reference_linux_original *c){
 size_t i,j;uint64_t previous;
 if(!same(c)||c->next!=8||c->prior_tick<c->origin_tick||c->pidfd<0)return 0;
 previous=c->prior_tick;
 for(i=0;i<8;++i){
  if(!c->called[i]||!c->returned[i]||!c->before_returned[i]||!c->after_returned[i]||
   !c->before_result[i]||!c->after_result[i]||!c->before[i].complete||!c->after[i].complete||
   c->before[i].tick<previous||c->after[i].tick<c->before[i].tick||c->after[i].tick>=c->expires||c->status[i]<0)return 0;
  for(j=0;j<6;++j){
   uint64_t expected=0;
   if(i==6&&j==0)expected=(uint64_t)c->status[0];
   if(i==7&&j==0)expected=(uint64_t)c->pidfd;
   if(i==7&&j==1)expected=F_GETFD;
   if(c->args[i][j]!=expected)return 0;
  }
  previous=c->after[i].tick;
 }
 return c->status[0]>0&&c->status[0]<=INT_MAX&&c->status[1]>0&&c->status[1]<=INT_MAX&&
  (uint64_t)c->status[2]<=UINT32_MAX&&(uint64_t)c->status[3]<=UINT32_MAX&&
  (uint64_t)c->status[4]<=UINT32_MAX&&(uint64_t)c->status[5]<=UINT32_MAX&&
  c->status[6]==c->pidfd&&(c->status[7]&FD_CLOEXEC)!=0;
}
int slcli_bq_process_reference_linux_matches_original(
 const struct slcli_bq_process_reference_linux_original *c){
 return raw(c)&&c->observed&&!c->failed&&c->finish_returned&&c->finish_result&&
  c->finish.complete&&c->finish.tick>=c->after[7].tick&&c->finish.tick<c->expires;
}
int slcli_bq_process_reference_linux_acquire_original(
 struct slcli_bq_process_reference_linux_original *c,struct slcli_arena *a,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior){
 size_t i;uint64_t previous;
 if(!c||!a||!slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !slcli_arena_live_block(a,caller,caller_bytes)||!slcli_source_deadline_original_matches(a,d,caller)||
  !prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  prior->tick<d->origin->tick||prior->tick>=d->expires||
  !apart(c,sizeof(*c),caller,caller_bytes)||!apart(c,sizeof(*c),prior,sizeof(*prior))||
  !slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
 c->started=1;c->failed=1;c->pidfd=-1;c->arena=a;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->prior=prior;c->prior_tick=prior->tick;c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
 previous=c->prior_tick;
 for(i=0;i<8;++i){
  c->next=(uint32_t)i+1;
  if(i==6)c->args[i][0]=(uint64_t)c->status[0];
  if(i==7){c->args[i][0]=(uint64_t)c->pidfd;c->args[i][1]=F_GETFD;}
  c->before_result[i]=slcli_native_clock_read(&c->before[i]);c->before_returned[i]=1;
  if(!same(c)||!c->before_result[i]||c->before[i].tick<previous||c->before[i].tick>=c->expires)return 0;
  c->called[i]=1;
  c->status[i]=slcli_kernel_call(number((unsigned)i),(long)c->args[i][0],(long)c->args[i][1],
   (long)c->args[i][2],(long)c->args[i][3],(long)c->args[i][4],(long)c->args[i][5]);
  c->returned[i]=1;
  if(i==6&&c->status[i]>=0&&c->status[i]<=INT_MAX)c->pidfd=(int)c->status[i];
  c->after_result[i]=slcli_native_clock_read(&c->after[i]);c->after_returned[i]=1;
  if(!same(c)||!c->after_result[i]||c->after[i].tick<c->before[i].tick||c->after[i].tick>=c->expires||
   c->status[i]<0||(i<2&&(c->status[i]<=0||c->status[i]>INT_MAX))||
   (i>=2&&i<=5&&(uint64_t)c->status[i]>UINT32_MAX)||(i==6&&c->pidfd<0))return 0;
  previous=c->after[i].tick;
 }
 if(!raw(c))return 0;
 c->finish_result=slcli_native_clock_read(&c->finish);c->finish_returned=1;
 if(!same(c)||!c->finish_result||c->finish.tick<previous||c->finish.tick>=c->expires)return 0;
 c->observed=1;c->failed=0;return 1;
}
static int offer_raw(
 const struct slcli_bq_process_reference_linux_offer *c){
 struct slcli_bq_subject_route decoded;
 return c&&c->arena&&slcli_arena_live_block(c->arena,c,sizeof(*c))&&c->started&&
  c->acquire_returned&&c->acquire_result&&c->encode_returned&&c->encode_result&&c->send_returned&&c->send_result&&
  slcli_bq_channel_linux_matches_original(c->channel)&&
  slcli_bq_process_reference_linux_matches_original(c->process)&&
  c->readiness_returned&&c->readiness_result&&slcli_bq_readiness_linux_matches_original(&c->readiness)&&
   c->readiness.arena==c->arena&&c->readiness.deadline==c->deadline&&c->readiness.caller==c->caller&&
   c->readiness.caller_bytes==c->caller_bytes&&c->readiness.prior==&c->process->finish&&
   c->readiness.endpoint==c->channel->endpoints[0]&&c->readiness.requested_events==POLLOUT&&
   slcli_bq_send_linux_matches_original(c->send)&&c->channel->arena==c->arena&&c->process->arena==c->arena&&
  c->send->arena==c->arena&&c->channel->deadline==c->deadline&&c->process->deadline==c->deadline&&
  c->send->deadline==c->deadline&&c->channel->caller==c->caller&&c->process->caller==c->caller&&
  c->send->caller==c->caller&&c->channel->caller_bytes==c->caller_bytes&&c->process->caller_bytes==c->caller_bytes&&
  c->send->caller_bytes==c->caller_bytes&&c->prior_original&&
  slcli_arena_live_range(c->arena,c->prior_original,sizeof(*c->prior_original))&&
  c->prior_original->complete&&c->prior_original->tick==c->prior_tick&&
  c->prior_tick>=c->channel->validation_after.tick&&c->process->prior==c->prior_original&&
  c->process->prior_tick==c->prior_tick&&
  c->send->prior==&c->readiness.validation_after&&c->send->endpoint==c->channel->endpoints[0]&&
  c->send->source_fd==c->process->pidfd&&c->send->payload==c->payload&&c->send->payload_bytes==64&&
  c->route.message==SLCLI_BQ_SUBJECT_OFFER&&c->route.role==SLCLI_BQ_SUBJECT_REFERENCE&&!c->route.member_bytes&&
  slcli_arena_live_block(c->arena,c->route_original,sizeof(*c->route_original))&&
  slcli_bq_subject_route_equal(&c->route,c->route_original)&&
  slcli_bq_subject_route_decode(&decoded,c->payload,64)&&slcli_bq_subject_route_equal(&decoded,&c->route);
}
int slcli_bq_process_reference_linux_offer_matches_original(
 const struct slcli_bq_process_reference_linux_offer *c){
 return offer_raw(c)&&c->offered&&!c->failed&&c->validation_returned&&c->validation_result&&
  c->finish_returned&&c->finish_result&&c->finish.complete&&
  c->finish.tick>=c->send->validation_after.tick&&c->finish.tick<c->process->expires;
}
int slcli_bq_process_reference_linux_offer_original(
 struct slcli_bq_process_reference_linux_offer *c,struct slcli_arena *a,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_bq_channel_linux_original *channel,
 struct slcli_bq_process_reference_linux_original *process,struct slcli_bq_send_linux_original *send,
 unsigned char *payload,const struct slcli_bq_subject_route *route,const struct slcli_native_clock *prior){
 const void *blocks[6];size_t sizes[6],i,j;
 if(!a||!c||!process||!send||!payload||!route||!channel||
  !slcli_source_deadline_original_matches(a,d,caller)||!slcli_arena_live_block(a,caller,caller_bytes)||
  !slcli_bq_channel_linux_matches_original(channel)||channel->arena!=a||channel->deadline!=d||
  channel->caller!=caller||channel->caller_bytes!=caller_bytes||
  !slcli_arena_live_block(a,route,sizeof(*route))||route->message!=SLCLI_BQ_SUBJECT_OFFER||
  route->role!=SLCLI_BQ_SUBJECT_REFERENCE||route->member_bytes||!prior||
  !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  prior->tick<channel->validation_after.tick||prior->tick>=d->expires)return 0;
 blocks[0]=c;sizes[0]=sizeof(*c);blocks[1]=process;sizes[1]=sizeof(*process);
 blocks[2]=send;sizes[2]=sizeof(*send);blocks[3]=payload;sizes[3]=64;
 blocks[4]=route;sizes[4]=sizeof(*route);blocks[5]=channel;sizes[5]=sizeof(*channel);
 for(i=0;i<6;++i){
  if(!slcli_arena_live_block(a,blocks[i],sizes[i]))return 0;
  if(i<4&&(!zero(blocks[i],sizes[i])||!apart(blocks[i],sizes[i],caller,caller_bytes)||
   !apart(blocks[i],sizes[i],prior,sizeof(*prior))||
   !slcli_source_deadline_away(d,blocks[i],sizes[i])))return 0;
  for(j=0;j<i;++j)if(!apart(blocks[i],sizes[i],blocks[j],sizes[j]))return 0;
 }
 c->started=1;c->failed=1;c->arena=a;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->channel=channel;c->process=process;c->send=send;c->payload=payload;c->route_original=route;c->route=*route;
 c->prior_original=prior;c->prior_tick=prior->tick;
 c->encode_result=slcli_bq_subject_route_encode(payload,64,&c->route);c->encode_returned=1;
 if(!c->encode_result)return 0;
 c->acquire_result=slcli_bq_process_reference_linux_acquire_original(process,a,d,caller,caller_bytes,prior);
 c->acquire_returned=1;if(!c->acquire_result)return 0;
 c->readiness_result=slcli_bq_readiness_linux_preowned_original(&c->readiness,a,channel->endpoints[0],POLLOUT,
  d,caller,caller_bytes,&process->finish);c->readiness_returned=1;if(!c->readiness_result)return 0;
 c->send_result=slcli_bq_send_linux_preowned_original(send,a,channel->endpoints[0],process->pidfd,payload,64,
  d,caller,caller_bytes,&c->readiness.validation_after);c->send_returned=1;
 if(!c->send_result)return 0;
 c->validation_result=offer_raw(c);c->validation_returned=1;
 c->finish_result=slcli_native_clock_read(&c->finish);c->finish_returned=1;
 if(!c->validation_result||!c->finish_result||!c->finish.complete||
  c->finish.tick<send->validation_after.tick||c->finish.tick>=process->expires||
  !slcli_source_deadline_original_matches(a,d,caller)||d->origin!=process->origin||
  d->origin->tick!=process->origin_tick||d->expires!=process->expires)return 0;
 c->offered=1;c->failed=0;return 1;
}
#endif
