#include "primary_wire.h"
int slcli_runtime_wire_geometry_derive(uint32_t count,struct slcli_runtime_wire_geometry *out) {
 struct slcli_runtime_wire_geometry g={0};uint64_t bytes,fragments;
 if(!out||!count||count>=UINT16_MAX||SLCLI_RUNTIME_FRAGMENT_PAYLOAD_BYTES!=16253u)return 0;
 bytes=6u+(uint64_t)count*SLCLI_RUNTIME_ROW_BYTES;
 fragments=(bytes+SLCLI_RUNTIME_FRAGMENT_PAYLOAD_BYTES-1u)/SLCLI_RUNTIME_FRAGMENT_PAYLOAD_BYTES;
 if(bytes>UINT32_MAX||!fragments||fragments>UINT32_MAX)return 0;
 g.count=count;g.snapshot_bytes=(uint32_t)bytes;g.terminal_bitmap_bytes=count/8u+(count%8u!=0);
 g.fragment_count=(uint32_t)fragments;*out=g;return 1;
}
static int b32(struct slcli_cursor *c, const unsigned char **raw, int required) {
  unsigned char bits = 0;
  unsigned i;
  if (!slcli_take(c, 32, raw)) return 0;
  for (i = 0; i < 32; ++i) bits |= (*raw)[i];
  return !required || bits != 0;
}
static uint16_t r16(const unsigned char *p){return (uint16_t)((uint16_t)p[0]<<8|p[1]);}
static uint32_t r32(const unsigned char *p){return (uint32_t)p[0]<<24|(uint32_t)p[1]<<16|(uint32_t)p[2]<<8|p[3];}
static uint64_t r64(const unsigned char *p){return (uint64_t)r32(p)<<32|r32(p+4);}
static int nonzero32(const unsigned char *p){unsigned i;unsigned char n=0;for(i=0;i<32;i++)n|=p[i];return n!=0;}
static void w32(unsigned char *p,uint32_t n){p[0]=(unsigned char)(n>>24);p[1]=(unsigned char)(n>>16);p[2]=(unsigned char)(n>>8);p[3]=(unsigned char)n;}
static void w64(unsigned char *p,uint64_t n){w32(p,(uint32_t)(n>>32));w32(p+4,(uint32_t)n);}
static int outside_frame(const struct slcli_frame *frame,const void *raw,size_t bytes){
 uintptr_t a=(uintptr_t)frame,b=(uintptr_t)raw;
 if(!raw||!bytes||a>UINTPTR_MAX-sizeof(*frame)||b>UINTPTR_MAX-bytes)return 0;
 return b+bytes<=a||a+sizeof(*frame)<=b;
}
int slcli_snapshot_fragment_claim_encode(struct slcli_frame *frame,uint32_t rows,
 const struct slcli_snapshot_fragment_claim *input){
 struct slcli_runtime_wire_geometry g;struct slcli_snapshot_fragment_claim f;
 uint64_t offset;uint32_t wanted,i;unsigned char *raw;
 if(!frame||!input||!slcli_runtime_wire_geometry_derive(rows,&g))return 0;
 f=*input;
 if(!f.sequence||f.sequence>1024||f.phase>7||f.whole_bytes!=g.snapshot_bytes||
  f.fragment_count!=g.fragment_count||f.ordinal>=g.fragment_count)return 0;
 offset=(uint64_t)f.ordinal*SLCLI_RUNTIME_FRAGMENT_PAYLOAD_BYTES;
 if(offset>=f.whole_bytes||offset!=f.offset)return 0;
 wanted=f.whole_bytes-f.offset;if(wanted>SLCLI_RUNTIME_FRAGMENT_PAYLOAD_BYTES)wanted=SLCLI_RUNTIME_FRAGMENT_PAYLOAD_BYTES;
 if(f.chunk_bytes!=wanted||!outside_frame(frame,f.chunk,wanted)||
  !outside_frame(frame,f.reservation,32)||!outside_frame(frame,f.digest,32)||
  !outside_frame(frame,f.frontier,32)||!nonzero32(f.reservation)||!nonzero32(f.digest))return 0;
 raw=frame->body;raw[0]=3;w32(raw+1,f.sequence);raw[5]=3;
 for(i=0;i<32;i++){raw[6+i]=f.reservation[i];raw[55+i]=f.digest[i];raw[95+i]=f.frontier[i];}
 raw[38]=f.phase;w32(raw+39,f.whole_bytes);w32(raw+43,f.ordinal);
 w32(raw+47,f.fragment_count);w32(raw+51,f.offset);w64(raw+87,f.revision);w32(raw+127,wanted);
 for(i=0;i<wanted;i++)raw[131+i]=f.chunk[i];
 frame->length=SLCLI_RUNTIME_FRAGMENT_ENVELOPE_BYTES+wanted;return 1;
}
int slcli_snapshot_fragment_claim_parse(const struct slcli_frame *frame,uint32_t rows,
 struct slcli_snapshot_fragment_claim *out) {
 struct slcli_snapshot_fragment_claim f={0};struct slcli_runtime_wire_geometry g;
 const unsigned char *raw;uint64_t offset;uint32_t remaining,wanted;
 if(!frame||!out||!slcli_runtime_wire_geometry_derive(rows,&g)||
  frame->length<=SLCLI_RUNTIME_FRAGMENT_ENVELOPE_BYTES||frame->length>SLCLI_FRAME_BYTES)return 0;
 raw=frame->body;
 if(raw[0]!=3||raw[5]!=3)return 0;
 f.sequence=r32(raw+1);f.reservation=raw+6;f.phase=raw[38];f.whole_bytes=r32(raw+39);
 f.ordinal=r32(raw+43);f.fragment_count=r32(raw+47);f.offset=r32(raw+51);
 f.digest=raw+55;f.revision=r64(raw+87);f.frontier=raw+95;f.chunk_bytes=r32(raw+127);f.chunk=raw+131;
 if(!f.sequence||f.sequence>1024||!nonzero32(f.reservation)||f.phase>7||
  !nonzero32(f.digest)||f.whole_bytes!=g.snapshot_bytes||f.fragment_count!=g.fragment_count||
  f.ordinal>=f.fragment_count)return 0;
 offset=(uint64_t)f.ordinal*SLCLI_RUNTIME_FRAGMENT_PAYLOAD_BYTES;
 if(offset>=f.whole_bytes||offset!=f.offset)return 0;
 remaining=f.whole_bytes-f.offset;wanted=remaining<SLCLI_RUNTIME_FRAGMENT_PAYLOAD_BYTES?remaining:SLCLI_RUNTIME_FRAGMENT_PAYLOAD_BYTES;
 if(f.chunk_bytes!=wanted||frame->length!=SLCLI_RUNTIME_FRAGMENT_ENVELOPE_BYTES+f.chunk_bytes)return 0;
 *out=f;return 1;
}
int slcli_runtime_row_claim_parse(const unsigned char *b,uint32_t ordinal,
 uint32_t count,struct slcli_runtime_row_claim *out){
 struct slcli_runtime_row_claim r={0};uint16_t f;unsigned born,policy=2,membership;
 struct slcli_runtime_wire_geometry geometry;
 if(!b||!out||ordinal>=count||!slcli_runtime_wire_geometry_derive(count,&geometry))return 0;
 r.id=r32(b);r.kind=b[4];r.state=b[5];r.charged=r64(b+6);r.identity=b+14;r.handle_index=r32(b+46);r.observation=b+50;
 r.owner_domain=b[82];r.process_role=b[83];r.producer_slot=b[84];r.consumer_slot=b[85];r.source_member=r32(b+86);r.native_rights=r64(b+90);r.alias_of=r16(b+98);r.physical_extent=r16(b+100);r.capture_first=r32(b+102);r.capture_count=r32(b+106);r.start_cut=r32(b+110);r.end_cut=r32(b+114);r.effect_flags=r16(b+118);r.input_slot=b[120];r.applicability=b[121];
 membership=(r.applicability>>4)&7;f=r.effect_flags;born=(f&8)!=0;
 if(b[122]||b[123]||r.kind>SLCLI_RUNTIME_MAX_NATIVE_CLASS||r.state>3||(r.owner_domain!=0&&r.owner_domain!=1&&r.owner_domain!=2&&r.owner_domain!=4)||r.process_role>7||!r.producer_slot||r.producer_slot>128||!r.source_member||membership>4||((r.applicability&128)&&membership))return 0;
 if((r.consumer_slot==0)!=((r.applicability&1)!=0)||r.consumer_slot>128||(r.input_slot==0)!=((r.applicability&2)!=0)||r.input_slot>128)return 0;
 if((r.physical_extent==UINT16_MAX)!=((r.applicability&4)!=0)||(r.physical_extent!=UINT16_MAX&&r.physical_extent>=256)||((r.kind>=5&&r.kind<=13||r.kind==22)&&r.physical_extent==UINT16_MAX))return 0;
 if((r.handle_index==UINT32_MAX)!=((r.applicability&8)!=0)||(r.handle_index!=UINT32_MAX&&r.handle_index>=count)||(r.alias_of!=UINT16_MAX&&r.alias_of>=ordinal))return 0;
 if(!nonzero32(r.observation)||!r.capture_first||!r.capture_count||r.capture_count-1>UINT32_MAX-r.capture_first||!r.start_cut||r.start_cut>13||r.end_cut>13)return 0;
 if((f&~255u)||!(f&1)||((f&4)&&!(f&2))||((f&8)&&(f&6)!=6)||((f&64)&&!(f&32))||((f&32)&&!born)||((f&128)&&(!born||r.kind>1)))return 0;
 if(nonzero32(r.identity)!=born)return 0;
 if(!born&&(r.state<2||r.handle_index!=UINT32_MAX||r.native_rights||r.end_cut||(f&224)))return 0;
 if(!born&&!(f&2)&&f!=1)return 0;
 if(r.state==0&&((f&15)!=15||(f&112)||r.end_cut))return 0;
 if(r.state==1&&((f&15)!=15||(f&16)||!r.end_cut||r.handle_index!=UINT32_MAX||((f&96)&&(f&96)!=96)||(r.kind<=1&&!(f&128))))return 0;
 if(r.state>=2&&(r.end_cut||((f&2)&&!(f&16))))return 0;
 switch(r.kind){case 0:case 2:case 3:case 4:case 19:case 21:case 22:policy=1;break;case 5:case 6:case 7:case 9:case 10:case 11:case 12:case 13:case 15:case 16:policy=0;break;default:break;}
 if((policy==0&&r.handle_index!=UINT32_MAX)||(r.state==0&&policy==1&&r.handle_index==UINT32_MAX))return 0;
 *out=r;return 1;
}
int slcli_snapshot_claim_parse_exact_shape(const unsigned char *raw,uint32_t bytes,
 uint32_t rows,uint64_t original_limit,struct slcli_snapshot_claim *out){
 struct slcli_snapshot_claim r={0};struct slcli_runtime_row_claim row;uint32_t i,previous=0;
 struct slcli_runtime_wire_geometry g;
 /* Failure never assigns a partial parsed candidate to the caller. */
 if(!raw||!out||!original_limit||!slcli_runtime_wire_geometry_derive(rows,&g)||
  bytes!=g.snapshot_bytes||raw[0]!=SLCLI_RUNTIME_SNAPSHOT_VERSION||raw[1]>7)return 0;
 r.phase=raw[1];r.count=r32(raw+2);if(r.count!=rows)return 0;
 for(i=0;i<r.count;i++){
  if(!slcli_runtime_row_claim_parse(raw+6+i*SLCLI_RUNTIME_ROW_BYTES,i,r.count,&row)||(i&&row.id<=previous)||row.charged>original_limit-r.charged)return 0;
  previous=row.id;r.charged+=row.charged;
 }
 r.raw=raw;r.bytes=bytes;*out=r;return 1;
}
int slcli_snapshot_claim_parse(const unsigned char *raw,uint32_t bytes,
 uint64_t original_limit,struct slcli_snapshot_claim *out){
 uint32_t rows;
 if(!raw||bytes<6||bytes>SLCLI_RUNTIME_SNAPSHOT_BYTES)return 0;
 rows=r32(raw+2);if(!rows||rows>128)return 0;
 return slcli_snapshot_claim_parse_exact_shape(raw,bytes,rows,original_limit,out);
}
int slcli_primary_request_parse(const struct slcli_frame *frame,
                                uint64_t original_limit,
                                struct slcli_primary_request *out) {
  struct slcli_cursor c;
  struct slcli_primary_request result = {0};
  const unsigned char *raw;
  uint8_t phase;
  uint32_t bytes;
  if (!frame || !out || !original_limit || frame->length < 5 ||
      frame->length > SLCLI_FRAME_BYTES) return 0;
  c.raw = frame->body; c.bytes = frame->length; c.offset = 0; c.failed = 0;
  if (!slcli_u8(&c, &result.operation) || !slcli_u32(&c, &result.sequence)) return 0;
  switch (result.operation) {
  case 1:
    if (result.sequence || !slcli_cursor_end(&c)) return 0;
    break;
  case 3:
    if (!result.sequence || result.sequence > 1024 ||
        !b32(&c, &result.reservation, 1) || !slcli_u8(&c, &phase) || phase > 7 ||
        !slcli_u32(&c, &bytes) || bytes < 6 || bytes > SLCLI_RUNTIME_SNAPSHOT_BYTES ||
        !slcli_take(&c, bytes, &raw) || !slcli_cursor_end(&c) ||
        !slcli_snapshot_claim_parse(raw, bytes, original_limit, &result.snapshot) ||
        result.snapshot.phase != phase) return 0;
    break;
  case 5:
    if (!result.sequence || result.sequence > 1025 || frame->length > 256 ||
        !b32(&c, &result.reservation, 1) || !b32(&c, &result.snapshot_digest, 1) ||
        !b32(&c, &result.observer, 1) || !slcli_u8(&c, &result.expected) ||
        result.expected || !slcli_u64(&c, &result.deadline) ||
        !result.deadline || !slcli_cursor_end(&c)) return 0;
    break;
  default: return 0;
  }
  *out = result;
  return 1;
}
