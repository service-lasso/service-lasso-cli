#include "evidence_native.h"
#include "evidence_deadline.h"
#if defined(_WIN32) || defined(__linux__)
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int storage(const struct slcli_evidence_file *e) {
  return e&&!e->close_started&&slcli_evidence_deadline_ready(e)&&e->captures&&e->capture_capacity&&e->capture_capacity<=SIZE_MAX/sizeof(*e->captures)&&
    e->capture_count<=e->capture_capacity&&
    separate(e,sizeof(*e),e->captures,e->capture_capacity*sizeof(*e->captures))&&e->workspace&&
    separate(e,sizeof(*e),e->workspace,sizeof(*e->workspace))&&
    separate(e->captures,e->capture_capacity*sizeof(*e->captures),e->workspace,sizeof(*e->workspace));
}
struct slcli_evidence_file *slcli_evidence_allocate(struct slcli_arena *a,size_t count) {
  struct slcli_evidence_file *e;unsigned char *raw;size_t offset=sizeof(*e),align=_Alignof(slcli_input_capture);
  if(!a||!count||count>SIZE_MAX/sizeof(slcli_input_capture)||offset>SIZE_MAX-(align-1))return 0;
  offset=(offset+align-1)/align*align;if(count*sizeof(slcli_input_capture)>SIZE_MAX-offset)return 0;
  raw=slcli_arena_calloc(a,1,offset+count*sizeof(slcli_input_capture));if(!raw)return 0;
  e=(void *)raw;e->captures=(void *)(raw+offset);e->capture_capacity=count;
#if defined(__linux__)
  e->original=-1;
#endif
  return e;
}
static slcli_input_capture *row(struct slcli_evidence_file *e) {
  slcli_input_capture *r;size_t i;if(e->capture_count>=e->capture_capacity)return 0;
  r=&e->captures[e->capture_count++];
  for(i=0;i<sizeof(*r);i++)if(((const unsigned char *)r)[i])return 0;return r;
}
static int preserve(struct slcli_evidence_file *e,slcli_input_capture *r,const unsigned char *raw,size_t bytes) {
  return e->capture_sink(e->capture_original,r,raw,bytes);
}
static int observe(struct slcli_evidence_file *e,slcli_input_capture *r) {
  int ok,kept;if(!r||!slcli_evidence_deadline_before(e,r,e->original))return 0;
#if defined(_WIN32)
  ok=slcli_windows_file_observe(e->original,r);
#else
  ok=slcli_linux_file_observe(e->original,r);
#endif
  kept=preserve(e,r,0,0);return slcli_evidence_deadline_after(e,r,ok&&kept)&&ok&&kept;
}
static uint64_t size(const slcli_input_capture *r) {
#if defined(_WIN32)
  return (uint64_t)r->standard.EndOfFile.QuadPart;
#else
  return (uint64_t)r->original_stat.st_size;
#endif
}
static int same(const slcli_input_capture *a,const slcli_input_capture *b) {
#if defined(_WIN32)
  return a->identity.VolumeSerialNumber==b->identity.VolumeSerialNumber&&
    slcli_equal(a->identity.FileId.Identifier,b->identity.FileId.Identifier,16)&&size(a)==size(b)&&
    a->owner_sid_bytes==b->owner_sid_bytes&&slcli_equal(a->owner_sid,b->owner_sid,a->owner_sid_bytes)&&
    a->attributes.FileAttributes==b->attributes.FileAttributes&&a->mode==b->mode&&
    a->basic.LastWriteTime.QuadPart==b->basic.LastWriteTime.QuadPart&&
    a->basic.ChangeTime.QuadPart==b->basic.ChangeTime.QuadPart;
#else
  return a->original_stat.st_dev==b->original_stat.st_dev&&a->original_stat.st_ino==b->original_stat.st_ino&&
    size(a)==size(b)&&a->original_stat.st_mode==b->original_stat.st_mode&&
    a->original_stat.st_uid==b->original_stat.st_uid&&a->original_stat.st_gid==b->original_stat.st_gid&&
    a->original_stat.st_nlink==b->original_stat.st_nlink&&a->original_stat.st_mtime==b->original_stat.st_mtime&&
    a->original_stat.st_mtime_nsec==b->original_stat.st_mtime_nsec&&
    a->original_stat.st_ctime==b->original_stat.st_ctime&&a->original_stat.st_ctime_nsec==b->original_stat.st_ctime_nsec;
#endif
}
static int begin(struct slcli_evidence_file *e,slcli_input_handle original,
                          slcli_input_handle root,uint64_t maximum,slcli_store_capture_sink sink,void *owner,const struct slcli_profile *profile) {
  slcli_input_capture *r;
  if(!storage(e)||e->started||!maximum||(!profile&&maximum>8388608)||
    maximum>INT64_MAX||profile!=e->deadline_owner->bq_profile||!sink||!owner||original==root||
    original!=e->original||root!=e->root||maximum!=e->maximum||
    sink!=e->capture_sink||owner!=e->capture_original||
    e->deadline_owner->rows.deadline_owner->unborn)return 0;
  e->started=1;e->failed=1;e->original=original;e->root=root;e->maximum=maximum;
  e->capture_sink=sink;e->capture_original=owner;r=row(e);
  if(!r||!observe(e,r)||size(r))return 0;slcli_sha256_init(&e->content);e->failed=0;return 1;
}
int slcli_evidence_begin(struct slcli_evidence_file *e,slcli_input_handle original,
  slcli_input_handle root,uint64_t maximum,slcli_store_capture_sink sink,void *owner) {
  return begin(e,original,root,maximum,sink,owner,0);
}
int slcli_evidence_begin_bq(struct slcli_evidence_file *e,slcli_input_handle original,
  slcli_input_handle root,const struct slcli_profile *profile,slcli_store_capture_sink sink,void *owner) {
  if(!storage(e)||!profile||e->deadline_owner->bq_profile!=profile)return 0;
  return begin(e,original,root,e->deadline_owner->maximum,sink,owner,profile);
}
int slcli_evidence_append(void *context,const unsigned char *raw,size_t bytes) {
  struct slcli_evidence_file *e=context;slcli_input_capture *r;size_t done=0,n;uint64_t got;int ok;
  if(!storage(e)||e->deadline_owner->bq_profile||!e->started||e->failed||e->sealed||!raw||!bytes||e->count>e->maximum||
     bytes>e->maximum-e->count||!separate(e,sizeof(*e),raw,bytes)||
     !separate(e->captures,e->capture_capacity*sizeof(*e->captures),raw,bytes))return 0;
  e->failed=1;
  while(done<bytes) {
    n=bytes-done;if(n>16384)n=16384;r=row(e);
    if(!r||!slcli_evidence_deadline_before(e,r,e->original))return 0;got=0;
#if defined(_WIN32)
    ok=slcli_windows_file_write(e->original,raw+done,(DWORD)n,e->count,r);
    if(r->bytes_observed)got=r->returned_bytes;
#else
    ok=slcli_linux_file_write(e->original,raw+done,n,e->count,r);
    if(r->returned_bytes_observed)got=r->returned_bytes;
#endif
    {int kept=preserve(e,r,raw+done,n);
      int completed=slcli_evidence_deadline_after(e,r,ok&&kept&&got&&got<=n);
      if(!kept||!completed||!ok||!got||got>n||
        !slcli_sha256_update(&e->content,raw+done,(size_t)got))return 0;}
    done+=(size_t)got;e->count+=got;
  }
  e->failed=0;return 1;
}
static int flush(struct slcli_evidence_file *e,slcli_input_handle h) {
  slcli_input_capture *r=row(e);int ok,kept;
  if(!r||!slcli_evidence_deadline_before(e,r,h))return 0;
#if defined(_WIN32)
  ok=slcli_windows_file_flush(h,r);
#else
  ok=slcli_linux_file_flush(h,r);
#endif
  kept=preserve(e,r,0,0);return slcli_evidence_deadline_after(e,r,ok&&kept)&&ok&&kept;
}
static int readback(struct slcli_evidence_file *e,int recheck) {
  slcli_input_capture *before,*after,*r;uint64_t offset=0,got;size_t n;
  struct slcli_sha256 h,original;unsigned char expected[32],actual[32];int ok;
  if(!storage(e)||e->deadline_owner->bq_profile||!e->started||e->failed||!!e->sealed!=!!recheck||
     (!!e->readback_original!=!!recheck)||
     !slcli_workspace_take(e->workspace,e))return 0;e->failed=1;
  if(!flush(e,e->original))return 0;before=row(e);
  if(!before||!observe(e,before)||size(before)!=e->count||
    (recheck&&!same(e->readback_original,before)))return 0;
  original=e->content;if(!slcli_sha256_finish(&original,expected)||
    (recheck&&!slcli_equal(expected,e->digest,32)))return 0;slcli_sha256_init(&h);
  do {
    n=offset<e->count?(size_t)(e->count-offset):1;
    if(n>sizeof(e->workspace->readback))n=sizeof(e->workspace->readback);
    r=row(e);if(!r||!slcli_evidence_deadline_before(e,r,e->original))return 0;got=0;
#if defined(_WIN32)
    ok=slcli_windows_file_read(e->original,e->workspace->readback,(DWORD)n,offset,r);
    if(r->bytes_observed)got=r->returned_bytes;
#else
    ok=slcli_linux_file_read(e->original,e->workspace->readback,n,offset,r);
    if(r->returned_bytes_observed)got=r->returned_bytes;
#endif
    {int kept=preserve(e,r,e->workspace->readback,got<=n?(size_t)got:0);
      int completed=slcli_evidence_deadline_after(e,r,ok&&kept&&got<=n);
      if(!kept||!completed||!ok||got>n)return 0;}
    if(offset==e->count){if(got)return 0;break;}
    if(!got||!slcli_sha256_update(&h,e->workspace->readback,(size_t)got))return 0;offset+=got;
  } while(1);
  after=row(e);
  if(!after||!observe(e,after)||!same(before,after)||!slcli_sha256_finish(&h,actual)||
     !slcli_equal(actual,expected,32)||!flush(e,e->root))return 0;
  {unsigned i;for(i=0;i<32;i++)e->digest[i]=actual[i];}
  if(!slcli_workspace_release(e->workspace,e))return 0;
  if(!recheck)e->readback_original=after;
  e->sealed=1;e->failed=0;return 1;
}
int slcli_evidence_flush_readback(struct slcli_evidence_file *e) { return readback(e,0); }
int slcli_evidence_recheck_readback(struct slcli_evidence_file *e) { return readback(e,1); }
#endif
