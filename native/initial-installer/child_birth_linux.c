#if defined(__linux__)
#include "child_birth_linux.h"
#include "../host-service/native_syscall_linux.h"
#include <linux/wait.h>
#include <linux/ptrace.h>
#include <limits.h>
#include <linux/fcntl.h>
#include <errno.h>
static int apart(const void *a,uint64_t n,const void *b,uint64_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&n<=SIZE_MAX&&m<=SIZE_MAX&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&
 (x+n<=y||y+m<=x);
}
static int zero(const void *p,uint64_t n) {
 const unsigned char *s=p;uint64_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;
}
static int unborn_stage(const struct slcli_initial_child_wait_stage_linux *);
static const char child_empty_path[]="";
static int clocks(const struct slcli_initial_custody_call_linux *r,uint64_t prior,uint64_t expiry) {
 return r->called==1&&r->returned==1&&slcli_native_clock_linux_matches_original(&r->before)&&
 slcli_native_clock_linux_matches_original(&r->after)&&r->before.tick>=prior&&
 r->before.tick<expiry&&r->after.tick>=r->before.tick&&r->after.tick<expiry;
}
static int plan(uint64_t a,uint64_t b,uint64_t *bytes,uint64_t *calls) {
 uint64_t count;
 if(!a||!b||a>UINT64_MAX-b)return 0;count=a+b;
 if(count>SIZE_MAX/sizeof(struct slcli_initial_child_wait_linux)||count>(UINT64_MAX-14)/3)return 0;
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
 b->parent_native_attempts=calls+12;
 b->stages[0].rows_original=first;b->stages[0].capacity=a;
 b->stages[1].rows_original=second;b->stages[1].capacity=n;return 1;
}
static int enrollment(const struct slcli_initial_child_birth_linux *b,uint64_t *expiry) {
 uint64_t bytes[2],i,total,calls;
 if(!b||!b->map_original||!slcli_initial_child_control_map_linux_output_away_original(
 b->map_original,b,sizeof(*b)))return 0;
 if(!plan(b->stages[0].capacity,b->stages[1].capacity,&total,&calls)||
 b->wait_row_bytes!=total||b->wait_native_attempts!=calls||b->parent_native_attempts!=calls+12)return 0;
 *expiry=b->map_original->expires;
 for(i=0;i<2;i++) {
 if(!b->stages[i].capacity||b->stages[i].capacity>SIZE_MAX/sizeof(*b->stages[i].rows_original))return 0;
 bytes[i]=b->stages[i].capacity*sizeof(*b->stages[i].rows_original);
 if(!apart(b,sizeof(*b),b->stages[i].rows_original,bytes[i])||
 !slcli_initial_child_control_map_linux_output_away_original(b->map_original,
 b->stages[i].rows_original,bytes[i]))return 0;
 }
 return apart(b->stages[0].rows_original,bytes[0],b->stages[1].rows_original,bytes[1]);
}
static uint32_t slot3_kind(const struct slcli_initial_child_birth_linux *b,long raw) {
 const struct slcli_initial_held_file_linux *e=b->map_original->executable_original;
 const struct slcli_initial_custody_memfd_linux *c=e->custody_original;
 /* A held source occurrence cannot coexist with an absent observation. */
 if(c->descriptor_original==3)return raw==FD_CLOEXEC?SLCLI_INITIAL_SLOT3_CUSTODY:0;
 if(c->launcher_original->identity.raw[7]==3)return raw==FD_CLOEXEC?SLCLI_INITIAL_SLOT3_SELF_PIDFD:0;
 if(e->opened_descriptor_original==3)return raw==FD_CLOEXEC?SLCLI_INITIAL_SLOT3_OPENED_FILE:0;
 return raw==-EBADF?SLCLI_INITIAL_SLOT3_ABSENT:0;
}
int slcli_initial_child_birth_linux_slot3_matches_original(const struct slcli_initial_child_birth_linux *b) {
 const struct slcli_initial_custody_call_linux *r;uint64_t expiry;uint32_t kind;
 if(!enrollment(b,&expiry)||b->slot3_started!=1||b->slot3_completed!=1||
 b->slot3_failed||b->slot3_pending)return 0;r=&b->slot3_call;
 kind=slot3_kind(b,r->raw);
 return kind&&b->slot3_kind==kind&&r->number==__NR_fcntl&&r->args[0]==3&&
 r->args[1]==F_GETFD&&!r->args[2]&&!r->args[3]&&!r->args[4]&&!r->args[5]&&
 clocks(r,b->map_original->finish.tick,expiry);
}
int slcli_initial_child_birth_linux_probe_slot3_original(struct slcli_initial_child_birth_linux *b) {
 struct slcli_initial_custody_call_linux *r;uint64_t expiry;
 if(!enrollment(b,&expiry)||b->slot3_started||b->slot3_completed||b->slot3_failed||
 b->slot3_pending||b->slot3_kind||!zero(&b->slot3_call,sizeof(b->slot3_call))||
 !zero(&b->clone_call,sizeof(b->clone_call))||!zero(&b->clone_input,sizeof(b->clone_input))||
 b->pidfd_original!=-1)return 0;
 b->slot3_started=1;b->slot3_failed=1;r=&b->slot3_call;
 r->number=__NR_fcntl;r->args[0]=3;r->args[1]=F_GETFD;
 if(!slcli_native_clock_read(&r->before)||!slcli_native_clock_linux_matches_original(&r->before)||
 r->before.tick<b->map_original->finish.tick||r->before.tick>=expiry){b->slot3_pending=1;return 0;}
 r->called=1;r->raw=slcli_kernel_call(r->number,3,F_GETFD,0,0,0,0);r->returned=1;
 if(!slcli_native_clock_read(&r->after)||!clocks(r,b->map_original->finish.tick,expiry)){
 b->slot3_pending=1;return 0;}
 b->slot3_kind=slot3_kind(b,r->raw);if(!b->slot3_kind)return 0;
 b->slot3_completed=1;b->slot3_failed=0;return 1;
}
static int source(const struct slcli_initial_child_birth_linux *b,uint64_t *expiry) {
 const struct slcli_initial_custody_call_linux *r;
 if(!enrollment(b,expiry)||!slcli_initial_child_birth_linux_slot3_matches_original(b))return 0;
 if(b->birth_started!=1||b->birth_returned!=1||b->birth_failed||b->birth_pending)return 0;
 r=&b->clone_call;
 return r->number==__NR_clone3&&r->args[0]==(long)(uintptr_t)&b->clone_input&&
 r->args[1]==sizeof(b->clone_input)&&!r->args[2]&&!r->args[3]&&!r->args[4]&&!r->args[5]&&
 r->raw>0&&r->raw<=INT_MAX&&b->pidfd_original>=0&&
 b->clone_input.flags==CLONE_PIDFD&&b->clone_input.exit_signal==SIGCHLD&&
 b->clone_input.pidfd==(uintptr_t)&b->pidfd_original&&!b->clone_input.child_tid&&
 !b->clone_input.parent_tid&&!b->clone_input.stack&&!b->clone_input.stack_size&&!b->clone_input.tls&&
 !b->clone_input.set_tid&&!b->clone_input.set_tid_size&&!b->clone_input.cgroup&&
 clocks(r,b->slot3_call.after.tick,*expiry);
}
/* These are new child-owned clock rows in the separately acquired shared map.
 * Parent copied clock pointers are never replayed as child-native originals. */
