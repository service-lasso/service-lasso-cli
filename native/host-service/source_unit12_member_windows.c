#include "source_unit12_member_windows.h"
#include "evidence_deadline.h"
#include <stddef.h>
#if defined(_WIN32)
static int separate(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int zero(const void *p,size_t n){const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;}
static int native_call(struct slcli_source_unit12_member_window_windows *c,unsigned i,
 unsigned char *output,size_t bytes,uint64_t offset){
 struct slcli_source_unit12_native *t=c->table;struct slcli_windows_file_capture *r=c->captures[i];
 c->before_result[i]=slcli_native_clock_read(&c->clocks[i][0]);c->before_returned[i]=1;
 if(!c->before_result[i]||c->clocks[i][0].tick<c->previous_tick||
  c->clocks[i][0].tick>=t->deadline->expires||
  !slcli_evidence_deadline_precede(c->foundation->extent->readback->creator->evidence_file,&c->clocks[i][0]))return 0;
 c->native_called[i]=1;
 c->native_result[i]=output?slcli_windows_file_read(t->original,output,(DWORD)bytes,offset,r):
  slcli_windows_source_observe(t->original,r);
 c->native_returned[i]=1;
 c->after_result[i]=slcli_native_clock_read(&c->clocks[i][1]);c->after_returned[i]=1;
 if(!c->after_result[i]||c->clocks[i][1].tick<c->clocks[i][0].tick||
  c->clocks[i][1].tick<r->tick||c->clocks[i][1].tick>=t->deadline->expires||
  !slcli_evidence_deadline_precede(c->foundation->extent->readback->creator->evidence_file,&c->clocks[i][1]))return 0;
 c->previous_tick=c->clocks[i][1].tick;
 return c->native_result[i]&&r->tick>=c->clocks[i][0].tick;
}
int slcli_source_unit12_member_window_windows_read_original(
 struct slcli_source_unit12_member_window_windows *c,struct slcli_arena *a,
 struct slcli_source_unit12_native *t,size_t member_range,struct slcli_writer_capture_ranges_windows *f){
 const struct slcli_source_unit12_native_range *r;unsigned i,pass;struct slcli_sha256 hash;
 struct slcli_source_unit12_window geometry;
 const struct slcli_evidence_deadline_owner *capture_deadline;uint64_t record_limit;size_t payload_limit;
 if(!c||!a||!t||!f||!slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !slcli_arena_live_block(a,t,sizeof(*t))||t->arena!=a||!t->table_observed||t->failed||
  !t->ranges||t->range_count<2||t->range_count>SIZE_MAX/sizeof(*t->ranges)||
  t->member_window_original||!member_range||member_range>=t->range_count||
  t->completed!=t->range_count||!t->reader.table_complete||t->reader.failed||
  !slcli_arena_live_block(a,f,sizeof(*f))||f->arena!=a||!f->prepared||f->failed||
  f->attempted>f->capacity||f->capacity-f->attempted<8||!f->extent||
  !slcli_arena_live_block(a,f->extent,sizeof(*f->extent))||
  !slcli_writer_capture_extent_windows_matches_original(f->extent)||
  f->extent->readback->deadline!=t->deadline||f->extent->readback->caller!=t->caller||
  !separate(c,sizeof(*c),t,sizeof(*t))||!separate(c,sizeof(*c),f,sizeof(*f))||
  !separate(c,sizeof(*c),f->extent,sizeof(*f->extent))||
  !separate(c,sizeof(*c),t->caller,t->caller_bytes)||
  !slcli_source_deadline_away(t->deadline,c,sizeof(*c)))return 0;
 if(!slcli_source_unit12_native_matches_original(t))return 0;
 if(!slcli_arena_live_block(a,t->ranges,t->range_count*sizeof(*t->ranges)))return 0;
 r=&t->ranges[member_range];
 if(
  !separate(c,sizeof(*c),t->ranges,t->range_count*sizeof(*t->ranges))||
  !r->member_decoded||!r->decode_returned||!r->decode_result||!r->after_returned||!r->after_result||
  !r->after.complete||r->member.index!=member_range-1||r->member.offset>t->file_bytes||
  r->member.bytes>t->file_bytes-r->member.offset||
  !t->ranges[0].body||!slcli_arena_live_range(a,t->ranges[0].body,sizeof(*t->ranges[0].body))||
  !slcli_input_deadline_ready(t->ranges[0].body)||!t->ranges[0].body->complete||
  !t->ranges[0].body->recheck_complete||t->ranges[0].body->failed||
  !t->ranges[0].body->capture_count)return 0;
 capture_deadline=f->extent->readback->creator->evidence_file->deadline_owner;
 if(!capture_deadline->bq_profile||!capture_deadline->record_maximum||
  sizeof(struct slcli_windows_file_capture)>16384-48)return 0;
 record_limit=capture_deadline->record_maximum;
 if(record_limit>16384)record_limit=16384;
 if(record_limit<=48+sizeof(struct slcli_windows_file_capture))return 0;
 payload_limit=(size_t)(record_limit-48-sizeof(struct slcli_windows_file_capture));
 if(!slcli_source_unit12_window_geometry(&r->member,t->file_bytes,0,payload_limit,&geometry))return 0;
 c->started=1;c->failed=1;c->arena=a;c->table=t;c->foundation=f;c->member_range=member_range;
 c->member=r->member;c->geometry=geometry;c->source_offset=geometry.source_offset;
 c->storage_charge=slcli_arena_allocation_bytes(sizeof(*c));
 if(!a->owner||!c->storage_charge||c->storage_charge>a->owner->charged)return 0;
 c->journal_record_maximum=capture_deadline->record_maximum;
 c->journal_payload_maximum=payload_limit;
 c->requested=geometry.requested;
 c->previous_tick=t->ranges[t->range_count-1].after.tick;
 if(c->previous_tick<f->after.tick)c->previous_tick=f->after.tick;
 t->member_window_original=c;
 c->capture_first_ordinal=f->attempted;
 for(i=0;i<8;i++){
  c->captures[i]=slcli_writer_capture_ranges_windows_claim_original(f,(i%4==0||i%4==3)?1:2);
  if(!c->captures[i])return 0;c->reserved_captures=i+1;
 }
 for(pass=0;pass<2;pass++){
  unsigned base=pass*4;unsigned char *out=pass?c->readback:c->body;
  if(!native_call(c,base,0,0,0)||
   !slcli_input_original_same(&t->ranges[0].body->captures[0],c->captures[base],t->file_bytes))return 0;
  /* Empty member uses no payload read: its separate read row stays reserved,
   * not fabricated as a native success. Actual whole-file EOF still executes. */
  if(c->requested){
   if(!native_call(c,base+1,out,c->requested,c->source_offset)||
    !c->captures[base+1]->bytes_observed||c->captures[base+1]->returned_bytes!=c->requested)return 0;
   c->returned_bytes[pass]=(size_t)c->captures[base+1]->returned_bytes;
  }
  if(!native_call(c,base+2,out+c->requested,1,t->file_bytes)||
   !c->captures[base+2]->bytes_observed||c->captures[base+2]->returned_bytes||
   !native_call(c,base+3,0,0,0)||
   !slcli_input_original_same(c->captures[base],c->captures[base+3],t->file_bytes))return 0;
 }
 if(!slcli_equal(c->body,c->readback,c->requested))return 0;
 slcli_sha256_init(&hash);if(!slcli_sha256_update(&hash,c->body,c->requested)||
  !slcli_sha256_finish(&hash,c->digest))return 0;
 if(c->member.bytes==c->requested){
  if(!slcli_equal(c->digest,c->member.digest,32))return 0;
  c->full_member_bytes_compared=1;
 }
 c->finish_result=slcli_native_clock_read(&c->finish);c->finish_returned=1;
 if(!c->finish_result||c->finish.tick<c->previous_tick||c->finish.tick>=t->deadline->expires||
  !slcli_evidence_deadline_precede(f->extent->readback->creator->evidence_file,&c->finish))return 0;
 c->previous_tick=c->finish.tick;
 c->observed=1;c->failed=0;return 1;
}
static uint32_t member_u32(const unsigned char *p){
 return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
static uint64_t member_u64(const unsigned char *p){
 uint64_t n=0;unsigned i;for(i=0;i<8;i++)n|=(uint64_t)p[i]<<(8*i);return n;
}
static int member_observe_original(const struct slcli_windows_file_capture *r,HANDLE h,uintptr_t original){
 uintptr_t start,sid=(uintptr_t)r->owner_pointer;size_t offset,bytes;
 const ACCESS_MASK forbidden=0x00000002|0x00000004|0x00000010|0x00000100|
  0x00010000|0x00040000|0x00080000|0x10000000|0x40000000;
 if(!original||original>UINTPTR_MAX-sizeof(*r))return 0;
 start=original+offsetof(struct slcli_windows_file_capture,security);
 if(r->original!=h||r->operation!=1||!r->observed||!r->completed||r->retained_native_io||
  !r->owner_sid_bytes||r->owner_sid_bytes>sizeof(r->owner_sid)||
  !r->identity_status||!r->standard_status||!r->attributes_status||!r->basic_status||
  !r->security_status||!r->owner_status||!r->sid_status||
  r->security_bytes>sizeof(r->security)||r->security_bytes<offsetof(SID,SubAuthority)||
  r->standard.Directory||r->standard.DeletePending||r->standard.EndOfFile.QuadPart<0||
  (r->attributes.FileAttributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT))||
  r->file_type!=FILE_TYPE_DISK||
  r->type_args[0]!=(uintptr_t)h||r->access_status||r->access_bytes!=sizeof(r->access_raw)||
  (r->access_raw.GrantedAccess&0x00120081)!=0x00120081||(r->access_raw.GrantedAccess&forbidden)||
  r->access_args[0]!=(uintptr_t)h||r->access_args[1]||
  r->access_args[2]!=original+offsetof(struct slcli_windows_file_capture,access_raw)||r->access_args[3]!=sizeof(r->access_raw)||
  r->access_args[4]!=original+offsetof(struct slcli_windows_file_capture,access_bytes)||!r->handle_status||
  (r->handle_flags&HANDLE_FLAG_INHERIT)||r->handle_args[0]!=(uintptr_t)h||
  r->handle_args[1]!=original+offsetof(struct slcli_windows_file_capture,handle_flags)||r->mode_status||r->mode_io.Status||
  r->mode_io.Information!=sizeof(r->mode)||!(r->mode&0x30)||
  r->mode_args[0]!=(uintptr_t)h||r->mode_args[1]!=original+offsetof(struct slcli_windows_file_capture,mode_io)||
  r->mode_args[2]!=original+offsetof(struct slcli_windows_file_capture,mode)||r->mode_args[3]!=sizeof(r->mode)||r->mode_args[4]!=16)return 0;
 if(sid<start||sid%_Alignof(SID)||sid-start>r->security_bytes- offsetof(SID,SubAuthority))return 0;
 offset=(size_t)(sid-start);
 bytes=offsetof(SID,SubAuthority)+((const SID *)(r->security+offset))->SubAuthorityCount*sizeof(DWORD);
 if(bytes!=r->owner_sid_bytes||bytes>r->security_bytes-offset||
  !slcli_equal(r->owner_sid,r->security+offset,bytes))return 0;
 return r->information_args[0][0]==(uintptr_t)h&&r->information_args[0][1]==FileIdInfo&&
  r->information_args[0][2]==original+offsetof(struct slcli_windows_file_capture,identity)&&r->information_args[0][3]==sizeof(r->identity)&&
  r->information_args[1][0]==(uintptr_t)h&&r->information_args[1][1]==FileStandardInfo&&
  r->information_args[1][2]==original+offsetof(struct slcli_windows_file_capture,standard)&&r->information_args[1][3]==sizeof(r->standard)&&
  r->information_args[2][0]==(uintptr_t)h&&r->information_args[2][1]==FileAttributeTagInfo&&
  r->information_args[2][2]==original+offsetof(struct slcli_windows_file_capture,attributes)&&r->information_args[2][3]==sizeof(r->attributes)&&
  r->information_args[3][0]==(uintptr_t)h&&r->information_args[3][1]==FileBasicInfo&&
  r->information_args[3][2]==original+offsetof(struct slcli_windows_file_capture,basic)&&r->information_args[3][3]==sizeof(r->basic)&&
  r->security_args[0]==(uintptr_t)h&&r->security_args[1]==OWNER_SECURITY_INFORMATION&&
  r->security_args[2]==start&&r->security_args[3]==sizeof(r->security)&&
  r->security_args[4]==original+offsetof(struct slcli_windows_file_capture,security_bytes);
}
int slcli_source_unit12_member_window_windows_matches_original(
 const struct slcli_source_unit12_member_window_windows *c){
 const struct slcli_source_unit12_native *t;const struct slcli_writer_capture_ranges_windows *f;
 const struct slcli_source_unit12_record *m;const slcli_input_capture *anchor;
 struct slcli_source_unit12_window geometry;
 uint64_t previous,limit;size_t payload,i;
 static const unsigned char magic[8]={'S','L','N','C','1','0','6','1'};
 if(!c||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||
  !c->started||c->failed||!c->observed||c->reserved_captures!=8)return 0;
 t=c->table;f=c->foundation;
 if(!slcli_source_unit12_native_matches_original(t))return 0;
 if(!t||!f||!slcli_arena_live_block(c->arena,t,sizeof(*t))||t->arena!=c->arena||
  t->failed||!t->table_observed||t->member_window_original!=c||
  !t->reader.table_complete||t->reader.failed||t->completed!=t->range_count||
  t->range_count<2||t->range_count>SIZE_MAX/sizeof(*t->ranges)||
  !slcli_arena_live_block(c->arena,t->ranges,t->range_count*sizeof(*t->ranges))||
  !c->member_range||c->member_range>=t->range_count||
  !slcli_arena_live_block(c->arena,f,sizeof(*f))||f->arena!=c->arena||
  !slcli_writer_capture_ranges_windows_matches_original(f)||
  f->extent->bytes>SIZE_MAX||
  (uintptr_t)f->extent->views[0]>UINTPTR_MAX-(size_t)f->extent->bytes||
  (uintptr_t)f->extent->views[1]>UINTPTR_MAX-(size_t)f->extent->bytes||
  f->extent->readback->deadline!=t->deadline||f->extent->readback->caller!=t->caller||
  c->capture_first_ordinal>f->attempted||8>f->attempted-c->capture_first_ordinal||
  !t->ranges[0].body||!slcli_arena_live_range(c->arena,t->ranges[0].body,sizeof(*t->ranges[0].body))||
  !slcli_input_deadline_ready(t->ranges[0].body)||!t->ranges[0].body->complete||
  !t->ranges[0].body->recheck_complete||t->ranges[0].body->failed||
  !t->ranges[0].body->capture_count)return 0;
 m=&t->ranges[c->member_range].member;anchor=&t->ranges[0].body->captures[0];
 if(!member_observe_original(anchor,t->original,(uintptr_t)anchor))return 0;
 if(!t->ranges[c->member_range].member_decoded||!t->ranges[c->member_range].decode_returned||
  !t->ranges[c->member_range].decode_result||!t->ranges[c->member_range].after_returned||
  !t->ranges[c->member_range].after_result||!t->ranges[c->member_range].after.complete||m->index!=c->member_range-1||
  m->offset>t->file_bytes||m->bytes>t->file_bytes-m->offset||
  c->member.index!=m->index||c->member.offset!=m->offset||c->member.bytes!=m->bytes||
  c->member.kind!=m->kind||c->member.source_project!=m->source_project||
  !slcli_equal(c->member.association,m->association,32)||!slcli_equal(c->member.capture,m->capture,32)||
  !slcli_equal(c->member.digest,m->digest,32)||c->source_offset!=m->offset)return 0;
 limit=f->extent->readback->creator->evidence_file->deadline_owner->record_maximum;
 if(!f->extent->readback->creator->evidence_file->deadline_owner->bq_profile||
  c->journal_record_maximum!=limit||sizeof(struct slcli_windows_file_capture)>16384-48)return 0;
 if(limit>16384)limit=16384;
 if(limit<=48+sizeof(struct slcli_windows_file_capture))return 0;
 payload=(size_t)(limit-48-sizeof(struct slcli_windows_file_capture));
 if(!slcli_source_unit12_window_geometry(m,t->file_bytes,0,payload,&geometry)||
  c->geometry.member_index!=geometry.member_index||c->geometry.member_offset!=geometry.member_offset||
  c->geometry.relative_offset!=geometry.relative_offset||c->geometry.source_offset!=geometry.source_offset||
  c->geometry.requested!=geometry.requested||c->source_offset!=geometry.source_offset||
  c->journal_payload_maximum!=payload||c->requested!=geometry.requested||
  c->returned_bytes[0]!=c->requested||c->returned_bytes[1]!=c->requested||
  c->storage_charge!=slcli_arena_allocation_bytes(sizeof(*c))||!c->arena->owner||
  c->storage_charge>c->arena->owner->charged||
  !slcli_equal(c->body,c->readback,c->requested)||
  c->full_member_bytes_compared!=(m->bytes==c->requested)||
  (c->full_member_bytes_compared&&!slcli_equal(c->digest,m->digest,32)))return 0;
 previous=t->ranges[t->range_count-1].after.tick;if(previous<f->after.tick)previous=f->after.tick;
 for(i=0;i<8;i++){
  size_t ordinal=c->capture_first_ordinal+i;unsigned lane=(unsigned)(i%4);
  const unsigned char *ro=(const unsigned char *)f->extent->views[1]+ordinal*f->stride;
  uintptr_t original=(uintptr_t)f->extent->views[0]+ordinal*f->stride+f->data_offset;
  const struct slcli_windows_file_capture *r=(const struct slcli_windows_file_capture *)(ro+f->data_offset);
  const unsigned char *output=i<4?c->body:c->readback;size_t bytes=lane==2?1:c->requested;
  uint64_t offset=lane==2?t->file_bytes:c->source_offset;uint32_t kind=(lane==0||lane==3)?1:2;
  /* Original native output addresses are observations, not dereferenced after
   * their writable view retires. Read only through the preborn SAME RO alias. */
  if((uintptr_t)c->captures[i]!=original||original>UINTPTR_MAX-sizeof(*r)||
   !slcli_equal(ro,magic,8)||member_u32(ro+8)!=f->version||
   member_u32(ro+12)!=sizeof(*r)||member_u64(ro+16)!=ordinal||
   member_u32(ro+24)!=1||member_u32(ro+28)!=kind||
   (f->version==2&&(member_u64(ro+32)!=f->payload_capacity||member_u64(ro+40))))return 0;
  if(lane==1&&!c->requested){
   if(!zero(r,sizeof(*r))||c->before_returned[i]||c->native_called[i]||c->native_returned[i]||
    c->after_returned[i]||c->before_result[i]||c->native_result[i]||c->after_result[i])return 0;
   continue;
  }
  if(!c->before_returned[i]||!c->before_result[i]||!c->clocks[i][0].complete||
   c->clocks[i][0].tick<previous||!c->native_called[i]||!c->native_returned[i]||!c->native_result[i]||
   !c->after_returned[i]||!c->after_result[i]||!c->clocks[i][1].complete||
   c->clocks[i][1].tick<c->clocks[i][0].tick||c->clocks[i][1].tick>=t->deadline->expires||
   r->original!=t->original||r->operation!=kind||!r->completed||r->retained_native_io||
   r->tick<c->clocks[i][0].tick||r->tick>c->clocks[i][1].tick)return 0;
  previous=c->clocks[i][1].tick;
  if(kind==1){if(!member_observe_original(r,t->original,original)||
    !slcli_input_original_same(anchor,r,t->file_bytes))return 0;}
  else {
   if(lane==2)output+=c->requested;
   if(!r->bytes_observed||r->requested!=bytes||r->offset!=offset||
    r->returned_bytes!=(lane==2?0:c->requested)||r->returned_bytes!=r->transferred_raw||
    r->status!=(uint32_t)r->read_status||
    (!r->read_status&&!(r->error_observed&&r->error==ERROR_HANDLE_EOF&&!r->transferred_raw))||
    !r->position_status||r->position_requested.QuadPart<0||r->position_returned.QuadPart<0||
    (uint64_t)r->position_requested.QuadPart!=offset||(uint64_t)r->position_returned.QuadPart!=offset||
    r->mode_status||r->mode_io.Status||r->mode_io.Information!=sizeof(r->mode)||!(r->mode&0x30)||
    r->mode_args[0]!=(uintptr_t)t->original||r->mode_args[1]!=original+offsetof(struct slcli_windows_file_capture,mode_io)||
    r->mode_args[2]!=original+offsetof(struct slcli_windows_file_capture,mode)||r->mode_args[3]!=sizeof(r->mode)||r->mode_args[4]!=16||
    r->read_args[0]!=(uintptr_t)t->original||r->read_args[1]!=(uintptr_t)output||r->read_args[2]!=bytes||
    r->read_args[3]!=original+offsetof(struct slcli_windows_file_capture,transferred_raw)||r->read_args[4])return 0;
  }
 }
 return c->finish_returned&&c->finish_result&&c->finish.complete&&c->finish.tick>=previous&&
  c->finish.tick<t->deadline->expires&&c->previous_tick==c->finish.tick;
}
#endif
