#include "process_native.h"
#if defined(__linux__)
#include "native_syscall_linux.h"
#include <linux/wait.h>
#include <asm/signal.h>
#endif
#if defined(_WIN32) || defined(__linux__)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&
    !(x<y+m&&y<x+n);
}
int slcli_process_exit_original(struct slcli_process_exit_capture *c,struct slcli_arena *a,
  const struct slcli_source_deadline_original *d,const void *caller,
  size_t caller_bytes,const struct slcli_native_clock *prior,slcli_input_handle original) {
  uint64_t remaining;size_t i;
  if(!c||c->started||!a||!slcli_arena_live_block(a,c,sizeof(*c))||
     !caller_bytes||!slcli_arena_live_block(a,caller,caller_bytes)||
     !slcli_source_deadline_original_matches(a,d,caller)||!prior||
     !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
     prior->tick<d->origin->tick||prior->tick>=d->expires||
     !slcli_source_deadline_away(d,c,sizeof(*c))||
     !away(c,sizeof(*c),caller,caller_bytes)||!away(c,sizeof(*c),prior,sizeof(*prior))||
     !away(c,sizeof(*c),a,sizeof(*a))||!a->owner||
     !away(c,sizeof(*c),a->owner,sizeof(*a->owner)))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
#if defined(_WIN32)
  if(!original||original==INVALID_HANDLE_VALUE)return 0;
#else
  if(original<0)return 0;
#endif
  c->started=1;c->original=original;c->deadline_original=d;c->caller_original=caller;c->caller_bytes=caller_bytes;
  c->origin_original=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
  c->predecessor_original=prior;c->predecessor_tick=prior->tick;
  if(!slcli_native_clock_read(&c->before)||c->before.tick<c->predecessor_tick||
     c->before.tick>=c->expires)return 0;
  remaining=c->expires-c->before.tick;
#if defined(_WIN32)
  /* Floor ms does not widen the original interval. No INFINITE sentinel;
   * actual scheduling/low-power latency is assessed by the SAME postclock. */
  remaining/=UINT64_C(1000000);if(remaining>=UINT32_MAX)return 0;
  c->wait_milliseconds=(DWORD)remaining;
  c->args[0]=(uintptr_t)original;c->args[1]=c->wait_milliseconds;
  c->wait_called=1;
  c->wait_status=WaitForSingleObject(original,c->wait_milliseconds);c->status=c->wait_status;c->observed=1;
  if(c->wait_status==WAIT_FAILED)c->error=GetLastError();
  if(c->wait_status==WAIT_OBJECT_0) {
    c->exit_args[0]=(uintptr_t)original;c->exit_args[1]=(uintptr_t)&c->exit_code;
    c->exit_called=1;
    c->exit_status=GetExitCodeProcess(original,&c->exit_code);
    c->exit_returned=1;
    if(!c->exit_status)c->exit_error=GetLastError();
    else {c->ended=1;c->natural_zero=c->exit_code==0;}
  }
#else
  {
    long status;
    /* ppoll consumes one finite native timespec, with no signal-mask change.
     * Keep its original timeout apart from the kernel's returned remainder. */
    c->readiness.fd=original;c->readiness.events=POLLIN;
    c->timeout_original.tv_sec=(long long)(remaining/UINT64_C(1000000000));
    c->timeout_original.tv_nsec=(long long)(remaining%UINT64_C(1000000000));
    c->timeout_returned=c->timeout_original;
    c->readiness_args[0]=(uintptr_t)&c->readiness;c->readiness_args[1]=1;
    c->readiness_args[2]=(uintptr_t)&c->timeout_returned;c->readiness_args[3]=0;c->readiness_args[4]=8;
    c->readiness_called=1;
    status=slcli_kernel_call(__NR_ppoll,(long)&c->readiness,1,(long)&c->timeout_returned,0,8,0);
    c->readiness_status=(uint64_t)status;c->readiness_returned=1;
    if(status!=1||!(c->readiness.revents&POLLIN)||
       (c->readiness.revents&(POLLERR|POLLNVAL)))goto postclock;
    c->args[0]=P_PIDFD;c->args[1]=original;c->args[2]=(uintptr_t)&c->info;
    c->args[3]=WEXITED|WNOWAIT|WNOHANG;c->args[4]=0;
    c->wait_called=1;
    status=slcli_kernel_call(__NR_waitid,P_PIDFD,original,(long)&c->info,
      WEXITED|WNOWAIT|WNOHANG,0,0);
    c->status=(uint64_t)status;c->observed=1;
    if(!status&&c->info.si_pid>0&&c->info.si_signo==SIGCHLD) {
      c->ended=c->info.si_code==CLD_EXITED||c->info.si_code==CLD_KILLED||c->info.si_code==CLD_DUMPED;
      c->natural_zero=c->info.si_code==CLD_EXITED&&c->info.si_status==0;
    }
  }
#endif
#if defined(__linux__)
postclock:
#endif
  if(!slcli_native_clock_read(&c->clock))return 0;
  if(!slcli_source_deadline_original_matches(a,d,caller)||d->origin!=c->origin_original||
     d->origin->tick!=c->origin_tick||d->expires!=c->expires||
     !prior->complete||prior->tick!=c->predecessor_tick||
     c->clock.tick<c->before.tick||c->clock.tick>=c->expires)return 0;
  if(!c->ended)return 0;c->complete=1;return 1;
}
#endif
