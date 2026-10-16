#include "thread_windows.h"
#if defined(_WIN32)
#include "early_allocator.h"
#include "allocator_caller.h"
#include <intrin.h>
/* The original classified finite runtime-R/thread/stack/guard/TLS/module and physical
 * extent256 reservation must precede this genuine constructor. This component
 * retains actual native effects; it does not enroll those missing originals or
 * infer a measured full-process B0/B2 ceiling from logical arena containment. */
struct slcli_node_thread_birth {
  struct slcli_node_child_reservation *reservation;
  LPSECURITY_ATTRIBUTES security;
  SIZE_T stack_bytes;
  LPTHREAD_START_ROUTINE procedure;
  LPTHREAD_START_ROUTINE native_procedure;
  void *parameter;
  void *user_context;
  void *native_parameter;
  DWORD *native_id_output;
  size_t parameter_bytes;
  DWORD original_flags,native_flags,native_id,native_error;
  HANDLE handle;
  HANDLE observer,observer_process;
  uint64_t observer_args[7];
  BOOL observer_status;
  DWORD observer_error;
  size_t observer_ordinal;
  uint32_t observer_called,observer_returned,observer_qualified;
  size_t observer_wait_ordinal,observer_exit_ordinal;
  DWORD observer_wait_status,observer_wait_error,observer_exit_code,observer_exit_error;
  uint32_t observer_wait_milliseconds;
  BOOL observer_exit_status;
  uint32_t observer_wait_attempted,observer_wait_called,observer_wait_returned;
  uint32_t observer_exit_called,observer_exit_returned,observer_ended;
  DWORD resume_result,resume_error;
  size_t create_ordinal,resume_ordinal;
  uint64_t child_predecessor;
  uint32_t create_called,create_returned,native_id_observed,error_called,error_returned;
  uint32_t resume_called,resume_returned,resume_error_called,resume_error_returned;
  atomic_int phase; /* 0 unpublished, 1 resume completion pending, 2 ready, 3 unresolved */
};
size_t slcli_node_thread_birth_bytes_original(void){return sizeof(struct slcli_node_thread_birth);}
/* One source-owned TLS correlation pointer, not another quota/object pool.
 * Every retained birth remains in its original charged reservation and parent
 * capture row. Full TLS/source/runtime numeric fit remains UNKNOWN. */
