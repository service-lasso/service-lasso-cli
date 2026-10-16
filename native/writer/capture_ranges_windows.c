#include "capture_ranges_windows.h"
#include "capture_view_windows.h"
#if defined(_WIN32)
static int separate(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int zero(const unsigned char *p,size_t n){size_t i;for(i=0;i<n;i++)if(p[i])return 0;return 1;}
static void w32(unsigned char *p,uint32_t n){unsigned i;for(i=0;i<4;i++)p[i]=(unsigned char)(n>>(8*i));}
static void w64(unsigned char *p,uint64_t n){unsigned i;for(i=0;i<8;i++)p[i]=(unsigned char)(n>>(8*i));}
static void header(unsigned char out[48],const struct slcli_writer_capture_ranges_windows *c,
 size_t ordinal,uint32_t attempted,uint32_t kind,size_t requested){
 static const unsigned char magic[8]={'S','L','N','C','1','0','6','1'};size_t i;
 for(i=0;i<48;i++)out[i]=0;
 for(i=0;i<8;i++)out[i]=magic[i];w32(out+8,c->version);w32(out+12,sizeof(struct slcli_windows_file_capture));
 w64(out+16,ordinal);w32(out+24,attempted);w32(out+28,kind);
 if(c->version==2){w64(out+32,c->payload_capacity);w64(out+40,requested);}
}
static int layout(size_t count,size_t payload,uint32_t version,size_t *data,size_t *output,size_t *stride){
 size_t align=_Alignof(struct slcli_windows_file_capture),n;
 size_t h=version==1?32:48;
 if((version!=1&&version!=2)||(version==1&&payload)||(version==2&&(!payload||payload>16384))||
  !count||sizeof(struct slcli_windows_file_capture)>UINT32_MAX||h>SIZE_MAX-(align-1))return 0;
 n=(h+align-1)/align*align;if(sizeof(struct slcli_windows_file_capture)>SIZE_MAX-n)return 0;
 *data=n;n+=sizeof(struct slcli_windows_file_capture);*output=n;
 if(payload>SIZE_MAX-n)return 0;n+=payload;if(n>SIZE_MAX-(align-1))return 0;
 n=(n+align-1)/align*align;if(count>SIZE_MAX/n)return 0;*stride=n;return 1;
}
int slcli_writer_capture_ranges_windows_payload_geometry(size_t count,size_t payload,
 size_t *data,size_t *output,size_t *stride){
 size_t d,o,s;if(!data||!output||!stride||
  !separate(data,sizeof(*data),output,sizeof(*output))||
  !separate(data,sizeof(*data),stride,sizeof(*stride))||
  !separate(output,sizeof(*output),stride,sizeof(*stride))||!layout(count,payload,2,&d,&o,&s))return 0;
 *data=d;*output=o;*stride=s;return 1;
}
static int same(const struct slcli_writer_capture_ranges_windows *c){
 size_t data,output,stride;
 return c&&c->started&&c->arena&&slcli_arena_live_block(c->arena,c,sizeof(*c))&&c->extent&&
  slcli_arena_live_block(c->arena,c->extent,sizeof(*c->extent))&&c->extent->arena==c->arena&&
  c->extent->ranges_original==c&&
  slcli_writer_capture_extent_windows_matches_original(c->extent)&&
  (!c->extent->backing_original||slcli_writer_capture_view_windows_batch_matches_original(c->extent->backing_original))&&
  layout(c->capacity,c->payload_capacity,c->version,&data,&output,&stride)&&
  data==c->data_offset&&output==c->payload_offset&&stride==c->stride&&
  c->capacity*c->stride<=c->extent->bytes&&c->attempted<=c->capacity&&c->initialized<=c->capacity&&
  !((uintptr_t)c->extent->views[0]%_Alignof(struct slcli_windows_file_capture))&&
  !((uintptr_t)c->extent->views[1]%_Alignof(struct slcli_windows_file_capture));
}
int slcli_writer_capture_ranges_windows_matches_original(const struct slcli_writer_capture_ranges_windows *c){
 return same(c)&&c->prepared&&!c->failed&&c->initialized==c->capacity&&
  c->before_returned&&c->before_result&&c->before.complete&&
  c->after_returned&&c->after_result&&c->after.complete&&
  c->before.tick>=(c->extent->backing_original?c->extent->backing_original->views_finish.tick:c->extent->previous_tick)&&
  c->after.tick>=c->before.tick&&c->after.tick<c->extent->readback->deadline->expires;
}
static int prepare(
 struct slcli_writer_capture_ranges_windows *c,struct slcli_arena *a,
 struct slcli_writer_capture_extent_windows *e,size_t count,size_t payload,uint32_t version){
 size_t data,output,stride,i,h=version==1?32:48;unsigned char expected[48];
 if(!c||!a||!e||!slcli_arena_live_block(a,c,sizeof(*c))||!zero((const unsigned char *)c,sizeof(*c))||
  !slcli_arena_live_block(a,e,sizeof(*e))||e->arena!=a||e->ranges_original||
  !slcli_writer_capture_extent_windows_matches_original(e)||
  (e->backing_original&&!slcli_writer_capture_view_windows_batch_matches_original(e->backing_original))||
  !layout(count,payload,version,&data,&output,&stride)||count*stride>e->bytes||
  !separate(c,sizeof(*c),e,sizeof(*e))||!separate(c,sizeof(*c),e->readback,sizeof(*e->readback))||
  !separate(c,sizeof(*c),e->readback->creator,sizeof(*e->readback->creator))||
  !separate(c,sizeof(*c),e->owner,sizeof(*e->owner))||
  !separate(c,sizeof(*c),e->readback->caller,e->readback->caller_bytes)||
  !slcli_source_deadline_away(e->readback->deadline,c,sizeof(*c)))return 0;
 c->started=1;c->failed=1;c->arena=a;c->extent=e;c->capacity=count;c->stride=stride;c->data_offset=data;
 c->version=version;c->payload_capacity=payload;c->payload_offset=output;
 e->ranges_original=c;
 if(!same(c))return 0;
 c->before_result=slcli_native_clock_read(&c->before);c->before_returned=1;
 if(!c->before_result||c->before.tick<(e->backing_original?e->backing_original->views_finish.tick:e->previous_tick)||c->before.tick>=e->readback->deadline->expires||
  !slcli_evidence_deadline_precede(e->readback->creator->evidence_file,&c->before))return 0;
 for(i=0;i<count;i++){
  unsigned char *raw=(unsigned char *)e->views[0]+i*stride;
  const unsigned char *read=(const unsigned char *)e->views[1]+i*stride;
  /* Actual original zero scan was completed before any output publication;
   * this source-specific scan forbids a reused range during initialization. */
  if(!zero(raw,stride)||!zero(read,stride))return 0;
  header(expected,c,i,0,0,0);
  {size_t j;for(j=0;j<h;j++)raw[j]=expected[j];}
  c->initialized=i+1;
 }
 c->after_result=slcli_native_clock_read(&c->after);c->after_returned=1;
 if(!c->after_result||c->after.tick<c->before.tick||c->after.tick>=e->readback->deadline->expires||!same(c)||
  !slcli_evidence_deadline_precede(e->readback->creator->evidence_file,&c->after))return 0;
 c->prepared=1;c->failed=0;return 1;
}
int slcli_writer_capture_ranges_windows_prepare_original(
 struct slcli_writer_capture_ranges_windows *c,struct slcli_arena *a,
 struct slcli_writer_capture_extent_windows *e,size_t count){return prepare(c,a,e,count,0,1);}
int slcli_writer_capture_ranges_windows_prepare_payload_original(
 struct slcli_writer_capture_ranges_windows *c,struct slcli_arena *a,
 struct slcli_writer_capture_extent_windows *e,size_t count,size_t payload){return prepare(c,a,e,count,payload,2);}
static struct slcli_windows_file_capture *claim(
 struct slcli_writer_capture_ranges_windows *c,uint32_t kind,size_t requested,
 unsigned char **output,const unsigned char **readonly){
 unsigned char expected[48],*raw;const unsigned char *read;size_t i,index,h;
 if(!same(c)||!c->prepared||c->failed||c->write_close_bq_original||c->initialized!=c->capacity||
  c->attempted>=c->capacity||kind<1||kind>6||(kind>=5&&c->version!=2)||requested>c->payload_capacity||
  (requested&&(kind!=2||c->version!=2||!output||!readonly)))return 0;
 h=c->version==1?32:48;
 index=c->attempted;raw=(unsigned char *)c->extent->views[0]+index*c->stride;
 read=(const unsigned char *)c->extent->views[1]+index*c->stride;header(expected,c,index,0,0,0);
 if(!slcli_equal(raw,expected,h)||!slcli_equal(read,expected,h)||
  !zero(raw+h,c->stride-h)||!zero(read+h,c->stride-h)){c->failed=1;return 0;}
 /* Consume first. A death before native return leaves attempted/uncertain,
  * never an invented claim that zeroed raw fields establish no native effect. */
 c->attempted=index+1;header(expected,c,index,1,kind,requested);
 for(i=0;i<h;i++)raw[i]=expected[i];
 if(requested){*output=raw+c->payload_offset;*readonly=read+c->payload_offset;}
 return (struct slcli_windows_file_capture *)(raw+c->data_offset);
}
struct slcli_windows_file_capture *slcli_writer_capture_ranges_windows_claim_original(
 struct slcli_writer_capture_ranges_windows *c,uint32_t kind){return claim(c,kind,0,0,0);}
struct slcli_windows_file_capture *slcli_writer_capture_ranges_windows_claim_read_original(
 struct slcli_writer_capture_ranges_windows *c,size_t bytes,unsigned char **output,const unsigned char **readonly){
 if(!same(c)||!bytes||!output||!readonly||
  !slcli_arena_live_range(c->arena,output,sizeof(*output))||
  !slcli_arena_live_range(c->arena,readonly,sizeof(*readonly))||
  !zero((const unsigned char *)output,sizeof(*output))||!zero((const unsigned char *)readonly,sizeof(*readonly))||
  !separate(output,sizeof(*output),readonly,sizeof(*readonly))||
  !separate(output,sizeof(*output),c,sizeof(*c))||!separate(readonly,sizeof(*readonly),c,sizeof(*c))||
  !separate(output,sizeof(*output),c->extent,sizeof(*c->extent))||
  !separate(readonly,sizeof(*readonly),c->extent,sizeof(*c->extent))||
  !separate(output,sizeof(*output),c->extent->views[0],c->extent->bytes)||
  !separate(output,sizeof(*output),c->extent->views[1],c->extent->bytes)||
  !separate(readonly,sizeof(*readonly),c->extent->views[0],c->extent->bytes)||
  !separate(readonly,sizeof(*readonly),c->extent->views[1],c->extent->bytes))return 0;
 return claim(c,2,bytes,output,readonly);
}
#endif
