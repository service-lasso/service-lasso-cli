#include "input_body.h"
#include "input_deadline.h"
#include "unit19.h"
#if defined(__linux__)
#include <linux/fcntl.h>
#endif
#if defined(_WIN32) || defined(__linux__)
_Static_assert(_Alignof(struct slcli_input_body)<=16,
               "Original arena must align input control storage");
_Static_assert(_Alignof(slcli_input_capture)<=16,
               "Original arena must align native capture storage");
static struct slcli_input_body *allocate_storage(struct slcli_arena *arena,
  uint64_t expected,size_t rows,uint64_t maximum) {
  size_t capacity,offset,capture_bytes,total,alignment;
  unsigned char *block;
  struct slcli_input_body *body;
  if(!arena||!expected||expected>maximum||expected>1048576||
     rows<2*((expected+16383)/16384+3)||rows>2*(expected+3)||
     rows>SIZE_MAX/sizeof(slcli_input_capture)) return 0;
  capacity=(size_t)expected+1; /* Original extra byte for actual EOF observation. */
  if(capacity>(SIZE_MAX-sizeof(*body))/2) return 0;
  offset=sizeof(*body)+2*capacity;
  alignment=_Alignof(slcli_input_capture);
  if(offset>SIZE_MAX-(alignment-1)) return 0;
  offset=(offset+alignment-1)&~(alignment-1);
  capture_bytes=rows*sizeof(slcli_input_capture);
  if(capture_bytes>SIZE_MAX-offset) return 0;
  total=offset+capture_bytes;
  /* calloc walks only this actual live arena block, whose header, padding,
   * body, control and all native output rows remain in the existing extent
   * reservation. No native operation starts before storage is fully owned. */
  block=slcli_arena_calloc(arena,1,total);
  if(!block) return 0;
  body=(struct slcli_input_body *)block;
  body->body=block+sizeof(*body); body->capacity=capacity;
  body->readback=body->body+capacity; body->readback_capacity=capacity;
  body->captures=(slcli_input_capture *)(block+offset);
  body->capture_capacity=rows;
  return body;
}
struct slcli_input_body *slcli_input_body_allocate(struct slcli_arena *arena,
  uint64_t expected,size_t rows) {
  return allocate_storage(arena,expected,rows,262144);
}
struct slcli_input_body *slcli_input_ha2_member_storage_allocate(struct slcli_arena *arena,
  uint32_t ordinal,size_t rows) {
  uint64_t expected=slcli_unit19_derive_member_limit(ordinal);
  return allocate_storage(arena,expected,rows,1048576);
}
static int disjoint(const void *left,size_t n,const void *right,size_t m) {
  uintptr_t a=(uintptr_t)left,b=(uintptr_t)right;
  return left&&right&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&
           !(a<b+m&&b<a+n);
}
static int observe_input(struct slcli_input_body *b,slcli_input_handle h,slcli_input_capture *c) {
  int ok;
  if(!slcli_input_deadline_before(b,c))return 0;
#if defined(_WIN32)
  ok=slcli_windows_source_observe(h,c);
#else
  ok=slcli_linux_file_observe(h,c);
#endif
  return slcli_input_deadline_after(b,c,0,0,ok);
}
static int read_input(struct slcli_input_body *b,slcli_input_handle h,unsigned char *raw,
  size_t wanted,uint64_t offset,slcli_input_capture *c) {
  int ok;
  if(!slcli_input_deadline_before(b,c))return 0;
#if defined(_WIN32)
  ok=slcli_windows_file_read(h,raw,(DWORD)wanted,offset,c);
#else
  ok=slcli_linux_file_read(h,raw,wanted,offset,c);
#endif
  return slcli_input_deadline_after(b,c,raw,wanted,ok);
}
int slcli_input_original_same(const slcli_input_capture *a,const slcli_input_capture *b,
                         uint64_t size) {
  if(!a||!b)return 0;
#if defined(_WIN32)
  size_t i;
  if(!a->completed||!b->completed||
     a->identity.VolumeSerialNumber!=b->identity.VolumeSerialNumber||
     a->standard.EndOfFile.QuadPart<0||b->standard.EndOfFile.QuadPart<0||
     (uint64_t)a->standard.EndOfFile.QuadPart!=size||
     (uint64_t)b->standard.EndOfFile.QuadPart!=size||
     a->owner_sid_bytes!=b->owner_sid_bytes||!a->owner_sid_bytes||
     a->basic.ChangeTime.QuadPart!=b->basic.ChangeTime.QuadPart||
     a->basic.LastWriteTime.QuadPart!=b->basic.LastWriteTime.QuadPart||
     a->basic.FileAttributes!=b->basic.FileAttributes||
     a->standard.NumberOfLinks!=b->standard.NumberOfLinks) return 0;
  if(a->access_raw.GrantedAccess!=b->access_raw.GrantedAccess||
     a->handle_flags!=b->handle_flags||a->mode!=b->mode) return 0;
  for(i=0;i<16;++i) if(a->identity.FileId.Identifier[i]!=b->identity.FileId.Identifier[i]) return 0;
  for(i=0;i<a->owner_sid_bytes;++i) if(a->owner_sid[i]!=b->owner_sid[i]) return 0;
  return 1;
#else
  return a->stat_observed&&b->stat_observed&&a->stat_status==0&&b->stat_status==0&&
    a->flags_observed&&b->flags_observed&&
    a->descriptor_observed&&b->descriptor_observed&&
    a->flags_status==b->flags_status&&a->descriptor_status==b->descriptor_status&&
    a->original_stat.st_dev==b->original_stat.st_dev&&
    a->original_stat.st_ino==b->original_stat.st_ino&&
    a->original_stat.st_uid==b->original_stat.st_uid&&
    a->original_stat.st_gid==b->original_stat.st_gid&&
    a->original_stat.st_mode==b->original_stat.st_mode&&
    a->original_stat.st_nlink==b->original_stat.st_nlink&&
    a->original_stat.st_size>=0&&b->original_stat.st_size>=0&&
    (uint64_t)a->original_stat.st_size==size&&
    (uint64_t)b->original_stat.st_size==size&&
    a->original_stat.st_mtime==b->original_stat.st_mtime&&
    a->original_stat.st_mtime_nsec==b->original_stat.st_mtime_nsec&&
    a->original_stat.st_ctime==b->original_stat.st_ctime&&
    a->original_stat.st_ctime_nsec==b->original_stat.st_ctime_nsec;
#endif
}
int slcli_input_body_read(struct slcli_input_body *s,slcli_input_handle original,
                          uint64_t expected_bytes,const unsigned char expected[32]) {
  size_t captures_bytes,wanted; uint64_t count; slcli_input_capture *c;
  struct slcli_sha256 hash;
  if(!s||s->started||!expected||!s->body||!s->captures||
     !expected_bytes||expected_bytes>262144||expected_bytes>=s->capacity||
     s->capture_capacity>SIZE_MAX/sizeof(*s->captures)||
     (uintptr_t)s->captures%_Alignof(slcli_input_capture)) return 0;
  captures_bytes=s->capture_capacity*sizeof(*s->captures);
  if(s->capture_capacity<(expected_bytes+16383)/16384+3||
     !disjoint(s,sizeof(*s),s->body,s->capacity)||
     !disjoint(s,sizeof(*s),s->captures,captures_bytes)||
     !disjoint(s->body,s->capacity,s->captures,captures_bytes)||
     !disjoint(expected,32,s->body,s->capacity)||
     !disjoint(expected,32,s->captures,captures_bytes)||
     !disjoint(expected,32,s,sizeof(*s))) return 0;
  s->started=1; s->failed=1; s->original=original; s->bytes=0;
  s->complete=0; s->eof_observed=0; s->capture_count=1;
#if defined(_WIN32)
  /* Captures must originate in zeroed owner storage. Windows mode/identity
   * query preserves pending native output; no row is reused in this attempt.
   * Actual granted read-only rights come from original NtQueryObject plus
   * GetHandleInformation/GetFileType captures, never inferred from ReadFile. */
  if(!observe_input(s,original,&s->captures[0])) return 0;
#else
  if(!observe_input(s,original,&s->captures[0])||
     (s->captures[0].flags_status&O_ACCMODE)!=O_RDONLY||
     (s->captures[0].flags_status&O_PATH)||
     !(s->captures[0].descriptor_status&FD_CLOEXEC)) return 0;
#endif
  for(;;) {
    if(s->capture_count>=s->capture_capacity) return 0;
    c=&s->captures[s->capture_count++];
    wanted=s->bytes<(size_t)expected_bytes?(size_t)expected_bytes-s->bytes:1;
    if(wanted>16384) wanted=16384;
#if defined(_WIN32)
    if(!read_input(s,original,s->body+s->bytes,wanted,s->bytes,c)) return 0;
    if(!c->bytes_observed) return 0; count=c->returned_bytes;
#else
    if(!read_input(s,original,s->body+s->bytes,wanted,s->bytes,c)) return 0;
    if(!c->returned_bytes_observed) return 0; count=c->returned_bytes;
#endif
    if(count>wanted||count>(size_t)expected_bytes-s->bytes) return 0;
    if(!count) {s->eof_observed=1; break;}
    s->bytes+=(size_t)count;
  }
  if(s->bytes!=expected_bytes||s->capture_count>=s->capture_capacity) return 0;
  c=&s->captures[s->capture_count++];
#if defined(_WIN32)
  if(!observe_input(s,original,c)) return 0;
#else
  if(!observe_input(s,original,c)||
     c->flags_status!=s->captures[0].flags_status||
     c->descriptor_status!=s->captures[0].descriptor_status) return 0;
#endif
  if(!slcli_input_original_same(&s->captures[0],c,expected_bytes)) return 0;
  slcli_sha256_init(&hash);
  if(!slcli_sha256_update(&hash,s->body,s->bytes)||
     !slcli_sha256_finish(&hash,s->digest)) return 0;
  if(!slcli_equal(s->digest,expected,32)) return 0;
  s->complete=1; s->failed=0;
  /* Complete means actual held-file EOF/hash/correlation only. All captures,
   * returned raw bytes and original handle remain owned through independent
   * source admission and original archive persistence/readback. */
  return 1;
}
int slcli_input_body_recheck(struct slcli_input_body *s) {
  size_t offset=0,wanted,i,capture_bytes; uint64_t count;
  slcli_input_capture *before,*after,*c;
  struct slcli_sha256 hash; unsigned char digest[32];
  if(!s||!s->started||!s->complete||s->failed||!s->eof_observed||
     s->recheck_started||!s->body||!s->captures||!s->bytes||
     s->bytes>262144||s->bytes>=s->capacity||!s->readback||
     s->bytes>=s->readback_capacity||!s->capture_count||
     s->capture_count>s->capture_capacity||
     s->capture_capacity>SIZE_MAX/sizeof(*s->captures)||
     (uintptr_t)s->captures%_Alignof(slcli_input_capture)) return 0;
  capture_bytes=s->capture_capacity*sizeof(*s->captures);
  if(!disjoint(s,sizeof(*s),s->body,s->capacity)||
     !disjoint(s,sizeof(*s),s->captures,capture_bytes)||
     !disjoint(s->body,s->capacity,s->captures,capture_bytes)||
     !disjoint(s,sizeof(*s),s->readback,s->readback_capacity)||
     !disjoint(s->body,s->capacity,s->readback,s->readback_capacity)||
     !disjoint(s->readback,s->readback_capacity,s->captures,capture_bytes)) return 0;
  /* Reserve every prospective capture before the first native effect. Short
   * reads may exhaust earlier: they retain custody instead of reusing a row. */
  s->recheck_started=1; s->failed=1;
  if(s->capture_capacity-s->capture_count<(s->bytes+16383)/16384+3) return 0;
  before=&s->captures[s->capture_count++];
#if defined(_WIN32)
  if(!observe_input(s,s->original,before)) return 0;
#else
  if(!observe_input(s,s->original,before)) return 0;
#endif
  if(!slcli_input_original_same(&s->captures[0],before,s->bytes)) return 0;
  /* Saved body bytes are also rehashed; unchanged native metadata cannot turn
   * a caller-mutated saved body or a changed held file into the original. */
  slcli_sha256_init(&hash);
  if(!slcli_sha256_update(&hash,s->body,s->bytes)||
     !slcli_sha256_finish(&hash,digest)||!slcli_equal(digest,s->digest,32)) return 0;
  for(;;) {
    if(s->capture_count>=s->capture_capacity) return 0;
    c=&s->captures[s->capture_count++];
    wanted=offset<s->bytes?s->bytes-offset:1;
    if(wanted>16384) wanted=16384;
#if defined(_WIN32)
    if(!read_input(s,s->original,s->readback+offset,wanted,offset,c)||
       !c->bytes_observed) return 0;
#else
    if(!read_input(s,s->original,s->readback+offset,wanted,offset,c)||
       !c->returned_bytes_observed) return 0;
#endif
    count=c->returned_bytes;
    if(count>wanted||count>s->bytes-offset) return 0;
    if(!count) { if(offset!=s->bytes) return 0; break; }
    for(i=0;i<(size_t)count;++i)
      if(s->readback[offset+i]!=s->body[offset+i]) return 0;
    offset+=(size_t)count;
    /* Every original returned byte remains in this separate precharged copy.
     * Neither scratch nor capture rows are reused, including unknown Windows
     * completion, changed bytes or later EOF/identity failure. */
  }
  if(s->capture_count>=s->capture_capacity) return 0;
  after=&s->captures[s->capture_count++];
#if defined(_WIN32)
  if(!observe_input(s,s->original,after)) return 0;
#else
  if(!observe_input(s,s->original,after)) return 0;
#endif
  if(!slcli_input_original_same(before,after,s->bytes)) return 0;
  s->recheck_complete=1; s->failed=0; return 1;
}
#endif
