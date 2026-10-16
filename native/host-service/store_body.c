#include "store_body.h"
#include "ha2_journal_codec.h"
#if defined(_WIN32) || defined(__linux__)
static int disjoint(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int storage(const struct slcli_store_stream *s) {
  return s&&((!s->capture_before&&!s->before_original)||(s->capture_before&&s->before_original))&&
    ((!s->capture_after&&!s->after_original)||(s->capture_after&&s->after_original))&&
    s->captures&&s->capture_capacity&&
    s->capture_capacity<=SIZE_MAX/sizeof(*s->captures)&&
    s->capture_count<=s->capture_capacity&&
    s->buffered<=(s->unit_geometry?16384:1024)&&
    disjoint(s,sizeof(*s),s->captures,s->capture_capacity*sizeof(*s->captures))&&
    ((!s->unit_geometry&&!s->unit_chunk&&!s->unit_readback)||
      (s->unit_geometry==1&&s->unit_chunk&&s->unit_readback&&
       disjoint(s,sizeof(*s),s->unit_chunk,16384)&&
       disjoint(s,sizeof(*s),s->unit_readback,16385)&&
       disjoint(s->captures,s->capture_capacity*sizeof(*s->captures),s->unit_chunk,16384)&&
       disjoint(s->captures,s->capture_capacity*sizeof(*s->captures),s->unit_readback,16385)&&
       disjoint(s->unit_chunk,16384,s->unit_readback,16385)));
}
static unsigned char *chunk(struct slcli_store_stream *s) {
  return s->unit_geometry?s->unit_chunk:s->chunk;
}
static unsigned char *readback(struct slcli_store_stream *s) {
  return s->unit_geometry?s->unit_readback:s->readback;
}
static size_t chunk_capacity(const struct slcli_store_stream *s) {
  return s->unit_geometry?16384:sizeof(s->chunk);
}
struct slcli_store_stream *slcli_store_stream_allocate(struct slcli_arena *a,size_t count) {
  unsigned char *raw;struct slcli_store_stream *s;size_t offset=sizeof(*s),align=_Alignof(slcli_input_capture);
  if(!a||!count||count>SIZE_MAX/sizeof(slcli_input_capture)||offset>SIZE_MAX-(align-1))return 0;
  offset=(offset+align-1)/align*align;
  if(count*sizeof(slcli_input_capture)>SIZE_MAX-offset)return 0;
  raw=slcli_arena_calloc(a,1,offset+count*sizeof(slcli_input_capture));if(!raw)return 0;
  s=(void *)raw;s->captures=(void *)(raw+offset);s->capture_capacity=count;return s;
}
struct slcli_store_stream *slcli_store_unit19_allocate(struct slcli_arena *a,size_t count) {
  unsigned char *raw;struct slcli_store_stream *s;
  size_t offset=sizeof(*s),alignment=_Alignof(slcli_input_capture),captures,total;
  if(!a||!count||count>SIZE_MAX/sizeof(slcli_input_capture)||offset>SIZE_MAX-(alignment-1))return 0;
  offset=(offset+alignment-1)/alignment*alignment;captures=count*sizeof(slcli_input_capture);
  if(captures>SIZE_MAX-offset||offset+captures>SIZE_MAX-32769)return 0;
  total=offset+captures+32769;raw=slcli_arena_calloc(a,1,total);if(!raw)return 0;
  s=(void *)raw;s->captures=(void *)(raw+offset);s->capture_capacity=count;
  s->unit_chunk=raw+offset+captures;s->unit_readback=s->unit_chunk+16384;
  s->unit_geometry=1;return s;
}
static slcli_input_capture *row(struct slcli_store_stream *s) {
  slcli_input_capture *c;
  if(s->capture_count>=s->capture_capacity){s->failed=1;return 0;}
  c=&s->captures[s->capture_count++];if(c->operation){s->failed=1;return 0;}
  if(s->capture_before&&!s->capture_before(s->before_original,s,c)){s->failed=1;return 0;}return c;
}
static int observe(slcli_input_handle h,slcli_input_capture *c) {
#if defined(_WIN32)
  return slcli_windows_file_observe(h,c);
#else
  return slcli_linux_file_observe(h,c);
#endif
}
static uint64_t size(const slcli_input_capture *c) {
#if defined(_WIN32)
  return (uint64_t)c->standard.EndOfFile.QuadPart;
#else
  return (uint64_t)c->original_stat.st_size;
#endif
}
static int same(const slcli_input_capture *a,const slcli_input_capture *b) {
#if defined(_WIN32)
  return a->identity.VolumeSerialNumber==b->identity.VolumeSerialNumber&&
    slcli_equal(a->identity.FileId.Identifier,b->identity.FileId.Identifier,16)&&
    a->owner_sid_bytes==b->owner_sid_bytes&&slcli_equal(a->owner_sid,b->owner_sid,a->owner_sid_bytes)&&
    a->attributes.FileAttributes==b->attributes.FileAttributes&&a->mode==b->mode&&
    a->basic.LastWriteTime.QuadPart==b->basic.LastWriteTime.QuadPart&&
    a->basic.ChangeTime.QuadPart==b->basic.ChangeTime.QuadPart&&size(a)==size(b);
#else
  return a->original_stat.st_dev==b->original_stat.st_dev&&a->original_stat.st_ino==b->original_stat.st_ino&&
    a->original_stat.st_mode==b->original_stat.st_mode&&a->original_stat.st_uid==b->original_stat.st_uid&&
    a->original_stat.st_gid==b->original_stat.st_gid&&a->original_stat.st_nlink==b->original_stat.st_nlink&&
    a->original_stat.st_mtime==b->original_stat.st_mtime&&a->original_stat.st_mtime_nsec==b->original_stat.st_mtime_nsec&&
    a->original_stat.st_ctime==b->original_stat.st_ctime&&a->original_stat.st_ctime_nsec==b->original_stat.st_ctime_nsec&&
    a->flags_status==b->flags_status&&a->descriptor_status==b->descriptor_status&&size(a)==size(b);
#endif
}
/* Writes change length and timestamps, but never the held object's identity,
 * owner, access mode, link count or descriptor custody. */
int slcli_store_original_same(const slcli_input_capture *a,const slcli_input_capture *b) {
  if(!a||!b)return 0;
#if defined(_WIN32)
  return a->completed&&b->completed&&a->owner_sid_bytes&&
    a->identity.VolumeSerialNumber==b->identity.VolumeSerialNumber&&
    slcli_equal(a->identity.FileId.Identifier,b->identity.FileId.Identifier,16)&&
    a->owner_sid_bytes==b->owner_sid_bytes&&slcli_equal(a->owner_sid,b->owner_sid,a->owner_sid_bytes)&&
    a->attributes.FileAttributes==b->attributes.FileAttributes&&a->mode==b->mode&&
    a->access_raw.GrantedAccess==b->access_raw.GrantedAccess&&
    a->handle_flags==b->handle_flags&&a->standard.NumberOfLinks==b->standard.NumberOfLinks;
#else
  return a->stat_observed&&b->stat_observed&&a->stat_status==0&&b->stat_status==0&&
    a->flags_observed&&b->flags_observed&&a->descriptor_observed&&b->descriptor_observed&&
    a->original_stat.st_dev==b->original_stat.st_dev&&a->original_stat.st_ino==b->original_stat.st_ino&&
    a->original_stat.st_mode==b->original_stat.st_mode&&a->original_stat.st_uid==b->original_stat.st_uid&&
    a->original_stat.st_gid==b->original_stat.st_gid&&a->original_stat.st_nlink==b->original_stat.st_nlink&&
    a->flags_status==b->flags_status&&a->descriptor_status==b->descriptor_status;
#endif
}
static int preserve(struct slcli_store_stream *s,slcli_input_capture *c,const unsigned char *raw,size_t n,int native_ok) {
  if(!s->capture_sink(s->capture_original,c,raw,n)){s->failed=1;return 0;}
  if(s->capture_after&&!s->capture_after(s->after_original,c,raw,n,native_ok)){s->failed=1;return 0;}return 1;
}
static int begin_range(struct slcli_store_stream *s,slcli_input_handle original,
  uint32_t mode,uint64_t expected,uint64_t prior,uint64_t maximum,
  slcli_store_capture_sink sink,void *owner) {
  slcli_input_capture *c;
  if(!storage(s)||s->attempted||s->failed||mode>1||!expected||
     expected>maximum||prior>=expected||(mode&&prior)||!sink||!owner)return 0;
#if defined(_WIN32)
  if(!original||original==INVALID_HANDLE_VALUE)return 0;
#else
  if(original<0)return 0;
#endif
  s->attempted=1;s->failed=1;s->original=original;s->mode=mode;s->expected=expected;
  s->offset=prior;
  s->capture_sink=sink;s->capture_original=owner;slcli_sha256_init(&s->bytes_hash);
  c=row(s);if(!c)return 0;
  if(!observe(original,c)){preserve(s,c,0,0,0);return 0;}
  if(!preserve(s,c,0,0,1)||size(c)!=(mode?expected:prior))return 0;
  s->failed=0;return 1;
}
int slcli_store_stream_begin(struct slcli_store_stream *s,slcli_input_handle original,
  uint32_t mode,uint64_t expected,slcli_store_capture_sink sink,void *owner) {
  if(!s||s->unit_geometry)return 0;
  return begin_range(s,original,mode,expected,0,SLCLI_LEDGER_LIMIT+36,sink,owner);
}
int slcli_store_stream_append_begin(struct slcli_store_stream *s,slcli_input_handle original,
  uint64_t prior,uint64_t expected,slcli_store_capture_sink sink,void *owner) {
  if(!s||s->unit_geometry)return 0;
  return begin_range(s,original,0,expected,prior,SLCLI_LEDGER_LIMIT+36,sink,owner);
}
int slcli_store_journal_compare_begin(struct slcli_store_stream *s,slcli_input_handle original,
  uint64_t expected,slcli_store_capture_sink sink,void *owner) {
  if(!s||s->unit_geometry)return 0;
  if(!expected||expected%SLCLI_HA2_JOURNAL_BYTES||expected>SLCLI_HA2_JOURNAL_FILE_BYTES)return 0;
  return begin_range(s,original,1,expected,0,SLCLI_HA2_JOURNAL_FILE_BYTES,sink,owner);
}
int slcli_store_journal_append_begin(struct slcli_store_stream *s,slcli_input_handle original,
  uint64_t prior,uint64_t expected,slcli_store_capture_sink sink,void *owner) {
  if(!s||s->unit_geometry)return 0;
  if(prior%SLCLI_HA2_JOURNAL_BYTES||prior>=SLCLI_HA2_JOURNAL_FILE_BYTES||
     expected!=prior+SLCLI_HA2_JOURNAL_BYTES)return 0;
  return begin_range(s,original,0,expected,prior,SLCLI_HA2_JOURNAL_FILE_BYTES,sink,owner);
}
int slcli_store_unit19_begin(struct slcli_store_stream *s,slcli_input_handle original,
  uint32_t mode,const struct slcli_unit19_reader *r,slcli_store_capture_sink sink,void *owner) {
  uint64_t maximum=4194304;
  if(!s||s->unit_geometry!=1||!r||!r->started||r->failed||!r->complete||r->next!=r->count||
     r->type<SLCLI_ROLE19||r->type>SLCLI_HA2_COMPOUND19||
     r->next_offset!=r->file_bytes||r->file_bytes>4194304)return 0;
  if(r->type==SLCLI_HA2_CERT19||r->type==SLCLI_CONSUMER_CERT19)maximum=33088;
  if(r->type==SLCLI_HA2_DERIVE19)maximum=1951296;
  if(r->type==SLCLI_HA2_COMPOUND19)maximum=320+32+16384;
  return begin_range(s,original,mode,r->file_bytes,0,maximum,sink,owner);
}
static int flush_chunk(struct slcli_store_stream *s) {
  slcli_input_capture *c;size_t done=0,remaining;uint64_t n;int ok;
  while(done<s->buffered) {
    remaining=s->buffered-done;c=row(s);if(!c)return 0;
    if(s->mode) {
#if defined(_WIN32)
      ok=slcli_windows_file_read(s->original,readback(s),(DWORD)remaining,s->offset,c);
#else
      ok=slcli_linux_file_read(s->original,readback(s),remaining,s->offset,c);
#endif
    } else {
#if defined(_WIN32)
      ok=slcli_windows_file_write(s->original,chunk(s)+done,(DWORD)remaining,s->offset,c);
#else
      ok=slcli_linux_file_write(s->original,chunk(s)+done,remaining,s->offset,c);
#endif
    }
#if defined(_WIN32)
    n=c->bytes_observed?c->returned_bytes:0;
#else
    n=c->returned_bytes_observed?c->returned_bytes:0;
#endif
    /* Preserve failed/unresolved ABI rows too. No callback receives an invented
     * zero-byte success body when the native count was never observed. */
    if(!preserve(s,c,s->mode?readback(s):chunk(s)+done,
                  s->mode?(n<=remaining?(size_t)n:0):remaining,ok))return 0;
    if(!ok||!n||n>remaining||(s->mode&&!slcli_equal(chunk(s)+done,readback(s),(size_t)n))) {
      s->failed=1;return 0;
    }
    if(!slcli_sha256_update(&s->bytes_hash,chunk(s)+done,(size_t)n)){s->failed=1;return 0;}
    done+=(size_t)n;s->offset+=n;
  }
  s->buffered=0;return 1;
}
int slcli_store_stream_append(void *context,const unsigned char *raw,size_t bytes) {
  struct slcli_store_stream *s=context;size_t i=0,n,j;
  if(!storage(s)||!s->attempted||s->failed||!raw||!bytes||s->offset>s->expected||
     s->buffered>s->expected-s->offset||bytes>s->expected-s->offset-s->buffered||
     !disjoint(raw,bytes,s,sizeof(*s))||
     !disjoint(raw,bytes,s->captures,s->capture_capacity*sizeof(*s->captures))||
     (s->unit_geometry&&(!disjoint(raw,bytes,s->unit_chunk,16384)||
       !disjoint(raw,bytes,s->unit_readback,16385))))return 0;
  while(i<bytes) {
    n=chunk_capacity(s)-s->buffered;if(n>bytes-i)n=bytes-i;
    for(j=0;j<n;j++)chunk(s)[s->buffered+j]=raw[i+j];s->buffered+=n;i+=n;
    if(s->buffered==chunk_capacity(s)&&!flush_chunk(s))return 0;
  }
  return 1;
}
int slcli_store_stream_finish(struct slcli_store_stream *s) {
  slcli_input_capture *c;int ok;
  if(!storage(s)||!s->attempted||s->failed||s->held_verified||
     !flush_chunk(s)||s->offset!=s->expected)return 0;
  if(!s->mode) {
    c=row(s);if(!c)return 0;
#if defined(_WIN32)
    ok=slcli_windows_file_flush(s->original,c);
#else
    ok=slcli_linux_file_flush(s->original,c);
#endif
    if(!preserve(s,c,0,0,ok)||!ok){s->failed=1;return 0;}
  } else {
    c=row(s);if(!c)return 0;
#if defined(_WIN32)
    ok=slcli_windows_file_read(s->original,readback(s),1,s->expected,c);
    if(!preserve(s,c,readback(s),c->bytes_observed&&c->returned_bytes<=1?(size_t)c->returned_bytes:0,ok)||
       !ok||!c->bytes_observed||c->returned_bytes){s->failed=1;return 0;}
#else
    ok=slcli_linux_file_read(s->original,readback(s),1,s->expected,c);
    if(!preserve(s,c,readback(s),c->returned_bytes_observed&&c->returned_bytes<=1?(size_t)c->returned_bytes:0,ok)||
       !ok||!c->returned_bytes_observed||c->returned_bytes){s->failed=1;return 0;}
#endif
  }
  c=row(s);if(!c)return 0;
  if(!observe(s->original,c)){preserve(s,c,0,0,0);s->failed=1;return 0;}
  if(!preserve(s,c,0,0,1)||size(c)!=s->expected||
     (s->mode?!same(&s->captures[0],c):!slcli_store_original_same(&s->captures[0],c))||
     !slcli_sha256_finish(&s->bytes_hash,s->digest)){s->failed=1;return 0;}
  s->held_verified=1;return 1;
}
static int originals(const struct slcli_store_stream *s,const struct slcli_ledger_state *l,
                       const unsigned char *key,unsigned char *out) {
  size_t n;if(!storage(s)||s->unit_geometry||!l||!key||!out)return 0;n=s->capture_capacity*sizeof(*s->captures);
  return disjoint(s,sizeof(*s),l,sizeof(*l))&&disjoint(s->captures,n,l,sizeof(*l))&&
    disjoint(s,sizeof(*s),key,32)&&disjoint(s->captures,n,key,32)&&
    disjoint(out,32,s,sizeof(*s))&&disjoint(out,32,s->captures,n)&&
    disjoint(out,32,l,sizeof(*l))&&disjoint(out,32,key,32);
}
int slcli_store_ledger_write(struct slcli_store_stream *s,const struct slcli_ledger_state *l,
                             const unsigned char key[32],uint32_t *bytes,unsigned char hash[32]) {
  if(!originals(s,l,key,hash)||!bytes||s->mode||
     !disjoint(bytes,sizeof(*bytes),s,sizeof(*s))||
     !disjoint(bytes,sizeof(*bytes),s->captures,s->capture_capacity*sizeof(*s->captures))||
     !disjoint(bytes,sizeof(*bytes),l,sizeof(*l))||
     !disjoint(bytes,sizeof(*bytes),key,32)||!disjoint(bytes,sizeof(*bytes),hash,32)||
     !slcli_ledger_frame(l,key,slcli_store_stream_append,s,bytes,hash)||
     *bytes!=s->expected||!slcli_store_stream_finish(s))return 0;
  return 1;
}
int slcli_store_ledger_compare(struct slcli_store_stream *s,const struct slcli_ledger_state *l,
                               const unsigned char key[32],uint32_t bytes,unsigned char hash[32]) {
  uint32_t actual;
  if(!originals(s,l,key,hash)||!s->mode||bytes!=s->expected||
     !slcli_ledger_frame(l,key,slcli_store_stream_append,s,&actual,hash)||
     actual!=bytes||!slcli_store_stream_finish(s))return 0;
  return 1;
}
#endif
