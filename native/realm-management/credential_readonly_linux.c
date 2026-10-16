#include "credential_readonly_linux.h"
#if defined(__linux__)
#include <linux/fcntl.h>
#include "../host-service/native_syscall_linux.h"
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
static int source(struct slcli_management_credential_linux_readonly *c) {
  struct slcli_management_credential_linux_creation *b;size_t i;
  if(!c||!c->prepared||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||
     !slcli_management_credential_linux_factory_original_matches(c->factory))return 0;
  b=c->factory->creation;
  if(!b->name_original||b->name_bytes<2||b->name_bytes>sizeof(b->name)||
     !slcli_arena_live_range(c->arena,b->name_original,b->name_bytes))return 0;
  for(i=0;i<b->name_bytes;i++)if(b->name[i]!=b->name_original[i])return 0;
  return c->factory->arena==c->arena&&c->deadline==b->deadline&&c->origin==b->origin&&
    c->caller_original==b->caller_original&&c->caller_bytes==b->caller_bytes&&
    c->expires==b->expires&&c->origin_tick==b->origin_tick&&
    slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller_original)&&
    c->body&&c->input_owner&&c->body->deadline_owner==c->input_owner&&
    c->input_owner->body==c->body&&c->input_owner->deadline==c->deadline&&
    c->input_owner->caller_original==c->caller_original&&
    c->input_owner->origin==c->origin&&c->input_owner->expires==c->expires&&
    c->input_owner->origin_tick==c->origin_tick&&slcli_input_deadline_ready(c->body)&&
    c->credential&&slcli_arena_live_block(c->arena,c->credential,sizeof(*c->credential))&&
    c->credential->arena==c->arena&&away(c,sizeof(*c),c->credential,sizeof(*c->credential));
}
struct slcli_management_credential_linux_readonly *slcli_management_credential_linux_readonly_allocate(
  struct slcli_arena *a,struct slcli_management_credential_linux_factory *f) {
  struct slcli_management_credential_linux_readonly *c;
  struct slcli_management_credential_linux_creation *b;
  size_t io_bytes;
  if(!a||!f||!slcli_management_credential_linux_factory_original_matches(f)||f->arena!=a||
     f->started||f->unresolved||f->complete)return 0;b=f->creation;
  c=slcli_arena_calloc(a,1,sizeof(*c));if(!c)return 0;
  c->arena=a;c->factory=f;c->deadline=b->deadline;c->origin=b->origin;
  c->caller_original=b->caller_original;c->caller_bytes=b->caller_bytes;
  c->expires=b->expires;c->origin_tick=b->origin_tick;c->previous_tick=b->origin_tick;
  if(!away(c,sizeof(*c),b,sizeof(*b))||!away(c,sizeof(*c),f,sizeof(*f))||
     !away(c,sizeof(*c),c->caller_original,c->caller_bytes)||
     !away(c,sizeof(*c),b->capture_original,b->capture_original_bytes)||
     !away(c,sizeof(*c),b->name_original,b->name_bytes)||
     !slcli_source_deadline_away(c->deadline,c,sizeof(*c)))return 0;
  c->body=slcli_input_body_allocate(a,64,134);if(!c->body)return 0;
  c->input_owner=slcli_input_deadline_bind(a,c->body,c->deadline,c->caller_original,
    c->caller_bytes,c->origin,b->sink,b->capture_original,b->capture_original_bytes);
  c->credential=slcli_management_credential_allocate(a);
  if(!c->input_owner||!c->credential)return 0;
  io_bytes=(sizeof(*c->input_owner)+_Alignof(struct slcli_native_clock)-1)/
    _Alignof(struct slcli_native_clock)*_Alignof(struct slcli_native_clock)+134*sizeof(struct slcli_native_clock);
  if(!away(c,sizeof(*c),c->body,c->input_owner->body_storage_bytes)||
     !away(c,sizeof(*c),c->input_owner,io_bytes)||
     !away(c->credential,sizeof(*c->credential),c->body,c->input_owner->body_storage_bytes)||
     !away(c->credential,sizeof(*c->credential),c->input_owner,io_bytes)||
     !slcli_source_deadline_away(c->deadline,c->credential,sizeof(*c->credential)))return 0;
  c->prepared=1;if(!source(c))return 0;return c;
}
static int clock_row(struct slcli_management_credential_linux_readonly *c,size_t row) {
  struct slcli_native_clock *r=&c->clocks[row];
  if(!source(c)||!slcli_native_clock_read(r)||r->tick<c->previous_tick||
     r->tick>=c->expires||!source(c))return 0;c->previous_tick=r->tick;return 1;
}
static int same_transfer(const struct slcli_linux_file_capture *a,
  const struct slcli_linux_file_capture *b) {
  /* Access decreases; exact inode/content/owner custody does not. Namespace
   * ownership is independently admitted and is not inferred from a name. */
  return a&&b&&a->stat_observed&&b->stat_observed&&!a->stat_status&&!b->stat_status&&
    a->flags_observed&&b->flags_observed&&a->descriptor_observed&&b->descriptor_observed&&
    a->original_stat.st_dev==b->original_stat.st_dev&&a->original_stat.st_ino==b->original_stat.st_ino&&
    a->original_stat.st_uid==b->original_stat.st_uid&&a->original_stat.st_gid==b->original_stat.st_gid&&
    a->original_stat.st_mode==b->original_stat.st_mode&&a->original_stat.st_nlink==b->original_stat.st_nlink&&
    a->original_stat.st_size==64&&b->original_stat.st_size==64&&
    a->original_stat.st_mtime==b->original_stat.st_mtime&&a->original_stat.st_mtime_nsec==b->original_stat.st_mtime_nsec&&
    a->original_stat.st_ctime==b->original_stat.st_ctime&&a->original_stat.st_ctime_nsec==b->original_stat.st_ctime_nsec&&
    (a->flags_status&O_ACCMODE)==O_RDWR&&(b->flags_status&O_ACCMODE)==O_RDONLY&&
    !(b->flags_status&O_PATH)&&(b->descriptor_status&FD_CLOEXEC);
}
static int captured(struct slcli_management_credential_linux_readonly *c,
  struct slcli_linux_file_capture *r,int ok) {
  /* Preserve raw native output even when a later clock failed. */
  if(!c->factory->creation->sink(c->factory->creation->capture_original,r,0,0)||
     !ok||!r->clock_observed||!r->tick||r->tick<c->previous_tick||r->tick>=c->expires)return 0;
  c->previous_tick=r->tick;return source(c);
}
int slcli_management_credential_linux_readonly_run(struct slcli_management_credential_linux_readonly *c) {
  struct slcli_management_credential_persistence *p;
  struct slcli_management_credential_linux_creation *b;
  const struct slcli_linux_file_capture *last;long result;int ok;
  if(!source(c)||c->started||c->unresolved||c->factory->started||c->factory->complete)return 0;
  c->started=1;c->unresolved=1;
  if(!slcli_management_credential_linux_factory_run(c->factory)||!source(c))return 0;
  p=c->factory->persistence;b=c->factory->creation;c->previous_tick=p->previous_tick;
  if(!p->complete||p->unresolved||!p->second->capture_count)return 0;
  last=&p->second->captures[p->second->capture_count-1];c->writer_original=p->original;
  if(!clock_row(c,0))return 0;
  ok=slcli_linux_directory_source_observe(b->parent_original,&c->parent_capture);
  if(!captured(c,&c->parent_capture,ok)||!clock_row(c,1)||
     !slcli_store_original_same(&b->parent_after,&c->parent_capture)||!clock_row(c,2))return 0;
  c->open_args[0]=(uint64_t)b->parent_original;c->open_args[1]=(uintptr_t)b->name;
  c->open_args[2]=O_RDONLY|O_NOFOLLOW|O_CLOEXEC;c->open_called=1;
  result=slcli_kernel_call(__NR_openat,b->parent_original,(long)b->name,
    O_RDONLY|O_NOFOLLOW|O_CLOEXEC,0,0,0);
  c->open_status=(uint64_t)result;c->open_observed=1;
  if(result>=0&&result<=INT32_MAX){c->readonly_original=(int)result;c->alias_acquired=1;}
  /* No retry or replacement. Both original aliases remain charged before
   * diagnostics; an open/post-clock failure cannot erase a born descriptor. */
  if(!clock_row(c,3)||!c->alias_acquired||!clock_row(c,4))return 0;
  ok=slcli_linux_file_observe(c->readonly_original,&c->readonly_before);
  if(!captured(c,&c->readonly_before,ok)||!clock_row(c,5)||
     !same_transfer(last,&c->readonly_before)||!clock_row(c,6))return 0;
  ok=slcli_linux_file_close(c->writer_original,&c->close_capture);
  if(c->close_capture.descriptor_closed)c->writer_closed=1;
  if(!captured(c,&c->close_capture,ok)||!clock_row(c,7)||!c->writer_closed||!clock_row(c,8))return 0;
  ok=slcli_linux_file_observe(c->readonly_original,&c->readonly_after);
  if(!captured(c,&c->readonly_after,ok)||!clock_row(c,9)||
     !slcli_input_original_same(&c->readonly_before,&c->readonly_after,64)||
     !slcli_input_deadline_precede(c->input_owner,&c->clocks[9])||
     !slcli_input_body_read(c->body,c->readonly_original,64,p->second->digest)||
     !slcli_input_deadline_ready(c->body)||
     !slcli_input_original_same(&c->readonly_after,&c->body->captures[0],64))return 0;
  c->body_read=1;
  if(!slcli_management_credential_consume(c->credential,c->body)||!source(c)||
     !slcli_input_deadline_ready(c->body)||c->input_owner->predecessor!=&c->clocks[9])return 0;
  c->credential_consumed=1;c->complete=1;c->unresolved=0;return 1;
}
int slcli_management_credential_linux_readonly_native_positive(struct slcli_management_credential_linux_readonly *c) {
  size_t n;uint64_t tick;int alias;
  if(!c||!slcli_management_credential_linux_readonly_run(c)||!c->complete||c->unresolved||
     !c->alias_acquired||!c->writer_closed||!c->body_read||!c->credential_consumed||
     !c->credential->complete||c->credential->failed||!c->body->recheck_complete)return 0;
  n=c->body->capture_count;tick=c->input_owner->previous_tick;alias=c->readonly_original;
  if(slcli_management_credential_linux_readonly_run(c))return 0;
  return c->complete&&!c->unresolved&&c->body->capture_count==n&&
    c->input_owner->previous_tick==tick&&c->readonly_original==alias&&c->writer_closed;
}
#endif