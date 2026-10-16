#include "stream_native.h"
#if defined(__linux__)
#include "native_syscall_linux.h"
#include <linux/fcntl.h>
#endif
#if defined(_WIN32) || defined(__linux__)
int slcli_stream_transfer_original(struct slcli_stream_capture *c,slcli_stream_transfer_sink sink,
  void *owner,const unsigned char *raw,size_t n) {
  unsigned phase=0;
  if(!c||!c->started||!sink||!owner||raw!=c->buffer||n>c->requested)return 0;
#if defined(_WIN32)
  phase=(c->event_wait.started||c->completion_observed)?1u:0u;
#endif
  if(c->transfer_attempted[phase])return c->transfer_owner[phase]==owner&&
    c->transfer_sink[phase]==sink&&c->transfer_bytes[phase]==raw&&c->transfer_lengths[phase]==n&&
    c->transfer_returned[phase]&&c->transfer_result[phase];
  c->transfer_owner[phase]=owner;c->transfer_sink[phase]=sink;
  c->transfer_bytes[phase]=raw;c->transfer_lengths[phase]=n;
  c->transfer_attempted[phase]=1;
  c->transfer_result[phase]=sink(owner,c,raw,n);
  c->transfer_returned[phase]=1;return c->transfer_result[phase];
}
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
#if defined(_WIN32)
int slcli_channel_event_wait_original(struct slcli_channel_event_wait *w,struct slcli_arena *a,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  const struct slcli_native_clock *prior,HANDLE event) {
  uint64_t remaining;size_t i;
  if(!w||!a||!caller_bytes||!slcli_arena_live_range(a,w,sizeof(*w))||
     !slcli_arena_live_block(a,caller,caller_bytes)||
     !slcli_source_deadline_original_matches(a,d,caller)||!prior||
     !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
     prior->tick<d->origin->tick||prior->tick>=d->expires||
     !slcli_source_deadline_away(d,w,sizeof(*w))||
     !separate(w,sizeof(*w),caller,caller_bytes)||
     !separate(w,sizeof(*w),prior,sizeof(*prior))||
     !event||event==INVALID_HANDLE_VALUE)return 0;
  for(i=0;i<sizeof(*w);i++)if(((const unsigned char *)w)[i])return 0;
  w->started=1;w->deadline=d;w->origin=d->origin;w->origin_tick=d->origin->tick;
  w->caller=caller;w->caller_bytes=caller_bytes;w->expires=d->expires;
  w->predecessor=prior;w->predecessor_tick=prior->tick;w->event=event;
  if(!slcli_native_clock_read(&w->before)||w->before.tick<w->predecessor_tick||
     w->before.tick>=w->expires)return 0;
  remaining=(w->expires-w->before.tick)/UINT64_C(1000000);
  if(remaining>=UINT32_MAX)return 0;
  w->milliseconds=(DWORD)remaining;w->args[0]=(uintptr_t)event;w->args[1]=w->milliseconds;
  w->called=1;w->status=WaitForSingleObject(event,w->milliseconds);w->returned=1;
  if(w->status==WAIT_FAILED)w->error=GetLastError();
  /* Actual wait result survives post-clock failure. Relative kernel timeout
   * is not a scheduler or low-power absolute-completion guarantee. */
  if(!slcli_native_clock_read(&w->after)||w->after.tick<w->before.tick||
     w->after.tick>=w->expires||!slcli_source_deadline_original_matches(a,d,caller)||
     d->origin!=w->origin||d->origin->tick!=w->origin_tick||d->expires!=w->expires||
     !prior->complete||prior->tick!=w->predecessor_tick)return 0;
  w->qualified=w->status==WAIT_OBJECT_0;return w->qualified;
}
#endif
#if defined(__linux__)
int slcli_channel_readiness_original(struct slcli_channel_readiness *w,
  const struct slcli_channel_ready_inputs *in,int fd,short events) {
  const struct slcli_source_deadline_original *d;const struct slcli_native_clock *p;
  uint64_t remaining;long result;size_t i;
  if(!w||!in||!in->arena||fd<0||(events!=POLLIN&&events!=POLLOUT)||
     !slcli_arena_live_range(in->arena,w,sizeof(*w))||!in->caller_bytes||
     !slcli_arena_live_block(in->arena,in->caller,in->caller_bytes)||
     !slcli_source_deadline_original_matches(in->arena,in->deadline,in->caller)||
     !(p=in->predecessor)||!slcli_arena_live_range(in->arena,p,sizeof(*p))||!p->complete||
     !slcli_source_deadline_away(in->deadline,w,sizeof(*w))||
     !separate(w,sizeof(*w),in->caller,in->caller_bytes)||!separate(w,sizeof(*w),p,sizeof(*p)))return 0;
  d=in->deadline;if(p->tick<d->origin->tick||p->tick>=d->expires)return 0;
  for(i=0;i<sizeof(*w);i++)if(((const unsigned char *)w)[i])return 0;
  w->started=1;w->deadline=d;w->origin=d->origin;w->origin_tick=d->origin->tick;
  w->expires=d->expires;w->caller=in->caller;w->caller_bytes=in->caller_bytes;
  w->predecessor=p;w->predecessor_tick=p->tick;
  if(!slcli_native_clock_read(&w->before)||w->before.tick<p->tick||w->before.tick>=w->expires)return 0;
  remaining=w->expires-w->before.tick;w->descriptor.fd=fd;w->descriptor.events=events;
  w->timeout_original.tv_sec=(long long)(remaining/UINT64_C(1000000000));
  w->timeout_original.tv_nsec=(long long)(remaining%UINT64_C(1000000000));
  w->timeout_returned=w->timeout_original;
  w->args[0]=(uintptr_t)&w->descriptor;w->args[1]=1;
  w->args[2]=(uintptr_t)&w->timeout_returned;w->args[3]=0;w->args[4]=8;
  w->called=1;result=slcli_kernel_call(__NR_ppoll,(long)&w->descriptor,1,
    (long)&w->timeout_returned,0,8,0);w->status=(uint64_t)result;w->returned=1;
  if(!slcli_native_clock_read(&w->after)||w->after.tick<w->before.tick||w->after.tick>=w->expires||
     !slcli_source_deadline_original_matches(in->arena,d,w->caller)||d->origin!=w->origin||
     d->origin->tick!=w->origin_tick||d->expires!=w->expires||!p->complete||p->tick!=w->predecessor_tick)return 0;
  /* HUP is an actual readable EOF opportunity, not a synthesized byte/exit.
   * The subsequent native read retains its own zero/error outcome. */
  w->qualified=result==1&&!(w->descriptor.revents&(POLLERR|POLLNVAL))&&
    ((w->descriptor.revents&events)||(events==POLLIN&&(w->descriptor.revents&POLLHUP)));
  return w->qualified;
}
#endif
static int read_original(struct slcli_stream_capture *c,slcli_input_handle original,
  unsigned char *buffer,size_t n,slcli_input_handle event,const struct slcli_stream_capture *prior,int owned,
  const struct slcli_channel_ready_inputs *ready) {
  if(!c||!buffer||!n||n>16384||!separate(c,sizeof(*c),buffer,n))return 0;
  /* Owning constructor supplies wholly byte-zeroed, precharged storage.
   * Marker0 alone cannot erase original partial native outputs or padding. */
  {size_t i;const unsigned char *raw=(const unsigned char *)c;
    for(i=0;i<sizeof(*c);i++)if(raw[i])return 0;}
#if defined(_WIN32)
  (void)ready;
  if(!original||original==INVALID_HANDLE_VALUE||!event||event==INVALID_HANDLE_VALUE||original==event)return 0;
#else
  if(original<0)return 0;(void)event;(void)prior;(void)owned;
#endif
  c->started=1;c->failed=1;c->original=original;c->buffer=buffer;c->requested=n;
#if defined(_WIN32)
  c->overlapped.hEvent=event;
  c->event_predecessor=prior;
  if(owned) {
    c->reset_original=event;c->reset_status=ResetEvent(event);c->reset_returned=1;
    if(!c->reset_status){c->reset_error=GetLastError();slcli_native_clock_read(&c->clock);return 0;}
  }
  c->args[0]=(uintptr_t)original;c->args[1]=(uintptr_t)buffer;c->args[2]=n;
  c->args[3]=(uintptr_t)&c->initial_count;c->args[4]=(uintptr_t)&c->overlapped;
  c->pending=1;
  c->initial_status=ReadFile(original,buffer,(DWORD)n,&c->initial_count,&c->overlapped);
  c->status=(uint32_t)c->initial_status;c->observed=1;
  if(!c->initial_status)c->initial_error=GetLastError();
  c->error=c->initial_error;
  if(!c->initial_status&&c->initial_error==ERROR_IO_PENDING) {
    if(!slcli_native_clock_read(&c->clock))return 0;c->failed=0;return 1;
  }
  c->pending=0;
  if(c->initial_status||c->initial_error==ERROR_BROKEN_PIPE||c->initial_error==ERROR_HANDLE_EOF) {
    if(c->initial_count>n)return 0;
    c->returned=c->initial_count;c->count_observed=1;c->pending=0;c->complete=1;
    c->eof=!c->returned;c->failed=0;
  }
#else
  {
    long flags,result;
    c->flags_args[0]=original;c->flags_args[1]=F_GETFL;
    flags=slcli_kernel_call(__NR_fcntl,original,F_GETFL,0,0,0,0);
    c->flags_status=(uint64_t)flags;c->flags_observed=1;
    if(flags<0||!(flags&O_NONBLOCK))return 0;
    if(ready&&!slcli_channel_readiness_original(&c->readiness,ready,original,POLLIN))return 0;
    c->args[0]=original;c->args[1]=(uintptr_t)buffer;c->args[2]=n;
    result=slcli_kernel_call(__NR_read,original,(long)buffer,(long)n,0,0,0);
    c->status=(uint64_t)result;c->observed=1;
    if(result>=0&&(uint64_t)result<=n) {
      c->returned=(size_t)result;c->count_observed=1;c->complete=1;
      c->eof=result==0;c->failed=0;
    }
    /* Negative EAGAIN/EINTR is retained as the actual native result, never an
     * empty stream/EOF. A later readiness/read attempt owns a distinct row. */
  }
#endif
  if(!slcli_native_clock_read(&c->clock)){c->failed=1;return 0;}
  return c->complete&&!c->failed;
}
int slcli_stream_read_original(struct slcli_stream_capture *c,slcli_input_handle h,
  unsigned char *b,size_t n,slcli_input_handle event) {
  return read_original(c,h,b,n,event,0,0,0);
}
#if defined(__linux__)
int slcli_stream_read_ready_original(struct slcli_stream_capture *c,slcli_input_handle h,
  unsigned char *b,size_t n,const struct slcli_channel_ready_inputs *ready) {
  if(!ready)return 0;return read_original(c,h,b,n,-1,0,0,ready);
}
#endif
#if defined(_WIN32)
int slcli_stream_read_owned_original(struct slcli_stream_capture *c,slcli_input_handle h,
  unsigned char *b,size_t n,HANDLE event,const struct slcli_stream_capture *prior) {
  return read_original(c,h,b,n,event,prior,1,0);
}
int slcli_stream_read_complete(struct slcli_stream_capture *c) {
  if(!c||!c->started||!c->pending||c->complete||c->completion_observed)return 0;
  c->completion_args[0]=(uintptr_t)c->original;c->completion_args[1]=(uintptr_t)&c->overlapped;
  c->completion_args[2]=(uintptr_t)&c->completion_count;c->completion_args[3]=FALSE;
  c->completion_status=GetOverlappedResult(c->original,&c->overlapped,&c->completion_count,FALSE);
  c->completion_observed=1;
  if(!c->completion_status)c->completion_error=GetLastError();
  /* Publish actual completed native IO/count facts BEFORE diagnostic time.
   * The enclosing collector/receipt raw sink must retain observed body bytes
   * even when the later clock fails. Actual completion is not retirement. */
  if(c->completion_status||c->completion_error==ERROR_BROKEN_PIPE||c->completion_error==ERROR_HANDLE_EOF||
     c->completion_error==ERROR_OPERATION_ABORTED) {
    c->pending=0;c->complete=1;
    if(c->completion_count>c->requested)c->failed=1;
    else {
      c->returned=c->completion_count;c->count_observed=1;
      c->eof=!c->returned&&(c->completion_status||c->completion_error==ERROR_BROKEN_PIPE||c->completion_error==ERROR_HANDLE_EOF);
      c->failed=c->completion_error==ERROR_OPERATION_ABORTED;
    }
  } else if(c->completion_error!=ERROR_IO_INCOMPLETE)c->failed=1;
  /* Preserve time for unsuccessful and incomplete actual observations too. */
  if(!slcli_native_clock_read(&c->completion_clock)){c->failed=1;return 0;}
  /* Incomplete IO retains its SAME output/buffer/OVERLAPPED; another one-use
   * observation requires separately retained completion storage, not reset. */
  if(!c->completion_status&&c->completion_error==ERROR_IO_INCOMPLETE)return 0;
  return c->complete&&!c->failed;
}
int slcli_stream_read_cancel(struct slcli_stream_capture *c) {
  if(!c||!c->started||!c->pending||c->cancel_started)return 0;c->cancel_started=1;
  c->cancel_args[0]=(uintptr_t)c->original;c->cancel_args[1]=(uintptr_t)&c->overlapped;
  c->cancel_status=CancelIoEx(c->original,&c->overlapped);c->cancel_observed=1;
  if(!c->cancel_status)c->cancel_error=GetLastError();
  /* Cancellation request, including ERROR_NOT_FOUND, is not completion or
   * permission to retire the buffer/event/channel. Original completion follows. */
  return c->cancel_status!=0;
}
#endif
#endif