static _Thread_local struct slcli_node_thread_birth *last_birth;
_Static_assert(ATOMIC_INT_LOCK_FREE==2,"child publication must be lock-free");
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
static int child_clock(struct slcli_node_thread_birth *b,
  struct slcli_node_allocator_caller *c,struct slcli_node_allocator_wait_row *row) {
  row->thread.started=1;row->thread.result=GetCurrentThreadId();row->thread.observed=1;
  if(row->thread.result!=b->native_id||c->deadline->origin!=c->origin||
    c->deadline->caller_original!=c->caller_original||c->deadline->expires!=c->expires||
    c->origin->tick!=c->origin_tick||!c->origin->started||!c->origin->observed||
    !c->origin->complete)return 0;
  if(!slcli_native_clock_read(&row->clock)||row->clock.tick<c->previous_tick||
    row->clock.tick>=c->expires)return 0;
  c->previous_tick=row->clock.tick;return 1;
}
static DWORD WINAPI child_start(void *parameter) {
  struct slcli_node_thread_birth *b=parameter;
  struct slcli_node_allocator_caller *c;
  int phase=atomic_load_explicit(&b->phase,memory_order_acquire);
  c=b->reservation->child;
  if(phase!=1&&phase!=2)goto failed;
  /* No original CRT PTD, apartment initialization or user/JS callback precedes
   * this join. Native birth facts remain distinct from actor/source authority. */
  c->thread.started=1;c->thread.result=GetCurrentThreadId();c->thread.observed=1;
  if(c->thread.result!=b->native_id||!b->native_id_observed||!b->create_returned||!b->handle||
    !c->prepared||!c->unborn_thread||c->unresolved)goto failed;
  c->native_thread=c->thread.result;
  /* Parent publishes the exact final birth/resume predecessor before phase2.
   * While resume diagnostics are pending, each wait consumes a fresh original
   * row under the SAME deadline. Four rows stay reserved for TLS attachment. */
  while(c->used<=c->capacity&&c->capacity-c->used>=5) {
    struct slcli_node_allocator_wait_row *row=&c->rows[c->used++];
    phase=atomic_load_explicit(&b->phase,memory_order_acquire);
    if(phase==3)goto failed;
    if(phase==2&&c->previous_tick<b->child_predecessor)c->previous_tick=b->child_predecessor;
    if(!child_clock(b,c,row))goto failed;
    if(phase==2) {
      c->child_phase=&b->phase;c->unborn_thread=0;
      if(!slcli_node_allocator_attach_caller_original(c))goto failed;
      if(atomic_load_explicit(&b->phase,memory_order_acquire)!=2)goto failed;
      return b->procedure(b->parameter);
    }
  }
failed:
  c->unresolved=1;
  /* A void/implicit native thread entry cannot continue into CRT allocations
   * without its genuine original caller. Fatal process/EOF/archive custody
   * remains required; this is not successful child/native retirement. */
  __fastfail(FAST_FAIL_FATAL_APP_EXIT);
}
struct slcli_node_thread_birth *slcli_node_thread_create_original(
  LPSECURITY_ATTRIBUTES security,SIZE_T stack_bytes,LPTHREAD_START_ROUTINE procedure,
  void *parameter,size_t parameter_bytes,void *user_context,DWORD flags) {
  struct slcli_node_child_reservation *reservation;
  struct slcli_node_thread_birth *b;
  struct slcli_node_allocator_caller *creator;
  size_t ordinal;
  int observer_raw_retained;
  if(!procedure||!parameter||!parameter_bytes||
    !slcli_node_allocator_allocation_original(parameter,parameter_bytes))return 0;
  reservation=slcli_node_allocator_reserve_child_original(sizeof(*b));if(!reservation)return 0;
  b=reservation->storage;creator=reservation->child->creator_original;
  /* Native output storage may never alias original deadline/caller/rows or the
   * actual parameter/security input. The original security extent still needs
   * its genuine admitted native ancestry; sizeof alone is not that admission. */
  if(!away(b,sizeof(*b),parameter,parameter_bytes)||
    !away(reservation,sizeof(*reservation),parameter,parameter_bytes)||
    !away(reservation->child,sizeof(*reservation->child),parameter,parameter_bytes)||
    !away(reservation->rows,reservation->capacity*sizeof(*reservation->rows),parameter,parameter_bytes)||
    !away(creator,sizeof(*creator),parameter,parameter_bytes)||
    !away(creator->rows,creator->capacity*sizeof(*creator->rows),parameter,parameter_bytes)||
    !away(creator->caller_original,creator->caller_bytes,parameter,parameter_bytes)||
    !slcli_source_deadline_away(creator->deadline,parameter,parameter_bytes)||
    (security&&(!away(b,sizeof(*b),security,sizeof(*security))||
      !away(reservation->child,sizeof(*reservation->child),security,sizeof(*security))||
      !away(reservation->rows,reservation->capacity*sizeof(*reservation->rows),security,sizeof(*security))))||
    !slcli_source_deadline_away(creator->deadline,b,sizeof(*b))||
    !away(b,sizeof(*b),creator->caller_original,creator->caller_bytes))return 0;
  b->reservation=reservation;b->security=security;b->stack_bytes=stack_bytes;
  b->procedure=procedure;b->parameter=parameter;b->parameter_bytes=parameter_bytes;b->original_flags=flags;
  b->user_context=user_context;last_birth=b;
  b->native_flags=flags|CREATE_SUSPENDED;atomic_init(&b->phase,0);
  b->native_procedure=child_start;b->native_parameter=b;b->native_id_output=&b->native_id;
  if(!slcli_node_allocator_native_before_original(5,b,sizeof(*b),stack_bytes,
       b->native_flags,&ordinal))goto failed;
  b->create_ordinal=ordinal;
  b->create_called=1;
  b->handle=CreateThread(b->security,b->stack_bytes,b->native_procedure,
    b->native_parameter,b->native_flags,b->native_id_output);
  b->create_returned=1;
  if(b->handle)b->native_id_observed=1; /* Actual API's successful birth output. */
  if(!b->handle){b->error_called=1;b->native_error=GetLastError();b->error_returned=1;}
  /* HANDLE and actual ID/error survive before every post-clock diagnostic. */
  if(!slcli_node_allocator_native_after_original(5,b,sizeof(*b),b->create_ordinal,
       (uint64_t)(uintptr_t)b->handle)||!b->handle)goto failed;
  /* Separate actual HANDLE lifetime before the suspended child can run.
   * Detach/join only receive the public original, never this observer alias.
   * The complete containing book must additionally enroll both originals. */
  if(!slcli_node_allocator_native_before_original(10,b,sizeof(*b),
       (uintptr_t)b->handle,THREAD_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,&ordinal))goto failed;
  b->observer_ordinal=ordinal;b->observer_process=GetCurrentProcess();
  b->observer_args[0]=(uintptr_t)b->observer_process;b->observer_args[1]=(uintptr_t)b->handle;
  b->observer_args[2]=(uintptr_t)b->observer_process;b->observer_args[3]=(uintptr_t)&b->observer;
  b->observer_args[4]=THREAD_QUERY_LIMITED_INFORMATION|SYNCHRONIZE;
  b->observer_called=1;
  b->observer_status=DuplicateHandle(b->observer_process,b->handle,b->observer_process,
    &b->observer,THREAD_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,0);
  b->observer_returned=1;
  observer_raw_retained=slcli_node_allocator_native_raw_return_original(10,b,sizeof(*b),ordinal,(uint32_t)b->observer_status);
  if(!b->observer_status)b->observer_error=GetLastError();
  if(!observer_raw_retained||!slcli_node_allocator_native_after_original(10,b,sizeof(*b),ordinal,
       (uint64_t)(uintptr_t)b->observer)||!b->observer_status||!b->observer||b->observer==b->handle)goto failed;
  b->observer_qualified=1;
  b->child_predecessor=creator->previous_tick;
  atomic_store_explicit(&b->phase,2,memory_order_release);return b;
failed:
  atomic_store_explicit(&b->phase,3,memory_order_release);return b;
}
HANDLE slcli_node_thread_handle_original(const struct slcli_node_thread_birth *b) {
  return b&&b->create_returned?b->handle:0;
}
DWORD slcli_node_thread_id_original(const struct slcli_node_thread_birth *b) {
  return b&&b->create_returned&&b->handle?b->native_id:0;
}
DWORD slcli_node_thread_error_original(const struct slcli_node_thread_birth *b) {
  return b&&b->error_returned?b->native_error:ERROR_SUCCESS;
}
DWORD slcli_node_thread_resume_error_original(const struct slcli_node_thread_birth *b) {
  return b&&b->resume_error_returned?b->resume_error:ERROR_SUCCESS;
}
int slcli_node_thread_birth_ready_original(const struct slcli_node_thread_birth *b) {
  return b&&b->create_returned&&b->handle&&b->observer_qualified&&b->observer&&
    atomic_load_explicit(&b->phase,memory_order_acquire)==2;
}
static int resume_original(struct slcli_node_thread_birth *b,int explicit_suspended) {
  struct slcli_node_allocator_caller *creator;
  size_t ordinal;
  if(!slcli_node_thread_birth_ready_original(b)||b->resume_called||
    (!!(b->original_flags&CREATE_SUSPENDED)!=explicit_suspended))return 0;
  creator=b->reservation->child->creator_original;
  if(!slcli_node_allocator_is_current_original(creator))return 0;
  if(!slcli_node_allocator_native_before_original(6,b,sizeof(*b),
       (uint64_t)(uintptr_t)b->handle,0,&ordinal))goto failed;
  b->resume_ordinal=ordinal;
  atomic_store_explicit(&b->phase,1,memory_order_release);
  b->resume_called=1;b->resume_result=ResumeThread(b->handle);b->resume_returned=1;
  if(b->resume_result==(DWORD)-1){b->resume_error_called=1;
    b->resume_error=GetLastError();b->resume_error_returned=1;}
  if(!slcli_node_allocator_native_after_original(6,b,sizeof(*b),b->resume_ordinal,
       b->resume_result)||b->resume_result==(DWORD)-1)goto failed;
  b->child_predecessor=creator->previous_tick;
  atomic_store_explicit(&b->phase,2,memory_order_release);return 1;
failed:
  atomic_store_explicit(&b->phase,3,memory_order_release);return 0;
}
int slcli_node_thread_resume_original(struct slcli_node_thread_birth *b) {
  return resume_original(b,0);
}
const struct slcli_node_thread_birth *slcli_node_thread_checkpoint_original(void) {
  return last_birth;
}
struct slcli_node_thread_birth *slcli_node_thread_birth_after_original(
  const struct slcli_node_thread_birth *prior,const void *context,HANDLE handle) {
  struct slcli_node_thread_birth *b=last_birth;
  if(!b||b==prior||b->user_context!=context||!b->create_returned||!b->handle||
    (handle&&b->handle!=handle))return 0;
  /* Read the actual retained birth even if its SAME parent is unresolved.
   * This return only tells the original caller to retain its parameter. */
  return b;
}
int slcli_node_thread_resume_suspended_original(struct slcli_node_thread_birth *b) {
  if(!b||b!=last_birth)return 0;
  return resume_original(b,1);
}
static struct slcli_node_thread_completion close_original(HANDLE *handle) {
  struct slcli_node_thread_completion answer={0,0,0};
  HANDLE original;BOOL result;DWORD error=0;size_t ordinal;int retained;
  if(!handle||!(*handle))return answer;
  original=*handle;
  if(!slcli_node_allocator_native_before_original(8,handle,sizeof(*handle),
       (uint64_t)(uintptr_t)original,0,&ordinal))return answer;
  result=CloseHandle(original);
  /* Retain actual caller output before post-clock, never retry a possibly
   * closed/reused native handle after diagnostic failure. */
  if(result)*handle=0;
  retained=slcli_node_allocator_native_raw_return_original(8,handle,sizeof(*handle),ordinal,(uint32_t)result);
  if(!result){error=GetLastError();answer.native_failure=1;answer.native_error=error;}
  if(!retained)return answer;
  if(!slcli_node_allocator_native_after_original(8,handle,sizeof(*handle),ordinal,
       ((uint64_t)error<<32)|(uint32_t)result))return answer;
  answer.complete=!!result;return answer;
}
struct slcli_node_thread_completion slcli_node_thread_detach_original(HANDLE *handle) {
  return close_original(handle);
}
struct slcli_node_thread_completion slcli_node_thread_join_original(HANDLE *handle) {
  struct slcli_node_thread_completion answer={0,0,0};
  HANDLE original;DWORD result,error;uint32_t milliseconds;size_t ordinal;int retained;
  if(!handle||!(*handle))return answer;
  original=*handle;
  for(;;) {
    if(*handle!=original||!slcli_node_allocator_wait_before_original(
         handle,sizeof(*handle),(uint64_t)(uintptr_t)original,&milliseconds,&ordinal))return answer;
    /* Real waiting yields normal scheduling within the fresh SAME original
     * remaining interval. Native/low-power return latency remains unqualified;
     * paired clocks cannot bound a stuck call or renew the original deadline. */
    result=WaitForSingleObject(original,(DWORD)milliseconds);error=0;
    retained=slcli_node_allocator_native_raw_return_original(7,handle,sizeof(*handle),ordinal,result);
    if(result==WAIT_FAILED){error=GetLastError();answer.native_failure=1;answer.native_error=error;}
    if(!retained)return answer;
    if(!slcli_node_allocator_native_after_original(7,handle,sizeof(*handle),ordinal,
         ((uint64_t)error<<32)|result))return answer;
    if(result==WAIT_OBJECT_0)return close_original(handle);
    if(result!=WAIT_TIMEOUT)return answer;
  }
}
struct slcli_node_thread_completion slcli_node_thread_observer_exit_original(
 struct slcli_node_thread_birth *b) {
 struct slcli_node_thread_completion answer={0,0,0};size_t ordinal;int retained;
 uint32_t milliseconds;
 /* Do not dereference a supplied address until the actual one-use schedule
  * has found that exact paid consumed typed storage under its owning marker. */
 if(!slcli_node_allocator_child_storage_original(b,sizeof(*b))||
  !b->observer_qualified||!b->observer||b->observer==b->handle||
  b->observer_wait_attempted)return answer;
 b->observer_wait_attempted=1;
 if(!slcli_node_allocator_wait_before_original(b,sizeof(*b),(uintptr_t)b->observer,
      &milliseconds,&ordinal))return answer;
 b->observer_wait_milliseconds=milliseconds;b->observer_wait_ordinal=ordinal;
 b->observer_wait_called=1;
 b->observer_wait_status=WaitForSingleObject(b->observer,(DWORD)milliseconds);
 b->observer_wait_returned=1;
 retained=slcli_node_allocator_native_raw_return_original(7,b,sizeof(*b),ordinal,b->observer_wait_status);
 if(b->observer_wait_status==WAIT_FAILED){b->observer_wait_error=GetLastError();
  answer.native_failure=1;answer.native_error=b->observer_wait_error;}
 if(!retained||!slcli_node_allocator_native_after_original(7,b,sizeof(*b),ordinal,
      ((uint64_t)b->observer_wait_error<<32)|b->observer_wait_status)||
  b->observer_wait_status!=WAIT_OBJECT_0)return answer;
 if(!slcli_node_allocator_native_before_original(11,b,sizeof(*b),(uintptr_t)b->observer,
      (uintptr_t)&b->observer_exit_code,&ordinal))return answer;
 b->observer_exit_ordinal=ordinal;b->observer_exit_called=1;
 b->observer_exit_status=GetExitCodeThread(b->observer,&b->observer_exit_code);
 b->observer_exit_returned=1;
 retained=slcli_node_allocator_native_raw_return_original(11,b,sizeof(*b),ordinal,(uint32_t)b->observer_exit_status);
 if(!b->observer_exit_status){b->observer_exit_error=GetLastError();
  answer.native_failure=1;answer.native_error=b->observer_exit_error;}
 if(!retained||!slcli_node_allocator_native_after_original(11,b,sizeof(*b),ordinal,
      ((uint64_t)b->observer_exit_error<<32)|b->observer_exit_code)||!b->observer_exit_status)return answer;
 /* WAIT_OBJECT_0, rather than an exit-code sentinel, establishes ended state.
  * The numeric exit code may legitimately equal STILL_ACTIVE. */
 b->observer_ended=1;answer.complete=1;return answer;
}
void slcli_node_thread_exit_original(HMODULE module,DWORD code) {
  struct terminal_call { HMODULE module;DWORD code;uint32_t called; } call={module,code,0};
  size_t ordinal;
  if(!slcli_node_allocator_native_before_original(9,&call,sizeof(call),
       (uint64_t)(uintptr_t)module,code,&ordinal))__fastfail(FAST_FAIL_FATAL_APP_EXIT);
  call.called=1;
  /* The actual native call does not return. Its reserved completion row stays
   * unobserved; only an independent authentic native exit observation can join
   * this attempt to disposition. No post-clock/retirement is fabricated here. */
  if(call.module)FreeLibraryAndExitThread(call.module,call.code);
  else ExitThread(call.code);
  __fastfail(FAST_FAIL_FATAL_APP_EXIT);
}
#endif
