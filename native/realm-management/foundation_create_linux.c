#include "foundation_create_linux.h"
#if defined(__linux__)
#include "../host-service/native_syscall_linux.h"
#include <linux/fcntl.h>
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
static int role(uint32_t n){return n==97||n==98||n==21||n==15||n==6;}
static int source(struct slcli_management_foundation_linux_creation *c) {
  size_t i;
  if(!c||!c->prepared||!c->arena||!c->sink||c->parent_original<0||
     c->name_bytes<2||c->name_bytes>65||!slcli_arena_live_block(c->arena,c,sizeof(*c))||
     !slcli_arena_live_block(c->arena,c->caller_original,c->caller_bytes)||
     !slcli_arena_live_block(c->arena,c->capture_original,c->capture_original_bytes)||
     !slcli_arena_live_block(c->arena,c->name_original,c->name_bytes)||
     !slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller_original)||
     c->deadline->origin!=c->origin||c->deadline->expires!=c->expires||
     c->origin->tick!=c->origin_tick||!role(c->original_role)||c->parent_link_delta>1)return 0;
  for(i=0;i<c->name_bytes;i++)if(c->name[i]!=c->name_original[i])return 0;
  return 1;
}
struct slcli_management_foundation_linux_creation *slcli_management_foundation_linux_creation_allocate(
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,size_t bytes,
  int parent,uint32_t slot,uint32_t link_delta,const char *name,size_t name_bytes,
  slcli_store_capture_sink sink,void *owner,size_t owner_bytes) {
  struct slcli_management_foundation_linux_creation *c;size_t i;
  if(!a||!a->owner||!bytes||!owner_bytes||parent<0||!role(slot)||link_delta>1||!sink||
     name_bytes<2||name_bytes>65||!slcli_arena_live_block(a,name,name_bytes)||
     !slcli_arena_live_block(a,caller,bytes)||!slcli_arena_live_block(a,owner,owner_bytes)||
     !slcli_source_deadline_original_matches(a,d,caller)||
     !away(caller,bytes,owner,owner_bytes)||!away(name,name_bytes,owner,owner_bytes)||
     !slcli_source_deadline_away(d,caller,bytes)||!slcli_source_deadline_away(d,owner,owner_bytes)||
     !slcli_source_deadline_away(d,name,name_bytes)||
     !away(caller,bytes,a,sizeof(*a))||!away(caller,bytes,a->owner,sizeof(*a->owner))||
     !away(owner,owner_bytes,a,sizeof(*a))||!away(owner,owner_bytes,a->owner,sizeof(*a->owner))||
     !away(name,name_bytes,a,sizeof(*a))||!away(name,name_bytes,a->owner,sizeof(*a->owner)))return 0;
  if(name[name_bytes-1]||name[0]=='.')return 0;
  for(i=0;i+1<name_bytes;i++)if(!((name[i]>='a'&&name[i]<='z')||
    (name[i]>='A'&&name[i]<='Z')||(name[i]>='0'&&name[i]<='9')||name[i]=='-'||name[i]=='_'))return 0;
  c=slcli_arena_calloc(a,1,sizeof(*c));if(!c)return 0;
  if(!away(c,sizeof(*c),caller,bytes)||!away(c,sizeof(*c),owner,owner_bytes)||
     !away(c,sizeof(*c),name,name_bytes)||!slcli_source_deadline_away(d,c,sizeof(*c))||
     !away(c,sizeof(*c),a,sizeof(*a))||!away(c,sizeof(*c),a->owner,sizeof(*a->owner)))return 0;
  c->arena=a;c->deadline=d;c->origin=d->origin;c->caller_original=caller;c->caller_bytes=bytes;
  c->name_original=name;c->name_bytes=name_bytes;for(i=0;i<name_bytes;i++)c->name[i]=name[i];
  c->parent_original=parent;c->created_original=-1;c->original_role=slot;c->parent_link_delta=link_delta;
  c->sink=sink;c->capture_original=owner;c->capture_original_bytes=owner_bytes;
  c->expires=d->expires;c->origin_tick=d->origin->tick;c->previous_tick=c->origin_tick;
  c->prepared=1;return c;
}
static int clock_row(struct slcli_management_foundation_linux_creation *c,size_t row) {
  struct slcli_native_clock *r=&c->clocks[row];
  if(!source(c)||!slcli_native_clock_read(r)||r->tick<c->previous_tick||
     r->tick>=c->expires||!source(c))return 0;c->previous_tick=r->tick;return 1;
}
static int observe(struct slcli_management_foundation_linux_creation *c,
  int fd,struct slcli_linux_file_capture *r,size_t row) {
  int ok;if(!clock_row(c,row))return 0;ok=slcli_linux_directory_source_observe(fd,r);
  if(!c->sink(c->capture_original,r,0,0)||!ok||!source(c)||!r->tick||
     r->tick<c->previous_tick||r->tick>=c->expires)return 0;
  c->previous_tick=r->tick;return clock_row(c,row+1);
}
static int parent_same_created(const struct slcli_management_foundation_linux_creation *c) {
  const struct slcli_linux_file_capture *a=&c->parent_before,*b=&c->parent_after;
  uint64_t links=(uint64_t)a->original_stat.st_nlink;
  /* Namespace creation may change parent length/timestamps and link count.
   * Compare only the EXACT independently qualified original filesystem
   * plan's permitted transition; do not weaken ordinary file comparisons or
   * discover a permissive plan from actual outputs after effect. */
  return c->directory_created&&c->parent_link_delta<=1&&
    links<=UINT64_MAX-c->parent_link_delta&&
    (uint64_t)b->original_stat.st_nlink==links+c->parent_link_delta&&
    a->stat_observed&&b->stat_observed&&!a->stat_status&&!b->stat_status&&
    a->flags_observed&&b->flags_observed&&a->descriptor_observed&&b->descriptor_observed&&
    a->original_stat.st_dev==b->original_stat.st_dev&&a->original_stat.st_ino==b->original_stat.st_ino&&
    a->original_stat.st_mode==b->original_stat.st_mode&&a->original_stat.st_uid==b->original_stat.st_uid&&
    a->original_stat.st_gid==b->original_stat.st_gid&&a->flags_status==b->flags_status&&
    a->descriptor_status==b->descriptor_status;
}
int slcli_management_foundation_linux_create(struct slcli_management_foundation_linux_creation *c) {
  long result;
  if(!source(c)||c->started||c->unresolved)return 0;c->started=1;c->unresolved=1;
  if(!observe(c,c->parent_original,&c->parent_before,0)||
     (uint64_t)c->parent_before.original_stat.st_nlink>UINT64_MAX-c->parent_link_delta||
     !clock_row(c,2))return 0;
  c->uid_called=1;result=slcli_kernel_call(__NR_getuid,0,0,0,0,0,0);
  c->uid_status=(uint64_t)result;c->uid_observed=1;
  if(!clock_row(c,3)||result<0||(uint64_t)result>UINT32_MAX||!clock_row(c,4))return 0;
  c->mkdir_args[0]=(uint64_t)c->parent_original;c->mkdir_args[1]=(uintptr_t)c->name;
  c->mkdir_args[2]=0700;c->mkdir_called=1;
  result=slcli_kernel_call(__NR_mkdirat,c->parent_original,(long)c->name,0700,0,0,0);
  c->mkdir_status=(uint64_t)result;c->mkdir_observed=1;if(!result)c->directory_created=1;
  if(!clock_row(c,5)||result||!clock_row(c,6))return 0;
  c->open_args[0]=(uint64_t)c->parent_original;c->open_args[1]=(uintptr_t)c->name;
  c->open_args[2]=O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC;c->open_called=1;
  result=slcli_kernel_call(__NR_openat,c->parent_original,(long)c->name,
    (long)c->open_args[2],0,0,0);
  c->open_status=(uint64_t)result;c->open_observed=1;
  if(result>=0&&result<=INT32_MAX){c->created_original=(int)result;c->descriptor_acquired=1;}
  /* Retain the actual born fd and created namespace effect BEFORE this clock.
   * Failure never closes, removes, retries or pretends either effect vanished. */
  if(!clock_row(c,7)||!c->descriptor_acquired||
     !observe(c,c->created_original,&c->created,8)||
     c->created.original_stat.st_uid!=c->uid_status||
     (c->created.original_stat.st_mode&07777)!=0700||
     !observe(c,c->parent_original,&c->parent_after,10)||
     !parent_same_created(c))return 0;
  c->complete=1;c->unresolved=0;return 1;
}
#endif
