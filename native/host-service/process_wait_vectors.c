#include "process_wait_vectors.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&
    !(x<y+m&&y<x+n);
}
static int wait_arguments(const struct slcli_process_exit_capture *c) {
  if(!c||!c->started||!c->before.complete||!c->clock.started||!c->clock.observed||
     c->predecessor_tick>c->before.tick||!c->expires||c->before.tick>=c->expires)return 0;
#if defined(_WIN32)
  return c->wait_called&&c->observed&&c->wait_milliseconds!=INFINITE&&
    c->args[0]==(uintptr_t)c->original&&c->args[1]==c->wait_milliseconds&&
    c->wait_milliseconds==(c->expires-c->before.tick)/UINT64_C(1000000);
#else
  return c->readiness_called&&c->readiness_returned&&c->readiness.fd==c->original&&
    c->readiness.events==POLLIN&&
    c->timeout_original.tv_sec==(long long)((c->expires-c->before.tick)/UINT64_C(1000000000))&&
    c->timeout_original.tv_nsec==(long long)((c->expires-c->before.tick)%UINT64_C(1000000000))&&
    c->readiness_args[0]==(uintptr_t)&c->readiness&&c->readiness_args[1]==1&&
    c->readiness_args[2]==(uintptr_t)&c->timeout_returned&&
    !c->readiness_args[3]&&c->readiness_args[4]==8;
#endif
}
int slcli_process_wait_positive_vector(struct slcli_process_exit_capture *c,struct slcli_arena *a,
  const struct slcli_source_deadline_original *d,const void *caller,size_t bytes,
  const struct slcli_native_clock *prior,slcli_input_handle original) {
  if(!slcli_process_exit_original(c,a,d,caller,bytes,prior,original))return 0;
  /* The independently admitted fixture makes the original process exit while
   * this single readiness wait is pending; this result is not a birth proof. */
  if(!wait_arguments(c)||!c->complete||!c->ended||!c->natural_zero||
     !c->clock.complete||c->clock.tick<c->before.tick||c->clock.tick>=c->expires||
     c->deadline_original!=d||c->caller_original!=caller||c->caller_bytes!=bytes||
     c->predecessor_original!=prior||c->original!=original)return 0;
#if defined(_WIN32)
  return c->wait_status==WAIT_OBJECT_0&&c->exit_called&&c->exit_returned&&
    c->exit_status&&c->exit_code==0&&!c->exit_error;
#else
  return c->readiness_status==1&&c->wait_called&&c->observed&&c->status==0&&
    c->info.si_pid>0&&c->info.si_code==CLD_EXITED&&c->info.si_status==0;
#endif
}
int slcli_process_wait_timeout_vector(struct slcli_process_exit_capture *c,
  struct slcli_process_exit_capture *copy,struct slcli_arena *a,
  const struct slcli_source_deadline_original *d,const void *caller,size_t bytes,
  const struct slcli_native_clock *prior,slcli_input_handle original) {
  if(!c||!copy||!a||!slcli_arena_live_block(a,copy,sizeof(*copy))||
     !away(copy,sizeof(*copy),c,sizeof(*c))||
     !away(copy,sizeof(*copy),caller,bytes)||!away(copy,sizeof(*copy),prior,sizeof(*prior))||
     !away(copy,sizeof(*copy),a,sizeof(*a))||!a->owner||
     !away(copy,sizeof(*copy),a->owner,sizeof(*a->owner))||
     !slcli_source_deadline_away(d,copy,sizeof(*copy)))return 0;
  if(slcli_process_exit_original(c,a,d,caller,bytes,prior,original)||
     !wait_arguments(c)||c->complete||c->ended||c->natural_zero)return 0;
#if defined(_WIN32)
  if(c->wait_status!=WAIT_TIMEOUT||c->exit_called||c->exit_returned)return 0;
#else
  if(c->readiness_status!=0||c->wait_called||c->observed)return 0;
#endif
  *copy=*c;
  if(slcli_process_exit_original(c,a,d,caller,bytes,prior,original))return 0;
  /* Timeout preserves its original raw return and clocks. It cannot consume
   * another native call or overwrite them by replaying the one-use capture. */
  return slcli_equal((const unsigned char *)c,(const unsigned char *)copy,sizeof(*c));
}
#endif
