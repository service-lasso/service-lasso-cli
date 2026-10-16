#if defined(__linux__)
#include "unit19_seal_linux.h"
#include <linux/fcntl.h>
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
struct slcli_unit19_seal_linux *slcli_unit19_seal_linux_allocate(struct slcli_arena *a) {
  struct slcli_unit19_seal_linux *s;
  if(!a)return 0;s=slcli_arena_calloc(a,1,sizeof(*s));if(!s)return 0;
  s->arena=a;s->failed=1;s->verity=slcli_verity_linux_allocate(a);
  if(s->verity)s->failed=0;return s;
}
static int same_content(const struct slcli_linux_file_capture *a,
                         const struct slcli_linux_file_capture *b,uint64_t bytes) {
  /* Intentional different access modes on distinct actual descriptors of the
   * SAME inode. Preserve both original rights; never alias readonly to writer. */
  return a->stat_observed&&b->stat_observed&&!a->stat_status&&!b->stat_status&&
    a->original_stat.st_dev==b->original_stat.st_dev&&
    a->original_stat.st_ino==b->original_stat.st_ino&&
    a->original_stat.st_uid==b->original_stat.st_uid&&
    a->original_stat.st_gid==b->original_stat.st_gid&&
    a->original_stat.st_mode==b->original_stat.st_mode&&
    a->original_stat.st_nlink==b->original_stat.st_nlink&&
    a->original_stat.st_size>=0&&b->original_stat.st_size>=0&&
    (uint64_t)a->original_stat.st_size==bytes&&
    (uint64_t)b->original_stat.st_size==bytes&&
    a->original_stat.st_mtime==b->original_stat.st_mtime&&
    a->original_stat.st_mtime_nsec==b->original_stat.st_mtime_nsec&&
    a->original_stat.st_ctime==b->original_stat.st_ctime&&
    a->original_stat.st_ctime_nsec==b->original_stat.st_ctime_nsec;
}
int slcli_unit19_seal_linux_original(struct slcli_unit19_seal_linux *s,
  struct slcli_unit19_native *u,int readonly_fd,uint32_t block_size,
  const struct slcli_source_deadline_original *d,const void *caller,
  slcli_store_capture_sink sink,void *capture_original) {
  const struct slcli_linux_file_capture *last;uint64_t expires,origin_tick;
  const struct slcli_native_clock *origin;int ok;unsigned i;
  if(!s||s->started||s->failed||!s->arena||!u||u->arena!=s->arena||
     !slcli_arena_live_range(s->arena,s,sizeof(*s))||
     !slcli_arena_live_range(s->arena,u,sizeof(*u))||
     !s->verity||!slcli_arena_live_range(s->arena,s->verity,sizeof(*s->verity))||
     !away(s,sizeof(*s),u,sizeof(*u))||!away(s,sizeof(*s),s->verity,sizeof(*s->verity))||
     !slcli_source_deadline_original_matches(s->arena,d,caller)||
     !slcli_source_deadline_away(d,s,sizeof(*s))||
     !slcli_source_deadline_away(d,s->verity,sizeof(*s->verity))||
     !u->prepared||u->failed||u->seal_attempted||!u->held||!u->write||
     !slcli_arena_live_range(s->arena,u->held,sizeof(*u->held))||
     !slcli_arena_live_range(s->arena,u->write,sizeof(*u->write))||
     !u->held->held_verified||!u->write->held_verified||!u->held->capture_count||
     !u->write->capture_count||u->write->original!=u->held->original||
     readonly_fd<0||readonly_fd==u->write->original||!sink||!capture_original||
     !block_size||(block_size&(block_size-1)))return 0;
  for(i=0;i<2;i++) {
    struct slcli_store_stream *stream=i?u->held:u->write;size_t n;
    if(stream->capture_capacity>SIZE_MAX/sizeof(*stream->captures)||
       stream->capture_count>stream->capture_capacity)return 0;
    n=stream->capture_capacity*sizeof(*stream->captures);
    if(!slcli_arena_live_range(s->arena,stream->captures,n)||
       !away(s,sizeof(*s),stream,sizeof(*stream))||
       !away(s,sizeof(*s),stream->captures,n)||
       !away(s->verity,sizeof(*s->verity),stream,sizeof(*stream))||
       !away(s->verity,sizeof(*s->verity),stream->captures,n)||
       !away(s,sizeof(*s),stream->unit_chunk,16384)||
       !away(s,sizeof(*s),stream->unit_readback,16385)||
       !away(s->verity,sizeof(*s->verity),stream->unit_chunk,16384)||
       !away(s->verity,sizeof(*s->verity),stream->unit_readback,16385))return 0;
  }
  last=&u->held->captures[u->held->capture_count-1];
  if(!slcli_arena_live_range(s->arena,last,sizeof(*last))||
     !away(s,sizeof(*s),last,sizeof(*last))||
     !away(s->verity,sizeof(*s->verity),u,sizeof(*u))||
     !away(s->verity,sizeof(*s->verity),last,sizeof(*last)))return 0;
  s->started=1;s->failed=1;s->prepared=u;s->readonly_original=readonly_fd;
  u->seal_attempted=1;expires=d->expires;origin=d->origin;origin_tick=origin->tick;
  if(!slcli_native_clock_read(&s->readonly_entry)||
     s->readonly_entry.tick<origin_tick||s->readonly_entry.tick>=expires)return 0;
  ok=slcli_linux_file_observe(readonly_fd,&s->readonly_before);
  if(!sink(capture_original,&s->readonly_before,0,0))ok=0;
  if(!ok||s->readonly_before.tick<s->readonly_entry.tick||s->readonly_before.tick>=expires||
     (s->readonly_before.flags_status&O_ACCMODE)!=O_RDONLY||
     (s->readonly_before.flags_status&O_PATH)||
     !(s->readonly_before.descriptor_status&FD_CLOEXEC)||
     !same_content(last,&s->readonly_before,u->parsed.file_bytes))return 0;
  if(d->expires!=expires||d->origin!=origin||d->caller_original!=caller||
     origin->tick!=origin_tick||!slcli_native_clock_read(&s->close_entry)||
     s->close_entry.tick<s->readonly_before.tick||s->close_entry.tick>=expires)return 0;
  ok=slcli_linux_file_close(u->write->original,&s->creator_close);
  if(s->creator_close.descriptor_closed){s->creator_closed=1;u->creator_closed=1;}
  if(!sink(capture_original,&s->creator_close,0,0))ok=0;
  if(!ok||s->creator_close.tick<s->close_entry.tick||s->creator_close.tick>=expires||
     d->expires!=expires||d->origin!=origin||d->caller_original!=caller||
     origin->tick!=origin_tick)return 0;
  if(!slcli_verity_linux_original(s->verity,readonly_fd,1,block_size,&s->readonly_before,
     &s->creator_close,u->parsed.file_bytes,d,caller,sink,capture_original))return 0;
  /* Whole prepared raw SHA and SP1 members still require SAME held native
   * postseal consumption; verity success alone is not input admission. */
  s->sealed=1;s->failed=0;return 1;
}
#endif
