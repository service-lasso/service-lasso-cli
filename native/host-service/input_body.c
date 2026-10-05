#include "input_body.h"
#if defined(__linux__)
#include <linux/fcntl.h>
#endif
#if defined(_WIN32) || defined(__linux__)
static int disjoint(const void *left,size_t n,const void *right,size_t m) {
  uintptr_t a=(uintptr_t)left,b=(uintptr_t)right;
  return left&&right&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&
           !(a<b+m&&b<a+n);
}
static int same_object(const slcli_input_capture *a,const slcli_input_capture *b,
                         uint64_t size) {
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
  if(!slcli_windows_source_observe(original,&s->captures[0])) return 0;
#else
  if(!slcli_linux_file_observe(original,&s->captures[0])||
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
    if(!slcli_windows_file_read(original,s->body+s->bytes,(DWORD)wanted,s->bytes,c)) return 0;
    if(!c->bytes_observed) return 0; count=c->returned_bytes;
#else
    if(!slcli_linux_file_read(original,s->body+s->bytes,wanted,s->bytes,c)) return 0;
    if(!c->returned_bytes_observed) return 0; count=c->returned_bytes;
#endif
    if(count>wanted||count>(size_t)expected_bytes-s->bytes) return 0;
    if(!count) {s->eof_observed=1; break;}
    s->bytes+=(size_t)count;
  }
  if(s->bytes!=expected_bytes||s->capture_count>=s->capture_capacity) return 0;
  c=&s->captures[s->capture_count++];
#if defined(_WIN32)
  if(!slcli_windows_source_observe(original,c)) return 0;
#else
  if(!slcli_linux_file_observe(original,c)||
     c->flags_status!=s->captures[0].flags_status||
     c->descriptor_status!=s->captures[0].descriptor_status) return 0;
#endif
  if(!same_object(&s->captures[0],c,expected_bytes)) return 0;
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
#endif
