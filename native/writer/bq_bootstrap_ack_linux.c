/* SPDX-License-Identifier: Apache-2.0 */
#include "bq_bootstrap_ack_linux.h"
#if defined(__linux__)
#include "../host-service/native_syscall_linux.h"
static int apart(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&n<=UINTPTR_MAX-x&&m<=UINTPTR_MAX-y&&(x+n<=y||y+m<=x);
}
static void copy(void *to,const void *from,size_t n){size_t i;for(i=0;i<n;++i)((unsigned char *)to)[i]=((const unsigned char *)from)[i];}
static int source(const struct slcli_bq_bootstrap_ack_linux_original *r){
 if(!r||!r->arena||!r->deadline||!r->channel_original||!r->offer_original||!r->prior||!r->origin||!r->caller||!r->caller_bytes||
  !apart(r,sizeof(*r),r->arena,sizeof(*r->arena))||!r->arena->owner||
  !apart(r,sizeof(*r),r->arena->owner,sizeof(*r->arena->owner))||!slcli_memory_owner_mutable(r->arena->owner)||
  !apart(r,sizeof(*r),r->deadline,sizeof(*r->deadline))||!apart(r,sizeof(*r),r->channel_original,sizeof(*r->channel_original))||
  !apart(r,sizeof(*r),r->offer_original,sizeof(*r->offer_original))||!apart(r,sizeof(*r),r->prior,sizeof(*r->prior))||
  !apart(r,sizeof(*r),r->origin,sizeof(*r->origin))||!apart(r,sizeof(*r),r->caller,r->caller_bytes)||
  !slcli_arena_live_block(r->arena,r,sizeof(*r))||!slcli_arena_live_block(r->arena,r->caller,r->caller_bytes)||
  !slcli_arena_live_block(r->arena,r->channel_original,sizeof(*r->channel_original))||
  !slcli_arena_live_range(r->arena,r->offer_original,sizeof(*r->offer_original))||
  !slcli_arena_live_range(r->arena,r->prior,sizeof(*r->prior))||
  !slcli_source_deadline_original_matches(r->arena,r->deadline,r->caller)||
  !slcli_bq_channel_linux_matches_original(r->channel_original))return 0;
 return r->channel_original->arena==r->arena&&r->channel_original->deadline==r->deadline&&
  r->channel_original->caller==r->caller&&r->channel_original->caller_bytes==r->caller_bytes&&
  slcli_bq_subject_route_equal(r->offer_original,&r->offer_snapshot)&&
  r->offer_snapshot.message==SLCLI_BQ_SUBJECT_OFFER&&r->offer_snapshot.cut==SLCLI_BQ_SUBJECT_INITIAL_PROVIDER&&
  r->offer_snapshot.sequence>=1&&r->offer_snapshot.sequence<=3&&
  r->deadline->origin==r->origin&&r->origin->complete&&r->origin->observed&&r->origin->tick==r->origin_tick&&
  r->deadline->expires==r->expires&&r->prior->complete&&r->prior->observed&&r->prior->tick==r->prior_tick&&
  r->prior_tick>=r->origin_tick&&r->prior_tick<r->expires;
}
/* Inspect returned control before any fallible clock/decoder. Header loads are
 * byte copies, and every complete SCM_RIGHTS int is preserved even on refusal. */
