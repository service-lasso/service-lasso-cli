#include "image_reader.h"
#include "image_deadline.h"
#if defined(__linux__)
#include <linux/fcntl.h>
#endif
#if defined(_WIN32) || defined(__linux__)
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static slcli_input_capture *next(struct slcli_image_reader *r) {
  slcli_input_capture *c;size_t i;if(r->capture_count>=r->capture_capacity)return 0;
  c=&r->captures[r->capture_count++];
  /* Composite constructor explicitly byte-zeroes every row. A markerless
   * original raw fact may never be replaced by a new image observation. */
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;return c;
}
static int retain(struct slcli_image_reader *r,slcli_input_capture *c,
  const unsigned char *raw,size_t bytes) {
  return c&&r->capture_sink(r->capture_original,c,raw,bytes);
}
static int observe(struct slcli_image_reader *r,slcli_input_capture *c) {
  int ok,kept;if(!c||!slcli_image_deadline_before(r,c))return 0;
#if defined(_WIN32)
  ok=slcli_windows_source_observe(r->original,c);
#else
  ok=slcli_linux_file_observe(r->original,c);
  if(ok)ok=(c->flags_status&O_ACCMODE)==O_RDONLY&&!(c->flags_status&O_PATH)&&
    (c->descriptor_status&FD_CLOEXEC)!=0;
#endif
  kept=retain(r,c,0,0);
  return slcli_image_deadline_after(r,c,ok&&kept)&&ok&&kept;
}
static uint64_t bytes(const slcli_input_capture *c) {
#if defined(_WIN32)
  return (uint64_t)c->standard.EndOfFile.QuadPart;
#else
  return (uint64_t)c->original_stat.st_size;
#endif
}
static int same(const slcli_input_capture *a,const slcli_input_capture *b) {
#if defined(_WIN32)
  return a->completed&&b->completed&&a->identity.VolumeSerialNumber==b->identity.VolumeSerialNumber&&
    slcli_equal(a->identity.FileId.Identifier,b->identity.FileId.Identifier,16)&&
    a->owner_sid_bytes==b->owner_sid_bytes&&slcli_equal(a->owner_sid,b->owner_sid,a->owner_sid_bytes)&&
    bytes(a)==bytes(b)&&a->attributes.FileAttributes==b->attributes.FileAttributes&&
    a->basic.LastWriteTime.QuadPart==b->basic.LastWriteTime.QuadPart&&
    a->basic.ChangeTime.QuadPart==b->basic.ChangeTime.QuadPart&&
    a->standard.NumberOfLinks==b->standard.NumberOfLinks;
#else
  return a->original_stat.st_dev==b->original_stat.st_dev&&a->original_stat.st_ino==b->original_stat.st_ino&&
    bytes(a)==bytes(b)&&a->original_stat.st_mode==b->original_stat.st_mode&&
    a->original_stat.st_uid==b->original_stat.st_uid&&a->original_stat.st_gid==b->original_stat.st_gid&&
    a->original_stat.st_nlink==b->original_stat.st_nlink&&
    a->original_stat.st_mtime==b->original_stat.st_mtime&&
    a->original_stat.st_mtime_nsec==b->original_stat.st_mtime_nsec&&
    a->original_stat.st_ctime==b->original_stat.st_ctime&&
    a->original_stat.st_ctime_nsec==b->original_stat.st_ctime_nsec;
#endif
}
int slcli_image_read_original(struct slcli_image_reader *r) {
  slcli_input_capture *c;struct slcli_sha256 hash;uint64_t got;size_t n;int ok;
  if(!r||r->used||r->failed||r->complete||r->offset||r->bytes||r->before||r->after||
    !r->workspace||!r->captures||r->capture_count||!r->capture_capacity||
    r->capture_capacity>SIZE_MAX/sizeof(*r->captures)||!r->capture_sink||!r->capture_original||
    !slcli_image_deadline_ready(r)||
    !separate(r,sizeof(*r),r->captures,r->capture_capacity*sizeof(*r->captures))||
    !separate(r,sizeof(*r),r->workspace,sizeof(*r->workspace))||
    !separate(r->captures,r->capture_capacity*sizeof(*r->captures),r->workspace,sizeof(*r->workspace)))return 0;
  r->used=1;r->failed=1;if(!slcli_workspace_take(r->workspace,r))return 0;
  c=next(r);if(!observe(r,c)||!bytes(c)||bytes(c)>INT64_MAX)return 0;
  r->before=c;r->bytes=bytes(c);slcli_sha256_init(&hash);
  for(;;) {
    n=r->offset<r->bytes?(size_t)(r->bytes-r->offset):1;
    if(n>sizeof(r->workspace->readback))n=sizeof(r->workspace->readback);
    c=next(r);if(!c||!slcli_image_deadline_before(r,c))return 0;got=0;
#if defined(_WIN32)
    ok=slcli_windows_file_read(r->original,r->workspace->readback,(DWORD)n,r->offset,c);
    if(c->bytes_observed)got=c->returned_bytes;
#else
    ok=slcli_linux_file_read(r->original,r->workspace->readback,n,r->offset,c);
    if(c->returned_bytes_observed)got=c->returned_bytes;
#endif
    {int kept=retain(r,c,r->workspace->readback,got<=n?(size_t)got:0);
      int completed=slcli_image_deadline_after(r,c,ok&&kept&&got<=n);
      if(!kept||!completed||!ok||got>n)return 0;}
    if(r->offset==r->bytes){if(got)return 0;break;}
    if(!got||!slcli_sha256_update(&hash,r->workspace->readback,(size_t)got))return 0;r->offset+=got;
  }
  c=next(r);if(!observe(r,c)||!same(r->before,c)||!slcli_sha256_finish(&hash,r->digest))return 0;
  r->after=c;if(!slcli_image_deadline_ready(r)||!slcli_workspace_release(r->workspace,r))return 0;
  r->complete=1;r->failed=0;return 1;
}
#endif
