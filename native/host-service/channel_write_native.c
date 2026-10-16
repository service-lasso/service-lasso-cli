#include "channel_write_native.h"
#if defined(__linux__)
#include "native_syscall_linux.h"
#include <linux/fcntl.h>
#endif
#if defined(_WIN32) || defined(__linux__)
static int write_original(struct slcli_channel_write_capture *c,slcli_input_handle h,
  const unsigned char *body,size_t n,slcli_input_handle event,const struct slcli_channel_write_capture *prior,int owned,
  const struct slcli_channel_ready_inputs *ready) {
  uintptr_t a=(uintptr_t)c,b=(uintptr_t)body;
  if(!c||!body||!n||n>16384||a>UINTPTR_MAX-sizeof(*c)||b>UINTPTR_MAX-n||
     (a<b+n&&b<a+sizeof(*c)))return 0;
  /* Fresh storage is a constructor-owned byte-zeroed reservation, including
   * padding. Never replace prior raw metadata merely because started is0. */
  {size_t i;const unsigned char *raw=(const unsigned char *)c;
    for(i=0;i<sizeof(*c);i++)if(raw[i])return 0;}
#if defined(_WIN32)
  (void)ready;
  if(!h||h==INVALID_HANDLE_VALUE||!event||event==INVALID_HANDLE_VALUE||h==event)return 0;
#else
  if(h<0)return 0;(void)event;(void)prior;(void)owned;
#endif
  c->started=1;c->failed=1;c->original=h;c->body=body;c->requested=n;
#if defined(_WIN32)
  c->overlapped.hEvent=event;c->args[0]=(uintptr_t)h;c->args[1]=b;c->args[2]=n;
  c->event_predecessor=prior;
  if(owned) {
    c->reset_original=event;c->reset_status=ResetEvent(event);c->reset_returned=1;
    if(!c->reset_status){c->reset_error=GetLastError();slcli_native_clock_read(&c->clock);return 0;}
  }
  c->args[3]=(uintptr_t)&c->initial_count;c->args[4]=(uintptr_t)&c->overlapped;
  c->initial_status=WriteFile(h,body,(DWORD)n,&c->initial_count,&c->overlapped);
  c->status=(uint32_t)c->initial_status;c->observed=1;
  if(!c->initial_status)c->initial_error=GetLastError();c->error=c->initial_error;
  if(!c->initial_status&&c->initial_error==ERROR_IO_PENDING)c->pending=1;
  else if(c->initial_status&&c->initial_count<=n) {
    c->returned=c->initial_count;c->count_observed=1;c->complete=1;c->failed=!c->returned;
  }
  if(!slcli_native_clock_read(&c->clock)){c->failed=1;return 0;}
  if(c->pending){c->failed=0;return 1;}
#else
  {
    long flags,status;c->flags_args[0]=h;c->flags_args[1]=F_GETFL;
    flags=slcli_kernel_call(__NR_fcntl,h,F_GETFL,0,0,0,0);
    c->flags_status=(uint64_t)flags;c->flags_observed=1;
    if(flags<0||!(flags&O_NONBLOCK))return 0;
    if(ready&&!slcli_channel_readiness_original(&c->readiness,ready,h,POLLOUT))return 0;
    c->args[0]=h;c->args[1]=b;c->args[2]=n;
    status=slcli_kernel_call(__NR_write,h,(long)body,(long)n,0,0,0);
    c->status=(uint64_t)status;c->observed=1;
    if(status>=0&&(uint64_t)status<=n) {
      c->returned=(size_t)status;c->count_observed=1;c->complete=1;c->failed=!c->returned;
    }
  }
  if(!slcli_native_clock_read(&c->clock)){c->failed=1;return 0;}
#endif
  return c->complete&&!c->failed;
}
int slcli_channel_write_original(struct slcli_channel_write_capture *c,slcli_input_handle h,
  const unsigned char *b,size_t n,slcli_input_handle event) {
  return write_original(c,h,b,n,event,0,0,0);
}
#if defined(__linux__)
int slcli_channel_write_ready_original(struct slcli_channel_write_capture *c,slcli_input_handle h,
  const unsigned char *b,size_t n,const struct slcli_channel_ready_inputs *ready) {
  if(!ready)return 0;return write_original(c,h,b,n,-1,0,0,ready);
}
#endif
#if defined(_WIN32)
int slcli_channel_write_owned_original(struct slcli_channel_write_capture *c,slcli_input_handle h,
  const unsigned char *b,size_t n,HANDLE event,const struct slcli_channel_write_capture *prior) {
  return write_original(c,h,b,n,event,prior,1,0);
}
int slcli_channel_write_complete(struct slcli_channel_write_capture *c) {
  if(!c||!c->started||!c->pending||c->complete||c->completion_observed)return 0;
  c->completion_args[0]=(uintptr_t)c->original;c->completion_args[1]=(uintptr_t)&c->overlapped;
  c->completion_args[2]=(uintptr_t)&c->completion_count;c->completion_args[3]=FALSE;
  c->completion_status=GetOverlappedResult(c->original,&c->overlapped,&c->completion_count,FALSE);
  c->completion_observed=1;if(!c->completion_status)c->completion_error=GetLastError();
  /* Native successful completion/count (including observed zero) precedes
   * diagnostics. Later clock failure never means pending IO or no bytes sent. */
  if(c->completion_status) {
    c->pending=0;c->complete=1;c->count_observed=1;c->returned=c->completion_count;
    c->failed=!c->returned||c->returned>c->requested;
  } else c->failed=1; /* Negative output validity/disposition is not invented. */
  if(!slcli_native_clock_read(&c->completion_clock)){c->failed=1;return 0;}
  return c->complete&&!c->failed;
}
#endif
#endif
