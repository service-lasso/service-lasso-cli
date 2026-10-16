#include "archive_native.h"
#if defined(__linux__)
#include <linux/fcntl.h>
#endif
#if defined(_WIN32) || defined(__linux__)
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
struct slcli_archive_copy *slcli_archive_copy_allocate(struct slcli_arena *a,size_t count) {
  unsigned char *raw;struct slcli_archive_copy *c;size_t offset=sizeof(*c),align=_Alignof(slcli_input_capture);
  if(!a||!count||count>SIZE_MAX/sizeof(slcli_input_capture)||offset>SIZE_MAX-(align-1))return 0;
  offset=(offset+align-1)/align*align;
  if(count*sizeof(slcli_input_capture)>SIZE_MAX-offset)return 0;
  raw=slcli_arena_calloc(a,1,offset+count*sizeof(slcli_input_capture));if(!raw)return 0;
  c=(void *)raw;c->captures=(void *)(raw+offset);c->capture_capacity=count;return c;
}
static slcli_input_capture *row(struct slcli_archive_copy *c) {
  slcli_input_capture *r;
  if(c->capture_count>=c->capture_capacity)return 0;
  r=&c->captures[c->capture_count++];return r->operation?0:r;
}
static int preserve(struct slcli_archive_copy *c,slcli_input_capture *r,const unsigned char *body,size_t bytes) {
  return c->capture_sink(c->capture_original,r,body,bytes);
}
static int observe(struct slcli_archive_copy *c,slcli_input_handle h,slcli_input_capture *r,int source) {
  int ok;
#if defined(_WIN32)
  ok=source?slcli_windows_source_observe(h,r):slcli_windows_file_observe(h,r);
#else
  ok=slcli_linux_file_observe(h,r);
  if(ok&&source)ok=(r->flags_status&O_ACCMODE)==O_RDONLY&&!(r->flags_status&O_PATH)&&
    (r->descriptor_status&FD_CLOEXEC)!=0;
#endif
  return preserve(c,r,0,0)&&ok;
}
static uint64_t bytes(const slcli_input_capture *r) {
#if defined(_WIN32)
  return (uint64_t)r->standard.EndOfFile.QuadPart;
#else
  return (uint64_t)r->original_stat.st_size;
#endif
}
static int identity(const slcli_input_capture *a,const slcli_input_capture *b) {
#if defined(_WIN32)
  return a->identity.VolumeSerialNumber==b->identity.VolumeSerialNumber&&
    slcli_equal(a->identity.FileId.Identifier,b->identity.FileId.Identifier,16);
#else
  return a->original_stat.st_dev==b->original_stat.st_dev&&a->original_stat.st_ino==b->original_stat.st_ino;
#endif
}
static int same(const slcli_input_capture *a,const slcli_input_capture *b) {
  if(!identity(a,b)||bytes(a)!=bytes(b))return 0;
#if defined(_WIN32)
  return a->owner_sid_bytes==b->owner_sid_bytes&&slcli_equal(a->owner_sid,b->owner_sid,a->owner_sid_bytes)&&
    a->attributes.FileAttributes==b->attributes.FileAttributes&&a->mode==b->mode&&
    a->basic.LastWriteTime.QuadPart==b->basic.LastWriteTime.QuadPart&&
    a->basic.ChangeTime.QuadPart==b->basic.ChangeTime.QuadPart;
#else
  return a->original_stat.st_mode==b->original_stat.st_mode&&a->original_stat.st_uid==b->original_stat.st_uid&&
    a->original_stat.st_gid==b->original_stat.st_gid&&a->original_stat.st_nlink==b->original_stat.st_nlink&&
    a->original_stat.st_mtime==b->original_stat.st_mtime&&a->original_stat.st_mtime_nsec==b->original_stat.st_mtime_nsec&&
    a->original_stat.st_ctime==b->original_stat.st_ctime&&a->original_stat.st_ctime_nsec==b->original_stat.st_ctime_nsec;
#endif
}
static int archive_read(struct slcli_archive_copy *c,slcli_input_handle h,unsigned char *data,
                  size_t n,uint64_t offset,slcli_input_capture *r,size_t *actual) {
  int ok;uint64_t got=0;
#if defined(_WIN32)
  ok=slcli_windows_file_read(h,data,(DWORD)n,offset,r);
  if(r->bytes_observed)got=r->returned_bytes;
#else
  ok=slcli_linux_file_read(h,data,n,offset,r);
  if(r->returned_bytes_observed)got=r->returned_bytes;
#endif
  if(!preserve(c,r,data,got<=n?(size_t)got:0)||!ok||got>n)return 0;
  *actual=(size_t)got;return 1;
}
static int archive_write(struct slcli_archive_copy *c,const unsigned char *data,size_t n,
                   uint64_t offset,slcli_input_capture *r,size_t *actual) {
  int ok;uint64_t got=0;
#if defined(_WIN32)
  ok=slcli_windows_file_write(c->output,data,(DWORD)n,offset,r);
  if(r->bytes_observed)got=r->returned_bytes;
#else
  ok=slcli_linux_file_write(c->output,data,n,offset,r);
  if(r->returned_bytes_observed)got=r->returned_bytes;
#endif
  if(!preserve(c,r,data,n)||!ok||!got||got>n)return 0;*actual=(size_t)got;return 1;
}
static int flush(struct slcli_archive_copy *c,slcli_input_handle h,slcli_input_capture *r) {
  int ok;
#if defined(_WIN32)
  ok=slcli_windows_file_flush(h,r);
#else
  ok=slcli_linux_file_flush(h,r);
#endif
  return preserve(c,r,0,0)&&ok;
}
int slcli_archive_copy_original(struct slcli_archive_copy *c,slcli_input_handle source,
                                 slcli_input_handle output,slcli_input_handle root,uint64_t expected,
                                 const unsigned char expected_hash[32],slcli_store_capture_sink sink,void *owner) {
  slcli_input_capture *source_before,*output_before,*output_written,*r;
  size_t n,got,done,written,compare;struct slcli_sha256 h;unsigned char digest[32];
  if(!c||c->started||!c->captures||!c->capture_capacity||!expected_hash||!sink||!owner||!c->workspace||
     c->capture_capacity>SIZE_MAX/sizeof(*c->captures)||expected>8388608||
     source==output||source==root||output==root||
     !separate(c,sizeof(*c),c->captures,c->capture_capacity*sizeof(*c->captures))||
     !separate(c,sizeof(*c),c->workspace,sizeof(*c->workspace))||
     !separate(c->captures,c->capture_capacity*sizeof(*c->captures),c->workspace,sizeof(*c->workspace))||
     !separate(c->workspace,sizeof(*c->workspace),expected_hash,32)||
     !separate(c,sizeof(*c),expected_hash,32)||
     !separate(c->captures,c->capture_capacity*sizeof(*c->captures),expected_hash,32))return 0;
  if(!slcli_workspace_take(c->workspace,c))return 0;
  c->started=1;c->failed=1;c->source=source;c->output=output;c->root=root;c->expected=expected;
  c->capture_sink=sink;c->capture_original=owner;
  source_before=row(c);output_before=row(c);
  if(!source_before||!output_before||!observe(c,source,source_before,1)||bytes(source_before)!=expected||
     !observe(c,output,output_before,0)||bytes(output_before)||identity(source_before,output_before))return 0;
  slcli_sha256_init(&h);
  while(c->offset<expected) {
    n=(size_t)(expected-c->offset);if(n>sizeof(c->workspace->input))n=sizeof(c->workspace->input);r=row(c);
    if(!r||!archive_read(c,source,c->workspace->input,n,c->offset,r,&got)||!got||
       !slcli_sha256_update(&h,c->workspace->input,got))return 0;
    done=0;
    while(done<got) {
      r=row(c);if(!r||!archive_write(c,c->workspace->input+done,got-done,c->offset+done,r,&written))return 0;
      done+=written;
    }
    compare=0;
    while(compare<got) {
      r=row(c);if(!r||!archive_read(c,output,c->workspace->readback,got-compare,c->offset+compare,r,&written)||!written||
        !slcli_equal(c->workspace->input+compare,c->workspace->readback,written))return 0;compare+=written;
    }
    c->offset+=got;
  }
  r=row(c);if(!r||!archive_read(c,source,c->workspace->input,1,expected,r,&got)||got)return 0;
  r=row(c);if(!r||!observe(c,source,r,1)||!same(source_before,r))return 0;
  if(!slcli_sha256_finish(&h,c->digest)||!slcli_equal(c->digest,expected_hash,32))return 0;
  r=row(c);if(!r||!flush(c,output,r))return 0;
  output_written=row(c);if(!output_written||!observe(c,output,output_written,0)||
     bytes(output_written)!=expected||!identity(output_before,output_written))return 0;
  slcli_sha256_init(&h);c->offset=0;
  while(c->offset<expected) {
    n=(size_t)(expected-c->offset);if(n>sizeof(c->workspace->readback))n=sizeof(c->workspace->readback);r=row(c);
    if(!r||!archive_read(c,output,c->workspace->readback,n,c->offset,r,&got)||!got||
       !slcli_sha256_update(&h,c->workspace->readback,got))return 0;c->offset+=got;
  }
  r=row(c);if(!r||!archive_read(c,output,c->workspace->readback,1,expected,r,&got)||got)return 0;
  r=row(c);if(!r||!observe(c,output,r,0)||!same(output_written,r)||
     !slcli_sha256_finish(&h,digest)||!slcli_equal(digest,c->digest,32))return 0;
  r=row(c);if(!r||!flush(c,root,r))return 0;
  if(!slcli_workspace_release(c->workspace,c))return 0;
  c->complete=1;c->failed=0;return 1;
}
#endif
