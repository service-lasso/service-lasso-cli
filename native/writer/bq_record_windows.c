#include "bq_record_windows.h"
#include <stddef.h>
#if defined(_WIN32)
static int zero(const void *p,size_t n){const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;}
static int apart(const void *a,size_t n,const void *b,size_t m){uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);}
static void w32(unsigned char *p,uint32_t v){unsigned i;for(i=0;i<4;i++)p[i]=(unsigned char)(v>>(8*i));}
static void w64(unsigned char *p,uint64_t v){unsigned i;for(i=0;i<8;i++)p[i]=(unsigned char)(v>>(8*i));}
static uint32_t r32(const unsigned char *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static uint64_t r64(const unsigned char *p){uint64_t n=0;unsigned i;for(i=0;i<8;i++)n|=(uint64_t)p[i]<<(8*i);return n;}
static int file_io_original(const struct slcli_windows_file_capture *r,HANDLE handle,
 const void *data,size_t bytes,uint64_t offset,uintptr_t original){
 return original&&original<=UINTPTR_MAX-sizeof(*r)&&r->requested==bytes&&r->offset==offset&&r->bytes_observed&&r->returned_bytes<=bytes&&
  r->returned_bytes==r->transferred_raw&&r->status==(uint32_t)r->read_status&&
  (r->read_status||(r->operation==2&&r->error_observed&&r->error==ERROR_HANDLE_EOF&&!r->transferred_raw))&&
  r->position_status&&r->position_requested.QuadPart>=0&&r->position_returned.QuadPart>=0&&
  (uint64_t)r->position_requested.QuadPart==offset&&(uint64_t)r->position_returned.QuadPart==offset&&
  !r->mode_status&&!r->mode_io.Status&&r->mode_io.Information==sizeof(r->mode)&&(r->mode&0x30)&&
  r->mode_args[0]==(uintptr_t)handle&&r->mode_args[1]==original+offsetof(struct slcli_windows_file_capture,mode_io)&&
  r->mode_args[2]==original+offsetof(struct slcli_windows_file_capture,mode)&&r->mode_args[3]==sizeof(r->mode)&&r->mode_args[4]==16&&
  r->read_args[0]==(uintptr_t)handle&&r->read_args[1]==(uintptr_t)data&&r->read_args[2]==bytes&&
  r->read_args[3]==original+offsetof(struct slcli_windows_file_capture,transferred_raw)&&!r->read_args[4];
}
static const struct slcli_windows_file_capture *capture_readonly_original(const struct slcli_writer_capture_ranges_windows *f,
 const struct slcli_windows_file_capture *capture,uint32_t kind,size_t requested){
 size_t i;static const unsigned char magic[8]={'S','L','N','C','1','0','6','1'};
 if(!f||!f->arena||!slcli_arena_live_block(f->arena,f,sizeof(*f))||
  !slcli_writer_capture_ranges_windows_matches_original(f)||
  !f->prepared||f->failed||f->attempted>f->capacity||!f->stride||
  f->data_offset<32||f->data_offset>f->stride||sizeof(*capture)>f->stride-f->data_offset||
  f->capacity>SIZE_MAX/f->stride||f->capacity*f->stride>f->extent->bytes||
  f->extent->bytes>SIZE_MAX||
  (uintptr_t)f->extent->views[0]>UINTPTR_MAX-(size_t)f->extent->bytes||
  (uintptr_t)f->extent->views[1]>UINTPTR_MAX-(size_t)f->extent->bytes)return 0;
 for(i=0;i<f->attempted;i++){
  uintptr_t original=(uintptr_t)f->extent->views[0]+i*f->stride+f->data_offset;
  const unsigned char *read=(const unsigned char *)f->extent->views[1]+i*f->stride;
  if(original!=(uintptr_t)capture)continue;
  if((f->version!=1&&f->version!=2)||!slcli_equal(read,magic,8)||r32(read+8)!=f->version||
   r32(read+12)!=sizeof(*capture)||r64(read+16)!=i||r32(read+24)!=1||r32(read+28)!=kind||
   (f->version==1?requested:(f->data_offset<48||r64(read+32)!=f->payload_capacity||
    requested>f->payload_capacity||r64(read+40)!=requested)))return 0;
  return (const struct slcli_windows_file_capture *)(read+f->data_offset);
 }return 0;
}
static int member_record_derive(const struct slcli_source_unit12_member_window_windows *p,
 unsigned lane,uint64_t sequence,uint64_t offset,struct slcli_writer_bq_member_record_windows *r){
 unsigned within=lane%4;size_t returned=within==1?p->requested:0;
 if(lane>=8||(within==1&&!p->requested)||!p->native_called[lane]||
  !p->native_returned[lane]||!p->native_result[lane]||!sequence||
  returned>UINT32_MAX||sizeof(*p->captures[lane])>UINT32_MAX||
  returned>SIZE_MAX-48-sizeof(*p->captures[lane]))return 0;
 r->capture=p->captures[lane];r->producer_lane=lane;r->sequence=sequence;r->journal_offset=offset;
 r->kind=within==0?1:(within==1?2:(within==2?3:4));
 r->requested=within==1?(uint32_t)p->requested:(within==2?1:0);r->returned=(uint32_t)returned;
 r->source_read_offset=within==1?p->source_offset:(within==2?p->table->file_bytes:0);
 r->payload=within==1?(lane<4?p->body:p->readback):0;
 r->record_bytes=48+sizeof(*r->capture)+returned;
 return r->record_bytes<=16384&&r->record_bytes<=p->journal_record_maximum&&
  r->record_bytes<=UINT64_MAX-offset;
}
static int member_roster(struct slcli_writer_bq_record_windows *c,int prepare){
 unsigned lane;size_t count=0;uint64_t offset=16;
 if(!slcli_source_unit12_member_window_windows_matches_original(c->producer))return 0;
 for(lane=0;lane<8;lane++){
  struct slcli_writer_bq_member_record_windows expected={0};
  struct slcli_writer_bq_member_record_windows *r;
  if(lane%4==1&&!c->producer->requested)continue;
  if(!member_record_derive(c->producer,lane,count+1,offset,&expected))return 0;
  r=&c->member_records[count];
  if(prepare){
   r->capture=expected.capture;r->payload=expected.payload;r->sequence=expected.sequence;
   r->journal_offset=expected.journal_offset;r->source_read_offset=expected.source_read_offset;
   r->record_bytes=expected.record_bytes;r->producer_lane=expected.producer_lane;
   r->kind=expected.kind;r->requested=expected.requested;r->returned=expected.returned;
  }
  else if(r->capture!=expected.capture||r->payload!=expected.payload||r->sequence!=expected.sequence||
   r->journal_offset!=expected.journal_offset||r->source_read_offset!=expected.source_read_offset||
   r->record_bytes!=expected.record_bytes||r->producer_lane!=expected.producer_lane||
   r->kind!=expected.kind||r->requested!=expected.requested||r->returned!=expected.returned)return 0;
  offset+=expected.record_bytes;count++;
 }
 if(offset>c->journal->creator->evidence_file->maximum)return 0;
 if(prepare){c->member_record_count=count;c->planned_journal_bytes=offset;return 1;}
 for(;count<8;count++)if(!zero(&c->member_records[count],sizeof(c->member_records[count])))return 0;
 return c->member_record_count==(c->producer->requested?8:6)&&c->planned_journal_bytes==offset;
}
static int source_control_original(const struct slcli_writer_bq_record_windows *c){
 const struct slcli_bq_source_control_sink_windows *s=c->source_control;
 if(!s)return !c->source_control_count&&!c->source_control_used;
 return slcli_arena_live_block(c->arena,s,sizeof(*s))&&s->arena==c->arena&&s->record_borrower==c&&
  s->ranges==c->foundation_control&&!s->first_ordinal&&s->claimed==c->source_control_count&&
  s->count==c->source_control_count&&s->used==c->source_control_used&&
  s->bodies==c->producer->table->bodies_original&&s->body_count==c->producer->table->range_count&&
  slcli_bq_source_control_sink_windows_matches_original(s)&&
  s->factory->inputs->deadline==c->journal->deadline&&s->factory->inputs->caller==c->journal->caller&&
  s->factory->inputs->caller_bytes==c->journal->caller_bytes;
}
static int same(struct slcli_writer_bq_record_windows *c){
 struct slcli_writer_windows_path *p=c->journal->creator;
 return p->bq_record_borrower==c&&slcli_writer_windows_readback_matches_original(c->journal)&&
  slcli_writer_capture_ranges_windows_matches_original(c->foundation)&&
  slcli_writer_capture_ranges_windows_matches_original(c->foundation_output)&&
  c->foundation_output!=c->foundation&&
  c->foundation_output->arena==c->arena&&c->foundation->arena==c->arena&&
  c->foundation_output->extent->backing_original&&
  c->foundation_output->extent->backing_original==c->foundation->extent->backing_original&&
  c->foundation_output->extent!=c->foundation->extent&&
  c->foundation_output->extent->readback==c->foundation->extent->readback&&
  c->foundation_control&&slcli_writer_capture_ranges_windows_matches_original(c->foundation_control)&&
  c->foundation_control->arena==c->arena&&c->foundation_control!=c->foundation&&
  c->foundation_control!=c->foundation_output&&
  c->foundation_control->extent!=c->foundation->extent&&
  c->foundation_control->extent!=c->foundation_output->extent&&
  c->foundation_control->extent->backing_original==c->foundation->extent->backing_original&&
  c->foundation_control->extent->readback==c->foundation->extent->readback&&
  source_control_original(c)&&member_roster(c,0)&&
  c->producer->foundation==c->foundation&&c->producer->table->member_window_original==c->producer&&
  c->journal->deadline==c->producer->table->deadline&&c->journal->caller==c->producer->table->caller&&
  c->foundation->extent->readback->deadline==c->journal->deadline&&
  c->foundation->extent->readback->caller==c->journal->caller&&
  p->evidence_file->deadline_owner->bq_profile==
   c->foundation->extent->readback->creator->evidence_file->deadline_owner->bq_profile&&
  c->io_attempted<=c->io_capacity;
}
static int io(struct slcli_writer_bq_record_windows *c,uint32_t kind,HANDLE handle,
 const unsigned char *source,unsigned char *destination,size_t bytes,uint64_t offset){
 struct slcli_writer_bq_record_io_windows *r;struct slcli_windows_file_capture *raw;
 if(!same(c)||c->io_attempted>=c->io_capacity||kind<1||kind>4||
  (kind==2&&(!destination||!bytes||bytes>16384))||
  (kind==3&&(!source||!bytes||bytes>16384))||
  ((kind==1||kind==4)&&(source||destination||bytes||offset))||
  (kind==2?handle!=c->journal->original:
   (kind==1?handle!=c->journal->original&&handle!=c->journal->creator->original:
    handle!=c->journal->creator->original)))return 0;
 r=&c->io[c->io_attempted++];r->kind=kind;r->handle_original=handle;
 r->write_original=source;r->comparison_original=destination;
 r->bytes_original=bytes;r->offset_original=offset;
 r->before_result=slcli_native_clock_read(&r->before);r->before_returned=1;
 if(!r->before_result||r->before.tick<c->previous_tick||r->before.tick>=c->journal->deadline->expires||
  !slcli_evidence_deadline_precede(c->journal->creator->evidence_file,&r->before)||
  !slcli_evidence_deadline_precede(c->foundation->extent->readback->creator->evidence_file,&r->before))return 0;
 if(kind==2){
  r->payload_requested=bytes;
  raw=slcli_writer_capture_ranges_windows_claim_read_original(c->foundation,bytes,
   &r->payload_original,&r->payload_readonly);
 }else raw=slcli_writer_capture_ranges_windows_claim_original(c->foundation,kind);
 r->capture=raw;if(!raw)return 0;
 r->called=1;
 if(kind==1)r->result=handle==c->journal->original?slcli_windows_source_observe(handle,raw):
  slcli_windows_file_observe(handle,raw);
 else if(kind==2)r->result=slcli_windows_file_read(handle,r->payload_original,(DWORD)bytes,offset,raw);
 else if(kind==3)r->result=slcli_windows_file_write(handle,source,(DWORD)bytes,offset,raw);
 else r->result=slcli_windows_file_flush(handle,raw);
 r->returned=1;
 r->after_result=slcli_native_clock_read(&r->after);r->after_returned=1;
 if(!r->after_result||r->after.tick<r->before.tick||r->after.tick<raw->tick||
  r->after.tick>=c->journal->deadline->expires||!same(c)||
  !slcli_evidence_deadline_precede(c->journal->creator->evidence_file,&r->after)||
  !slcli_evidence_deadline_precede(c->foundation->extent->readback->creator->evidence_file,&r->after))return 0;
 c->previous_tick=r->after.tick;
 if(!r->result||!raw->completed||raw->retained_native_io||raw->tick<r->before.tick)return 0;
 if(kind==2){size_t i;
  if(!destination||!raw->bytes_observed||raw->returned_bytes>bytes||
   !slcli_equal(r->payload_original,r->payload_readonly,(size_t)raw->returned_bytes))return 0;
  /* Returned bytes remain in their one-use106 lane, including after local
   * comparison fails. The local window is only a charged comparison copy. */
  for(i=0;i<(size_t)raw->returned_bytes;i++)destination[i]=r->payload_readonly[i];
 }
 return 1;
}
static int observe(struct slcli_writer_bq_record_windows *c,HANDLE h,uint64_t bytes){
 const struct slcli_windows_file_capture *r,*original=h==c->journal->original?
  &c->journal->observed:&c->journal->creator->mode.file;
 if(!io(c,1,h,0,0,0,0))return 0;r=c->io[c->io_attempted-1].capture;
 return r->standard.EndOfFile.QuadPart>=0&&(uint64_t)r->standard.EndOfFile.QuadPart==bytes&&
  r->access_status==0&&r->access_bytes==sizeof(r->access_raw)&&
  r->access_raw.GrantedAccess==original->access_raw.GrantedAccess&&
  r->handle_flags==original->handle_flags&&
  r->identity.VolumeSerialNumber==original->identity.VolumeSerialNumber&&
  slcli_equal(r->identity.FileId.Identifier,original->identity.FileId.Identifier,16);
}
static int write_all(struct slcli_writer_bq_record_windows *c,const unsigned char *p,size_t n,
 uint64_t start,size_t *done){
 HANDLE h=c->journal->creator->original;
 while(*done<n){const struct slcli_windows_file_capture *r;
  if(!observe(c,h,start+*done)||!io(c,3,h,p+*done,0,n-*done,start+*done))return 0;
  r=c->io[c->io_attempted-1].capture;
  if(!r->bytes_observed||!r->returned_bytes||r->returned_bytes>n-*done)return 0;
  *done+=(size_t)r->returned_bytes;
  if(!observe(c,h,start+*done))return 0;
 }return 1;
}
static int read_all(struct slcli_writer_bq_record_windows *c,unsigned char *p,const unsigned char *expected,
 size_t n,uint64_t start,size_t *done){
 while(*done<n){const struct slcli_windows_file_capture *r;
  if(!io(c,2,c->journal->original,0,p+*done,n-*done,start+*done))return 0;
  r=c->io[c->io_attempted-1].capture;
  if(!r->bytes_observed||!r->returned_bytes||r->returned_bytes>n-*done||
   !slcli_equal(p+*done,expected+*done,(size_t)r->returned_bytes))return 0;
  *done+=(size_t)r->returned_bytes;
 }return 1;
}
static int foundation_readback(struct slcli_writer_bq_record_windows *);
static int foundation_unmap(struct slcli_writer_bq_record_windows *);
static int control_readback(struct slcli_writer_bq_record_windows *);
int slcli_writer_bq_record_windows_first_original(struct slcli_writer_bq_record_windows *c,
 struct slcli_arena *a,struct slcli_writer_windows_readback_original *journal,
 struct slcli_source_unit12_member_window_windows *producer,struct slcli_writer_capture_ranges_windows *foundation,
 struct slcli_writer_capture_ranges_windows *foundation_output,
 struct slcli_writer_capture_ranges_windows *foundation_control,
 struct slcli_bq_source_control_sink_windows *source_control,
 struct slcli_writer_bq_record_io_windows *rows,size_t count,
 struct slcli_writer_bq_record_io_windows *foundation_rows,size_t foundation_count,
 struct slcli_writer_bq_record_io_windows *control_rows,size_t control_count){
 const struct slcli_windows_file_capture *original;size_t i,n=48+sizeof(*original),prefix_bound,chunks;uint64_t previous;
 /* Minimum actual calls: two writes each bracketed by two observations,
  * one flush, readonly before, two reads, zero-EOF, readonly after. Partial
  * native returns consume additional preowned rows, never late allocation. */
 static const unsigned char magic[8]={'S','L','S','A','J','0','0','1'};
 if(!a||!c||!journal||!producer||!foundation||!foundation_output||!rows||!foundation_rows||
  count<(2*3+1+1+2+1+1)||count>SIZE_MAX/sizeof(*rows)||
  !foundation_count||foundation_count>SIZE_MAX/sizeof(*foundation_rows)||
  n>16384||!slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !slcli_arena_live_block(a,journal,sizeof(*journal))||!slcli_writer_windows_readback_matches_original(journal)||
  !slcli_arena_live_block(a,producer,sizeof(*producer))||!producer->observed||producer->failed||
  producer->capture_first_ordinal||
  !producer->table||!slcli_arena_live_block(a,producer->table,sizeof(*producer->table))||
  !slcli_arena_live_block(a,foundation,sizeof(*foundation))||foundation!=producer->foundation||
  foundation->version!=2||foundation->payload_capacity<16||foundation->attempted!=8||foundation->write_close_bq_original||
  !foundation->extent||!slcli_arena_live_block(a,foundation->extent,sizeof(*foundation->extent))||
  !slcli_writer_capture_ranges_windows_matches_original(foundation)||
  !slcli_arena_live_block(a,foundation_output,sizeof(*foundation_output))||
  !slcli_writer_capture_ranges_windows_matches_original(foundation_output)||foundation_output==foundation||foundation_output->arena!=a||
  foundation_output->version!=2||!foundation_output->payload_capacity||foundation_output->attempted||foundation_output->write_close_bq_original||
  !foundation_output->extent->backing_original||
  foundation_output->extent->backing_original!=foundation->extent->backing_original||
  foundation_output->extent==foundation->extent||
  foundation_output->extent->readback!=foundation->extent->readback||
  !slcli_arena_live_block(a,rows,count*sizeof(*rows))||!zero(rows,count*sizeof(*rows))||
  !slcli_arena_live_block(a,foundation_rows,foundation_count*sizeof(*foundation_rows))||
  !zero(foundation_rows,foundation_count*sizeof(*foundation_rows))||
  !apart(foundation_rows,foundation_count*sizeof(*foundation_rows),rows,count*sizeof(*rows))||
  !apart(foundation_rows,foundation_count*sizeof(*foundation_rows),c,sizeof(*c))||
  !apart(foundation_rows,foundation_count*sizeof(*foundation_rows),producer,sizeof(*producer))||
  !apart(foundation_rows,foundation_count*sizeof(*foundation_rows),journal,sizeof(*journal))||
  !apart(foundation_rows,foundation_count*sizeof(*foundation_rows),foundation,sizeof(*foundation))||
  !apart(c,sizeof(*c),rows,count*sizeof(*rows))||!apart(c,sizeof(*c),producer,sizeof(*producer))||
  !apart(c,sizeof(*c),journal,sizeof(*journal))||!apart(c,sizeof(*c),foundation,sizeof(*foundation))||
  !apart(rows,count*sizeof(*rows),producer,sizeof(*producer))||!apart(rows,count*sizeof(*rows),journal,sizeof(*journal))||
  !apart(rows,count*sizeof(*rows),foundation,sizeof(*foundation))||
  !apart(foundation_output,sizeof(*foundation_output),c,sizeof(*c))||
  !apart(foundation_output,sizeof(*foundation_output),rows,count*sizeof(*rows))||
  !apart(foundation_output,sizeof(*foundation_output),foundation_rows,foundation_count*sizeof(*foundation_rows))||
  !apart(foundation_output,sizeof(*foundation_output),producer,sizeof(*producer))||
  !apart(foundation_output,sizeof(*foundation_output),journal,sizeof(*journal))||
  !apart(foundation_output,sizeof(*foundation_output),foundation,sizeof(*foundation))||
  journal->creator->bq_record_borrower||
  !journal->creator->evidence_file->deadline_owner->bq_profile||
  journal->creator->evidence_file->count||journal->observed.standard.EndOfFile.QuadPart||
  foundation->attempted>foundation->capacity||count>foundation->capacity-foundation->attempted||
  foundation_count>=foundation_output->capacity-foundation_output->attempted||
  n>foundation->payload_capacity||n>journal->creator->evidence_file->deadline_owner->record_maximum||
  journal->creator->evidence_file->maximum<16+n)return 0;
 prefix_bound=(foundation->attempted+count)*foundation->stride;
 chunks=prefix_bound/foundation_output->payload_capacity+(prefix_bound%foundation_output->payload_capacity!=0);
 /* This is the no-partial minimum, not a claim that short native returns
  * have that maximum. Supplied finite foundation_count bounds ALL attempts;
  * every short read consumes a row, and exhaustion preserves its prefix. */
 if(!prefix_bound||chunks>SIZE_MAX-5||foundation_count<chunks+5)return 0;
 /* All third-unit storage and the entire second readback attempt schedule
  * precede first105 IO. Exact arena blocks cannot overlap interior ranges. */
 if(!foundation_control||!control_rows||!control_count||control_count>SIZE_MAX/sizeof(*control_rows)||
  !slcli_arena_live_block(a,foundation_control,sizeof(*foundation_control))||
  !slcli_writer_capture_ranges_windows_matches_original(foundation_control)||
  foundation_control->arena!=a||foundation_control==foundation||foundation_control==foundation_output||
  foundation_control->version!=2||!foundation_control->payload_capacity||
  foundation_control->write_close_bq_original||
  foundation_control->extent==foundation->extent||foundation_control->extent==foundation_output->extent||
  foundation_control->extent->backing_original!=foundation->extent->backing_original||
  foundation_control->extent->readback!=foundation->extent->readback||
  !slcli_arena_live_block(a,control_rows,control_count*sizeof(*control_rows))||
  !zero(control_rows,control_count*sizeof(*control_rows))||foundation_control->attempted>foundation_control->capacity||
  control_count>foundation_control->capacity-foundation_control->attempted)return 0;
 if(source_control){
  if(!slcli_arena_live_block(a,source_control,sizeof(*source_control))||source_control->record_borrower||
   source_control->arena!=a||source_control->ranges!=foundation_control||source_control->first_ordinal||
   source_control->claimed!=foundation_control->attempted||
   source_control->bodies!=producer->table->bodies_original||source_control->body_count!=producer->table->range_count||
   !slcli_bq_source_control_sink_windows_matches_original(source_control)||
   source_control->factory->inputs->deadline!=journal->deadline||
   source_control->factory->inputs->caller!=journal->caller)return 0;
 }else if(foundation_control->attempted)return 0;
 {const void *blocks[7]={c,producer,journal,foundation,foundation_output,rows,foundation_rows};
  size_t sizes[7]={sizeof(*c),sizeof(*producer),sizeof(*journal),sizeof(*foundation),sizeof(*foundation_output),
   count*sizeof(*rows),foundation_count*sizeof(*foundation_rows)};
  if(!apart(foundation_control,sizeof(*foundation_control),control_rows,control_count*sizeof(*control_rows)))return 0;
  for(i=0;i<7;i++)if(!apart(foundation_control,sizeof(*foundation_control),blocks[i],sizes[i])||
   !apart(control_rows,control_count*sizeof(*control_rows),blocks[i],sizes[i]))return 0;
 }
 prefix_bound=(foundation_count+1)*foundation_output->stride;
 chunks=prefix_bound/foundation_control->payload_capacity+(prefix_bound%foundation_control->payload_capacity!=0);
 if(chunks>SIZE_MAX-6||control_count<chunks+6)return 0;
 original=capture_readonly_original(foundation,producer->captures[0],1,0);
 if(!original||!producer->native_called[0]||!producer->native_returned[0]||!producer->native_result[0]||
  original->operation!=1||!original->completed||original->retained_native_io||
  original->original!=producer->table->original)return 0;
 if(journal->observed.identity.VolumeSerialNumber==foundation->extent->readback->observed.identity.VolumeSerialNumber&&
  slcli_equal(journal->observed.identity.FileId.Identifier,
   foundation->extent->readback->observed.identity.FileId.Identifier,16))return 0;
 c->started=1;c->failed=1;c->append_pending=1;c->arena=a;c->journal=journal;c->producer=producer;
 c->foundation=foundation;c->io=rows;c->io_capacity=count;c->record_bytes=n;
 c->foundation_output=foundation_output;c->foundation_output_first_ordinal=foundation_output->attempted;
 c->foundation_control=foundation_control;c->control_io=control_rows;c->control_io_capacity=control_count;
 c->source_control=source_control;c->source_control_count=foundation_control->attempted;
 if(source_control){c->source_control_used=source_control->used;source_control->record_borrower=c;}
 c->foundation_io=foundation_rows;c->foundation_io_capacity=foundation_count;
 c->capture_first_ordinal=foundation->attempted;
 journal->creator->bq_record_borrower=c;
 /* All actual producer calls are retained before the first105 effect. This
  * does not authorize writing record2 while record1 is APPEND_PENDING. */
 if(!member_roster(c,1))return 0;
 previous=producer->finish.tick;if(previous<journal->observe_after.tick)previous=journal->observe_after.tick;
 if(previous<foundation->after.tick)previous=foundation->after.tick;c->previous_tick=previous;
 if(c->previous_tick<foundation_output->after.tick)c->previous_tick=foundation_output->after.tick;
 if(c->previous_tick<foundation_control->after.tick)c->previous_tick=foundation_control->after.tick;
 if(!same(c))return 0;
 for(i=0;i<8;i++)c->header[i]=magic[i];w32(c->header+8,1);
 w32(c->write_window,(uint32_t)c->member_records[0].record_bytes);
 w32(c->write_window+4,c->member_records[0].kind);w64(c->write_window+8,c->member_records[0].sequence);
 w64(c->write_window+16,producer->member.offset);
 w32(c->write_window+40,(uint32_t)sizeof(*original));
 for(i=0;i<sizeof(*original);i++)c->write_window[48+i]=((const unsigned char *)original)[i];
 if(!write_all(c,c->header,16,0,&c->header_written)||
  !write_all(c,c->write_window,n,16,&c->record_written)||
  !io(c,4,journal->creator->original,0,0,0,0)||!observe(c,journal->original,16+n)||
  !read_all(c,c->header_readback,c->header,16,0,&c->header_read)||
  !read_all(c,c->readback_window,c->write_window,n,16,&c->record_read)||
  !io(c,2,journal->original,0,&c->eof,1,16+n))return 0;
 if(!c->io[c->io_attempted-1].capture->bytes_observed||c->io[c->io_attempted-1].capture->returned_bytes||
  !observe(c,journal->original,16+n))return 0;
 /* Native IO observation is not a disposition. All buffers/handles/captures
  * remain live, append_pending stays set, no next-record or reuse API exists. */
 c->io_observed=1;c->failed=0;
 c->validation_before_result=slcli_native_clock_read(&c->validation_before);c->validation_before_returned=1;
 if(!c->validation_before_result||c->validation_before.tick<c->previous_tick||
  c->validation_before.tick>=journal->deadline->expires||
  !slcli_evidence_deadline_precede(journal->creator->evidence_file,&c->validation_before)||
  !slcli_evidence_deadline_precede(foundation->extent->readback->creator->evidence_file,&c->validation_before)){
  c->failed=1;return 0;
 }
 c->validation_called=1;
 c->validation_result=slcli_writer_bq_record_windows_matches_original(c);c->validation_returned=1;
 c->validation_after_result=slcli_native_clock_read(&c->validation_after);c->validation_after_returned=1;
 if(!c->validation_result||!c->validation_after_result||
  c->validation_after.tick<c->validation_before.tick||c->validation_after.tick>=journal->deadline->expires||
  !slcli_evidence_deadline_precede(journal->creator->evidence_file,&c->validation_after)||
  !slcli_evidence_deadline_precede(foundation->extent->readback->creator->evidence_file,&c->validation_after)){
  c->failed=1;return 0;
 }
 if(!foundation_readback(c)||!foundation_unmap(c)||!control_readback(c)){c->failed=1;return 0;}return 1;
}
int slcli_writer_bq_record_windows_matches_original(struct slcli_writer_bq_record_windows *c){
 size_t i,hw=0,rw=0,hr=0,rr=0,n;uint32_t stage=0;uint64_t previous;
 const struct slcli_windows_file_capture *producer;
 static const unsigned char magic[8]={'S','L','S','A','J','0','0','1'};
 if(!c||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||
  !c->started||c->failed||!c->io_observed||!c->append_pending||!same(c)||
  !c->io_capacity||c->io_capacity>SIZE_MAX/sizeof(*c->io)||
  !slcli_arena_live_block(c->arena,c->io,c->io_capacity*sizeof(*c->io))||
  c->foundation->version!=2||c->capture_first_ordinal>c->foundation->capacity||
  c->io_attempted>c->foundation->capacity-c->capture_first_ordinal||
  c->foundation->attempted<c->capture_first_ordinal+c->io_attempted)return 0;
 producer=capture_readonly_original(c->foundation,c->producer->captures[0],1,0);n=48+sizeof(*producer);
 if(!producer||!c->producer->native_called[0]||
  !c->producer->native_returned[0]||!c->producer->native_result[0]||producer->operation!=1||
  !producer->completed||producer->retained_native_io||producer->original!=c->producer->table->original||
  c->record_bytes!=n||n>16384||
  n>c->foundation->payload_capacity||!slcli_equal(c->header,magic,8)||r32(c->header+8)!=1||r32(c->header+12)||
  r32(c->write_window)!=n||r32(c->write_window+4)!=1||r64(c->write_window+8)!=1||
  r64(c->write_window+16)!=c->producer->member.offset||r64(c->write_window+24)||
  r32(c->write_window+32)||r32(c->write_window+36)||r32(c->write_window+40)!=sizeof(*producer)||
  r32(c->write_window+44)||!slcli_equal(c->write_window+48,(const unsigned char *)producer,sizeof(*producer)))return 0;
 previous=c->producer->finish.tick;
 if(previous<c->journal->observe_after.tick)previous=c->journal->observe_after.tick;
 if(previous<c->foundation->after.tick)previous=c->foundation->after.tick;
 if(previous<c->foundation_output->after.tick)previous=c->foundation_output->after.tick;
 if(previous<c->foundation_control->after.tick)previous=c->foundation_control->after.tick;
 for(i=0;i<c->io_attempted;i++){
  const struct slcli_writer_bq_record_io_windows *r=&c->io[i];
  const struct slcli_windows_file_capture *raw,*identity;
  HANDLE handle;const unsigned char *source=0,*expected=0;unsigned char *comparison=0;
  size_t bytes=0,returned;uint64_t offset=0,observed_bytes=0;uint32_t kind;
  if(stage>=12)return 0;
  kind=(stage==1||stage==4)?3:(stage==6?4:((stage>=8&&stage<=10)?2:1));
  handle=stage<=6?c->journal->creator->original:c->journal->original;
  if(stage==1){source=c->header+hw;bytes=16-hw;offset=hw;}
  if(stage==4){source=c->write_window+rw;bytes=n-rw;offset=16+rw;}
  if(stage==8){expected=c->header+hr;comparison=c->header_readback+hr;bytes=16-hr;offset=hr;}
  if(stage==9){expected=c->write_window+rr;comparison=c->readback_window+rr;bytes=n-rr;offset=16+rr;}
  if(stage==10){comparison=&c->eof;bytes=1;offset=16+n;}
  raw=capture_readonly_original(c->foundation,r->capture,kind,kind==2?bytes:0);
  if(stage<=2)observed_bytes=hw;else if(stage<=5)observed_bytes=16+rw;else observed_bytes=16+n;
  if(!raw||(uintptr_t)r->capture!=(uintptr_t)c->foundation->extent->views[0]+
    (c->capture_first_ordinal+i)*c->foundation->stride+c->foundation->data_offset||
   r->kind!=kind||r->handle_original!=handle||r->write_original!=source||r->comparison_original!=comparison||
   r->bytes_original!=bytes||r->offset_original!=offset||
   !r->before_returned||!r->before_result||!r->before.complete||r->before.tick<previous||
   !r->called||!r->returned||!r->result||!r->after_returned||!r->after_result||!r->after.complete||
   r->after.tick<r->before.tick||r->after.tick>=c->journal->deadline->expires||
   raw->original!=handle||raw->operation!=kind||
   !raw->completed||raw->retained_native_io||raw->tick<r->before.tick||raw->tick>r->after.tick)return 0;
  previous=r->after.tick;
  if(kind!=2&&(r->payload_original||r->payload_readonly||r->payload_requested))return 0;
  if(kind==1){
   identity=handle==c->journal->original?&c->journal->observed:&c->journal->creator->mode.file;
   if(raw->standard.EndOfFile.QuadPart<0||(uint64_t)raw->standard.EndOfFile.QuadPart!=observed_bytes||
    raw->access_status||raw->access_bytes!=sizeof(raw->access_raw)||
    raw->access_raw.GrantedAccess!=identity->access_raw.GrantedAccess||raw->handle_flags!=identity->handle_flags||
    raw->identity.VolumeSerialNumber!=identity->identity.VolumeSerialNumber||
    !slcli_equal(raw->identity.FileId.Identifier,identity->identity.FileId.Identifier,16))return 0;
  }else if(kind==2||kind==3){
   if(!file_io_original(raw,handle,kind==2?(const void *)r->payload_original:(const void *)source,bytes,offset,(uintptr_t)r->capture))return 0;
   returned=(size_t)raw->returned_bytes;
   if(kind==2){
    size_t index=c->capture_first_ordinal+i;
    if(r->payload_requested!=bytes||(uintptr_t)r->payload_original!=(uintptr_t)c->foundation->extent->views[0]+index*c->foundation->stride+c->foundation->payload_offset||
     r->payload_readonly!=(const unsigned char *)c->foundation->extent->views[1]+index*c->foundation->stride+c->foundation->payload_offset||
     raw->read_args[1]!=(uintptr_t)r->payload_original||
     (expected&&!slcli_equal(r->payload_readonly,expected,returned))||
     (comparison&&!slcli_equal(comparison,r->payload_readonly,returned)))return 0;
   }else if(raw->read_args[1]!=(uintptr_t)source||r->payload_original||r->payload_readonly||r->payload_requested)return 0;
   if(stage==10){if(returned)return 0;}
   else if(!returned)return 0;
   if(stage==1)hw+=returned;if(stage==4)rw+=returned;if(stage==8)hr+=returned;if(stage==9)rr+=returned;
  }else if(raw->read_args[0]!=(uintptr_t)handle||r->payload_original||r->payload_readonly||r->payload_requested)return 0;
  if(stage==2)stage=hw<16?0:3;
  else if(stage==5)stage=rw<n?3:6;
  else if(stage==8)stage=hr<16?8:9;
  else if(stage==9)stage=rr<n?9:10;
  else stage++;
 }
 return stage==12&&hw==16&&rw==n&&hr==16&&rr==n&&
  c->header_written==hw&&c->record_written==rw&&c->header_read==hr&&c->record_read==rr&&c->previous_tick==previous;
}
static int foundation_operation(struct slcli_writer_bq_record_windows *c,uint32_t kind,
 uint64_t offset,size_t bytes,const unsigned char *expected){
 struct slcli_writer_capture_ranges_windows *f=c->foundation_output;
 struct slcli_writer_capture_extent_windows *e=c->foundation->extent;
 struct slcli_writer_bq_record_io_windows *r;struct slcli_windows_file_capture *raw;
 HANDLE handle=(kind==4||kind==5)?e->readback->creator->original:e->readback->original;
 if(!same(c)||c->foundation->attempted!=c->foundation_prefix_bytes/c->foundation->stride||
  c->foundation_io_attempted>=c->foundation_io_capacity||
  (kind!=1&&kind!=2&&kind!=4&&kind!=5)||
  (kind==2&&(!bytes||bytes>f->payload_capacity))||
  (kind==5&&(offset||expected||!bytes||bytes!=c->foundation_prefix_bytes||bytes>e->bytes))||
  ((kind==1||kind==4)&&(offset||bytes||expected)))return 0;
 r=&c->foundation_io[c->foundation_io_attempted++];r->kind=kind;r->handle_original=handle;
 r->offset_original=offset;r->bytes_original=bytes;r->write_original=expected;
 r->before_result=slcli_native_clock_read(&r->before);r->before_returned=1;
 if(!r->before_result||r->before.tick<c->foundation_previous_tick||r->before.tick>=c->journal->deadline->expires||
  !slcli_evidence_deadline_precede(c->journal->creator->evidence_file,&r->before)||
  !slcli_evidence_deadline_precede(e->readback->creator->evidence_file,&r->before))return 0;
 if(kind==2){r->payload_requested=bytes;
  raw=slcli_writer_capture_ranges_windows_claim_read_original(f,bytes,&r->payload_original,&r->payload_readonly);
 }else raw=slcli_writer_capture_ranges_windows_claim_original(f,kind);
 r->capture=raw;if(!raw)return 0;r->called=1;
 if(kind==5)r->result=slcli_windows_file_flush_mapped(handle,e->views[0],bytes,raw);
 else if(kind==4)r->result=slcli_windows_file_flush(handle,raw);
 else if(kind==1)r->result=slcli_windows_source_observe(handle,raw);
 else r->result=slcli_windows_file_read(handle,r->payload_original,(DWORD)bytes,offset,raw);
 r->returned=1;r->after_result=slcli_native_clock_read(&r->after);r->after_returned=1;
 if(!r->after_result||r->after.tick<r->before.tick||r->after.tick>=c->journal->deadline->expires||
  !slcli_evidence_deadline_precede(c->journal->creator->evidence_file,&r->after)||
  !slcli_evidence_deadline_precede(e->readback->creator->evidence_file,&r->after)||
  !r->result||!raw->completed||raw->retained_native_io||raw->tick<r->before.tick||raw->tick>r->after.tick||!same(c))return 0;
 c->foundation_previous_tick=r->after.tick;
 if(kind==1){const struct slcli_windows_file_capture *identity=&e->readback->observed;
  return raw->standard.EndOfFile.QuadPart>=0&&(uint64_t)raw->standard.EndOfFile.QuadPart==e->file_bytes&&
   !raw->access_status&&raw->access_bytes==sizeof(raw->access_raw)&&
   raw->access_raw.GrantedAccess==identity->access_raw.GrantedAccess&&raw->handle_flags==identity->handle_flags&&
   raw->identity.VolumeSerialNumber==identity->identity.VolumeSerialNumber&&
   slcli_equal(raw->identity.FileId.Identifier,identity->identity.FileId.Identifier,16);
 }
 if(kind==2){
  if(!raw->bytes_observed||raw->returned_bytes>bytes||
   !slcli_equal(r->payload_original,r->payload_readonly,(size_t)raw->returned_bytes))return 0;
  if(expected)return raw->returned_bytes&&slcli_equal(expected,r->payload_readonly,(size_t)raw->returned_bytes);
  return !raw->returned_bytes;
 }
 return 1;
}
static int foundation_matches(struct slcli_writer_bq_record_windows *c){
 struct slcli_writer_capture_ranges_windows *f=c->foundation_output;
 struct slcli_writer_capture_extent_windows *e=c->foundation->extent;
 const struct slcli_windows_file_capture *identity=&e->readback->observed;
 size_t i,read=0,first;uint32_t stage=0;uint64_t previous=c->validation_after.tick;
 if(!same(c)||!c->foundation_started||!c->foundation_prefix_bytes||
  c->foundation_prefix_bytes%c->foundation->stride||c->foundation_prefix_bytes>e->bytes||
  c->foundation_io_attempted>c->foundation_io_capacity||
  c->foundation_io_capacity>SIZE_MAX/sizeof(*c->foundation_io)||
  !slcli_arena_live_block(c->arena,c->foundation_io,c->foundation_io_capacity*sizeof(*c->foundation_io)))return 0;
 first=c->foundation_output_first_ordinal;
 if(first||c->foundation_prefix_bytes/c->foundation->stride!=c->capture_first_ordinal+c->io_attempted||
  c->foundation->attempted!=c->capture_first_ordinal+c->io_attempted||
  first>f->capacity||c->foundation_io_attempted>f->capacity-first||
  f->attempted<first+c->foundation_io_attempted)return 0;
 for(i=0;i<c->foundation_io_attempted;i++){
  const struct slcli_writer_bq_record_io_windows *r=&c->foundation_io[i];
  const struct slcli_windows_file_capture *raw;uint32_t kind;
  HANDLE handle;size_t bytes=0,returned;uint64_t offset=0;const unsigned char *expected=0;
  if(stage>=6)return 0;kind=stage==0?5:(stage==1?4:((stage==3||stage==4)?2:1));
  handle=stage<2?e->readback->creator->original:e->readback->original;
  if(stage==0)bytes=c->foundation_prefix_bytes;
  if(stage==3){bytes=c->foundation_prefix_bytes-read;if(bytes>f->payload_capacity)bytes=f->payload_capacity;
   offset=e->file_offset+read;expected=(const unsigned char *)e->views[1]+read;}
  if(stage==4){bytes=1;offset=e->file_bytes;}
  raw=capture_readonly_original(f,r->capture,kind,kind==2?bytes:0);
  if(!raw||(uintptr_t)r->capture!=(uintptr_t)f->extent->views[0]+(first+i)*f->stride+f->data_offset||
   r->kind!=kind||r->handle_original!=handle||r->bytes_original!=bytes||r->offset_original!=offset||r->write_original!=expected||
   r->comparison_original||!r->before_returned||!r->before_result||!r->before.complete||r->before.tick<previous||
   !r->called||!r->returned||!r->result||!r->after_returned||!r->after_result||!r->after.complete||
   r->after.tick<r->before.tick||r->after.tick>=c->journal->deadline->expires||
   raw->original!=handle||raw->operation!=kind||
   !raw->completed||raw->retained_native_io||raw->tick<r->before.tick||raw->tick>r->after.tick)return 0;
  previous=r->after.tick;
  if(kind!=2&&(r->payload_original||r->payload_readonly||r->payload_requested))return 0;
  if(kind==5){if(raw->view_flush_address!=(uintptr_t)e->views[0]||raw->view_flush_bytes!=bytes||
    !raw->view_flush_called||!raw->view_flush_returned||!raw->view_flush_status||raw->status!=(uint32_t)raw->view_flush_status)return 0;
  }else if(kind==4){if(raw->read_args[0]!=(uintptr_t)handle)return 0;
  }else if(kind==1){
   if(raw->standard.EndOfFile.QuadPart<0||(uint64_t)raw->standard.EndOfFile.QuadPart!=e->file_bytes||
    raw->access_status||raw->access_bytes!=sizeof(raw->access_raw)||
    raw->access_raw.GrantedAccess!=identity->access_raw.GrantedAccess||raw->handle_flags!=identity->handle_flags||
    raw->identity.VolumeSerialNumber!=identity->identity.VolumeSerialNumber||
    !slcli_equal(raw->identity.FileId.Identifier,identity->identity.FileId.Identifier,16))return 0;
  }else{
   if(r->payload_requested!=bytes||(uintptr_t)r->payload_original!=(uintptr_t)f->extent->views[0]+(first+i)*f->stride+f->payload_offset||
    r->payload_readonly!=(const unsigned char *)f->extent->views[1]+(first+i)*f->stride+f->payload_offset||
    !file_io_original(raw,handle,r->payload_original,bytes,offset,(uintptr_t)r->capture))return 0;
   returned=(size_t)raw->returned_bytes;
   if((expected&&(!returned||!slcli_equal(expected,r->payload_readonly,returned)))||(!expected&&returned))return 0;
   if(stage==3)read+=returned;
  }
  if(stage==3)stage=read<c->foundation_prefix_bytes?3:4;else stage++;
 }
 return stage==6&&read==c->foundation_prefix_bytes&&read==c->foundation_read_bytes&&previous==c->foundation_previous_tick;
}
int slcli_writer_bq_record_windows_unmap_matches_original(struct slcli_writer_bq_record_windows *c){
 struct slcli_writer_bq_record_io_windows *r;const struct slcli_windows_file_capture *raw;
 struct slcli_writer_capture_ranges_windows *f,*out;
 if(!c||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||!same(c))return 0;
 f=c->foundation;out=c->foundation_output;r=&c->unmap_io;
 if(!c->unmap_started||f->write_close_bq_original!=c||!c->foundation_io_observed||
  !foundation_matches(c)||!c->foundation_finish_returned||!c->foundation_finish_result||
  c->producer->capture_first_ordinal||c->capture_first_ordinal!=8||
  out->attempted!=c->foundation_output_first_ordinal+c->foundation_io_attempted+1||
  (uintptr_t)r->capture!=(uintptr_t)out->extent->views[0]+(out->attempted-1)*out->stride+out->data_offset||
  r->kind!=6||r->handle_original!=f->extent->readback->creator->original||
  r->payload_original||r->payload_readonly||r->payload_requested||r->write_original||
  r->comparison_original||r->offset_original||r->bytes_original||
  !r->before_returned||!r->before_result||!r->before.complete||r->before.tick<c->foundation_finish.tick||
  !r->called||!r->returned||!r->result||!r->after_returned||!r->after_result||!r->after.complete||
  r->after.tick<r->before.tick||r->after.tick>=c->journal->deadline->expires)return 0;
 raw=capture_readonly_original(out,r->capture,6,0);
 return raw&&raw->original==r->handle_original&&raw->operation==6&&raw->completed&&!raw->retained_native_io&&
  raw->view_unmap_address==(uintptr_t)f->extent->views[0]&&raw->view_unmap_called&&raw->view_unmap_returned&&
  raw->view_unmap_status&&raw->status==(uint32_t)raw->view_unmap_status&&
  raw->tick>=r->before.tick&&raw->tick<=r->after.tick;
}
static int foundation_unmap(struct slcli_writer_bq_record_windows *c){
 struct slcli_writer_capture_ranges_windows *f=c->foundation,*out=c->foundation_output;
 struct slcli_writer_bq_record_io_windows *r=&c->unmap_io;
 const struct slcli_windows_file_capture *read;
 struct slcli_writer_capture_extent_windows *e=f->extent;
 if(c->unmap_started||f->write_close_bq_original||!c->foundation_io_observed||
  !c->foundation_finish_returned||!c->foundation_finish_result||
  c->producer->capture_first_ordinal||c->capture_first_ordinal!=8||
  f->attempted!=8+c->io_attempted||!foundation_matches(c)||
  out->attempted!=c->foundation_output_first_ordinal+c->foundation_io_attempted||
  out->attempted>=out->capacity)return 0;
 c->unmap_started=1;r->kind=6;r->handle_original=e->readback->creator->original;
 r->before_result=slcli_native_clock_read(&r->before);r->before_returned=1;
 if(!r->before_result||r->before.tick<c->foundation_finish.tick||
  r->before.tick>=c->journal->deadline->expires||
  !slcli_evidence_deadline_precede(c->journal->creator->evidence_file,&r->before)||
  !slcli_evidence_deadline_precede(e->readback->creator->evidence_file,&r->before))return 0;
 r->capture=slcli_writer_capture_ranges_windows_claim_original(out,6);
 if(!r->capture)return 0;
 /* Absorbing denial of further writes is installed before the native effect.
  * This association records an attempt, never successful alias closure. */
 f->write_close_bq_original=c;r->called=1;
 r->result=slcli_windows_file_unmap_original(r->handle_original,e->views[0],r->capture);
 r->returned=1;r->after_result=slcli_native_clock_read(&r->after);r->after_returned=1;
 if(!r->after_result||r->after.tick<r->before.tick||r->after.tick>=c->journal->deadline->expires)return 0;
 read=capture_readonly_original(out,r->capture,6,0);
 if(!read||!r->result||!read->completed||read->retained_native_io||
  read->original!=r->handle_original||read->operation!=6||
  read->view_unmap_address!=(uintptr_t)e->views[0]||!read->view_unmap_called||
  !read->view_unmap_returned||!read->view_unmap_status||read->status!=(uint32_t)read->view_unmap_status||
  read->tick<r->before.tick||read->tick>r->after.tick||f->write_close_bq_original!=c)return 0;
 if(!slcli_writer_bq_record_windows_unmap_matches_original(c))return 0;
 c->unmap_finish_result=slcli_native_clock_read(&c->unmap_finish);c->unmap_finish_returned=1;
 if(!c->unmap_finish_result||c->unmap_finish.tick<r->after.tick||
  c->unmap_finish.tick>=c->journal->deadline->expires||
  !slcli_evidence_deadline_precede(c->journal->creator->evidence_file,&c->unmap_finish)||
  !slcli_evidence_deadline_precede(e->readback->creator->evidence_file,&c->unmap_finish))return 0;
 c->unmap_observed=1;return 1;
}
static int control_operation(struct slcli_writer_bq_record_windows *c,uint32_t kind,
 uint64_t offset,size_t bytes,const unsigned char *expected){
 struct slcli_writer_capture_ranges_windows *f=c->foundation_control;
 struct slcli_writer_capture_extent_windows *e=c->foundation_output->extent;
 struct slcli_writer_bq_record_io_windows *r;struct slcli_windows_file_capture *raw;
 HANDLE h=(kind==4||kind==5||kind==6)?e->readback->creator->original:e->readback->original;
 if(!same(c)||c->control_io_attempted>=c->control_io_capacity||
  c->source_control_count>f->attempted||f->attempted-c->source_control_count!=c->control_io_attempted||
  c->foundation_output->attempted!=c->control_prefix_bytes/c->foundation_output->stride||
  (kind!=1&&kind!=2&&kind!=4&&kind!=5&&kind!=6)||
  (kind==2&&(!bytes||bytes>f->payload_capacity))||
  (kind==5&&(offset||expected||bytes!=c->control_prefix_bytes))||
  ((kind==1||kind==4||kind==6)&&(offset||bytes||expected)))return 0;
 r=&c->control_io[c->control_io_attempted++];r->kind=kind;r->handle_original=h;
 r->offset_original=offset;r->bytes_original=bytes;r->write_original=expected;
 r->before_result=slcli_native_clock_read(&r->before);r->before_returned=1;
 if(!r->before_result||r->before.tick<c->control_previous_tick||r->before.tick>=c->journal->deadline->expires||
  !slcli_evidence_deadline_precede(c->journal->creator->evidence_file,&r->before)||
  !slcli_evidence_deadline_precede(e->readback->creator->evidence_file,&r->before))return 0;
 if(kind==2){r->payload_requested=bytes;raw=slcli_writer_capture_ranges_windows_claim_read_original(f,bytes,
   &r->payload_original,&r->payload_readonly);}
 else raw=slcli_writer_capture_ranges_windows_claim_original(f,kind);
 r->capture=raw;if(!raw)return 0;
 if(kind==6){if(c->foundation_output->write_close_bq_original)return 0;
  c->foundation_output->write_close_bq_original=c;}
 r->called=1;
 if(kind==6)r->result=slcli_windows_file_unmap_original(h,e->views[0],raw);
 else if(kind==5)r->result=slcli_windows_file_flush_mapped(h,e->views[0],bytes,raw);
 else if(kind==4)r->result=slcli_windows_file_flush(h,raw);
 else if(kind==1)r->result=slcli_windows_source_observe(h,raw);
 else r->result=slcli_windows_file_read(h,r->payload_original,(DWORD)bytes,offset,raw);
 r->returned=1;r->after_result=slcli_native_clock_read(&r->after);r->after_returned=1;
 if(!r->after_result||r->after.tick<r->before.tick||r->after.tick>=c->journal->deadline->expires||
  !slcli_evidence_deadline_precede(c->journal->creator->evidence_file,&r->after)||
  !slcli_evidence_deadline_precede(e->readback->creator->evidence_file,&r->after)||
  !r->result||!raw->completed||raw->retained_native_io||raw->tick<r->before.tick||raw->tick>r->after.tick)return 0;
 c->control_previous_tick=r->after.tick;
 if(kind==2){if(!raw->bytes_observed||raw->returned_bytes>bytes||
   !slcli_equal(r->payload_original,r->payload_readonly,(size_t)raw->returned_bytes))return 0;
  return expected?(raw->returned_bytes&&slcli_equal(expected,r->payload_readonly,(size_t)raw->returned_bytes)):!raw->returned_bytes;}
 return 1;
}
static int control_matches(struct slcli_writer_bq_record_windows *c){
 struct slcli_writer_capture_ranges_windows *f=c->foundation_control;
 struct slcli_writer_capture_extent_windows *e=c->foundation_output->extent;
 size_t i,read=0;uint32_t stage=0;uint64_t previous=c->unmap_finish.tick;
 if(!same(c)||!c->control_started||!c->unmap_observed||!c->unmap_finish_returned||!c->unmap_finish_result||
  !slcli_writer_bq_record_windows_unmap_matches_original(c)||
  !c->control_prefix_bytes||c->control_prefix_bytes>e->bytes||
  c->control_prefix_bytes%c->foundation_output->stride||
  c->control_prefix_bytes/c->foundation_output->stride!=c->foundation_output->attempted||
  c->foundation_output->write_close_bq_original!=c||
  !c->control_io_capacity||c->control_io_capacity>SIZE_MAX/sizeof(*c->control_io)||
  !slcli_arena_live_block(c->arena,c->control_io,c->control_io_capacity*sizeof(*c->control_io))||
  c->control_io_attempted>c->control_io_capacity||c->source_control_count>f->attempted||
  f->attempted-c->source_control_count!=c->control_io_attempted)return 0;
 for(i=0;i<c->control_io_attempted;i++){
  const struct slcli_writer_bq_record_io_windows *r=&c->control_io[i];
  const struct slcli_windows_file_capture *raw,*identity=&e->readback->observed;
  uint32_t kind=stage==0?5:(stage==1?4:(stage==6?6:((stage==3||stage==4)?2:1)));
  HANDLE h=(stage<2||stage==6)?e->readback->creator->original:e->readback->original;
  size_t bytes=stage==0?c->control_prefix_bytes:0;uint64_t offset=0;const unsigned char *expected=0;
  if(stage>6)return 0;
  if(stage==3){bytes=c->control_prefix_bytes-read;if(bytes>f->payload_capacity)bytes=f->payload_capacity;
   offset=e->file_offset+read;expected=(const unsigned char *)e->views[1]+read;}
  if(stage==4){bytes=1;offset=e->file_bytes;}
  raw=capture_readonly_original(f,r->capture,kind,kind==2?bytes:0);
  if(!raw||(uintptr_t)r->capture!=(uintptr_t)f->extent->views[0]+(c->source_control_count+i)*f->stride+f->data_offset||
   r->kind!=kind||r->handle_original!=h||r->bytes_original!=bytes||r->offset_original!=offset||r->write_original!=expected||
   r->comparison_original||!r->before_returned||!r->before_result||!r->before.complete||r->before.tick<previous||
   !r->called||!r->returned||!r->result||!r->after_returned||!r->after_result||!r->after.complete||
   r->after.tick<r->before.tick||r->after.tick>=c->journal->deadline->expires||
   raw->original!=h||raw->operation!=kind||!raw->completed||raw->retained_native_io||
   raw->tick<r->before.tick||raw->tick>r->after.tick)return 0;
  previous=r->after.tick;
  if(kind!=2&&(r->payload_original||r->payload_readonly||r->payload_requested))return 0;
  if(kind==5){if(raw->view_flush_address!=(uintptr_t)e->views[0]||raw->view_flush_bytes!=bytes||
    !raw->view_flush_called||!raw->view_flush_returned||!raw->view_flush_status||raw->status!=(uint32_t)raw->view_flush_status)return 0;}
  else if(kind==6){if(raw->view_unmap_address!=(uintptr_t)e->views[0]||!raw->view_unmap_called||
    !raw->view_unmap_returned||!raw->view_unmap_status||raw->status!=(uint32_t)raw->view_unmap_status)return 0;}
  else if(kind==4){if(raw->read_args[0]!=(uintptr_t)h)return 0;}
  else if(kind==1){if(raw->standard.EndOfFile.QuadPart<0||(uint64_t)raw->standard.EndOfFile.QuadPart!=e->file_bytes||
    raw->access_status||raw->access_bytes!=sizeof(raw->access_raw)||
    raw->access_raw.GrantedAccess!=identity->access_raw.GrantedAccess||raw->handle_flags!=identity->handle_flags||
    raw->identity.VolumeSerialNumber!=identity->identity.VolumeSerialNumber||
    !slcli_equal(raw->identity.FileId.Identifier,identity->identity.FileId.Identifier,16))return 0;}
  else{if(r->payload_requested!=bytes||
    (uintptr_t)r->payload_original!=(uintptr_t)f->extent->views[0]+(c->source_control_count+i)*f->stride+f->payload_offset||
    r->payload_readonly!=(const unsigned char *)f->extent->views[1]+(c->source_control_count+i)*f->stride+f->payload_offset||
    !file_io_original(raw,h,r->payload_original,bytes,offset,(uintptr_t)r->capture))return 0;
   if(expected){if(!raw->returned_bytes||!slcli_equal(expected,r->payload_readonly,(size_t)raw->returned_bytes))return 0;
    read+=(size_t)raw->returned_bytes;}else if(raw->returned_bytes)return 0;}
  if(stage==3)stage=read<c->control_prefix_bytes?3:4;else stage++;
 }
 return stage==7&&read==c->control_prefix_bytes&&read==c->control_read_bytes&&previous==c->control_previous_tick;
}
static int control_readback(struct slcli_writer_bq_record_windows *c){
 struct slcli_writer_capture_extent_windows *e=c->foundation_output->extent;size_t n;
 if(c->control_started||!c->unmap_observed||!c->unmap_finish_returned||!c->unmap_finish_result||
  !slcli_writer_bq_record_windows_unmap_matches_original(c)||!source_control_original(c)||
  c->foundation_control->attempted!=c->source_control_count||
  c->foundation_output->write_close_bq_original)return 0;
 c->control_started=1;c->control_prefix_bytes=c->foundation_output->attempted*c->foundation_output->stride;
 c->control_previous_tick=c->unmap_finish.tick;
 if(!c->control_prefix_bytes||c->control_prefix_bytes>e->bytes||
  !control_operation(c,5,0,c->control_prefix_bytes,0)||!control_operation(c,4,0,0,0)||
  !control_operation(c,1,0,0,0))return 0;
 while(c->control_read_bytes<c->control_prefix_bytes){
  n=c->control_prefix_bytes-c->control_read_bytes;if(n>c->foundation_control->payload_capacity)n=c->foundation_control->payload_capacity;
  if(!control_operation(c,2,e->file_offset+c->control_read_bytes,n,(const unsigned char *)e->views[1]+c->control_read_bytes))return 0;
  c->control_read_bytes+=(size_t)c->control_io[c->control_io_attempted-1].capture->returned_bytes;
 }
 if(!control_operation(c,2,e->file_bytes,1,0)||!control_operation(c,1,0,0,0)||
  !control_operation(c,6,0,0,0)||!control_matches(c))return 0;
 c->control_finish_result=slcli_native_clock_read(&c->control_finish);c->control_finish_returned=1;
 if(!c->control_finish_result||c->control_finish.tick<c->control_previous_tick||
  c->control_finish.tick>=c->journal->deadline->expires||
  !same(c)||
  !slcli_evidence_deadline_precede(c->journal->creator->evidence_file,&c->control_finish)||
  !slcli_evidence_deadline_precede(e->readback->creator->evidence_file,&c->control_finish))return 0;
 c->control_observed=1;return 1;
}
static int foundation_readback(struct slcli_writer_bq_record_windows *c){
 struct slcli_writer_capture_ranges_windows *f=c->foundation;
 struct slcli_writer_capture_extent_windows *e=f->extent;size_t n;
 if(c->foundation_started||!c->validation_returned||!c->validation_result||
  !c->validation_after_returned||!c->validation_after_result||
  !slcli_writer_bq_record_windows_matches_original(c)||
  f->attempted!=c->capture_first_ordinal+c->io_attempted||
  c->foundation_output->attempted!=c->foundation_output_first_ordinal)return 0;
 c->foundation_started=1;c->foundation_failed=1;
 /* Freeze first-unit prefix. All following native outputs target the distinct
  * source-preplanned second unit, never this selected view's pages. */
 c->foundation_prefix_bytes=f->attempted*f->stride;
 c->foundation_previous_tick=c->validation_after.tick;
 if(!c->foundation_prefix_bytes||c->foundation_prefix_bytes>e->bytes||
  !foundation_operation(c,5,0,c->foundation_prefix_bytes,0)||
  !foundation_operation(c,4,0,0,0)||!foundation_operation(c,1,0,0,0))return 0;
 while(c->foundation_read_bytes<c->foundation_prefix_bytes){
  n=c->foundation_prefix_bytes-c->foundation_read_bytes;
  if(n>c->foundation_output->payload_capacity)n=c->foundation_output->payload_capacity;
  if(!foundation_operation(c,2,e->file_offset+c->foundation_read_bytes,n,
    (const unsigned char *)e->views[1]+c->foundation_read_bytes))return 0;
  c->foundation_read_bytes+=(size_t)c->foundation_io[c->foundation_io_attempted-1].capture->returned_bytes;
 }
 if(!foundation_operation(c,2,e->file_bytes,1,0)||!foundation_operation(c,1,0,0,0))return 0;
 c->foundation_validation_called=1;c->foundation_validation_result=foundation_matches(c);c->foundation_validation_returned=1;
 c->foundation_finish_result=slcli_native_clock_read(&c->foundation_finish);c->foundation_finish_returned=1;
 if(!c->foundation_validation_result||!c->foundation_finish_result||c->foundation_finish.tick<c->foundation_previous_tick||
  c->foundation_finish.tick>=c->journal->deadline->expires||
  !slcli_evidence_deadline_precede(c->journal->creator->evidence_file,&c->foundation_finish)||
  !slcli_evidence_deadline_precede(e->readback->creator->evidence_file,&c->foundation_finish))return 0;
 c->foundation_io_observed=1;c->foundation_failed=0;return 1;
}
#endif
