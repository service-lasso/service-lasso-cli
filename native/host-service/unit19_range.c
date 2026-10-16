#include "unit19_range.h"
#include "input_deadline.h"
#if defined(__linux__)
#include <linux/fcntl.h>
#endif
#if defined(_WIN32) || defined(__linux__)
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
struct slcli_unit19_range *slcli_unit19_range_allocate(struct slcli_arena *arena,
  uint64_t bytes,size_t rows) {
  struct slcli_unit19_range *r;
  if(!arena||!bytes||bytes>16384)return 0;
  r=slcli_arena_calloc(arena,1,sizeof(*r));if(!r)return 0;
  r->arena=arena;r->failed=1;r->copies=slcli_input_body_allocate(arena,bytes,rows);
  if(r->copies)r->failed=0;return r;
}
struct slcli_unit19_range *slcli_unit19_derivation_range_allocate(struct slcli_arena *a,
  uint32_t ordinal,size_t rows) {
  struct slcli_unit19_range *r;
  if(!a||!slcli_unit19_derive_member_limit(ordinal))return 0;
  r=slcli_arena_calloc(a,1,sizeof(*r));if(!r)return 0;
  r->arena=a;r->failed=1;r->derivation_geometry=1;r->designated_ordinal=ordinal;
  r->copies=slcli_input_ha2_member_storage_allocate(a,ordinal,rows);
  if(r->copies)r->failed=0;return r;
}
static slcli_input_capture *next(struct slcli_input_body *s) {
  if(s->capture_count>=s->capture_capacity)return 0;
  return &s->captures[s->capture_count++];
}
struct slcli_input_deadline_owner *slcli_unit19_range_deadline_bind(struct slcli_unit19_range *r,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  const struct slcli_native_clock *prior,slcli_store_capture_sink sink,void *owner,size_t owner_bytes) {
  if(!r||!r->arena||!slcli_arena_live_block(r->arena,r,sizeof(*r))||
     r->started||r->failed||r->complete||!r->copies)return 0;
  return slcli_input_deadline_bind(r->arena,r->copies,d,caller,caller_bytes,prior,sink,owner,owner_bytes);
}
static int observe(struct slcli_input_body *s,slcli_input_capture *c) {
  int ok;
  if(!c||!slcli_input_deadline_before(s,c))return 0;
#if defined(_WIN32)
  ok=slcli_windows_source_observe(s->original,c);
#else
  ok=slcli_linux_file_observe(s->original,c);
#endif
  if(!slcli_input_deadline_after(s,c,0,0,ok))return 0;
#if defined(__linux__)
  return (c->flags_status&O_ACCMODE)==O_RDONLY&&!(c->flags_status&O_PATH)&&
    (c->descriptor_status&FD_CLOEXEC);
#else
  return 1;
#endif
}
static int read_native(struct slcli_input_body *s,unsigned char *out,size_t bytes,
  uint64_t offset,slcli_input_capture *c,uint64_t *count) {
  int ok,observed;
  if(!c||!count||!slcli_input_deadline_before(s,c))return 0;
#if defined(_WIN32)
  ok=slcli_windows_file_read(s->original,out,(DWORD)bytes,offset,c);observed=c->bytes_observed!=0;
#else
  ok=slcli_linux_file_read(s->original,out,bytes,offset,c);observed=c->returned_bytes_observed!=0;
#endif
  /* Archive exact observed native body/status before post-clock diagnostics.
   * An overall failure retains body/captures/charges and never advances pass. */
  if(!slcli_input_deadline_after(s,c,out,bytes,ok)||!observed)return 0;
  *count=c->returned_bytes;return *count<=bytes;
}
static int pass(struct slcli_unit19_range *r,unsigned char *out) {
  struct slcli_input_body *s=r->copies;slcli_input_capture *before,*after,*c;
  uint64_t offset=0,count;size_t wanted;
  before=next(s);if(!observe(s,before))return 0;
  if(!slcli_input_original_same(&s->captures[0],before,r->whole_file_bytes))return 0;
  while(offset<r->selected.bytes) {
    c=next(s);wanted=(size_t)(r->selected.bytes-offset);
    if(wanted>16384)wanted=16384;
    if(!read_native(s,out+(size_t)offset,wanted,r->selected.offset+offset,c,&count)||
       !count||count>r->selected.bytes-offset)return 0;
    offset+=count;
  }
  /* The next member may follow this range. EOF is observed at genuine whole
   * unit end using a separate fresh capture/extra byte, never member-end. */
  c=next(s);
  if(!read_native(s,out+(size_t)r->selected.bytes,1,r->whole_file_bytes,c,&count)||count)return 0;
  after=next(s);
  return observe(s,after)&&
    slcli_input_original_same(before,after,r->whole_file_bytes);
}
int slcli_unit19_range_read(struct slcli_unit19_range *r,const struct slcli_unit19_reader *unit,
  uint32_t ordinal,slcli_input_handle original) {
  struct slcli_input_body *s;size_t captures;struct slcli_sha256 hash;unsigned char digest[32];
  uint64_t maximum=16384;
  if(r&&r->derivation_geometry) {
    if(r->derivation_geometry!=1||!unit||unit->type!=SLCLI_HA2_DERIVE19||
      unit->count!=12||unit->file_bytes>1951296||
      ordinal!=r->designated_ordinal||!(maximum=slcli_unit19_derive_member_limit(ordinal))||
      unit->member_limits[ordinal]!=maximum)return 0;
  }
  if(!r||!r->arena||r->started||r->failed||!unit||!unit->started||!unit->complete||unit->failed||
     unit->next!=unit->count||unit->next_offset!=unit->file_bytes||
     unit->type<SLCLI_ROLE19||unit->type>SLCLI_HA2_COMPOUND19||
     ordinal>=unit->count||unit->count>15||!(s=r->copies)||!unit->records[ordinal].bytes||
     s->started||s->complete||s->consumed||s->recheck_started||s->capture_count||
     unit->records[ordinal].bytes>maximum||unit->records[ordinal].bytes>=s->capacity||
     unit->records[ordinal].bytes>=s->readback_capacity||
     s->capture_capacity>SIZE_MAX/sizeof(*s->captures)||
     s->capture_capacity<2*((unit->records[ordinal].bytes+16383)/16384+3))return 0;
  if(r->derivation_geometry&&(s->capacity!=maximum+1||s->readback_capacity!=maximum+1))return 0;
  captures=s->capture_capacity*sizeof(*s->captures);
  if(!slcli_arena_live_range(r->arena,r,sizeof(*r))||
     !slcli_arena_live_range(r->arena,s,sizeof(*s))||
     !slcli_arena_live_range(r->arena,unit,sizeof(*unit))||
     !slcli_arena_live_range(r->arena,s->body,s->capacity)||
     !slcli_arena_live_range(r->arena,s->readback,s->readback_capacity)||
     !slcli_arena_live_range(r->arena,s->captures,captures)||
     !away(r,sizeof(*r),unit,sizeof(*unit))||!away(s,sizeof(*s),unit,sizeof(*unit))||
     !away(s->body,s->capacity,unit,sizeof(*unit))||
     !away(s->readback,s->readback_capacity,unit,sizeof(*unit))||
     !away(s->captures,captures,unit,sizeof(*unit))||
     unit->records[ordinal].offset<64+(uint64_t)unit->count*128||
     unit->records[ordinal].offset>unit->file_bytes||
     unit->records[ordinal].bytes>unit->file_bytes-unit->records[ordinal].offset)return 0;
  {
    const void *ranges[5]={r,s,s->body,s->readback,s->captures};
    size_t lengths[5]={sizeof(*r),sizeof(*s),s->capacity,s->readback_capacity,captures};
    unsigned i,j;
    for(i=0;i<5;i++)for(j=0;j<i;j++)
      if(!away(ranges[i],lengths[i],ranges[j],lengths[j]))return 0;
  }
  r->started=1;r->failed=1;r->unit=unit;r->ordinal=ordinal;r->selected=unit->records[ordinal];
  r->whole_file_bytes=unit->file_bytes;
  s->started=1;s->recheck_started=1;s->consumed=2;s->failed=1;s->original=original;
  if(!pass(r,s->body))return 0;
  s->bytes=(size_t)r->selected.bytes;s->complete=1;s->eof_observed=1;
  slcli_sha256_init(&hash);
  if(!slcli_sha256_update(&hash,s->body,s->bytes)||!slcli_sha256_finish(&hash,s->digest)||
     !slcli_equal(s->digest,r->selected.digest,32)||!pass(r,s->readback)||
     !slcli_equal(s->body,s->readback,s->bytes))return 0;
  slcli_sha256_init(&hash);
  if(!slcli_sha256_update(&hash,s->readback,s->bytes)||!slcli_sha256_finish(&hash,digest)||
     !slcli_equal(digest,s->digest,32))return 0;
  s->recheck_complete=1;s->failed=0;r->complete=1;r->failed=0;return 1;
}
#endif
