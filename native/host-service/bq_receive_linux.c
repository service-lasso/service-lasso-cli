#include "bq_receive_linux.h"
#if defined(__linux__)
#include "native_syscall_linux.h"
static int separate(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int claimed(const struct slcli_bq_receive_linux_original *c,
 struct slcli_binding_storage_original *s,uint32_t ordinal,uint32_t first_fd) {
 const struct slcli_binding_row_claim_original *claim;uintptr_t base,target;
 struct slcli_binding_storage_geometry g;uint32_t i;
 if(!s||!s->arena||!slcli_arena_live_block(s->arena,s,sizeof(*s))||!s->prepared||s->enrolled||
  ordinal>=s->attempted||s->attempted>s->capacity||s->capacity>SIZE_MAX/sizeof(*s->claims)||
  !slcli_arena_live_block(s->arena,s->claims,(size_t)s->capacity*sizeof(*s->claims))||
  !slcli_binding_storage_geometry_derive(s->capacity,&g)||
  !slcli_arena_live_block(s->arena,s->book,sizeof(*s->book))||s->book->storage_original!=s||
  s->book->count!=s->attempted||s->book->references!=s->references||s->book->original_ids!=s->ids||
  s->book->original_indices!=s->indices||s->book->primary_terminal_bitmap!=s->bitmap||
  s->book->initial_snapshot!=s->snapshot||s->book->primary_terminal_bitmap_bytes!=g.bitmap_bytes||
  !slcli_arena_live_block(s->arena,s->references,g.references_bytes)||
  !slcli_arena_live_block(s->arena,s->ids,g.ids_bytes)||!slcli_arena_live_block(s->arena,s->indices,g.indices_bytes)||
  !slcli_arena_live_block(s->arena,s->bitmap,g.bitmap_bytes)||!slcli_arena_live_block(s->arena,s->snapshot,g.snapshot_bytes)||
  s->references[ordinal].kind!=5)return 0;
 if(first_fd>s->attempted||SLCLI_BQ_LINUX_RECEIVED_FDS>s->attempted-first_fd||
  (ordinal>=first_fd&&ordinal-first_fd<SLCLI_BQ_LINUX_RECEIVED_FDS))return 0;
 claim=&s->claims[ordinal];base=(uintptr_t)claim->containing_block;target=(uintptr_t)c;
 if(!(claim->claimed&&claim->ordinal==ordinal&&claim->capture==c&&claim->capture_bytes==sizeof(*c)&&
  claim->native_class==SLCLI_NATIVE_ORIGINAL_BQ_RECVMSG_CAPTURE&&
  slcli_arena_live_block(s->arena,claim->containing_block,claim->containing_bytes)&&
  target>=base&&target-base<=claim->containing_bytes&&
  sizeof(*c)<=claim->containing_bytes-(target-base)&&claim->capture_offset==target-base))return 0;
 for(i=0;i<SLCLI_BQ_LINUX_RECEIVED_FDS;i++) {
  const struct slcli_binding_row_claim_original *fd=&s->claims[first_fd+i];
  if(!fd->claimed||fd->ordinal!=first_fd+i||fd->capture!=&c->received_fds[i]||
   fd->capture_bytes!=sizeof(c->received_fds[i])||
   fd->native_class!=SLCLI_NATIVE_ORIGINAL_BQ_RECEIVED_RIGHTS_OUTPUT||
   s->references[first_fd+i].kind!=2||
   fd->containing_block!=claim->containing_block||fd->containing_bytes!=claim->containing_bytes||
   fd->capture_offset!=(uintptr_t)&c->received_fds[i]-base)return 0;
 }
 return 1;
}
static int raw_matches(const struct slcli_bq_receive_linux_original *c) {
 size_t offset=0;uint32_t fds=0,credentials=0,rights=0;
 if(!c||!claimed(c,c->storage,c->ordinal,c->first_fd_ordinal)||!c->started||
  !c->called||!c->returned||c->endpoint<0||c->args[0]!=(uint64_t)c->endpoint||
  c->args[1]!=(uintptr_t)&c->message||c->args[2]!=(MSG_CMSG_CLOEXEC|MSG_DONTWAIT)||
  c->message.msg_name||c->message.msg_namelen||c->message.msg_iov!=&c->vector||c->message.msg_iovlen!=1||
  c->vector.iov_base!=c->payload||c->vector.iov_len!=sizeof(c->payload)||
  c->message.msg_control!=c->control.bytes||c->message.msg_controllen>sizeof(c->control.bytes)||
  c->status<=0||(uint64_t)c->status>sizeof(c->payload)||c->message.msg_flags&(MSG_TRUNC|MSG_CTRUNC)||
  !c->before_returned||!c->before_result||!c->before.complete||
  !c->after_returned||!c->after_result||!c->after.complete||
  !slcli_source_deadline_original_matches(c->storage->arena,c->deadline,c->caller)||
  !slcli_arena_live_block(c->storage->arena,c->caller,c->caller_bytes)||
  !c->prior||!slcli_arena_live_range(c->storage->arena,c->prior,sizeof(*c->prior))||
  !c->prior->complete||c->prior->tick!=c->prior_tick||c->deadline->origin!=c->origin||
  c->origin->tick!=c->origin_tick||c->deadline->expires!=c->expires||
  c->prior_tick<c->origin_tick||c->before.tick<c->prior_tick||c->after.tick<c->before.tick||
  c->after.tick>=c->expires)return 0;
 while(offset<c->message.msg_controllen) {
  const struct cmsghdr *h;const unsigned char *raw;size_t length,data,step,j;
  if(c->message.msg_controllen-offset<sizeof(struct cmsghdr))return 0;
  h=(const void *)(c->control.bytes+offset);length=h->cmsg_len;
  if(length<CMSG_LEN(0)||length>c->message.msg_controllen-offset)return 0;
  data=length-CMSG_LEN(0);raw=CMSG_DATA(h);
  if(h->cmsg_level==SOL_SOCKET&&h->cmsg_type==SCM_RIGHTS) {
   if(rights++||data%sizeof(int))return 0;
   for(j=0;j+sizeof(int)<=data;j+=sizeof(int)) {
    int fd;unsigned char *out=(void *)&fd;size_t k;
    for(k=0;k<sizeof(int);k++)out[k]=raw[j+k];
    if(fds>=SLCLI_BQ_LINUX_RECEIVED_FDS||c->received_fds[fds++]!=fd||fd<0)return 0;
   }
  } else if(h->cmsg_level==SOL_SOCKET&&h->cmsg_type==SCM_CREDENTIALS&&data==sizeof(struct ucred)) {
   const unsigned char *held=(const void *)&c->credentials;
   if(credentials++)return 0;for(j=0;j<data;j++)if(held[j]!=raw[j])return 0;
  } else return 0;
  step=CMSG_ALIGN(length);if(step<length)return 0;
  if(step>c->message.msg_controllen-offset){if(length!=c->message.msg_controllen-offset)return 0;break;}
  offset+=step;
 }
 if(fds!=1||rights!=1||credentials!=1||c->fd_count!=fds||c->rights_count!=rights||c->credential_count!=credentials)return 0;
 for(offset=fds;offset<SLCLI_BQ_LINUX_RECEIVED_FDS;offset++)if(c->received_fds[offset]!=-1)return 0;
 return 1;
}
static int validation_original(const struct slcli_bq_receive_linux_original *c) {
 return c->validation_returned&&c->validation_result&&
  c->validation_clock_returned&&c->validation_clock_result&&c->validation_after.complete&&
  c->validation_after.tick>=c->after.tick&&c->validation_after.tick<c->expires&&
  c->deadline->origin==c->origin&&c->origin->tick==c->origin_tick&&c->deadline->expires==c->expires&&
  c->prior->complete&&c->prior->tick==c->prior_tick;
}
int slcli_bq_receive_linux_matches_original(const struct slcli_bq_receive_linux_original *c) {
 return raw_matches(c)&&c->complete&&!c->failed&&validation_original(c);
}
int slcli_bq_receive_linux_preclaimed_original(struct slcli_bq_receive_linux_original *c,
 struct slcli_binding_storage_original *s,uint32_t ordinal,uint32_t first_fd,int endpoint,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior) {
 size_t i,offset=0;int shape=1;struct slcli_arena *a;
 if(!c||!claimed(c,s,ordinal,first_fd)||endpoint<0)return 0;a=s->arena;
 if(!caller_bytes||!slcli_arena_live_block(a,caller,caller_bytes)||
  !slcli_source_deadline_original_matches(a,d,caller)||!prior||
  !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  prior->tick<d->origin->tick||prior->tick>=d->expires||
  !separate(c,sizeof(*c),caller,caller_bytes)||!separate(c,sizeof(*c),prior,sizeof(*prior))||
  !slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
 for(i=0;i<sizeof(*c);i++)if(((unsigned char *)c)[i])return 0;
 c->started=1;c->failed=1;c->storage=s;c->ordinal=ordinal;c->first_fd_ordinal=first_fd;c->endpoint=endpoint;
 c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;c->prior=prior;c->prior_tick=prior->tick;
 c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
 for(i=0;i<SLCLI_BQ_LINUX_RECEIVED_FDS;i++)c->received_fds[i]=-1;
 c->vector.iov_base=c->payload;c->vector.iov_len=sizeof(c->payload);
 c->message.msg_iov=&c->vector;c->message.msg_iovlen=1;
 c->message.msg_control=c->control.bytes;c->message.msg_controllen=sizeof(c->control.bytes);
 c->args[0]=(uint64_t)endpoint;c->args[1]=(uintptr_t)&c->message;
 c->args[2]=MSG_CMSG_CLOEXEC|MSG_DONTWAIT;
 c->before_result=slcli_native_clock_read(&c->before);c->before_returned=1;
 if(!c->before_result||c->before.tick<c->prior_tick||c->before.tick>=c->expires||
  d->origin!=c->origin||c->origin->tick!=c->origin_tick||d->expires!=c->expires||!claimed(c,s,ordinal,first_fd))return 0;
 c->called=1;c->status=slcli_kernel_call(__NR_recvmsg,endpoint,(long)&c->message,(long)c->args[2],0,0,0);
 c->returned=1;
 c->after_result=slcli_native_clock_read(&c->after);c->after_returned=1;
 if(c->status<0)return 0;
 /* Extract every actual bounded rights output before rejecting extras or
  * truncated packets. Raw native control bytes remain retained unchanged. */
 if(c->message.msg_controllen>sizeof(c->control.bytes))return 0;
 while(offset<c->message.msg_controllen) {
  struct cmsghdr *h;size_t length,data,step,j;
  if(c->message.msg_controllen-offset<sizeof(struct cmsghdr)){shape=0;break;}
  h=(void *)(c->control.bytes+offset);length=h->cmsg_len;
  if(length<CMSG_LEN(0)||length>c->message.msg_controllen-offset){shape=0;break;}
  data=length-CMSG_LEN(0);
  if(h->cmsg_level==SOL_SOCKET&&h->cmsg_type==SCM_RIGHTS) {
   const unsigned char *raw=CMSG_DATA(h);
   if(c->rights_count++)shape=0;
   if(data%sizeof(int))shape=0;
   for(j=0;j+sizeof(int)<=data;j+=sizeof(int)) {
    int fd;unsigned char *out=(void *)&fd;size_t k;
    for(k=0;k<sizeof(int);k++)out[k]=raw[j+k];
    if(c->fd_count>=SLCLI_BQ_LINUX_RECEIVED_FDS){shape=0;break;}
    c->received_fds[c->fd_count++]=fd;
   }
  } else if(h->cmsg_level==SOL_SOCKET&&h->cmsg_type==SCM_CREDENTIALS&&data==sizeof(struct ucred)) {
   const unsigned char *raw=CMSG_DATA(h);unsigned char *out=(void *)&c->credentials;
   if(c->credential_count++)shape=0;
   else for(j=0;j<sizeof(c->credentials);j++)out[j]=raw[j];
  } else shape=0;
  step=CMSG_ALIGN(length);if(step<length){shape=0;break;}
  if(step>c->message.msg_controllen-offset){if(length!=c->message.msg_controllen-offset)shape=0;break;}
  offset+=step;
 }
 if(!c->after_result||c->after.tick<c->before.tick||c->after.tick>=c->expires||
  d->origin!=c->origin||c->origin->tick!=c->origin_tick||d->expires!=c->expires||
  !prior->complete||prior->tick!=c->prior_tick||!claimed(c,s,ordinal,first_fd)||
  !slcli_source_deadline_original_matches(a,d,caller)||!shape||
  c->status<=0||(uint64_t)c->status>sizeof(c->payload)||c->message.msg_flags&(MSG_TRUNC|MSG_CTRUNC)||
  c->credential_count!=1||c->rights_count!=1||c->fd_count!=1||c->received_fds[0]<0)return 0;
 c->validation_result=raw_matches(c);c->validation_returned=1;
 c->validation_clock_result=slcli_native_clock_read(&c->validation_after);c->validation_clock_returned=1;
 if(!validation_original(c))return 0;
 c->complete=1;c->failed=0;return 1;
}
#endif
