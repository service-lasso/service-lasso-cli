#include "capture_record.h"
#include "image_deadline.h"
#include "evidence_deadline.h"
#if defined(__linux__)
#include <linux/fcntl.h>
#endif
#if defined(_WIN32) || defined(__linux__)
#include <stddef.h>
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_capture_record_write(struct slcli_capture_record *r,uint32_t kind,
  const void *producer,size_t producer_bytes,const void *query,size_t query_bytes) {
  static const unsigned char magic[16]={'S','L','C','L','I','-','R','A','W','-','C','A','P','-','1',0};
  unsigned i;struct slcli_evidence_file *f;
  if(!r||r->used||!kind||!producer||!producer_bytes||producer_bytes>8192||
    query_bytes>65544||(!query&&query_bytes)||!(f=r->producer_file)||!f->started||
    f->failed||f->sealed||f->count||!separate(r,sizeof(*r),producer,producer_bytes)||
    !separate(r,sizeof(*r),f,sizeof(*f))||!separate(f,sizeof(*f),producer,producer_bytes)||
    (query_bytes&&(!separate(r,sizeof(*r),query,query_bytes)||
      !separate(f,sizeof(*f),query,query_bytes))))return 0;
  r->used=1;r->failed=1;r->producer_raw=producer;r->producer_bytes=producer_bytes;
  r->query_raw=query;r->query_bytes=query_bytes;
  for(i=0;i<16;i++)r->header[i]=magic[i];slcli_put_u32(r->header+16,kind);
  slcli_put_u32(r->header+20,(uint32_t)producer_bytes);slcli_put_u64(r->header+24,query_bytes);
  if(!slcli_evidence_append(f,r->header,32)||!slcli_evidence_append(f,producer,producer_bytes)||
    (query_bytes&&!slcli_evidence_append(f,query,query_bytes))||
    !slcli_evidence_flush_readback(f))return 0;
  r->complete=1;r->failed=0;return 1;
}
#if defined(_WIN32)
int slcli_peer_windows_record_sink(void *original,const struct slcli_peer_windows_capture *c,
  const WCHAR *query,size_t units) {
  struct slcli_capture_record *r=original;
  struct slcli_image_deadline_owner *held,*route;
  if(!r||!c||!c->used||units>32772||!r->producer_file||
    !c->held_reader||!c->route_reader||!(held=c->held_reader->deadline_owner)||
    !(route=c->route_reader->deadline_owner)||
    !slcli_native_clock_read(&r->producer_clock)||
    r->producer_clock.tick<held->previous_tick||r->producer_clock.tick<route->previous_tick||
    (c->after_clock.complete&&r->producer_clock.tick<c->after_clock.tick)||
    !slcli_evidence_deadline_precede(r->producer_file,&r->producer_clock))return 0;
  /* Native query outputs precede local state markers. Those outputs and their
   * actual arguments remain unchanged when the producer later marks complete.
   * Pointer values are preserved as raw ABI facts, never actor authority. */
  return slcli_capture_record_write(original,1,c,offsetof(struct slcli_peer_windows_capture,used),query,units*2);
}
#endif
static slcli_input_capture *next(struct slcli_capture_record_reader *r) {
  slcli_input_capture *c;size_t i;if(r->capture_count>=r->capture_capacity)return 0;
  c=&r->captures[r->capture_count++];
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;return c;
}
static int deadline_ready(const struct slcli_capture_record_reader *r) {
  const struct slcli_image_reader *v=r?&r->deadline_rows:0;
  return v&&v->original==r->original&&v->workspace==r->workspace&&
    v->captures==r->captures&&v->capture_capacity==r->capture_capacity&&
    v->capture_sink==r->capture_sink&&v->capture_original==r->capture_original&&
    v->capture_count==r->capture_count&&slcli_image_deadline_ready(v);
}
int slcli_capture_record_deadline_prepare_unborn(struct slcli_arena *a,
  struct slcli_capture_record_reader *r,const struct slcli_source_deadline_original *d,
  const void *caller,size_t caller_bytes,void *owner,size_t owner_bytes,uint64_t predecessor,
  slcli_input_handle *birth,const void *producer,size_t producer_bytes) {
  struct slcli_image_reader *v;size_t i;
  if(!a||!r||!slcli_arena_live_range(a,r,sizeof(*r))||r->used||r->failed||r->complete||
    r->producer||r->before||r->after||r->offset||r->bytes||r->capture_count||
    !r->capture_capacity||r->capture_capacity>SIZE_MAX/sizeof(*r->captures)||
    !slcli_source_deadline_original_matches(a,d,caller)||
    !separate(r,sizeof(*r),caller,caller_bytes)||!separate(r,sizeof(*r),owner,owner_bytes)||
    !separate(r,sizeof(*r),r->workspace,sizeof(*r->workspace))||
    !separate(r,sizeof(*r),r->captures,r->capture_capacity*sizeof(*r->captures))||
    !separate(r,sizeof(*r),d,sizeof(*d))||!separate(r,sizeof(*r),d->origin,sizeof(*d->origin))||
    !separate(r,sizeof(*r),producer,producer_bytes))return 0;
  v=&r->deadline_rows;
  for(i=0;i<sizeof(*v);i++)if(((const unsigned char *)v)[i])return 0;
  v->original=r->original;v->workspace=r->workspace;v->captures=r->captures;
  v->capture_capacity=r->capture_capacity;v->capture_sink=r->capture_sink;
  v->capture_original=r->capture_original;
  return slcli_image_deadline_prepare_unborn(a,v,d,caller,caller_bytes,owner,owner_bytes,
    predecessor,birth,producer,producer_bytes)&&deadline_ready(r);
}
int slcli_capture_record_deadline_born(struct slcli_capture_record_reader *r,
  slcli_input_handle *birth,const struct slcli_native_clock *clock) {
  int ok;if(!r||r->used||!deadline_ready(r))return 0;
  ok=slcli_image_deadline_born(&r->deadline_rows,birth,clock);
  /* Actual native birth precedes diagnostics. Even a failed clock leaves the
   * same real handle joined, never a fresh unbound reader or retry output. */
  r->original=r->deadline_rows.original;return ok&&deadline_ready(r);
}
static int deadline_before(struct slcli_capture_record_reader *r,slcli_input_capture *c) {
  struct slcli_image_reader *v=&r->deadline_rows;
  if(!c||r->capture_count!=v->capture_count+1||v->original!=r->original||
    v->workspace!=r->workspace||v->captures!=r->captures||
    v->capture_capacity!=r->capture_capacity||v->capture_sink!=r->capture_sink||
    v->capture_original!=r->capture_original)return 0;
  v->capture_count=r->capture_count;return slcli_image_deadline_before(v,c);
}
static int retain(struct slcli_capture_record_reader *r,slcli_input_capture *c,const unsigned char *raw,size_t n) {
  return c&&r->capture_sink(r->capture_original,c,raw,n);
}
static int observe(struct slcli_capture_record_reader *r,slcli_input_capture *c) {
  int ok,kept;if(!c||!deadline_before(r,c))return 0;
#if defined(_WIN32)
  ok=slcli_windows_source_observe(r->original,c);
#else
  ok=slcli_linux_file_observe(r->original,c);
  if(ok)ok=(c->flags_status&O_ACCMODE)==O_RDONLY&&!(c->flags_status&O_PATH)&&
    (c->descriptor_status&FD_CLOEXEC)!=0;
#endif
  kept=retain(r,c,0,0);
  return slcli_image_deadline_after(&r->deadline_rows,c,ok&&kept)&&ok&&kept;
}
static int same(const slcli_input_capture *a,const slcli_input_capture *b) {
#if defined(_WIN32)
  return a->completed&&b->completed&&a->identity.VolumeSerialNumber==b->identity.VolumeSerialNumber&&
    slcli_equal(a->identity.FileId.Identifier,b->identity.FileId.Identifier,16)&&
    a->owner_sid_bytes==b->owner_sid_bytes&&slcli_equal(a->owner_sid,b->owner_sid,a->owner_sid_bytes)&&
    a->standard.EndOfFile.QuadPart==b->standard.EndOfFile.QuadPart&&
    a->attributes.FileAttributes==b->attributes.FileAttributes&&
    a->basic.LastWriteTime.QuadPart==b->basic.LastWriteTime.QuadPart&&
    a->basic.ChangeTime.QuadPart==b->basic.ChangeTime.QuadPart&&
    a->standard.NumberOfLinks==b->standard.NumberOfLinks;
#else
  return a->original_stat.st_dev==b->original_stat.st_dev&&a->original_stat.st_ino==b->original_stat.st_ino&&
    a->original_stat.st_size==b->original_stat.st_size&&a->original_stat.st_mode==b->original_stat.st_mode&&
    a->original_stat.st_uid==b->original_stat.st_uid&&a->original_stat.st_gid==b->original_stat.st_gid&&
    a->original_stat.st_nlink==b->original_stat.st_nlink&&
    a->original_stat.st_mtime==b->original_stat.st_mtime&&a->original_stat.st_mtime_nsec==b->original_stat.st_mtime_nsec&&
    a->original_stat.st_ctime==b->original_stat.st_ctime&&a->original_stat.st_ctime_nsec==b->original_stat.st_ctime_nsec;
#endif
}
static int stable(const slcli_input_capture *a,const slcli_input_capture *b) {
#if defined(_WIN32)
  return a->completed&&b->completed&&a->identity.VolumeSerialNumber==b->identity.VolumeSerialNumber&&
    slcli_equal(a->identity.FileId.Identifier,b->identity.FileId.Identifier,16)&&
    a->owner_sid_bytes==b->owner_sid_bytes&&slcli_equal(a->owner_sid,b->owner_sid,a->owner_sid_bytes)&&
    a->standard.EndOfFile.QuadPart==b->standard.EndOfFile.QuadPart&&
    a->standard.NumberOfLinks==b->standard.NumberOfLinks&&
    a->attributes.FileAttributes==b->attributes.FileAttributes;
#else
  return same(a,b);
#endif
}
static int matches(const struct slcli_capture_record *p,const unsigned char *raw,size_t n,uint64_t offset) {
  size_t i;uint64_t at;unsigned char expected;
  for(i=0;i<n;i++) {
    at=offset+i;
    if(at<32)expected=p->header[at];
    else if(at-32<p->producer_bytes)expected=p->producer_raw[at-32];
    else {
      at-=32+p->producer_bytes;if(at>=p->query_bytes)return 0;
      if(p->query_detached)continue;expected=p->query_raw[at];
    }
    if(raw[i]!=expected)return 0;
  }
  return 1;
}
int slcli_capture_record_read_original(struct slcli_capture_record_reader *r) {
  const struct slcli_capture_record *p;const struct slcli_evidence_file *f;slcli_input_capture *c;
  struct slcli_sha256 hash;unsigned char digest[32];uint64_t got;size_t n;int ok;
  if(!r||r->used||!(p=r->producer)||!p->complete||p->failed||!p->producer_raw||
    !p->producer_bytes||p->producer_bytes>8192||p->query_bytes>65544||p->query_detached>1||
    (!p->query_detached&&!p->query_raw&&p->query_bytes)||
    (p->query_detached&&(!p->query_copy_parent||!p->query_copy_parent->complete||
      p->query_copy_parent->failed||p->query_copy_parent->producer!=p||
      p->query_copy_parent->original!=r->original||!p->query_copy_parent->before||
      !p->query_copy_parent->after))||
    !(f=p->producer_file)||!f->sealed||f->failed||!f->readback_original||
    f->count!=32+p->producer_bytes+p->query_bytes||!r->workspace||!r->captures||r->capture_count||
    !r->capture_capacity||r->capture_capacity>SIZE_MAX/sizeof(*r->captures)||
    !r->capture_sink||!r->capture_original||!deadline_ready(r)||
    !separate(r,sizeof(*r),p,sizeof(*p))||!separate(r,sizeof(*r),f,sizeof(*f))||
    !separate(r,sizeof(*r),r->captures,r->capture_capacity*sizeof(*r->captures))||
    !separate(r->workspace,sizeof(*r->workspace),p->producer_raw,p->producer_bytes)||
    (!p->query_detached&&p->query_bytes&&!separate(r->workspace,sizeof(*r->workspace),p->query_raw,p->query_bytes))||
    !separate(r->captures,r->capture_capacity*sizeof(*r->captures),p->producer_raw,p->producer_bytes)||
    (!p->query_detached&&p->query_bytes&&!separate(r->captures,r->capture_capacity*sizeof(*r->captures),p->query_raw,p->query_bytes)))return 0;
  r->used=1;r->failed=1;r->bytes=f->count;if(!slcli_workspace_take(r->workspace,r))return 0;
  c=next(r);if(!observe(r,c)||!stable(c,f->readback_original))return 0;r->before=c;
  if(p->query_detached&&!same(c,p->query_copy_parent->after))return 0;
  slcli_sha256_init(&hash);
  for(;;) {
    n=r->offset<r->bytes?(size_t)(r->bytes-r->offset):1;
    if(n>sizeof(r->workspace->readback))n=sizeof(r->workspace->readback);
    c=next(r);if(!c||!deadline_before(r,c))return 0;got=0;
#if defined(_WIN32)
    ok=slcli_windows_file_read(r->original,r->workspace->readback,(DWORD)n,r->offset,c);
    if(c->bytes_observed)got=c->returned_bytes;
#else
    ok=slcli_linux_file_read(r->original,r->workspace->readback,n,r->offset,c);
    if(c->returned_bytes_observed)got=c->returned_bytes;
#endif
    {int kept=retain(r,c,r->workspace->readback,got<=n?(size_t)got:0);
      int completed=slcli_image_deadline_after(&r->deadline_rows,c,ok&&kept&&got<=n);
      if(!kept||!completed||!ok||got>n)return 0;}
    if(r->offset==r->bytes){if(got)return 0;break;}
    if(!got||!matches(p,r->workspace->readback,(size_t)got,r->offset)||
      !slcli_sha256_update(&hash,r->workspace->readback,(size_t)got))return 0;r->offset+=got;
  }
  c=next(r);if(!observe(r,c)||!same(r->before,c)||!stable(c,f->readback_original)||
    !slcli_sha256_finish(&hash,digest)||!slcli_equal(digest,f->digest,32))return 0;
  r->after=c;if(!deadline_ready(r)||!slcli_workspace_release(r->workspace,r))return 0;
  r->complete=1;r->failed=0;return 1;
}
#endif
