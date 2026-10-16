#include "bq_channel_linux.h"
#if defined(__linux__)
#include "../host-service/native_syscall_linux.h"
static int separate(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int raw_original(const struct slcli_bq_send_linux_original *c) {
 const struct cmsghdr *h;const unsigned char *fd,*expected;size_t i;
 if(!c||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||!c->started||!c->called||!c->returned||
  c->endpoint<0||c->source_fd<0||!c->payload_bytes||c->payload_bytes>16384||
  !slcli_arena_live_range(c->arena,c->payload,c->payload_bytes)||
  !slcli_arena_live_block(c->arena,c->caller,c->caller_bytes)||
  !slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)||
  !c->prior||!slcli_arena_live_range(c->arena,c->prior,sizeof(*c->prior))||!c->prior->complete||c->prior->tick!=c->prior_tick||
  c->deadline->origin!=c->origin||c->origin->tick!=c->origin_tick||c->deadline->expires!=c->expires||
  c->args[0]!=(uint64_t)c->endpoint||c->args[1]!=(uintptr_t)&c->message||c->args[2]!=(MSG_DONTWAIT|MSG_NOSIGNAL)||
  c->message.msg_name||c->message.msg_namelen||c->message.msg_iov!=&c->vector||c->message.msg_iovlen!=1||
  c->vector.iov_base!=c->payload||c->vector.iov_len!=c->payload_bytes||
  c->message.msg_control!=c->control.bytes||c->message.msg_controllen!=sizeof(c->control.bytes)||c->message.msg_flags||
  !c->before_returned||!c->before_result||!c->before.complete||!c->after_returned||!c->after_result||!c->after.complete||
  c->prior_tick<c->origin_tick||c->before.tick<c->prior_tick||c->after.tick<c->before.tick||c->after.tick>=c->expires||
  c->status<0||(uint64_t)c->status!=c->payload_bytes)return 0;
 h=(const void *)c->control.bytes;
 if(h->cmsg_len!=CMSG_LEN(sizeof(int))||h->cmsg_level!=SOL_SOCKET||h->cmsg_type!=SCM_RIGHTS)return 0;
 fd=CMSG_DATA(h);expected=(const void *)&c->source_fd;
 for(i=0;i<sizeof(int);i++)if(fd[i]!=expected[i])return 0;
 return 1;
}
static int validation_original(const struct slcli_bq_send_linux_original *c) {
 return c->validation_returned&&c->validation_result&&c->validation_clock_returned&&
  c->validation_clock_result&&c->validation_after.complete&&c->validation_after.tick>=c->after.tick&&
  c->validation_after.tick<c->expires&&c->deadline->origin==c->origin&&
  c->origin->tick==c->origin_tick&&c->deadline->expires==c->expires&&c->prior->complete&&c->prior->tick==c->prior_tick;
}
int slcli_bq_send_linux_matches_original(const struct slcli_bq_send_linux_original *c) {
 return raw_original(c)&&c->complete&&!c->failed&&validation_original(c);
}
int slcli_bq_send_linux_preowned_original(struct slcli_bq_send_linux_original *c,
 struct slcli_arena *a,int endpoint,int source_fd,const unsigned char *payload,size_t bytes,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior) {
 struct cmsghdr *h;size_t i;unsigned char *fd_bytes;const unsigned char *source;
 if(!c||!a||endpoint<0||source_fd<0||!bytes||bytes>16384||!caller_bytes||
  !slcli_arena_live_block(a,c,sizeof(*c))||!slcli_arena_live_range(a,payload,bytes)||
  !slcli_arena_live_block(a,caller,caller_bytes)||!slcli_source_deadline_original_matches(a,d,caller)||
  !prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  prior->tick<d->origin->tick||prior->tick>=d->expires||
  !separate(c,sizeof(*c),payload,bytes)||!separate(c,sizeof(*c),caller,caller_bytes)||
  !separate(c,sizeof(*c),prior,sizeof(*prior))||!slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
 for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
 c->started=1;c->failed=1;c->arena=a;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->prior=prior;c->prior_tick=prior->tick;c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
 c->endpoint=endpoint;c->source_fd=source_fd;c->payload=payload;c->payload_bytes=bytes;
 c->vector.iov_base=(void *)payload;c->vector.iov_len=bytes;c->message.msg_iov=&c->vector;c->message.msg_iovlen=1;
 c->message.msg_control=c->control.bytes;c->message.msg_controllen=sizeof(c->control.bytes);
 h=(void *)c->control.bytes;h->cmsg_len=CMSG_LEN(sizeof(int));h->cmsg_level=SOL_SOCKET;h->cmsg_type=SCM_RIGHTS;
 fd_bytes=CMSG_DATA(h);source=(const void *)&c->source_fd;
 for(i=0;i<sizeof(int);i++)fd_bytes[i]=source[i];
 c->args[0]=(uint64_t)endpoint;c->args[1]=(uintptr_t)&c->message;c->args[2]=MSG_DONTWAIT|MSG_NOSIGNAL;
 c->before_result=slcli_native_clock_read(&c->before);c->before_returned=1;
 if(!c->before_result||c->before.tick<c->prior_tick||c->before.tick>=c->expires||
  d->origin!=c->origin||c->origin->tick!=c->origin_tick||d->expires!=c->expires)return 0;
 c->called=1;c->status=slcli_kernel_call(__NR_sendmsg,endpoint,(long)&c->message,(long)c->args[2],0,0,0);
 c->returned=1;c->after_result=slcli_native_clock_read(&c->after);c->after_returned=1;
 if(!c->after_result||c->after.tick<c->before.tick||c->after.tick>=c->expires||
  c->status<0||(uint64_t)c->status!=bytes||!prior->complete||prior->tick!=c->prior_tick||
  d->origin!=c->origin||c->origin->tick!=c->origin_tick||d->expires!=c->expires||
  !slcli_source_deadline_original_matches(a,d,caller))return 0;
 c->validation_result=raw_original(c);c->validation_returned=1;
 c->validation_clock_result=slcli_native_clock_read(&c->validation_after);c->validation_clock_returned=1;
 if(!validation_original(c))return 0;
 c->complete=1;c->failed=0;return 1;
}
#endif
