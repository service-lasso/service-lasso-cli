/* SPDX-License-Identifier: Apache-2.0 */
#include "bq_source_readable_linux.h"
#if defined(__linux__)
#include "native_syscall_linux.h"
#include <linux/fcntl.h>
#include <linux/magic.h>
#include <limits.h>
static int zero(const void *p,size_t n){const unsigned char *b=p;size_t i;for(i=0;i<n;++i)if(b[i])return 0;return 1;}
static int apart(const void *a,size_t n,const void *b,size_t m){uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&n<=UINTPTR_MAX-x&&m<=UINTPTR_MAX-y&&(x+n<=y||y+m<=x);}
static size_t decimal(char *out,uint32_t value){char reverse[10];size_t n=0,i;
 do{reverse[n++]=(char)('0'+value%10);value/=10;}while(value);
 for(i=0;i<n;++i)out[i]=reverse[n-1-i];out[n]=0;return n;}
static int text_equal(const char *a,const char *b,size_t n){size_t i;for(i=0;i<n;++i)if(a[i]!=b[i])return 0;return 1;}
static int identity_equal(const struct slcli_linux_file_capture *a,const struct slcli_linux_file_capture *b){
 const struct statx *x=&a->identity,*y=&b->identity;
 return x->stx_mask==y->stx_mask&&x->stx_ino==y->stx_ino&&x->stx_size==y->stx_size&&
  x->stx_dev_major==y->stx_dev_major&&x->stx_dev_minor==y->stx_dev_minor&&
  x->stx_mode==y->stx_mode&&x->stx_uid==y->stx_uid&&x->stx_gid==y->stx_gid&&
  x->stx_nlink==y->stx_nlink&&x->stx_mtime.tv_sec==y->stx_mtime.tv_sec&&
  x->stx_mtime.tv_nsec==y->stx_mtime.tv_nsec&&x->stx_ctime.tv_sec==y->stx_ctime.tv_sec&&
  x->stx_ctime.tv_nsec==y->stx_ctime.tv_nsec;
}
static int same(const struct slcli_bq_source_readable_linux *c){
 char path[32]="/proc/",name[11];size_t n,j;
 if(!c||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||!c->started||
  !slcli_arena_live_block(c->arena,c->caller,c->caller_bytes)||
  !slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)||
  c->deadline->origin!=c->origin||c->origin->tick!=c->origin_tick||c->deadline->expires!=c->expires||
  !slcli_arena_live_range(c->arena,c->prior,sizeof(*c->prior))||!c->prior->complete||
  c->prior->tick!=c->prior_tick||c->prior_tick<c->origin_tick||c->prior_tick>=c->expires||
  !slcli_bq_source_namespace_linux_matches_original(c->source)||
  !slcli_bq_source_namespace_linux_matches_original(c->proc_directory)||
  !slcli_bq_process_reference_linux_matches_original(c->process)||
  c->source->arena!=c->arena||c->proc_directory->arena!=c->arena||c->process->arena!=c->arena||
  c->source->deadline!=c->deadline||c->proc_directory->deadline!=c->deadline||c->process->deadline!=c->deadline||
  c->source->caller!=c->caller||c->proc_directory->caller!=c->caller||c->process->caller!=c->caller||
  c->source->leaf_kind!=S_IFREG||c->proc_directory->leaf_kind!=S_IFDIR||
  c->source->finish.tick>c->prior_tick||c->proc_directory->finish.tick>c->prior_tick||
  c->process->finish.tick>c->prior_tick)return 0;
 n=6+decimal(path+6,(uint32_t)c->process->status[0]);path[n++]='/';path[n++]='f';path[n++]='d';path[n++]=0;
 decimal(name,(uint32_t)c->source->components[c->source->count-1].descriptor);
 if(c->proc_directory->path_bytes!=n||!text_equal(c->proc_directory->path,path,n)||
  !text_equal(c->proc_path,path,n))return 0;
 for(j=n;j<sizeof(c->proc_path);++j)if(c->proc_path[j])return 0;
 for(j=0;j<sizeof(name);++j){if(c->descriptor_name[j]!=name[j])return 0;if(!name[j])break;}
 for(++j;j<sizeof(c->descriptor_name);++j)if(c->descriptor_name[j])return 0;
 return 1;
}
static int rows(const struct slcli_bq_source_readable_linux *c){
 size_t i,j;uint64_t prior;int source,proc;
 if(!same(c)||c->next!=5||c->descriptor<0)return 0;
 source=c->source->components[c->source->count-1].descriptor;
 proc=c->proc_directory->components[c->proc_directory->count-1].descriptor;
 prior=c->prior_tick;
 for(i=0;i<5;++i){
  if(!c->called[i]||!c->returned[i]||!c->before_returned[i]||!c->after_returned[i]||
   !c->before_result[i]||!c->after_result[i]||
   !slcli_native_clock_linux_matches_original(&c->before[i])||!slcli_native_clock_linux_matches_original(&c->after[i])||
   c->before[i].tick<prior||c->after[i].tick<c->before[i].tick||c->after[i].tick>=c->expires)return 0;
  prior=c->after[i].tick;
 }
 if(c->fs_status||c->filesystem.f_type!=PROC_SUPER_MAGIC||c->fs_args[0]!=proc||
  c->fs_args[1]!=(long)&c->filesystem||c->open_status!=c->descriptor||
  c->open_args[0]!=proc||c->open_args[1]!=(long)c->descriptor_name||
  c->open_args[2]!=(O_RDONLY|O_CLOEXEC))return 0;
 for(j=2;j<6;++j)if(c->fs_args[j])return 0;
 for(j=3;j<6;++j)if(c->open_args[j])return 0;
 if(!c->observe_result[0]||!c->observe_result[1]||!c->observe_result[2]||
  !slcli_linux_path_observation_matches_original(source,&c->source_before,S_IFREG)||
  !slcli_linux_file_observation_matches_original(c->descriptor,&c->readable,S_IFREG)||
  !slcli_linux_path_observation_matches_original(source,&c->source_after,S_IFREG)||
  c->source_before.tick<c->before[1].tick||c->source_before.tick>c->after[1].tick||
  c->readable.tick<c->before[3].tick||c->readable.tick>c->after[3].tick||
  c->source_after.tick<c->before[4].tick||c->source_after.tick>c->after[4].tick||
  !identity_equal(&c->source->components[c->source->count-1].observation,&c->source_before)||
  !identity_equal(&c->source_before,&c->readable)||!identity_equal(&c->readable,&c->source_after))return 0;
 return c->previous_tick==prior;
}
int slcli_bq_source_readable_linux_matches_original(const struct slcli_bq_source_readable_linux *c){
 return rows(c)&&c->observed&&!c->failed&&c->finish_returned&&c->finish_result&&
  slcli_native_clock_linux_matches_original(&c->finish)&&c->finish.tick>=c->previous_tick&&c->finish.tick<c->expires;
}
int slcli_bq_source_readable_linux_acquire_original(struct slcli_bq_source_readable_linux *c,
 struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_bq_source_namespace_linux *source,const struct slcli_bq_source_namespace_linux *proc,
 const struct slcli_bq_process_reference_linux_original *process,const struct slcli_native_clock *prior){
 const void *borrowed[17];size_t bytes[17],i,n;int source_fd,proc_fd;
 if(!c||!a||!d||!caller||!prior||!source||!proc||!process||
  !slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !slcli_source_deadline_original_matches(a,d,caller)||!slcli_arena_live_block(a,caller,caller_bytes)||
  !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  prior->tick<d->origin->tick||prior->tick>=d->expires||
  !slcli_bq_source_namespace_linux_matches_original(source)||
  !slcli_bq_source_namespace_linux_matches_original(proc)||
  !slcli_bq_process_reference_linux_matches_original(process))return 0;
 borrowed[0]=source;bytes[0]=sizeof(*source);borrowed[1]=proc;bytes[1]=sizeof(*proc);
 borrowed[2]=process;bytes[2]=sizeof(*process);borrowed[3]=caller;bytes[3]=caller_bytes;
 borrowed[4]=prior;bytes[4]=sizeof(*prior);borrowed[5]=a;bytes[5]=sizeof(*a);
 borrowed[6]=source->components;bytes[6]=source->count*sizeof(*source->components);
 borrowed[7]=source->names;bytes[7]=source->path_bytes+2;borrowed[8]=source->path;bytes[8]=source->path_bytes;
 borrowed[9]=proc->components;bytes[9]=proc->count*sizeof(*proc->components);
 borrowed[10]=proc->names;bytes[10]=proc->path_bytes+2;borrowed[11]=proc->path;bytes[11]=proc->path_bytes;
 borrowed[12]=d;bytes[12]=sizeof(*d);borrowed[13]=d->origin;bytes[13]=sizeof(*d->origin);
 borrowed[14]=source->prior;bytes[14]=sizeof(*source->prior);
 borrowed[15]=proc->prior;bytes[15]=sizeof(*proc->prior);
 borrowed[16]=process->prior;bytes[16]=sizeof(*process->prior);
 if(!slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
 for(i=0;i<17;++i)if(!apart(c,sizeof(*c),borrowed[i],bytes[i]))return 0;
 c->started=1;c->failed=1;c->descriptor=-1;c->arena=a;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->source=source;c->proc_directory=proc;c->process=process;c->prior=prior;c->prior_tick=prior->tick;
 c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;c->previous_tick=prior->tick;
 c->proc_path[0]='/';c->proc_path[1]='p';c->proc_path[2]='r';c->proc_path[3]='o';c->proc_path[4]='c';c->proc_path[5]='/';
 n=6+decimal(c->proc_path+6,(uint32_t)process->status[0]);c->proc_path[n++]='/';c->proc_path[n++]='f';c->proc_path[n]='d';
 source_fd=source->components[source->count-1].descriptor;proc_fd=proc->components[proc->count-1].descriptor;
 decimal(c->descriptor_name,(uint32_t)source_fd);
 if(!same(c))return 0;
 for(i=0;i<5;++i){
  c->next=(uint32_t)i+1;c->before_result[i]=slcli_native_clock_read(&c->before[i]);c->before_returned[i]=1;
  if(!same(c)||!c->before_result[i]||c->before[i].tick<c->previous_tick||c->before[i].tick>=c->expires)return 0;
  c->called[i]=1;
  if(i==0){c->fs_args[0]=proc_fd;c->fs_args[1]=(long)&c->filesystem;
   c->fs_status=slcli_kernel_call(__NR_fstatfs,proc_fd,(long)&c->filesystem,0,0,0,0);}
  else if(i==1)c->observe_result[0]=slcli_linux_path_source_observe(source_fd,&c->source_before,S_IFREG);
  else if(i==2){c->open_args[0]=proc_fd;c->open_args[1]=(long)c->descriptor_name;c->open_args[2]=O_RDONLY|O_CLOEXEC;
   c->open_status=slcli_kernel_call(__NR_openat,proc_fd,(long)c->descriptor_name,O_RDONLY|O_CLOEXEC,0,0,0);
   if(c->open_status>=0&&c->open_status<=INT_MAX)c->descriptor=(int)c->open_status;}
  else if(i==3)c->observe_result[1]=slcli_linux_file_observe(c->descriptor,&c->readable);
  else c->observe_result[2]=slcli_linux_path_source_observe(source_fd,&c->source_after,S_IFREG);
  c->returned[i]=1;c->after_result[i]=slcli_native_clock_read(&c->after[i]);c->after_returned[i]=1;
  if(!same(c)||!c->after_result[i]||c->after[i].tick<c->before[i].tick||c->after[i].tick>=c->expires)return 0;
  c->previous_tick=c->after[i].tick;
  if((i==0&&(c->fs_status||c->filesystem.f_type!=PROC_SUPER_MAGIC))||
   (i==1&&!c->observe_result[0])||(i==2&&c->descriptor<0)||
   (i==3&&!c->observe_result[1])||(i==4&&!c->observe_result[2]))return 0;
  if(i==1&&(c->source_before.tick<c->before[i].tick||c->source_before.tick>c->after[i].tick||
   !slcli_linux_path_observation_matches_original(source_fd,&c->source_before,S_IFREG)||
   !identity_equal(&source->components[source->count-1].observation,&c->source_before)))return 0;
  if(i==3&&(c->readable.tick<c->before[i].tick||c->readable.tick>c->after[i].tick||
   !slcli_linux_file_observation_matches_original(c->descriptor,&c->readable,S_IFREG)||
   !identity_equal(&c->source_before,&c->readable)))return 0;
  if(i==4&&(c->source_after.tick<c->before[i].tick||c->source_after.tick>c->after[i].tick||
   !identity_equal(&c->readable,&c->source_after)))return 0;
 }
 if(!rows(c))return 0;
 c->finish_result=slcli_native_clock_read(&c->finish);c->finish_returned=1;
 if(!same(c)||!c->finish_result||c->finish.tick<c->previous_tick||c->finish.tick>=c->expires)return 0;
 c->failed=0;c->observed=1;return 1;
}
#endif