static int ancillary(struct slcli_bq_bootstrap_ack_linux_original *r){
 size_t at=0,n=r->message.msg_controllen;int valid=1;
 if(n>sizeof(r->control.bytes)){n=sizeof(r->control.bytes);valid=0;}
 while(at<n){struct cmsghdr h;size_t bytes,step,i;
  if(n-at<sizeof(h)){valid=0;break;}
  copy(&h,r->control.bytes+at,sizeof(h));
  if(h.cmsg_len<CMSG_LEN(0)||h.cmsg_len>n-at){valid=0;break;}
  bytes=h.cmsg_len-CMSG_LEN(0);
  if(h.cmsg_level==SOL_SOCKET&&h.cmsg_type==SCM_RIGHTS){
   ++r->rights_count;
   for(i=0;i+sizeof(int)<=bytes;i+=sizeof(int)){
    int fd;copy(&fd,r->control.bytes+at+CMSG_LEN(0)+i,sizeof(fd));
    if(r->fd_count>=SLCLI_BQ_BOOTSTRAP_ACK_FDS){valid=0;break;}
    r->received_fds[r->fd_count++]=fd;
   }
   if(bytes%sizeof(int))valid=0;
  }else if(h.cmsg_level==SOL_SOCKET&&h.cmsg_type==SCM_CREDENTIALS){
   ++r->credential_count;
   if(bytes!=sizeof(r->credentials)||r->credential_count!=1)valid=0;
   else copy(&r->credentials,r->control.bytes+at+CMSG_LEN(0),sizeof(r->credentials));
  }else valid=0;
  step=CMSG_ALIGN(h.cmsg_len);
  if(step<h.cmsg_len){valid=0;break;}
  if(step>n-at){if(h.cmsg_len!=n-at)valid=0;break;}
  at+=step;
 }
 r->ancillary_scanned=1;return valid&&r->credential_count==1&&!r->rights_count&&!r->fd_count;
}
static int raw_matches(const struct slcli_bq_bootstrap_ack_linux_original *r){
 struct slcli_bq_subject_route expected,redecoded;struct cmsghdr header;struct ucred credentials;
 if(!source(r)||!r->started||!r->called||!r->returned||!r->readiness_returned||!r->readiness_result||
  !slcli_bq_readiness_linux_matches_original(&r->readiness)||r->readiness.prior!=r->prior||
  r->readiness.endpoint!=r->channel_original->endpoints[0]||r->readiness.requested_events!=POLLIN||
  !r->before_returned||!r->before_result||!r->before.complete||!r->before.observed||
  !r->after_returned||!r->after_result||!r->after.complete||!r->after.observed||
  r->before.tick<r->readiness.validation_after.tick||r->after.tick<r->before.tick||r->after.tick>=r->expires||
  r->raw_return!=64||!r->ancillary_result||!r->ancillary_scanned||r->fd_count||r->rights_count||r->credential_count!=1||
  (r->message.msg_flags&~MSG_CMSG_CLOEXEC)||r->message.msg_name||r->message.msg_namelen||
  r->message.msg_iov!=&r->vector||r->message.msg_iovlen!=1||
  r->message.msg_control!=r->control.bytes||r->message.msg_controllen!=CMSG_SPACE(sizeof(struct ucred))||
  r->vector.iov_base!=r->payload||r->vector.iov_len!=64||
  r->args[0]!=r->channel_original->endpoints[0]||r->args[1]!=(long)&r->message||
  r->args[2]!=(MSG_DONTWAIT|MSG_CMSG_CLOEXEC)||r->args[3]||r->args[4]||r->args[5]||
  !r->decode_returned||!r->decode_result)return 0;
  copy(&header,r->control.bytes,sizeof(header));
 if(header.cmsg_len!=CMSG_LEN(sizeof(credentials))||header.cmsg_level!=SOL_SOCKET||header.cmsg_type!=SCM_CREDENTIALS)return 0;
 copy(&credentials,r->control.bytes+CMSG_LEN(0),sizeof(credentials));
 if(credentials.pid!=r->credentials.pid||credentials.uid!=r->credentials.uid||credentials.gid!=r->credentials.gid||
  !slcli_bq_subject_route_decode(&redecoded,r->payload,64)||!slcli_bq_subject_route_equal(&redecoded,&r->decoded))return 0;
 expected=r->offer_snapshot;expected.message=SLCLI_BQ_SUBJECT_ARCHIVE_ACK;
 return slcli_bq_subject_route_equal(&expected,&r->decoded);
}
int slcli_bq_bootstrap_ack_linux_matches_original(const struct slcli_bq_bootstrap_ack_linux_original *r){
 return raw_matches(r)&&r->complete&&!r->failed&&!r->pending_borrow&&r->validation_returned&&r->validation_result&&
  r->validation_after.complete&&r->validation_after.observed&&r->validation_after.tick>=r->after.tick&&r->validation_after.tick<r->expires;
}
int slcli_bq_bootstrap_ack_linux_original(struct slcli_bq_bootstrap_ack_linux_original *r,struct slcli_arena *a,
 const struct slcli_bq_channel_linux_original *channel,const struct slcli_bq_subject_route *offer,
 const struct slcli_source_deadline_original *d,const void *caller,size_t bytes,const struct slcli_native_clock *prior){
 size_t i;
 if(!r||!a||!a->owner||!d||!channel||!offer||!prior||!caller||!bytes||!slcli_memory_owner_mutable(a->owner)||
  !slcli_arena_live_block(a,r,sizeof(*r))||!apart(r,sizeof(*r),a,sizeof(*a))||!apart(r,sizeof(*r),a->owner,sizeof(*a->owner))||
  !apart(r,sizeof(*r),d,sizeof(*d))||!apart(r,sizeof(*r),channel,sizeof(*channel))||
  !apart(r,sizeof(*r),offer,sizeof(*offer))||!apart(r,sizeof(*r),prior,sizeof(*prior))||!apart(r,sizeof(*r),caller,bytes)||
  !slcli_source_deadline_away(d,r,sizeof(*r))||!slcli_source_deadline_original_matches(a,d,caller)||
  !slcli_arena_live_range(a,offer,sizeof(*offer))||!slcli_arena_live_range(a,prior,sizeof(*prior)))return 0;
 for(i=0;i<sizeof(*r);++i)if(((const unsigned char *)r)[i])return 0;
 r->arena=a;r->channel_original=channel;r->deadline=d;r->caller=caller;r->caller_bytes=bytes;r->offer_original=offer;r->offer_snapshot=*offer;
 r->prior=prior;r->prior_tick=prior->tick;r->origin=d->origin;r->origin_tick=d->origin->tick;r->expires=d->expires;r->started=1;r->failed=1;
 if(!source(r))return 0;
 r->readiness_result=slcli_bq_readiness_linux_preowned_original(&r->readiness,a,channel->endpoints[0],POLLIN,d,caller,bytes,prior);
 r->readiness_returned=1;if(!r->readiness_result)return 0;
 r->before_result=slcli_native_clock_read(&r->before);r->before_returned=1;
 if(!r->before_result||!r->before.complete||!r->before.observed||!source(r)||
  r->before.tick<r->readiness.validation_after.tick||r->before.tick>=r->expires)return 0;
 r->vector.iov_base=r->payload;r->vector.iov_len=64;r->message.msg_iov=&r->vector;r->message.msg_iovlen=1;
 r->message.msg_control=r->control.bytes;r->message.msg_controllen=sizeof(r->control.bytes);
 r->args[0]=channel->endpoints[0];r->args[1]=(long)&r->message;r->args[2]=MSG_DONTWAIT|MSG_CMSG_CLOEXEC;
 r->pending_borrow=1;r->called=1;r->raw_return=slcli_kernel_call(__NR_recvmsg,r->args[0],r->args[1],r->args[2],0,0,0);r->returned=1;
 if(r->raw_return>=0)r->ancillary_result=ancillary(r);
 r->pending_borrow=0;
 r->after_result=slcli_native_clock_read(&r->after);r->after_returned=1;
 if(!r->ancillary_result)return 0;
 r->decode_result=slcli_bq_subject_route_decode(&r->decoded,r->payload,64);r->decode_returned=1;
 if(!raw_matches(r))return 0;
 r->validation_result=slcli_native_clock_read(&r->validation_after);r->validation_returned=1;
 if(!r->validation_result||!r->validation_after.complete||!r->validation_after.observed||
  r->validation_after.tick<r->after.tick||r->validation_after.tick>=r->expires||!raw_matches(r))return 0;
 r->complete=1;r->failed=0;
 if(!slcli_bq_bootstrap_ack_linux_matches_original(r)){r->failed=1;return 0;}return 1;
}
#endif