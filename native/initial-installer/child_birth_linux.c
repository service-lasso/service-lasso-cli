#if defined(__linux__)
#include "child_birth_linux.h"
#include "../host-service/native_syscall_linux.h"
#include <linux/wait.h>
#include <linux/ptrace.h>
#include <limits.h>
static int apart(const void *a,uint64_t n,const void *b,uint64_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&n<=SIZE_MAX&&m<=SIZE_MAX&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&
 (x+n<=y||y+m<=x);
}
static int zero(const void *p,uint64_t n) {
 const unsigned char *s=p;uint64_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;
}
static int clocks(const struct slcli_initial_custody_call_linux *r,uint64_t prior,uint64_t expiry) {
 return r->called==1&&r->returned==1&&slcli_native_clock_linux_matches_original(&r->before)&&
 slcli_native_clock_linux_matches_original(&r->after)&&r->before.tick>=prior&&
 r->before.tick<expiry&&r->after.tick>=r->before.tick&&r->after.tick<expiry;
}
static int plan(uint64_t a,uint64_t b,uint64_t *bytes,uint64_t *calls) {
 uint64_t count;
 if(!a||!b||a>UINT64_MAX-b)return 0;count=a+b;
 if(count>SIZE_MAX/sizeof(struct slcli_initial_child_wait_linux)||count>(UINT64_MAX-11)/3)return 0;
 *bytes=count*sizeof(struct slcli_initial_child_wait_linux);*calls=3*count+2;return 1;
}
int slcli_initial_child_birth_linux_prepare_wait_roster_original(
 struct slcli_initial_child_birth_linux *b,const struct slcli_initial_child_control_map_linux *m,
 struct slcli_initial_child_wait_linux *first,uint64_t a,
 struct slcli_initial_child_wait_linux *second,uint64_t n) {
 uint64_t bytes,calls,x,y;
 if(!b||!plan(a,n,&bytes,&calls))return 0;x=a*sizeof(*first);y=n*sizeof(*second);
 if(!slcli_initial_child_control_map_linux_output_away_original(m,b,sizeof(*b))||
 !slcli_initial_child_control_map_linux_output_away_original(m,first,x)||
 !slcli_initial_child_control_map_linux_output_away_original(m,second,y)||
 !apart(b,sizeof(*b),first,x)||!apart(b,sizeof(*b),second,y)||!apart(first,x,second,y)||
 !zero(b,sizeof(*b))||!zero(first,x)||!zero(second,y))return 0;
 b->map_original=m;b->pidfd_original=-1;b->wait_row_bytes=bytes;b->wait_native_attempts=calls;
 b->parent_native_attempts=calls+9;
 b->stages[0].rows_original=first;b->stages[0].capacity=a;
 b->stages[1].rows_original=second;b->stages[1].capacity=n;return 1;
}
static int source(const struct slcli_initial_child_birth_linux *b,uint64_t *expiry) {
 const struct slcli_initial_custody_call_linux *r;uint64_t bytes[2],i,total,calls;
 if(!b||!b->map_original||!slcli_initial_child_control_map_linux_output_away_original(
 b->map_original,b,sizeof(*b)))return 0;
 if(!plan(b->stages[0].capacity,b->stages[1].capacity,&total,&calls)||
 b->wait_row_bytes!=total||b->wait_native_attempts!=calls||b->parent_native_attempts!=calls+9)return 0;
 *expiry=b->map_original->expires;r=&b->clone_call;
 if(r->number!=__NR_clone3||r->args[0]!=(long)(uintptr_t)&b->clone_input||
 r->args[1]!=sizeof(b->clone_input)||r->args[2]||r->args[3]||r->args[4]||r->args[5]||
 r->raw<=0||r->raw>INT_MAX||b->pidfd_original<0||
 b->clone_input.flags!=CLONE_PIDFD||b->clone_input.exit_signal!=SIGCHLD||
 b->clone_input.pidfd!=(uintptr_t)&b->pidfd_original||b->clone_input.child_tid||
 b->clone_input.parent_tid||b->clone_input.stack||b->clone_input.stack_size||b->clone_input.tls||
 b->clone_input.set_tid||b->clone_input.set_tid_size||b->clone_input.cgroup||
 !clocks(r,b->map_original->finish.tick,*expiry))return 0;
 for(i=0;i<2;i++) {
 if(!b->stages[i].capacity||b->stages[i].capacity>SIZE_MAX/sizeof(*b->stages[i].rows_original))return 0;
 bytes[i]=b->stages[i].capacity*sizeof(*b->stages[i].rows_original);
 if(!apart(b,sizeof(*b),b->stages[i].rows_original,bytes[i])||
 !slcli_initial_child_control_map_linux_output_away_original(b->map_original,
 b->stages[i].rows_original,bytes[i]))return 0;
 }
 return apart(b->stages[0].rows_original,bytes[0],b->stages[1].rows_original,bytes[1]);
}
static int classify(const siginfo_t *s,long child) {
 if(!s->si_signo&&!s->si_pid&&!s->si_errno&&!s->si_code&&!s->si_uid&&!s->si_status)
 return SLCLI_INITIAL_WAIT_NO_EVENT;
 if(s->si_signo!=SIGCHLD||s->si_errno||s->si_pid!=child)return SLCLI_INITIAL_WAIT_UNEXPECTED;
 if(s->si_code==CLD_EXITED||s->si_code==CLD_KILLED||s->si_code==CLD_DUMPED)return SLCLI_INITIAL_WAIT_EXIT;
 if(s->si_code!=CLD_TRAPPED)return SLCLI_INITIAL_WAIT_UNEXPECTED;
 if(s->si_status==SIGSTOP)return SLCLI_INITIAL_WAIT_STOP;
 if(s->si_status==(SIGTRAP|(PTRACE_EVENT_EXEC<<8)))return SLCLI_INITIAL_WAIT_EXEC;
 return SLCLI_INITIAL_WAIT_UNEXPECTED;
}
static int replay(const struct slcli_initial_child_wait_stage_linux *s,long child,
 uint32_t stage,uint64_t prior,uint64_t expiry) {
 const struct slcli_initial_child_wait_linux *w;const struct slcli_initial_custody_call_linux *r;
 uint64_t i;int kind;
 if(s->started!=1||!s->used||s->used>s->capacity)return 0;
 for(i=0;i<s->used;i++) {
 w=&s->rows_original[i];r=&w->call;
 if(r->number!=__NR_waitid||r->args[0]!=P_PID||r->args[1]!=child||
 r->args[2]!=(long)(uintptr_t)&w->info||r->args[3]!=(WEXITED|WSTOPPED|WNOHANG)||
 r->args[4]||r->args[5]||r->raw||!clocks(r,prior,expiry))return 0;
 kind=classify(&w->info,child);
 if(w->classification!=(uint32_t)kind||kind!=(i+1==s->used?
 (stage?SLCLI_INITIAL_WAIT_EXEC:SLCLI_INITIAL_WAIT_STOP):SLCLI_INITIAL_WAIT_NO_EVENT))return 0;
 prior=r->after.tick;
 }
 return 1;
}
int slcli_initial_child_birth_linux_wait_stage_matches_original(
 const struct slcli_initial_child_birth_linux *b,uint32_t stage) {
 const struct slcli_initial_child_wait_stage_linux *s;
 const struct slcli_initial_custody_call_linux *r;uint64_t expiry,prior;
 if(stage>1||!source(b,&expiry))return 0;s=&b->stages[stage];prior=b->clone_call.after.tick;
 if(stage) {
 if(!slcli_initial_child_birth_linux_wait_stage_matches_original(b,0))return 0;
 prior=b->stages[0].finish.tick;r=&b->options_call;
 if(r->number!=__NR_ptrace||r->args[0]!=PTRACE_SETOPTIONS||r->args[1]!=b->clone_call.raw||
 r->args[2]||r->args[3]!=PTRACE_O_TRACEEXEC||r->args[4]||r->args[5]||r->raw||!clocks(r,prior,expiry))return 0;
 prior=r->after.tick;r=&b->continue_call;
 if(r->number!=__NR_ptrace||r->args[0]!=PTRACE_CONT||r->args[1]!=b->clone_call.raw||
 r->args[2]||r->args[3]||r->args[4]||r->args[5]||r->raw||!clocks(r,prior,expiry))return 0;
 prior=r->after.tick;
 }
 return s->completed==1&&!s->pending&&!s->failed&&replay(s,b->clone_call.raw,stage,prior,expiry)&&
 slcli_native_clock_linux_matches_original(&s->finish)&&
 s->finish.tick>=s->rows_original[s->used-1].call.after.tick&&s->finish.tick<expiry;
}
int slcli_initial_child_birth_linux_wait_stage_original(struct slcli_initial_child_birth_linux *b,uint32_t stage) {
 struct slcli_initial_child_wait_stage_linux *s;struct slcli_initial_child_wait_linux *w;
 struct slcli_initial_custody_call_linux *r;uint64_t expiry,prior,i;int kind;
 if(stage>1||!source(b,&expiry))return 0;s=&b->stages[stage];
 if(s->started||s->used||s->completed||s->pending||s->failed||!zero(&s->finish,sizeof(s->finish))||
 !zero(s->rows_original,s->capacity*sizeof(*s->rows_original)))return 0;
 prior=b->clone_call.after.tick;
 if(stage) {
 const struct slcli_initial_child_wait_stage_linux *first=&b->stages[0];
 if(first->completed!=1||first->failed||first->pending||
 !replay(first,b->clone_call.raw,0,prior,expiry)||
 !slcli_native_clock_linux_matches_original(&first->finish)||
 first->finish.tick<first->rows_original[first->used-1].call.after.tick||first->finish.tick>=expiry)return 0;
 prior=first->finish.tick;
 r=&b->options_call;
 if(r->number!=__NR_ptrace||r->args[0]!=PTRACE_SETOPTIONS||r->args[1]!=b->clone_call.raw||
 r->args[2]||r->args[3]!=PTRACE_O_TRACEEXEC||r->args[4]||r->args[5]||r->raw||!clocks(r,prior,expiry))return 0;
 prior=r->after.tick;r=&b->continue_call;
 if(r->number!=__NR_ptrace||r->args[0]!=PTRACE_CONT||r->args[1]!=b->clone_call.raw||
 r->args[2]||r->args[3]||r->args[4]||r->args[5]||r->raw||!clocks(r,prior,expiry))return 0;
 prior=r->after.tick;
 }
 s->started=1;s->failed=1;
 for(i=0;i<s->capacity;i++) {
 w=&s->rows_original[i];r=&w->call;s->used=i+1;
 r->number=__NR_waitid;r->args[0]=P_PID;r->args[1]=b->clone_call.raw;
 r->args[2]=(long)(uintptr_t)&w->info;r->args[3]=WEXITED|WSTOPPED|WNOHANG;
 if(!slcli_native_clock_read(&r->before)||!slcli_native_clock_linux_matches_original(&r->before)||
 r->before.tick<prior||r->before.tick>=expiry){s->pending=1;return 0;}
 r->called=1;r->raw=slcli_kernel_call(r->number,r->args[0],r->args[1],r->args[2],r->args[3],0,0);r->returned=1;
 if(!slcli_native_clock_read(&r->after)||!clocks(r,prior,expiry)){s->pending=1;return 0;}
 if(r->raw)return 0;kind=classify(&w->info,b->clone_call.raw);w->classification=kind;
 if(kind==SLCLI_INITIAL_WAIT_NO_EVENT){prior=r->after.tick;continue;}
 if(kind!=(stage?SLCLI_INITIAL_WAIT_EXEC:SLCLI_INITIAL_WAIT_STOP))return 0;
 if(!replay(s,b->clone_call.raw,stage,stage?b->continue_call.after.tick:b->clone_call.after.tick,expiry))return 0;
 if(!slcli_native_clock_read(&s->finish)||!slcli_native_clock_linux_matches_original(&s->finish)||
 s->finish.tick<r->after.tick||s->finish.tick>=expiry){s->pending=1;return 0;}
 s->completed=1;s->failed=0;return 1;
 }
 s->pending=1;return 0;
}
static int parent_ptrace(struct slcli_initial_child_birth_linux *b,
 struct slcli_initial_custody_call_linux *r,long request,long data,uint64_t prior) {
 uint64_t expiry=b->map_original->expires;
 if(!zero(r,sizeof(*r)))return 0;
 r->number=__NR_ptrace;r->args[0]=request;r->args[1]=b->clone_call.raw;r->args[3]=data;
 if(!slcli_native_clock_read(&r->before)||!slcli_native_clock_linux_matches_original(&r->before)||
 r->before.tick<prior||r->before.tick>=expiry)return -1;
 r->called=1;r->raw=slcli_kernel_call(r->number,r->args[0],r->args[1],0,r->args[3],0,0);r->returned=1;
 if(!slcli_native_clock_read(&r->after)||!clocks(r,prior,expiry))return -1;
 return !r->raw;
}
static int unborn_stage(const struct slcli_initial_child_wait_stage_linux *s) {
 return !s->started&&!s->completed&&!s->pending&&!s->failed&&!s->used&&
 zero(&s->finish,sizeof(s->finish))&&zero(s->rows_original,s->capacity*sizeof(*s->rows_original));
}
int slcli_initial_child_birth_linux_parent_matches_original(const struct slcli_initial_child_birth_linux *b) {
 return b&&b->parent_started==1&&b->parent_completed==1&&!b->parent_failed&&!b->parent_pending&&
 slcli_initial_child_birth_linux_wait_stage_matches_original(b,1);
}
int slcli_initial_child_birth_linux_parent_observe_original(struct slcli_initial_child_birth_linux *b) {
 uint64_t expiry;int result;
 if(!source(b,&expiry)||b->parent_started||b->parent_completed||b->parent_failed||b->parent_pending||
 !zero(&b->options_call,sizeof(b->options_call))||
 !zero(&b->continue_call,sizeof(b->continue_call))||
 !unborn_stage(&b->stages[0])||!unborn_stage(&b->stages[1]))return 0;
 b->parent_started=1;b->parent_failed=1;
 if(!slcli_initial_child_birth_linux_wait_stage_original(b,0)||
 !slcli_initial_child_birth_linux_wait_stage_matches_original(b,0)){
 b->parent_pending=b->stages[0].pending;return 0;}
 result=parent_ptrace(b,&b->options_call,PTRACE_SETOPTIONS,PTRACE_O_TRACEEXEC,b->stages[0].finish.tick);
 if(result!=1){b->parent_pending=result<0;return 0;}
 result=parent_ptrace(b,&b->continue_call,PTRACE_CONT,0,b->options_call.after.tick);
 if(result!=1){b->parent_pending=result<0;return 0;}
 if(!slcli_initial_child_birth_linux_wait_stage_original(b,1)){
 b->parent_pending=b->stages[1].pending;return 0;}
 b->parent_completed=1;b->parent_failed=0;return 1;
}
#endif
