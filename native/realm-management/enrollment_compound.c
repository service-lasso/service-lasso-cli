#include "enrollment_compound.h"
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
struct slcli_management_enrollment_compound *slcli_management_enrollment_compound_allocate(
  struct slcli_arena *a,size_t write_rows,size_t read_rows) {
  struct slcli_management_enrollment_compound *c;
  if(!a)return 0;c=slcli_arena_calloc(a,1,sizeof(*c));if(!c)return 0;c->arena=a;c->failed=1;
  c->native=slcli_unit19_native_allocate(a,write_rows,read_rows);
  if(c->native&&!c->native->failed)c->failed=0;return c;
}
int slcli_management_enrollment_compound_prepare(struct slcli_management_enrollment_compound *c,
  const struct slcli_management_enrollment_builder *b,const struct slcli_unit19_record records[2],
  uint32_t project,slcli_input_handle file,slcli_store_capture_sink sink,void *original,
  uint64_t prior_store_bytes) {
  size_t i;
  if(!c||!c->arena||c->started||c->failed||!c->native||
     !slcli_arena_live_range(c->arena,c,sizeof(*c))||
     !slcli_arena_live_range(c->arena,b,sizeof(*b))||b->arena!=c->arena||
     !b->complete||b->failed||!b->signer||!b->signer->signed_complete||
     !b->originals||!slcli_arena_live_range(c->arena,b->originals,sizeof(*b->originals))||
     !slcli_arena_live_range(c->arena,b->originals->anchor,32)||
     !slcli_arena_live_range(c->arena,records,2*sizeof(*records))||
     !away(c,sizeof(*c),b,sizeof(*b))||!away(c,sizeof(*c),records,2*sizeof(*records))||
     !away(c,sizeof(*c),b->originals->anchor,32)||
     records[0].offset!=320||records[0].bytes!=32||
     records[1].offset!=352||records[1].bytes!=b->bytes||!b->bytes||b->bytes>16384||
     !slcli_equal(records[0].digest,b->anchor_hash,32)||
     !slcli_equal(records[1].digest,b->decoded.raw_digest,32))return 0;
  c->started=1;c->failed=1;c->enrollment=b;
  for(i=0;i<2;i++)c->records[i]=records[i];
  c->members[0]=b->originals->anchor;c->members[1]=b->raw;
  if(!slcli_unit19_native_framing_storage(c->native,&c->framing,c->members,c->records,2,0,0)||
     !slcli_unit19_encode(&c->framing,SLCLI_HA2_COMPOUND19,project,c->records,2,0,0)||
     !slcli_unit19_native_prepare(c->native,SLCLI_HA2_COMPOUND19,project,c->framing.header,
       &c->framing.table[0][0],256,c->members,c->records,0,0,file,sink,original,prior_store_bytes))return 0;
  c->prepared=1;c->failed=0;return 1;
}
int slcli_management_enrollment_compound_positive_original(struct slcli_management_enrollment_compound *c,
  const struct slcli_management_enrollment_builder *b,const struct slcli_unit19_record records[2],
  uint32_t project,slcli_input_handle file,slcli_store_capture_sink sink,void *original,
  uint64_t prior_store_bytes) {
  if(!slcli_management_enrollment_compound_prepare(c,b,records,project,file,sink,original,
       prior_store_bytes))return 0;
  return c->prepared&&!c->failed&&c->native->prepared&&!c->native->failed&&
    c->framing.checked.complete&&c->native->parsed.complete&&
    c->native->parsed.type==SLCLI_HA2_COMPOUND19&&c->native->parsed.count==2&&
    c->native->parsed.file_bytes==352+b->bytes&&
    c->native->retained_store_bytes==prior_store_bytes+352+b->bytes&&
    slcli_equal(c->native->write->digest,c->native->held->digest,32);
}
