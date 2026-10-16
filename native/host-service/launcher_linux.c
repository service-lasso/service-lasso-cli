#include "launcher_linux.h"
#if defined(__linux__)
#include "native_syscall_linux.h"
#include <linux/futex.h>
#include <linux/fcntl.h>
#include <asm/signal.h>
#include <asm-generic/errno.h>
static int apart(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int target_lineage(const struct slcli_launcher_linux_capture *c) {
  const struct slcli_memory_owner *o=c->target_owner_original;size_t i;
  const struct slcli_preentry_backing_linux *b=c->runtime_backing_original;
  if(!o||(!slcli_memory_owner_prebirth_shape(o)&&!slcli_memory_owner_frozen(o))||
     o->charged!=c->target_charged_original||o->count!=c->target_count_original||
     o->next_sequence!=c->target_next_sequence_original)return 0;
  for(i=0;i<32;i++)if(o->original_birth[i]!=c->target_associations_original[0][i]||
    o->original_image[i]!=c->target_associations_original[1][i]||
    o->original_owner[i]!=c->target_associations_original[2][i]||
    o->original_source[i]!=c->target_associations_original[3][i])return 0;
  if(!b||b->physical_owner!=o||!b->section_only||b->mapped||!b->qualified||
     b->operation!=1||!b->backing_returned||!b->backing_result||b->descriptor<0||
     b->request.index>=o->count||!b->request.sequence||
     o->extents[b->request.index].state!=SLCLI_EXTENT_BACKING_ACQUIRED||
     o->extents[b->request.index].acquisition!=b->request.sequence||
     o->extents[b->request.index].reserved!=b->bytes)return 0;
  return 1;
}
static int same(const struct slcli_launcher_linux_capture *c,struct slcli_arena *a) {
  return target_lineage(c)&&slcli_source_deadline_original_matches(a,c->deadline,c->caller)&&
    c->deadline->origin==c->origin&&c->origin->tick==c->origin_tick&&
    c->deadline->expires==c->expires&&c->predecessor->complete&&
    c->predecessor->tick==c->predecessor_tick;
}
/* Each text is bounded by the existing logical arena block, not a new command
 * byte allowance. Copy complete originals before native birth. */
static char *text_copy(struct slcli_arena *a,const char *s,const void *owner,size_t owner_bytes) {
  size_t n=0,i;char *r;
  if(!s)return 0;
  while(n<SIZE_MAX&&slcli_arena_live_range(a,s,n+1)&&apart(s,n+1,owner,owner_bytes)) {if(!s[n++])break;}
  if(!n||s[n-1])return 0;
  r=(char *)slcli_arena_alloc(a,n);if(!r)return 0;
  for(i=0;i<n;i++)r[i]=s[i];return r;
}
static char **vector_copy(struct slcli_arena *a,const char *const *v,size_t n,const void *owner,size_t owner_bytes) {
  char **r;size_t i;
  if(n>=SIZE_MAX/sizeof(*r)||!slcli_arena_live_range(a,v,(n+1)*sizeof(*r))||!apart(v,(n+1)*sizeof(*r),owner,owner_bytes)||v[n])return 0;
  r=(char **)slcli_arena_calloc(a,n+1,sizeof(*r));if(!r)return 0;
  for(i=0;i<n;i++)if(!(r[i]=text_copy(a,v[i],owner,owner_bytes)))return 0;
  return r;
}
static void child_exit(struct slcli_launcher_linux_child *c) {
  c->exit_called=1;
  atomic_store_explicit(&c->publication,2,memory_order_release);
  c->exit_result=(uint64_t)slcli_kernel_call(__NR_exit_group,127,0,0,0,0,0);
  c->exit_returned=1;
  atomic_store_explicit(&c->publication,3,memory_order_release);
  __builtin_trap();
}
static void child_original(struct slcli_launcher_linux_capture *p) {
  struct slcli_launcher_linux_child *c=p->child;long result;size_t i;
  const struct slcli_native_clock *previous;
  c->started=1;
  c->process_result=(uint64_t)slcli_kernel_call(__NR_getpid,0,0,0,0,0,0);c->process_returned=1;
  c->thread_result=(uint64_t)slcli_kernel_call(__NR_gettid,0,0,0,0,0,0);c->thread_returned=1;
  if(!slcli_native_clock_read(&c->birth)||c->birth.tick<p->clone_before.tick||
     c->birth.tick>=p->expires||(int64_t)c->process_result<=0||
     (int64_t)c->thread_result<=0)child_exit(c);
  atomic_store_explicit(&c->birth_publication,1,memory_order_release);
  if(!slcli_native_clock_read(&c->before_wait)||c->before_wait.tick<c->birth.tick||
     c->before_wait.tick>=p->expires)child_exit(c);
  c->absolute_timeout.tv_sec=(long long)(p->expires/UINT64_C(1000000000));
  c->absolute_timeout.tv_nsec=(long long)(p->expires%UINT64_C(1000000000));
  if(!atomic_load_explicit(&c->gate,memory_order_acquire)) {
    c->wait_args[0]=(uintptr_t)&c->gate;c->wait_args[1]=FUTEX_WAIT_BITSET;
    c->wait_args[2]=0;c->wait_args[3]=(uintptr_t)&c->absolute_timeout;
    c->wait_args[4]=0;c->wait_args[5]=FUTEX_BITSET_MATCH_ANY;c->wait_called=1;
    result=slcli_kernel_call(__NR_futex,(long)&c->gate,FUTEX_WAIT_BITSET,0,
      (long)&c->absolute_timeout,0,FUTEX_BITSET_MATCH_ANY);
    c->wait_result=(uint64_t)result;c->wait_returned=1;
    /* EAGAIN is only the actual kernel observation that the value changed
     * before sleeping. The SAME gate and postclock still decide eligibility. */
    if(result&&result!=-EAGAIN)child_exit(c);
  }
  if(!slcli_native_clock_read(&c->after_wait)||c->after_wait.tick<c->before_wait.tick||
     c->after_wait.tick>=p->expires||atomic_load_explicit(&c->gate,memory_order_acquire)!=1)
    child_exit(c);
  if(!slcli_native_clock_read(&c->before_exec)||c->before_exec.tick<c->after_wait.tick||
     c->before_exec.tick>=p->expires)child_exit(c);
  c->directory_called=1;
  result=slcli_kernel_call(__NR_fchdir,p->original.directory,0,0,0,0,0);
  c->directory_result=(uint64_t)result;c->directory_returned=1;
  if(!slcli_native_clock_read(&c->after_directory)||c->after_directory.tick<c->before_exec.tick||
     c->after_directory.tick>=p->expires||result)child_exit(c);
  previous=&c->after_directory;
  for(i=0;i<c->descriptor_count;i++) {
    struct slcli_launcher_linux_fd_capture *row=&c->descriptors[i];
    if(!slcli_native_clock_read(&row->before)||row->before.tick<previous->tick||
       row->before.tick>=p->expires)child_exit(c);
    if(row->original.action==SLCLI_LAUNCH_CLOSE) {
      row->close_result=(uint64_t)slcli_kernel_call(__NR_close,row->original.fd,0,0,0,0,0);
      row->close_returned=1;
    } else {
      row->flags_result=(uint64_t)slcli_kernel_call(__NR_fcntl,row->original.fd,F_GETFD,0,0,0,0);
      row->flags_returned=1;
      if(!slcli_native_clock_read(&row->after_flags)||row->after_flags.tick<row->before.tick||
         row->after_flags.tick>=p->expires||(int64_t)row->flags_result<0)child_exit(c);
      row->set_result=(uint64_t)slcli_kernel_call(__NR_fcntl,row->original.fd,F_SETFD,
        row->original.action==SLCLI_LAUNCH_KEEP ? row->flags_result&~FD_CLOEXEC : row->flags_result|FD_CLOEXEC,0,0,0);
      row->set_returned=1;
    }
    if(!slcli_native_clock_read(&row->after_action)||
       row->after_action.tick<(row->flags_returned?row->after_flags.tick:row->before.tick)||
       row->after_action.tick>=p->expires||
       (row->close_returned?(int64_t)row->close_result:(int64_t)row->set_result))child_exit(c);
    previous=&row->after_action;
  }
  if(!slcli_native_clock_read(&c->exec_clock)||c->exec_clock.tick<previous->tick||
     c->exec_clock.tick>=p->expires)child_exit(c);
  c->exec_args[0]=(uint64_t)p->original.executable;c->exec_args[1]=(uintptr_t)"";
  c->exec_args[2]=(uintptr_t)p->argv;c->exec_args[3]=(uintptr_t)p->environment;
  c->exec_args[4]=AT_EMPTY_PATH;c->exec_called=1;
  atomic_store_explicit(&c->publication,1,memory_order_release);
  result=slcli_kernel_call(__NR_execveat,p->original.executable,(long)"",
    (long)p->argv,(long)p->environment,AT_EMPTY_PATH,0);
  c->exec_result=(uint64_t)result;c->exec_returned=1;child_exit(c);
}
int slcli_launcher_linux_create(struct slcli_launcher_linux_capture *c,struct slcli_arena *a,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  const struct slcli_native_clock *prior,const struct slcli_launcher_linux_inputs *in,const struct slcli_memory_owner *target,
  const struct slcli_preentry_backing_linux *runtime) {
  size_t i,descriptor_bytes,shared_bytes,child_charge,rows_charge;long result;
  if(!c||!a||!in||!slcli_arena_live_block(a,c,sizeof(*c))||
     !slcli_source_deadline_original_matches(a,d,caller)||
     !target||target==a->owner||!slcli_arena_live_block(a,target,sizeof(*target))||
     !slcli_memory_owner_prebirth_shape(target)||
     !runtime||!slcli_arena_live_block(a,runtime,sizeof(*runtime))||
     !apart(c,sizeof(*c),runtime,sizeof(*runtime))||
     runtime->deadline!=d||runtime->caller!=caller||runtime->origin!=d->origin||
     runtime->expires!=d->expires||
     !apart(c,sizeof(*c),target,sizeof(*target))||
     !slcli_arena_live_range(a,in,sizeof(*in))||!caller_bytes||
     !slcli_arena_live_block(a,caller,caller_bytes)||!slcli_source_deadline_original_matches(a,d,caller)||
     !prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
     prior->tick<d->origin->tick||prior->tick>=d->expires||
     !slcli_source_deadline_away(d,c,sizeof(*c))||!apart(c,sizeof(*c),caller,caller_bytes)||
     !apart(c,sizeof(*c),prior,sizeof(*prior))||!apart(c,sizeof(*c),in,sizeof(*in)))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  c->started=1;c->original=*in;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
  c->target_owner_original=target;c->runtime_backing_original=runtime;
  c->prefix_original=c->original.prefix;
  if(!slcli_node_prefix_original_matches(c->prefix_original,a,d,caller,target,&runtime->request)||
     prior->tick<c->prefix_original->after.tick)return 0;
  c->transport_original=c->original.transport;
  if(!c->transport_original||!slcli_arena_live_block(a,c->transport_original,sizeof(*c->transport_original))||
     !apart(c,sizeof(*c),c->transport_original,sizeof(*c->transport_original))||
     !c->transport_original->object_prepared||c->transport_original->publication_attempted||
     c->transport_original->terminal||c->transport_original->deadline!=d||
     c->transport_original->caller!=caller||c->transport_original->descriptor<0||
     c->transport_original->descriptor==runtime->descriptor||
     !c->original.bootstrap_bytes||!c->original.allocator_capacity||
     !c->original.thread_capture_attempts||c->original.thread_capture_attempts>SIZE_MAX||
     !c->original.birth_observation_capacity||
     c->original.birth_observation_capacity>SIZE_MAX/sizeof(*c->birth_observations)||
     prior->tick<c->transport_original->calls[1].after.tick)return 0;
  c->transport_descriptor_original=c->transport_original->descriptor;
  c->target_charged_original=target->charged;
  c->target_next_sequence_original=target->next_sequence;c->target_count_original=target->count;
  for(i=0;i<32;i++) {
    c->target_associations_original[0][i]=target->original_birth[i];
    c->target_associations_original[1][i]=target->original_image[i];
    c->target_associations_original[2][i]=target->original_owner[i];
    c->target_associations_original[3][i]=target->original_source[i];
  }
  c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
  c->predecessor=prior;c->predecessor_tick=prior->tick;
  if(!slcli_native_clock_read(&c->transport_constructor[0])||!same(c,a)||
     c->transport_constructor[0].tick<prior->tick||c->transport_constructor[0].tick>=c->expires)return 0;
  c->transport_payload=(struct slcli_node_bootstrap_transport *)slcli_arena_calloc(a,1,sizeof(*c->transport_payload));
  if(!c->transport_payload)return 0;
  if(!slcli_native_clock_read(&c->transport_constructor[1])||!same(c,a)||
     c->transport_constructor[1].tick<c->transport_constructor[0].tick||c->transport_constructor[1].tick>=c->expires)return 0;
  c->transport_birth=(struct slcli_launcher_linux_birth_view *)slcli_arena_calloc(a,1,sizeof(*c->transport_birth));
  if(!c->transport_birth)return 0;
  if(!slcli_native_clock_read(&c->transport_constructor[2])||!same(c,a)||
     c->transport_constructor[2].tick<c->transport_constructor[1].tick||c->transport_constructor[2].tick>=c->expires)return 0;
  c->birth_observations=(struct slcli_launcher_birth_observation *)slcli_arena_calloc(
    a,c->original.birth_observation_capacity,sizeof(*c->birth_observations));
  if(!c->birth_observations)return 0;
  if(c->original.executable<0||c->original.directory<0||!c->original.argc||
     !c->original.descriptor_count||c->original.descriptor_count>SIZE_MAX/sizeof(*c->descriptors))return 0;
  descriptor_bytes=c->original.descriptor_count*sizeof(*c->descriptors);
  if(!slcli_arena_live_range(a,c->original.descriptors,descriptor_bytes)||
     !apart(c,sizeof(*c),c->original.descriptors,descriptor_bytes))return 0;
  c->descriptors=(struct slcli_launcher_linux_fd *)slcli_arena_alloc(a,descriptor_bytes);
  if(!c->descriptors)return 0;
  { size_t j;int image=0,directory=0,runtime_kept=0,transport_kept=0;
    for(i=0;i<c->original.descriptor_count;i++) {
      c->descriptors[i]=c->original.descriptors[i];
      if(c->descriptors[i].fd<0||c->descriptors[i].action<SLCLI_LAUNCH_KEEP||
         c->descriptors[i].action>SLCLI_LAUNCH_CLOSE)return 0;
      for(j=0;j<i;j++)if(c->descriptors[j].fd==c->descriptors[i].fd)return 0;
      if(c->descriptors[i].fd==runtime->descriptor) {
        if(c->descriptors[i].action!=SLCLI_LAUNCH_KEEP)return 0;runtime_kept=1;
      }
      if(c->descriptors[i].fd==c->transport_descriptor_original) {
        if(c->descriptors[i].action!=SLCLI_LAUNCH_KEEP)return 0;transport_kept=1;
      }
      if(c->descriptors[i].fd==c->original.executable) {
        if(c->descriptors[i].action!=SLCLI_LAUNCH_EXEC_ONLY)return 0;image=1;
      }
      if(c->descriptors[i].fd==c->original.directory) {
        if(c->descriptors[i].action!=SLCLI_LAUNCH_EXEC_ONLY)return 0;directory=1;
      }
    }
    if(!image||!directory||!runtime_kept||!transport_kept||prior->tick<runtime->calls[3].after.tick)return 0;
  }
  if(c->original.descriptor_count>(SIZE_MAX-sizeof(*c->child))/sizeof(*c->child->descriptors))return 0;
  child_charge=slcli_arena_allocation_bytes(sizeof(*c->child));
  rows_charge=slcli_arena_allocation_bytes(c->original.descriptor_count*sizeof(*c->child->descriptors));
  if(!child_charge||!rows_charge||child_charge>SIZE_MAX-rows_charge)return 0;
  shared_bytes=child_charge+rows_charge;
  c->argv=vector_copy(a,c->original.argv,c->original.argc,c,sizeof(*c));
  c->environment=vector_copy(a,c->original.environment,c->original.environment_count,c,sizeof(*c));
  if(!c->argv||!c->environment)return 0;
  if(!slcli_native_clock_read(&c->before)||c->before.tick<prior->tick||
     c->before.tick>=c->expires||!same(c,a))return 0;
  if(!slcli_platform_acquire_shared(a->owner,SLCLI_RUNTIME,shared_bytes,&c->shared_memory))return 0;
  if(c->shared_memory.tick<c->before.tick||c->shared_memory.tick>=c->expires||!same(c,a)||
     !slcli_arena_init(&c->shared_arena,a->owner,c->shared_memory.request.index))return 0;
  c->child=(struct slcli_launcher_linux_child *)slcli_arena_calloc(&c->shared_arena,1,sizeof(*c->child));
  if(!c->child)return 0;
  atomic_init(&c->child->birth_publication,0);
  atomic_init(&c->child->publication,0);
  atomic_init(&c->child->gate,0);
  c->child->descriptors=(struct slcli_launcher_linux_fd_capture *)slcli_arena_calloc(
    &c->shared_arena,c->original.descriptor_count,sizeof(*c->child->descriptors));
  if(!c->child->descriptors)return 0;
  c->child->descriptor_count=c->original.descriptor_count;
  for(i=0;i<c->original.descriptor_count;i++)c->child->descriptors[i].original=c->descriptors[i];
  c->pidfd=-1;c->clone_original.flags=CLONE_PIDFD;
  c->clone_original.pidfd=(uintptr_t)&c->pidfd;c->clone_original.exit_signal=SIGCHLD;
  if(!slcli_native_clock_read(&c->clone_before)||!same(c,a)||
     c->clone_before.tick<c->shared_memory.tick||c->clone_before.tick>=c->expires)return 0;
  if(!slcli_memory_owner_prebirth_shape(target)||target->charged!=c->target_charged_original||
     target->next_sequence!=c->target_next_sequence_original||target->count!=c->target_count_original||
     !slcli_node_prefix_original_matches(c->prefix_original,a,d,caller,target,&runtime->request))return 0;
  c->possible_thread_attempts=c->original.thread_capture_attempts;
  c->possible_event_capacity=c->original.allocator_capacity;
  c->child_effects_possible=1;c->clone_called=1;
  result=slcli_kernel_call(__NR_clone3,(long)&c->clone_original,sizeof(c->clone_original),0,0,0,0);
  if(!result)child_original(c);
  c->clone_result=(uint64_t)result;c->clone_returned=1;c->created=result>0;
  if(!slcli_native_clock_read(&c->after)||!same(c,a)||c->after.tick<c->clone_before.tick||
     c->after.tick>=c->expires||!c->created||c->pidfd<0)return 0;
  c->qualified=1;return 1;
}
int slcli_launcher_linux_birth_original(const struct slcli_launcher_linux_capture *c,
  struct slcli_arena *a,struct slcli_launcher_linux_birth_view *out) {
  size_t i;uint32_t publication;
  if(!c||!a||!out||!slcli_arena_live_block(a,c,sizeof(*c))||
     !c->qualified||!c->clone_returned||!c->created||c->pidfd<0||
     !same(c,a)||
     !c->child||!slcli_arena_live_block(&c->shared_arena,c->child,sizeof(*c->child))||
     !slcli_arena_live_block(a,out,sizeof(*out))||
     !apart(out,sizeof(*out),c,sizeof(*c))||
     !apart(out,sizeof(*out),c->child,sizeof(*c->child))||
     !slcli_source_deadline_away(c->deadline,out,sizeof(*out))||
     !apart(out,sizeof(*out),c->caller,c->caller_bytes))return 0;
  for(i=0;i<sizeof(*out);i++)if(((const unsigned char *)out)[i])return 0;
  publication=atomic_load_explicit(&c->child->birth_publication,memory_order_acquire);
  out->observed=1;out->publication=publication;
  if(publication!=1)return 0;
  out->birth=c->child->birth;out->process_result=c->child->process_result;
  out->thread_result=c->child->thread_result;
  out->process_returned=c->child->process_returned;
  out->thread_returned=c->child->thread_returned;
  if(!out->birth.complete||!out->process_returned||!out->thread_returned||
     out->process_result!=c->clone_result||out->thread_result!=out->process_result||
     out->birth.tick<c->clone_before.tick||out->birth.tick>=c->expires)return 0;
  out->qualified=1;return 1;
}
int slcli_launcher_linux_failure_original(const struct slcli_launcher_linux_capture *c,
  struct slcli_arena *a,struct slcli_launcher_linux_failure_view *out) {
  uint32_t phase;size_t i;
  if(!c||!a||!out||!slcli_arena_live_block(a,c,sizeof(*c))||!c->clone_returned||
     !c->created||!c->child||!slcli_arena_live_block(a,out,sizeof(*out))||
     !apart(out,sizeof(*out),c,sizeof(*c))||!slcli_source_deadline_away(c->deadline,out,sizeof(*out))||
     !apart(out,sizeof(*out),c->caller,c->caller_bytes))return 0;
  for(i=0;i<sizeof(*out);i++)if(((const unsigned char *)out)[i])return 0;
  phase=atomic_load_explicit(&c->child->publication,memory_order_acquire);
  if(phase<2||phase>3)return 0;
  out->phase=phase;out->exec_called=c->child->exec_called;out->exec_returned=c->child->exec_returned;
  out->directory_called=c->child->directory_called;out->directory_returned=c->child->directory_returned;
  out->exit_called=c->child->exit_called;out->process_result=c->child->process_result;
  out->thread_result=c->child->thread_result;out->wait_result=c->child->wait_result;
  out->directory_result=c->child->directory_result;out->exec_result=c->child->exec_result;
  if(phase==3){out->exit_returned=c->child->exit_returned;out->exit_result=c->child->exit_result;}
  return 1;
}
int slcli_launcher_linux_release(struct slcli_launcher_linux_capture *c,struct slcli_arena *a,
  const struct slcli_native_clock *prior,const struct slcli_memory_owner *target,
  const unsigned char *owner_export,size_t owner_export_bytes) {
  long result;size_t i;struct slcli_launcher_birth_observation *observation;
  if(!c||!a||!slcli_arena_live_block(a,c,sizeof(*c))||!c->qualified||!c->created||
     c->release_attempted||c->birth_observation_failed||!prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||
     !prior->complete||prior->tick<c->after.tick||prior->tick>=c->expires||!same(c,a)||
     !target||target!=c->target_owner_original||target==a->owner||
     !slcli_arena_live_block(a,target,sizeof(*target))||
     target->charged!=c->target_charged_original||target->count!=c->target_count_original||
     target->next_sequence!=c->target_next_sequence_original||
     !slcli_arena_live_range(a,owner_export,owner_export_bytes)||
     !slcli_owner_handoff_matches(owner_export,owner_export_bytes,target)||
     !apart(c,sizeof(*c),target,sizeof(*target))||
     !apart(c,sizeof(*c),owner_export,owner_export_bytes))return 0;
  if(c->birth_observation_used>=c->original.birth_observation_capacity) {
    c->birth_observation_failed=1;return SLCLI_RELEASE_FAILED;
  }
  observation=&c->birth_observations[c->birth_observation_used++];
  if(!slcli_native_clock_read(&observation->before)||!same(c,a)||
     observation->before.tick<prior->tick||observation->before.tick>=c->expires||
     (c->birth_observation_used>1&&observation->before.tick<
       c->birth_observations[c->birth_observation_used-2].after.tick)) {
    c->birth_observation_failed=1;return SLCLI_RELEASE_FAILED;
  }
  observation->publication=atomic_load_explicit(&c->child->birth_publication,memory_order_acquire);
  observation->failure_publication=atomic_load_explicit(&c->child->publication,memory_order_acquire);
  observation->observed=1;
  if(!slcli_native_clock_read(&observation->after)||!same(c,a)||
     observation->after.tick<observation->before.tick||observation->after.tick>=c->expires) {
    c->birth_observation_failed=1;return SLCLI_RELEASE_FAILED;
  }
  if(observation->failure_publication>=2){c->birth_observation_failed=1;return SLCLI_RELEASE_FAILED;}
  if(!observation->publication)return SLCLI_RELEASE_PENDING;
  if(observation->publication!=1){c->birth_observation_failed=1;return SLCLI_RELEASE_FAILED;}
  c->release_attempted=1;
  c->target_owner_original=target;c->owner_export_original=owner_export;
  c->owner_export_bytes=owner_export_bytes;
  if(!slcli_native_clock_read(&c->release_before)||!same(c,a)||
     c->release_before.tick<observation->after.tick||c->release_before.tick>=c->expires||
     !slcli_owner_handoff_matches(owner_export,owner_export_bytes,target))return 0;
  /* Actual pre-exec child birth is acquire-published by this same clone. It is
   * not the future ELF address space, loader admission or parent authority. */
  c->transport_birth_attempted=1;
  c->transport_birth_result=slcli_launcher_linux_birth_original(c,a,c->transport_birth);
  c->transport_birth_returned=1;if(!c->transport_birth_result)return 0;
  {
    struct slcli_node_bootstrap_transport *t=c->transport_payload;
    const struct slcli_preentry_backing_linux *runtime=c->runtime_backing_original;
    t->origin=*c->origin;t->expires=c->expires;t->owner_limit=target->limit;
    if(!slcli_node_prefix_original_matches(c->prefix_original,a,c->deadline,c->caller,
       target,&runtime->request))return 0;
    t->frozen_owner=*target;t->runtime_request=runtime->request;
    t->prefix=c->prefix_original->snapshot;
    t->runtime_bytes=runtime->bytes;t->bootstrap_bytes=c->original.bootstrap_bytes;
    t->capacity=c->original.allocator_capacity;t->thread_capture_attempts=c->original.thread_capture_attempts;
    t->capabilities[0]=(uint64_t)runtime->descriptor;
    t->capabilities[1]=(uint64_t)c->transport_original->descriptor;
    t->process_original=c->transport_birth->process_result;
    t->thread_original=c->transport_birth->thread_result;
    for(i=0;i<2;i++)t->birth_raw[i]=c->transport_birth->birth.raw[i];
    for(i=0;i<32;i++) {
      t->associations[0][i]=target->original_birth[i];t->associations[1][i]=target->original_image[i];
      t->associations[2][i]=target->original_owner[i];t->associations[3][i]=target->original_source[i];
    }
    for(i=0;i<sizeof(runtime->backing_original);i++)
      t->expected_native[i]=((const unsigned char *)&runtime->backing_original)[i];
  }
  c->transport_publish_attempted=1;
  c->transport_publish_result=slcli_node_transport_linux_publish(c->transport_original,a,
    &c->release_before,c->transport_payload);
  c->transport_publish_returned=1;
  if(!c->transport_publish_result||!c->transport_original->returned||
     !c->transport_original->bytes_equal||!c->transport_original->seals_observed||
     !slcli_owner_handoff_matches(owner_export,owner_export_bytes,&c->transport_payload->frozen_owner)||
     !same(c,a)||c->transport_original->descriptor!=c->transport_descriptor_original)return 0;
  if(!slcli_native_clock_read(&c->transport_ready)||!same(c,a)||
     c->transport_ready.tick<c->transport_original->calls[6].after.tick||
     c->transport_ready.tick>=c->expires)return 0;
  atomic_store_explicit(&c->child->gate,1,memory_order_release);
  c->wake_args[0]=(uintptr_t)&c->child->gate;c->wake_args[1]=FUTEX_WAKE;c->wake_args[2]=1;
  c->wake_called=1;
  result=slcli_kernel_call(__NR_futex,(long)&c->child->gate,FUTEX_WAKE,1,0,0,0);
  c->wake_result=(uint64_t)result;c->wake_returned=1;
  if(!slcli_native_clock_read(&c->release_after)||!same(c,a)||
     c->release_after.tick<c->transport_ready.tick||c->release_after.tick>=c->expires||result<0||
     !slcli_owner_handoff_matches(owner_export,owner_export_bytes,target))return 0;
  c->release_qualified=1;return 1;
}
#endif