static int child_call(struct slcli_initial_child_control_linux *c,uint32_t index,
 long number,long a,long d,long e,long f,long g,long h,uint64_t prior,uint64_t expiry) {
 struct slcli_initial_custody_call_linux *r=&c->child_calls[index];
 if(!zero(r,sizeof(*r)))return 0;
 r->number=number;r->args[0]=a;r->args[1]=d;r->args[2]=e;
 r->args[3]=f;r->args[4]=g;r->args[5]=h;
 if(!slcli_native_clock_read(&r->before)||!slcli_native_clock_linux_matches_original(&r->before)||
 r->before.tick<prior||r->before.tick>=expiry)return 0;
 r->called=1;r->raw=slcli_kernel_call(number,a,d,e,f,g,h);r->returned=1;
 return slcli_native_clock_read(&r->after)&&clocks(r,prior,expiry);
}
static void child_failure(struct slcli_initial_child_control_linux *c) {
 struct slcli_initial_custody_call_linux *r=&c->child_calls[SLCLI_INITIAL_CHILD_EXIT];
 c->failed=1;
 /* Cleanup only: no fresh deadline, no claimed returned success/postclock. */
 r->number=__NR_exit_group;r->args[0]=127;c->exit_called=1;r->called=1;
 r->raw=slcli_kernel_call(__NR_exit_group,127,0,0,0,0,0);
 r->returned=1;c->exit_returned=1;c->trap_attempted=1;
 /* A returned exit is FAILED/UNKNOWN. This instruction proves no termination,
  * natural EOF, last borrower or release; external pending custody remains. */
 __builtin_trap();
}
static void child_original(struct slcli_initial_child_birth_linux *b) {
 struct slcli_initial_child_control_linux *c=b->map_original->control_original;
 const struct slcli_initial_held_file_linux *e=b->map_original->executable_original;
 const struct slcli_initial_child_input_linux *in=b->map_original->input_original;
 long custody=e->custody_original->descriptor_original,pid,tid,observed;
 uint64_t prior=b->clone_call.before.tick,expiry=b->map_original->expires;
 struct slcli_initial_custody_call_linux *r;
 c->clone_child_raw=0;c->clone_child_returned=1;
 if(!child_call(c,SLCLI_INITIAL_CHILD_TRACEME,__NR_ptrace,PTRACE_TRACEME,0,0,0,0,0,prior,expiry)||
 c->child_calls[SLCLI_INITIAL_CHILD_TRACEME].raw)child_failure(c);
 prior=c->child_calls[SLCLI_INITIAL_CHILD_TRACEME].after.tick;
 if(!child_call(c,SLCLI_INITIAL_CHILD_GETPID,__NR_getpid,0,0,0,0,0,0,prior,expiry))child_failure(c);
 pid=c->child_calls[SLCLI_INITIAL_CHILD_GETPID].raw;if(pid<=0||pid>INT_MAX)child_failure(c);
 prior=c->child_calls[SLCLI_INITIAL_CHILD_GETPID].after.tick;
 if(!child_call(c,SLCLI_INITIAL_CHILD_GETTID,__NR_gettid,0,0,0,0,0,0,prior,expiry))child_failure(c);
 tid=c->child_calls[SLCLI_INITIAL_CHILD_GETTID].raw;if(tid<=0||tid>INT_MAX)child_failure(c);
 prior=c->child_calls[SLCLI_INITIAL_CHILD_GETTID].after.tick;
 if(!child_call(c,SLCLI_INITIAL_CHILD_STOP,__NR_tgkill,pid,tid,SIGSTOP,0,0,0,prior,expiry)||
 c->child_calls[SLCLI_INITIAL_CHILD_STOP].raw)child_failure(c);
 prior=c->child_calls[SLCLI_INITIAL_CHILD_STOP].after.tick;
 if(!child_call(c,SLCLI_INITIAL_CHILD_GETFD3,__NR_fcntl,3,F_GETFD,0,0,0,0,prior,expiry))child_failure(c);
 observed=c->child_calls[SLCLI_INITIAL_CHILD_GETFD3].raw;
 if(observed!=b->slot3_call.raw)child_failure(c);
 prior=c->child_calls[SLCLI_INITIAL_CHILD_GETFD3].after.tick;
 if(b->slot3_kind==SLCLI_INITIAL_SLOT3_CUSTODY) {
 if(custody!=3||observed!=FD_CLOEXEC||
 !child_call(c,SLCLI_INITIAL_CHILD_SETFD3,__NR_fcntl,3,F_SETFD,0,0,0,0,prior,expiry)||
 c->child_calls[SLCLI_INITIAL_CHILD_SETFD3].raw)child_failure(c);
 prior=c->child_calls[SLCLI_INITIAL_CHILD_SETFD3].after.tick;
 } else {
 if(custody<0||custody==3)child_failure(c);
 if(b->slot3_kind!=SLCLI_INITIAL_SLOT3_ABSENT) {
 if((b->slot3_kind!=SLCLI_INITIAL_SLOT3_SELF_PIDFD&&b->slot3_kind!=SLCLI_INITIAL_SLOT3_OPENED_FILE)||
 observed!=FD_CLOEXEC||!child_call(c,SLCLI_INITIAL_CHILD_CLOSE3,__NR_close,3,0,0,0,0,0,prior,expiry)||
 c->child_calls[SLCLI_INITIAL_CHILD_CLOSE3].raw)child_failure(c);
 prior=c->child_calls[SLCLI_INITIAL_CHILD_CLOSE3].after.tick;
 } else if(observed!=-EBADF)child_failure(c);
 if(!child_call(c,SLCLI_INITIAL_CHILD_DUP3,__NR_dup3,custody,3,0,0,0,0,prior,expiry)||
 c->child_calls[SLCLI_INITIAL_CHILD_DUP3].raw!=3)child_failure(c);
 prior=c->child_calls[SLCLI_INITIAL_CHILD_DUP3].after.tick;
 }
 r=&c->child_calls[SLCLI_INITIAL_CHILD_EXEC];r->number=__NR_execveat;
 r->args[0]=e->descriptor_original;r->args[1]=(long)(uintptr_t)child_empty_path;
 r->args[2]=(long)(uintptr_t)in->argv_original;r->args[3]=(long)(uintptr_t)in->env_original;
 r->args[4]=AT_EMPTY_PATH;
 if(!slcli_native_clock_read(&r->before)||!slcli_native_clock_linux_matches_original(&r->before)||
 r->before.tick<prior||r->before.tick>=expiry)child_failure(c);
 r->called=1;r->raw=slcli_kernel_call(r->number,r->args[0],r->args[1],r->args[2],r->args[3],r->args[4],0);
 r->returned=1;c->failed=1;
 /* Only a failed exec returns; raw result survives even a failed postclock. */
 (void)slcli_native_clock_read(&r->after);child_failure(c);
}
int slcli_initial_child_birth_linux_capture_original(struct slcli_initial_child_birth_linux *b) {
 struct slcli_initial_custody_call_linux *r;uint64_t expiry;long result;
 if(!enrollment(b,&expiry)||!slcli_initial_child_birth_linux_slot3_matches_original(b)||
 b->birth_started||b->birth_returned||b->birth_failed||b->birth_pending||
 b->parent_started||b->parent_completed||b->parent_failed||b->parent_pending||
 !zero(&b->clone_call,sizeof(b->clone_call))||!zero(&b->clone_input,sizeof(b->clone_input))||
 !zero(&b->options_call,sizeof(b->options_call))||!zero(&b->continue_call,sizeof(b->continue_call))||
 !unborn_stage(&b->stages[0])||!unborn_stage(&b->stages[1])||
 !zero(b->map_original->control_original,sizeof(*b->map_original->control_original))||
 b->pidfd_original!=-1)return 0;
 b->birth_started=1;b->birth_failed=1;r=&b->clone_call;
 b->clone_input.flags=CLONE_PIDFD;b->clone_input.exit_signal=SIGCHLD;
 b->clone_input.pidfd=(uintptr_t)&b->pidfd_original;
 r->number=__NR_clone3;r->args[0]=(long)(uintptr_t)&b->clone_input;r->args[1]=sizeof(b->clone_input);
 if(!slcli_native_clock_read(&r->before)||!slcli_native_clock_linux_matches_original(&r->before)||
 r->before.tick<b->slot3_call.after.tick||r->before.tick>=expiry){b->birth_pending=1;return 0;}
 r->called=1;result=slcli_kernel_call(r->number,r->args[0],r->args[1],0,0,0,0);
 if(!result)child_original(b);
 r->raw=result;r->returned=1;b->birth_returned=1;
 if(!slcli_native_clock_read(&r->after)||!clocks(r,b->slot3_call.after.tick,expiry)){
 b->birth_pending=1;return 0;}
 if(result<=0||result>INT_MAX||b->pidfd_original<0)return 0;
 b->birth_failed=0;
 return slcli_initial_child_birth_linux_parent_observe_original(b);
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
static int child_row(const struct slcli_initial_child_control_linux *c,uint32_t index,
 long number,long a,long d,long e,long f,long g,long h,long raw,uint64_t *prior,uint64_t expiry) {
 const struct slcli_initial_custody_call_linux *r=&c->child_calls[index];
 if(r->number!=number||r->args[0]!=a||r->args[1]!=d||r->args[2]!=e||r->args[3]!=f||
 r->args[4]!=g||r->args[5]!=h||r->raw!=raw||!clocks(r,*prior,expiry))return 0;
 *prior=r->after.tick;return 1;
}
static int child_matches(const struct slcli_initial_child_birth_linux *b) {
 const struct slcli_initial_child_control_linux *c=b->map_original->control_original;
 const struct slcli_initial_held_file_linux *e=b->map_original->executable_original;
 const struct slcli_initial_child_input_linux *in=b->map_original->input_original;
 const struct slcli_initial_custody_call_linux *r;long pid=b->clone_call.raw;
 uint64_t prior=b->clone_call.before.tick,expiry=b->map_original->expires;
 if(c->clone_child_returned!=1||c->clone_child_raw||c->failed||c->exit_called||c->exit_returned||
 c->trap_attempted||!zero(&c->child_calls[SLCLI_INITIAL_CHILD_EXIT],sizeof(*c->child_calls)))return 0;
 if(!child_row(c,SLCLI_INITIAL_CHILD_TRACEME,__NR_ptrace,PTRACE_TRACEME,0,0,0,0,0,0,&prior,expiry)||
 !child_row(c,SLCLI_INITIAL_CHILD_GETPID,__NR_getpid,0,0,0,0,0,0,pid,&prior,expiry)||
 !child_row(c,SLCLI_INITIAL_CHILD_GETTID,__NR_gettid,0,0,0,0,0,0,pid,&prior,expiry)||
 !child_row(c,SLCLI_INITIAL_CHILD_STOP,__NR_tgkill,pid,pid,SIGSTOP,0,0,0,0,&prior,expiry)||
 prior<b->continue_call.before.tick||
 !child_row(c,SLCLI_INITIAL_CHILD_GETFD3,__NR_fcntl,3,F_GETFD,0,0,0,0,b->slot3_call.raw,&prior,expiry))return 0;
 if(b->slot3_kind==SLCLI_INITIAL_SLOT3_CUSTODY) {
 if(!zero(&c->child_calls[SLCLI_INITIAL_CHILD_CLOSE3],sizeof(*c->child_calls))||
 !zero(&c->child_calls[SLCLI_INITIAL_CHILD_DUP3],sizeof(*c->child_calls))||
 !child_row(c,SLCLI_INITIAL_CHILD_SETFD3,__NR_fcntl,3,F_SETFD,0,0,0,0,0,&prior,expiry))return 0;
 } else {
 if(!zero(&c->child_calls[SLCLI_INITIAL_CHILD_SETFD3],sizeof(*c->child_calls)))return 0;
 if(b->slot3_kind==SLCLI_INITIAL_SLOT3_ABSENT) {
 if(!zero(&c->child_calls[SLCLI_INITIAL_CHILD_CLOSE3],sizeof(*c->child_calls)))return 0;
 } else if(!child_row(c,SLCLI_INITIAL_CHILD_CLOSE3,__NR_close,3,0,0,0,0,0,0,&prior,expiry))return 0;
 if(!child_row(c,SLCLI_INITIAL_CHILD_DUP3,__NR_dup3,e->custody_original->descriptor_original,
 3,0,0,0,0,3,&prior,expiry))return 0;
 }
 r=&c->child_calls[SLCLI_INITIAL_CHILD_EXEC];
 return r->number==__NR_execveat&&r->args[0]==e->descriptor_original&&
 r->args[1]==(long)(uintptr_t)child_empty_path&&r->args[2]==(long)(uintptr_t)in->argv_original&&
 r->args[3]==(long)(uintptr_t)in->env_original&&r->args[4]==AT_EMPTY_PATH&&!r->args[5]&&
 r->called==1&&!r->returned&&!r->raw&&zero(&r->after,sizeof(r->after))&&
 slcli_native_clock_linux_matches_original(&r->before)&&r->before.tick>=prior&&
 r->before.tick<expiry&&r->before.tick<=b->stages[1].rows_original[b->stages[1].used-1].call.after.tick;
}
int slcli_initial_child_birth_linux_parent_matches_original(const struct slcli_initial_child_birth_linux *b) {
 return b&&b->parent_started==1&&b->parent_completed==1&&!b->parent_failed&&!b->parent_pending&&
 slcli_initial_child_birth_linux_wait_stage_matches_original(b,1)&&child_matches(b);
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
 if(!child_matches(b))return 0;
 b->parent_completed=1;b->parent_failed=0;return 1;
}
#endif
