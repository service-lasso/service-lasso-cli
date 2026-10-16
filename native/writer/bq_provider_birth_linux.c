/* SPDX-License-Identifier: Apache-2.0 */
#include "bq_provider_birth_linux.h"
#if defined(__linux__)
#include "../host-service/native_syscall_linux.h"
#include <linux/fcntl.h>
#include <linux/futex.h>
#include <linux/mman.h>
#include <asm/signal.h>
#include <asm-generic/errno.h>
#include <limits.h>
_Static_assert(sizeof(atomic_uint)==4,"Native shared futex width");
_Static_assert(ATOMIC_INT_LOCK_FREE==2,"Shared publication must not call an unowned atomic helper");
_Static_assert(_Alignof(atomic_uint)>=4,"Native futex alignment");
_Static_assert(offsetof(struct slcli_bq_provider_linux_child,gate)%4==0,"Native futex field alignment");
static int apart(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&n<=UINTPTR_MAX-x&&m<=UINTPTR_MAX-y&&(x+n<=y||y+m<=x);
}
static int zero(const void *p,size_t n){
 const unsigned char *b=p;size_t i;for(i=0;i<n;++i)if(b[i])return 0;return 1;
}
static int inputs_same(const struct slcli_bq_provider_linux_birth_original *c){
 const struct slcli_bq_provider_linux_birth_inputs *i=c->inputs,*o=&c->original;
 const struct slcli_bq_provider_roster *r=o->roster,*s=&c->roster_original;
 return i&&slcli_arena_live_block(c->arena,i,sizeof(*i))&&i->channel==o->channel&&
  i->allowlist==o->allowlist&&i->allowlist_count==o->allowlist_count&&i->executable==o->executable&&
  i->directory==o->directory&&i->argv==o->argv&&i->environment==o->environment&&i->argc==o->argc&&
  i->environment_count==o->environment_count&&i->endpoint_text==o->endpoint_text&&
  i->count_text==o->count_text&&i->roster==o->roster&&
  i->bootstrap_storage==o->bootstrap_storage&&
  slcli_bq_bootstrap_storage_linux_same_original(o->bootstrap_storage,&c->bootstrap_storage_original)&&
  i->shared_arena==o->shared_arena&&i->shared_original==o->shared_original&&i->child==o->child&&
  i->fds==o->fds&&i->gaps==o->gaps&&r&&slcli_arena_live_block(c->arena,r,sizeof(*r))&&
  r->phase==s->phase&&r->process_member==s->process_member&&
  r->requirements_per_command==s->requirements_per_command&&r->command_count==s->command_count&&
  r->subjects==s->subjects&&r->subject_count==s->subject_count;
}
static int same(const struct slcli_bq_provider_linux_birth_original *c){
 return c&&c->arena&&slcli_arena_live_block(c->arena,c,sizeof(*c))&&c->started&&
  slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)&&
  c->deadline->origin==c->origin&&c->origin->tick==c->origin_tick&&c->deadline->expires==c->expires&&
  c->prior&&slcli_arena_live_range(c->arena,c->prior,sizeof(*c->prior))&&
  slcli_native_clock_linux_matches_original(c->prior)&&slcli_native_clock_linux_matches_original(c->origin)&&
  c->prior->tick==c->prior_tick&&inputs_same(c);
}
static int shared(const struct slcli_bq_provider_linux_birth_inputs *in,struct slcli_arena *a){
 const struct slcli_memory_capture *m=in->shared_original;const struct slcli_extent *e;
 if(!in->shared_arena||!slcli_arena_live_block(a,in->shared_arena,sizeof(*in->shared_arena))||
  !m||!slcli_arena_live_block(a,m,sizeof(*m))||in->shared_arena->owner!=a->owner||
  in->shared_arena->extent==a->extent||m->request.index!=in->shared_arena->extent||
  m->request.index>=a->owner->count||m->request.index>=256||
  m->operation!=9||!m->observed||!m->native_acquired||!m->result||
  m->flags!=(MAP_SHARED|MAP_ANONYMOUS)||m->protection!=(PROT_READ|PROT_WRITE)||
  !m->tick_calls||!m->tick_returns||m->tick_status[0]||!m->tick||
  m->request_args[0]||m->request_args[1]!=m->bytes||m->request_args[2]!=m->protection||
  m->request_args[3]!=m->flags||m->request_args[4]!=UINT64_MAX||m->request_args[5])return 0;
 e=&a->owner->extents[m->request.index];
 return e->state==SLCLI_EXTENT_LIVE&&e->base==m->result&&e->reserved==m->bytes&&
  m->reserved==m->bytes&&e->acquisition==m->request.sequence&&
  in->shared_arena->acquisition==m->request.sequence;
}
static int child_effect(struct slcli_bq_provider_linux_birth_original *p,
 struct slcli_bq_provider_native_row *r,long number,long a,long b,long c,long d,long e,long f){
 struct slcli_bq_provider_linux_child *s=p->original.child;
 r->started=1;r->number=number;r->args[0]=a;r->args[1]=b;r->args[2]=c;r->args[3]=d;r->args[4]=e;r->args[5]=f;
 r->before_result=slcli_native_clock_read(&r->before);r->before_returned=1;
 if(!r->before_result||r->before.tick<s->previous_tick||r->before.tick>=p->expires)return 0;
 r->called=1;r->status=slcli_kernel_call(number,a,b,c,d,e,f);r->returned=1;
 r->after_result=slcli_native_clock_read(&r->after);r->after_returned=1;
 if(!r->after_result||r->after.tick<r->before.tick||r->after.tick>=p->expires)return 0;
 s->previous_tick=r->after.tick;return 1;
}
_Noreturn static void child_exit(struct slcli_bq_provider_linux_birth_original *p){
 struct slcli_bq_provider_linux_child *s=p->original.child;
 struct slcli_bq_provider_native_row *r=&s->exit;
 s->failed=1;r->started=1;r->number=__NR_exit_group;r->args[0]=127;
 r->before_result=slcli_native_clock_read(&r->before);r->before_returned=1;
 r->called=1;atomic_store_explicit(&s->publication,3,memory_order_release);
 r->status=slcli_kernel_call(__NR_exit_group,127,0,0,0,0,0);r->returned=1;
 atomic_store_explicit(&s->publication,4,memory_order_release);__builtin_trap();
}
_Noreturn static void child_run(struct slcli_bq_provider_linux_birth_original *p){
 const struct slcli_bq_provider_linux_birth_inputs *in=&p->original;
 struct slcli_bq_provider_linux_child *s=in->child;
 const long birth[6]={__NR_getpid,__NR_gettid,__NR_getuid,__NR_geteuid,__NR_getgid,__NR_getegid};
 size_t i;uint64_t first=0;struct slcli_bq_provider_native_row *r;
 s->started=1;s->previous_tick=p->before.tick;
 for(i=0;i<6;++i)if(!child_effect(p,&s->birth[i],birth[i],0,0,0,0,0,0)||
  s->birth[i].status<0||(i<2&&(s->birth[i].status<=0||s->birth[i].status>INT_MAX))||
  (i>=2&&(uint64_t)s->birth[i].status>UINT32_MAX))child_exit(p);
 atomic_store_explicit(&s->birth_publication,1,memory_order_release);
 s->timeout.tv_sec=(long long)(p->expires/UINT64_C(1000000000));
 s->timeout.tv_nsec=(long long)(p->expires%UINT64_C(1000000000));
 if(!atomic_load_explicit(&s->gate,memory_order_acquire)){
  if(!child_effect(p,&s->wait,__NR_futex,(long)&s->gate,FUTEX_WAIT_BITSET,0,
    (long)&s->timeout,0,FUTEX_BITSET_MATCH_ANY)||
   (s->wait.status&&s->wait.status!=-EAGAIN))child_exit(p);
 }
 if(atomic_load_explicit(&s->gate,memory_order_acquire)!=1)child_exit(p);
 if(!child_effect(p,&s->directory,__NR_fchdir,in->directory,0,0,0,0,0)||s->directory.status)child_exit(p);
 for(i=0;i<s->fd_count;++i){
  struct slcli_bq_provider_fd_row *fd=&s->fds[i];uint64_t next=(uint32_t)fd->original.fd;
  /* Empty gaps remain explicitly uncalled. Nonempty gaps close all descriptors,
   * including unexpected aliases, without requiring a guessed fd ceiling. */
  if(first<next&&(!child_effect(p,&s->gaps[i],__NR_close_range,(long)first,(long)(next-1),0,0,0,0)||
    s->gaps[i].status))child_exit(p);
  if(!child_effect(p,&fd->flags,__NR_fcntl,fd->original.fd,F_GETFD,0,0,0,0)||fd->flags.status<0)child_exit(p);
  if(!child_effect(p,&fd->set,__NR_fcntl,fd->original.fd,F_SETFD,
    fd->original.action==SLCLI_BQ_PROVIDER_KEEP?fd->flags.status&~FD_CLOEXEC:fd->flags.status|FD_CLOEXEC,0,0,0)||
    fd->set.status)child_exit(p);
  first=next+1;
 }
 if(first<=UINT32_MAX&&(!child_effect(p,&s->gaps[s->fd_count],__NR_close_range,(long)first,
   (long)UINT32_MAX,0,0,0,0)||s->gaps[s->fd_count].status))child_exit(p);
 r=&s->exec;r->started=1;r->number=__NR_execveat;r->args[0]=in->executable;
 r->args[1]=(long)"";r->args[2]=(long)in->argv;r->args[3]=(long)in->environment;r->args[4]=AT_EMPTY_PATH;
 r->before_result=slcli_native_clock_read(&r->before);r->before_returned=1;
 if(!r->before_result||r->before.tick<s->previous_tick||r->before.tick>=p->expires)child_exit(p);
 r->called=1;atomic_store_explicit(&s->publication,2,memory_order_release);
 r->status=slcli_kernel_call(__NR_execveat,in->executable,(long)"",(long)in->argv,(long)in->environment,AT_EMPTY_PATH,0);
 r->returned=1;child_exit(p);
}
static int text(struct slcli_arena *a,const char *p,const void *c,size_t bytes,const char *endpoint,const char *count,const struct slcli_bq_bootstrap_storage_linux_plan *plan){
 size_t n=0;
 if(!p)return 0;
 do{if(n==SIZE_MAX||!slcli_arena_live_range(a,p,n+1)||!apart(p,n+1,c,bytes)||
   !apart(p,n+1,endpoint,SLCLI_BQ_PROVIDER_ENDPOINT_TEXT_BYTES)||
   !apart(p,n+1,count,SLCLI_BQ_PROVIDER_COUNT_TEXT_BYTES))return 0;}while(p[n++]);
 return slcli_bq_bootstrap_storage_linux_away_original(plan,p,n);
}
int slcli_bq_provider_linux_bootstrap_borrowed_away_original(const struct slcli_bq_provider_linux_birth_original *c){
 const struct slcli_bq_provider_linux_birth_inputs *in;const struct slcli_bq_bootstrap_storage_linux_plan *p;
 const void *v[17];size_t n[17],i;
 if(!c||!slcli_bq_provider_linux_release_matches_original(c)||!(in=c->inputs)||!(p=in->bootstrap_storage)||
  !in->allowlist_count||in->allowlist_count>SIZE_MAX/sizeof(*in->allowlist)||
  in->allowlist_count>SIZE_MAX/sizeof(*in->fds)||in->allowlist_count>=SIZE_MAX/sizeof(*in->gaps)||
  in->argc<3||in->argc>=SIZE_MAX/sizeof(char *)||in->environment_count>=SIZE_MAX/sizeof(char *)||
  !slcli_bq_bootstrap_storage_linux_away_original(p,c,sizeof(*c)))return 0;
 v[0]=in;n[0]=sizeof(*in);v[1]=in->channel;n[1]=sizeof(*in->channel);
 v[2]=in->allowlist;n[2]=in->allowlist_count*sizeof(*in->allowlist);
 v[3]=in->argv;n[3]=(in->argc+1)*sizeof(char *);v[4]=in->environment;n[4]=(in->environment_count+1)*sizeof(char *);
 v[5]=in->shared_arena;n[5]=sizeof(*in->shared_arena);v[6]=in->shared_original;n[6]=sizeof(*in->shared_original);
 v[7]=in->endpoint_text;n[7]=11;v[8]=in->count_text;n[8]=21;v[9]=in->child;n[9]=sizeof(*in->child);
 v[10]=in->fds;n[10]=in->allowlist_count*sizeof(*in->fds);v[11]=in->gaps;n[11]=(in->allowlist_count+1)*sizeof(*in->gaps);
 v[12]=c->prior;n[12]=sizeof(*c->prior);v[13]=c->origin;n[13]=sizeof(*c->origin);
 v[14]=c->deadline;n[14]=sizeof(*c->deadline);v[15]=c->caller;n[15]=c->caller_bytes;
 v[16]=in->channel->prior;n[16]=sizeof(*in->channel->prior);
 for(i=0;i<17;i++)if(!slcli_bq_bootstrap_storage_linux_away_original(p,v[i],n[i]))return 0;
 if(c->release_prior!=&p->process_offer->finish||!slcli_bq_process_reference_linux_offer_matches_original(p->process_offer)||
  !apart(p->composer,sizeof(*p->composer),c->release_prior,sizeof(*c->release_prior)))return 0;
 for(i=0;i<in->argc;i++)if(i!=1&&i!=2&&!text(c->arena,in->argv[i],c,sizeof(*c),in->endpoint_text,in->count_text,p))return 0;
 for(i=0;i<in->environment_count;i++)if(!text(c->arena,in->environment[i],c,sizeof(*c),in->endpoint_text,in->count_text,p))return 0;
 return 1;
}
int slcli_bq_provider_linux_birth_preowned_original(
 struct slcli_bq_provider_linux_birth_original *c,struct slcli_arena *a,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const struct slcli_bq_provider_linux_birth_inputs *in){
 const void *mutable[6],*borrowed[10];size_t ms[6],bs[10],i,j,offer_count;int image=0,endpoint=0;
 long result;
 if(!a||!c||!in||!slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !slcli_arena_live_block(a,in,sizeof(*in))||!slcli_arena_live_block(a,caller,caller_bytes)||
  !slcli_source_deadline_original_matches(a,d,caller)||!prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||
  !slcli_native_clock_linux_matches_original(prior)||!slcli_native_clock_linux_matches_original(d->origin)||
  prior->tick<d->origin->tick||prior->tick>=d->expires||!shared(in,a)||
  !slcli_bq_channel_linux_matches_original(in->channel)||in->channel->arena!=a||
  in->channel->deadline!=d||in->channel->caller!=caller||in->channel->caller_bytes!=caller_bytes||
  prior->tick<in->channel->validation_after.tick||prior->tick<in->shared_original->tick||
  in->executable<0||in->directory<0||!in->allowlist_count||
  in->allowlist_count>SIZE_MAX/sizeof(*in->allowlist)||in->allowlist_count>SIZE_MAX/sizeof(*in->fds)||
  in->allowlist_count>=SIZE_MAX/sizeof(*in->gaps)||in->argc<3||in->argc>=SIZE_MAX/sizeof(char *)||
  in->environment_count>=SIZE_MAX/sizeof(char *)||
  !slcli_arena_live_block(a,in->roster,sizeof(*in->roster))||
  in->roster->command_count>SIZE_MAX/sizeof(*in->roster->requirements_per_command)||
  in->roster->subject_count>SIZE_MAX/sizeof(*in->roster->subjects))return 0;
  if(!slcli_bq_bootstrap_storage_linux_fresh_original(in->bootstrap_storage)||
  in->bootstrap_storage->arena_original!=a||in->bootstrap_storage->caller_original!=caller||
  in->bootstrap_storage->caller_bytes!=caller_bytes||in->bootstrap_storage->deadline_original!=d||
  in->bootstrap_storage->origin_original!=d->origin||in->bootstrap_storage->roster_original!=in->roster)return 0;
 mutable[0]=c;ms[0]=sizeof(*c);mutable[1]=in->endpoint_text;ms[1]=11;
 mutable[2]=in->count_text;ms[2]=21;
 mutable[3]=in->child;ms[3]=sizeof(*in->child);mutable[4]=in->fds;ms[4]=in->allowlist_count*sizeof(*in->fds);
 mutable[5]=in->gaps;ms[5]=(in->allowlist_count+1)*sizeof(*in->gaps);
 borrowed[0]=in;bs[0]=sizeof(*in);borrowed[1]=in->channel;bs[1]=sizeof(*in->channel);
 borrowed[2]=in->allowlist;bs[2]=in->allowlist_count*sizeof(*in->allowlist);
 borrowed[3]=in->argv;bs[3]=(in->argc+1)*sizeof(char *);
 borrowed[4]=in->environment;bs[4]=(in->environment_count+1)*sizeof(char *);
 borrowed[5]=in->shared_arena;bs[5]=sizeof(*in->shared_arena);borrowed[6]=in->shared_original;bs[6]=sizeof(*in->shared_original);
 borrowed[7]=in->roster;bs[7]=sizeof(*in->roster);
 borrowed[8]=in->roster->requirements_per_command;bs[8]=in->roster->command_count*sizeof(*in->roster->requirements_per_command);
 borrowed[9]=in->roster->subjects;bs[9]=in->roster->subject_count*sizeof(*in->roster->subjects);
  for(i=0;i<10;++i)if(bs[i]&&(!slcli_arena_live_block(a,borrowed[i],bs[i])||
  !slcli_bq_bootstrap_storage_linux_away_original(in->bootstrap_storage,borrowed[i],bs[i])))return 0;
 for(i=0;i<6;++i)if(!slcli_bq_bootstrap_storage_linux_away_original(in->bootstrap_storage,mutable[i],ms[i]))return 0;
 if(!slcli_bq_bootstrap_storage_linux_away_original(in->bootstrap_storage,prior,sizeof(*prior)))return 0;
 if(!slcli_bq_provider_roster_count(&offer_count,in->roster))return 0;
 for(i=0;i<6;++i){
  if(!slcli_arena_live_block(i<3?a:in->shared_arena,mutable[i],ms[i])||!zero(mutable[i],ms[i])||
   !apart(mutable[i],ms[i],caller,caller_bytes)||!apart(mutable[i],ms[i],prior,sizeof(*prior))||
   !slcli_source_deadline_away(d,mutable[i],ms[i]))return 0;
  for(j=0;j<10;++j)if(bs[j]&&!apart(mutable[i],ms[i],borrowed[j],bs[j]))return 0;
  for(j=0;j<i;++j)if(!apart(mutable[i],ms[i],mutable[j],ms[j]))return 0;
 }
 if(in->argv[in->argc]||in->environment[in->environment_count]||in->argv[1]!=in->endpoint_text||
  in->argv[2]!=in->count_text)return 0;
 for(i=0;i<in->argc;++i)if(i!=1&&i!=2&&!text(a,in->argv[i],c,sizeof(*c),in->endpoint_text,in->count_text,in->bootstrap_storage))return 0;
 for(i=0;i<in->environment_count;++i)if(!text(a,in->environment[i],c,sizeof(*c),in->endpoint_text,in->count_text,in->bootstrap_storage))return 0;
 for(i=0;i<in->allowlist_count;++i){
  const struct slcli_bq_provider_inherited_fd *r=&in->allowlist[i];
  if(r->fd<0||(i&&r->fd<=in->allowlist[i-1].fd)||
   (r->action!=SLCLI_BQ_PROVIDER_KEEP&&r->action!=SLCLI_BQ_PROVIDER_EXEC_ONLY)||
   r->fd==in->channel->endpoints[0])return 0;
  if(r->fd==in->executable){if(r->action!=SLCLI_BQ_PROVIDER_EXEC_ONLY)return 0;image=1;}
  if(r->fd==in->channel->endpoints[1]){if(r->action!=SLCLI_BQ_PROVIDER_KEEP)return 0;endpoint=1;}
 }
 if(!image||!endpoint)return 0;
 c->started=1;c->failed=1;c->arena=a;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;c->inputs=in;c->original=*in;
 c->roster_original=*in->roster;
 c->bootstrap_storage_original=*in->bootstrap_storage;
 c->prior=prior;c->prior_tick=prior->tick;c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;c->pidfd=-1;
 if(!slcli_bq_provider_endpoint_encode(in->endpoint_text,11,&c->endpoint_bytes,in->channel->endpoints[1]))return 0;
 c->offer_count=offer_count;
 if(!slcli_bq_provider_count_encode(in->count_text,21,&c->count_bytes,(uint64_t)offer_count))return 0;
 in->child->fds=in->fds;in->child->gaps=in->gaps;in->child->fd_count=in->allowlist_count;
 in->child->gap_count=in->allowlist_count+1;
 atomic_init(&in->child->birth_publication,0);atomic_init(&in->child->publication,0);atomic_init(&in->child->gate,0);
 for(i=0;i<in->allowlist_count;++i)in->fds[i].original=in->allowlist[i];
 c->clone.flags=CLONE_PIDFD;c->clone.pidfd=(uintptr_t)&c->pidfd;c->clone.exit_signal=SIGCHLD;
 c->clone_args[0]=(long)&c->clone;c->clone_args[1]=sizeof(c->clone);
 c->before_result=slcli_native_clock_read(&c->before);c->before_returned=1;
 if(!same(c)||!c->before_result||c->before.tick<c->prior_tick||c->before.tick>=c->expires)return 0;
 c->possible_child_effects=1;c->clone_called=1;
 result=slcli_kernel_call(__NR_clone3,c->clone_args[0],c->clone_args[1],0,0,0,0);
 if(!result){in->child->clone_status=result;in->child->clone_returned=1;child_run(c);}
 c->clone_status=result;c->clone_returned=1;c->born=result>0;
 c->after_result=slcli_native_clock_read(&c->after);c->after_returned=1;
 if(!same(c)||!c->after_result||c->after.tick<c->before.tick||c->after.tick>=c->expires||
  !c->born||c->clone_status>INT_MAX||c->pidfd<0)return 0;
 c->failed=0;return 1;
}
int slcli_bq_provider_linux_birth_matches_original(const struct slcli_bq_provider_linux_birth_original *c){
 size_t i,count;uint64_t encoded_count;int endpoint;
 if(!same(c)||c->failed||!c->possible_child_effects||!c->clone_called||!c->clone_returned||!c->born||
  c->clone_status<=0||c->clone_status>INT_MAX||c->pidfd<0||!c->before_returned||!c->after_returned||
  !c->before_result||!c->after_result||!slcli_native_clock_linux_matches_original(&c->before)||
  !slcli_native_clock_linux_matches_original(&c->after)||
  c->before.tick<c->prior_tick||c->after.tick<c->before.tick||c->after.tick>=c->expires||
  c->clone.flags!=CLONE_PIDFD||c->clone.pidfd!=(uintptr_t)&c->pidfd||c->clone.exit_signal!=SIGCHLD||
  c->clone.child_tid||c->clone.parent_tid||c->clone.stack||c->clone.stack_size||c->clone.tls||
  c->clone.set_tid||c->clone.set_tid_size||c->clone.cgroup||
  c->clone_args[0]!=(long)&c->clone||c->clone_args[1]!=sizeof(c->clone)||
  !shared(&c->original,c->arena)||!c->original.allowlist_count||
  c->original.allowlist_count>SIZE_MAX/sizeof(*c->original.allowlist)||
  c->original.allowlist_count>SIZE_MAX/sizeof(*c->original.fds)||
  c->original.allowlist_count>=SIZE_MAX/sizeof(*c->original.gaps)||
  !slcli_arena_live_block(c->arena,c->original.channel,sizeof(*c->original.channel))||
  !slcli_arena_live_block(c->arena,c->original.allowlist,c->original.allowlist_count*sizeof(*c->original.allowlist))||
  !slcli_arena_live_block(c->arena,c->original.endpoint_text,SLCLI_BQ_PROVIDER_ENDPOINT_TEXT_BYTES)||
  !slcli_arena_live_block(c->arena,c->original.count_text,SLCLI_BQ_PROVIDER_COUNT_TEXT_BYTES)||
  !slcli_arena_live_block(c->arena,c->original.roster,sizeof(*c->original.roster))||
  c->original.roster->command_count>SIZE_MAX/sizeof(*c->original.roster->requirements_per_command)||
  c->original.roster->subject_count>SIZE_MAX/sizeof(*c->original.roster->subjects)||
  (c->original.roster->command_count&&!slcli_arena_live_block(c->arena,
   c->original.roster->requirements_per_command,c->original.roster->command_count*sizeof(*c->original.roster->requirements_per_command)))||
  (c->original.roster->subject_count&&!slcli_arena_live_block(c->arena,
   c->original.roster->subjects,c->original.roster->subject_count*sizeof(*c->original.roster->subjects)))||
  !slcli_bq_provider_roster_count(&count,c->original.roster)||count!=c->offer_count||
  !slcli_bq_provider_count_decode(&encoded_count,c->original.count_text,c->count_bytes)||encoded_count!=count||
  c->original.argc<3||c->original.argc>=SIZE_MAX/sizeof(char *)||
  !slcli_arena_live_block(c->arena,c->original.argv,(c->original.argc+1)*sizeof(char *))||
  c->original.argv[1]!=c->original.endpoint_text||c->original.argv[2]!=c->original.count_text||
  c->original.argv[c->original.argc]||
  !slcli_arena_live_block(c->original.shared_arena,c->original.child,sizeof(*c->original.child))||
  !slcli_arena_live_block(c->original.shared_arena,c->original.fds,c->original.allowlist_count*sizeof(*c->original.fds))||
  !slcli_arena_live_block(c->original.shared_arena,c->original.gaps,(c->original.allowlist_count+1)*sizeof(*c->original.gaps))||
  !slcli_bq_channel_linux_matches_original(c->original.channel)||
  c->original.channel->arena!=c->arena||c->original.channel->deadline!=c->deadline||
  c->original.channel->caller!=c->caller||c->original.channel->caller_bytes!=c->caller_bytes||
  c->prior_tick<c->original.channel->validation_after.tick||c->prior_tick<c->original.shared_original->tick||
  !slcli_bq_provider_endpoint_decode(&endpoint,c->original.endpoint_text,c->endpoint_bytes)||
  endpoint!=c->original.channel->endpoints[1])return 0;
 for(i=2;i<6;++i)if(c->clone_args[i])return 0;
 for(i=0;i<c->original.allowlist_count;++i)if(c->original.fds[i].original.fd!=c->original.allowlist[i].fd||
  c->original.fds[i].original.action!=c->original.allowlist[i].action)return 0;
 return 1;
}
int slcli_bq_provider_linux_child_birth_matches_original(const struct slcli_bq_provider_linux_birth_original *c){
 const long numbers[6]={__NR_getpid,__NR_gettid,__NR_getuid,__NR_geteuid,__NR_getgid,__NR_getegid};
 const struct slcli_bq_provider_linux_child *s;size_t i,j;uint64_t previous;
 if(!slcli_bq_provider_linux_birth_matches_original(c))return 0;
 s=c->original.child;
 if(atomic_load_explicit(&s->birth_publication,memory_order_acquire)!=1||
  !s->clone_returned||s->clone_status)return 0;
 previous=c->before.tick;
 for(i=0;i<6;++i){
  const struct slcli_bq_provider_native_row *r=&s->birth[i];
  if(!r->started||!r->called||!r->returned||r->number!=numbers[i]||
   !r->before_returned||!r->after_returned||!r->before_result||!r->after_result||
   !slcli_native_clock_linux_matches_original(&r->before)||!slcli_native_clock_linux_matches_original(&r->after)||r->before.tick<previous||
   r->after.tick<r->before.tick||r->after.tick>=c->expires||r->status<0||
   (i<2&&(r->status<=0||r->status>INT_MAX))||(i>=2&&(uint64_t)r->status>UINT32_MAX))return 0;
  for(j=0;j<6;++j)if(r->args[j])return 0;
  previous=r->after.tick;
 }
 return s->birth[0].status==c->clone_status&&s->birth[1].status==c->clone_status;
}
static int first_offer_original(const struct slcli_bq_provider_linux_birth_original *c){
 struct slcli_bq_subject_route expected;
 return slcli_bq_provider_linux_child_birth_matches_original(c)&&c->first_offer_started&&
  c->first_offer_clock_returned&&c->first_offer_clock_result&&slcli_native_clock_linux_matches_original(&c->first_offer_prior)&&
  c->first_offer_prior.tick>=c->after.tick&&c->first_offer_prior.tick>=c->original.child->birth[5].after.tick&&
  c->first_offer_prior.tick<c->expires&&c->first_offer_returned&&c->first_offer_result&&
  slcli_arena_live_block(c->arena,c->first_offer,sizeof(*c->first_offer))&&
  slcli_arena_live_block(c->arena,c->first_route,sizeof(*c->first_route))&&
  slcli_bq_provider_roster_offer(&expected,c->original.roster,0)&&
  slcli_bq_subject_route_equal(&expected,c->first_route)&&
  c->first_offer->arena==c->arena&&c->first_offer->deadline==c->deadline&&
  c->first_offer->caller==c->caller&&c->first_offer->caller_bytes==c->caller_bytes&&
  c->first_offer->channel==c->original.channel&&c->first_offer->route_original==c->first_route&&
  c->first_offer->prior_original==&c->first_offer_prior&&
  slcli_bq_process_reference_linux_offer_matches_original(c->first_offer);
}
int slcli_bq_provider_linux_release_original(struct slcli_bq_provider_linux_birth_original *c,
 const struct slcli_native_clock *prior){
 struct slcli_bq_provider_linux_child *s;
 if(!first_offer_original(c)||c->release_started||
  !prior||!slcli_arena_live_range(c->arena,prior,sizeof(*prior))||!slcli_native_clock_linux_matches_original(prior)||
  prior->tick<c->first_offer->finish.tick||prior->tick>=c->expires)return 0;
 c->release_started=1;c->release_prior=prior;c->release_prior_tick=prior->tick;s=c->original.child;
 c->release_before_result=slcli_native_clock_read(&c->release_before);c->release_before_returned=1;
 if(!same(c)||!c->release_before_result||c->release_before.tick<c->release_prior_tick||c->release_before.tick>=c->expires)return 0;
 c->wake_args[0]=(long)&s->gate;c->wake_args[1]=FUTEX_WAKE;c->wake_args[2]=1;
 /* Gate publication itself is a possible release even if the wake fails. */
 c->gate_possible=1;atomic_store_explicit(&s->gate,1,memory_order_release);
 c->wake_called=1;c->wake_status=slcli_kernel_call(__NR_futex,c->wake_args[0],FUTEX_WAKE,1,0,0,0);c->wake_returned=1;
 /* Rejoin the potentially large original roster before the final clock. A
  * failed check leaves gate_possible and the natural child outcome retained. */
 if(!first_offer_original(c)||c->wake_status<0||c->wake_status>1||
  !slcli_native_clock_linux_matches_original(prior)||prior->tick!=c->release_prior_tick)return 0;
 c->release_after_result=slcli_native_clock_read(&c->release_after);c->release_after_returned=1;
 if(!same(c)||!c->release_after_result||c->release_after.tick<c->release_before.tick||
  c->release_after.tick>=c->expires||c->wake_status<0||!slcli_native_clock_linux_matches_original(prior)||prior->tick!=c->release_prior_tick)return 0;
 c->released=1;return 1;
}
int slcli_bq_provider_linux_release_matches_original(const struct slcli_bq_provider_linux_birth_original *c){
 size_t i;
 if(!first_offer_original(c)||!c->release_started||!c->released||
  !c->gate_possible||!c->wake_called||!c->wake_returned||c->wake_status<0||c->wake_status>1||
  !c->release_prior||!slcli_arena_live_range(c->arena,c->release_prior,sizeof(*c->release_prior))||
  !slcli_native_clock_linux_matches_original(c->release_prior)||c->release_prior->tick!=c->release_prior_tick||
  c->release_prior_tick<c->first_offer->finish.tick||
  c->release_prior_tick>=c->expires||
  !c->release_before_returned||!c->release_after_returned||
  !c->release_before_result||!c->release_after_result||
  !slcli_native_clock_linux_matches_original(&c->release_before)||!slcli_native_clock_linux_matches_original(&c->release_after)||
  c->release_before.tick<c->release_prior_tick||c->release_after.tick<c->release_before.tick||
  c->release_after.tick>=c->expires||
  c->wake_args[0]!=(long)&c->original.child->gate||c->wake_args[1]!=FUTEX_WAKE||c->wake_args[2]!=1||
  atomic_load_explicit(&c->original.child->gate,memory_order_acquire)!=1)return 0;
 for(i=3;i<6;++i)if(c->wake_args[i])return 0;
 return 1;
}
int slcli_bq_provider_linux_offer_then_release_original(
 struct slcli_bq_provider_linux_birth_original *c,struct slcli_bq_process_reference_linux_offer *offer,
 struct slcli_bq_process_reference_linux_original *process,struct slcli_bq_send_linux_original *send,
 unsigned char *payload,struct slcli_bq_subject_route *route){
 const struct slcli_bq_provider_linux_birth_inputs *in;const void *targets[5],*borrowed[13];
 size_t ts[5],bs[13],i,j;
 if(!slcli_bq_provider_linux_child_birth_matches_original(c)||c->release_started||c->first_offer_started)return 0;
 in=&c->original;
 if(!slcli_bq_bootstrap_storage_linux_fresh_original(in->bootstrap_storage)||
  offer!=in->bootstrap_storage->process_offer||process!=in->bootstrap_storage->process||
  send!=in->bootstrap_storage->process_send||payload!=in->bootstrap_storage->process_payload||
  route!=in->bootstrap_storage->process_route)return 0;
 if(in->environment_count>=SIZE_MAX/sizeof(char *))return 0;
 targets[0]=offer;ts[0]=sizeof(*offer);targets[1]=process;ts[1]=sizeof(*process);
 targets[2]=send;ts[2]=sizeof(*send);targets[3]=payload;ts[3]=64;targets[4]=route;ts[4]=sizeof(*route);
 borrowed[0]=c;bs[0]=sizeof(*c);borrowed[1]=c->inputs;bs[1]=sizeof(*c->inputs);
 borrowed[2]=in->channel;bs[2]=sizeof(*in->channel);borrowed[3]=in->allowlist;bs[3]=in->allowlist_count*sizeof(*in->allowlist);
 borrowed[4]=in->argv;bs[4]=(in->argc+1)*sizeof(char *);
 borrowed[5]=in->environment;bs[5]=(in->environment_count+1)*sizeof(char *);
 borrowed[6]=in->endpoint_text;bs[6]=11;borrowed[7]=in->count_text;bs[7]=21;
 borrowed[8]=in->roster;bs[8]=sizeof(*in->roster);
 borrowed[9]=in->roster->requirements_per_command;bs[9]=in->roster->command_count*sizeof(*in->roster->requirements_per_command);
 borrowed[10]=in->roster->subjects;bs[10]=in->roster->subject_count*sizeof(*in->roster->subjects);
 borrowed[11]=in->shared_arena;bs[11]=sizeof(*in->shared_arena);
 borrowed[12]=in->shared_original;bs[12]=sizeof(*in->shared_original);
 for(i=0;i<13;++i)if(bs[i]&&!slcli_arena_live_block(c->arena,borrowed[i],bs[i]))return 0;
 for(i=0;i<5;++i){
  if(!slcli_arena_live_block(c->arena,targets[i],ts[i])||!zero(targets[i],ts[i])||
   !apart(targets[i],ts[i],c->caller,c->caller_bytes)||
   !apart(targets[i],ts[i],c->prior,sizeof(*c->prior))||!slcli_source_deadline_away(c->deadline,targets[i],ts[i])||
   !apart(targets[i],ts[i],in->child,sizeof(*in->child))||
   !apart(targets[i],ts[i],in->fds,in->allowlist_count*sizeof(*in->fds))||
   !apart(targets[i],ts[i],in->gaps,(in->allowlist_count+1)*sizeof(*in->gaps)))return 0;
  for(j=0;j<i;++j)if(!apart(targets[i],ts[i],targets[j],ts[j]))return 0;
  for(j=0;j<13;++j)if(bs[j]&&!apart(targets[i],ts[i],borrowed[j],bs[j]))return 0;
  for(j=0;j<in->argc;++j)if(j!=1&&j!=2&&!text(c->arena,in->argv[j],targets[i],ts[i],in->endpoint_text,in->count_text,in->bootstrap_storage))return 0;
  for(j=0;j<in->environment_count;++j)if(!text(c->arena,in->environment[j],targets[i],ts[i],in->endpoint_text,in->count_text,in->bootstrap_storage))return 0;
 }
 c->first_offer_started=1;c->first_offer=offer;c->first_route=route;
 c->first_offer_clock_result=slcli_native_clock_read(&c->first_offer_prior);c->first_offer_clock_returned=1;
 if(!same(c)||!c->first_offer_clock_result||!slcli_native_clock_linux_matches_original(&c->first_offer_prior)||
  c->first_offer_prior.tick<c->after.tick||c->first_offer_prior.tick<in->child->birth[5].after.tick||
  c->first_offer_prior.tick>=c->expires||!slcli_bq_provider_roster_offer(route,in->roster,0))return 0;
 c->first_offer_result=slcli_bq_process_reference_linux_offer_original(offer,c->arena,c->deadline,
  c->caller,c->caller_bytes,in->channel,process,send,payload,route,&c->first_offer_prior);
 c->first_offer_returned=1;
 if(!c->first_offer_result||!slcli_bq_process_reference_linux_offer_matches_original(offer))return 0;
 c->first_release_result=slcli_bq_provider_linux_release_original(c,&offer->finish);c->first_release_returned=1;
 return c->first_release_result;
}
#endif
