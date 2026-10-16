#include "bq_observation_windows.h"
#if defined(_WIN32)
static int zero(const void *p,size_t n) {
 const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;
}
static int separate(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_bq_observation_prepare_preowned_original(
 struct slcli_bq_observation_preparation *c,struct slcli_binding_storage_original *s,
 struct slcli_bq_file_observation_slot *slots,const struct slcli_native_reference *plans,
 const uint32_t *ids,uint32_t count) {
 struct slcli_arena *a;struct slcli_binding_storage_geometry g;
 void *owned[8];const void *input[4];size_t owned_bytes[8],input_bytes[4];
 size_t bytes,pbytes,ibytes;uint32_t i,j;
 if(!s||!(a=s->arena)||!c||!slots||!plans||!ids||!count||
  count>SIZE_MAX/sizeof(*slots)||count>SIZE_MAX/sizeof(*plans)||count>SIZE_MAX/sizeof(*ids)||
  !slcli_arena_live_block(a,s,sizeof(*s))||!s->prepared||!s->started||s->enrolled||
  s->attempted>s->capacity||count>s->capacity-s->attempted||
  s->capacity>SIZE_MAX/sizeof(*s->ids)||
  !slcli_arena_live_block(a,s->ids,(size_t)s->capacity*sizeof(*s->ids))||
  !slcli_binding_storage_geometry_derive(s->capacity,&g)||
  !slcli_arena_live_block(a,s->book,sizeof(*s->book))||s->book->storage_original!=s||
  s->book->references!=s->references||s->book->original_ids!=s->ids||
  s->book->original_indices!=s->indices||s->book->primary_terminal_bitmap!=s->bitmap||
  s->book->initial_snapshot!=s->snapshot)return 0;
 bytes=(size_t)count*sizeof(*slots);pbytes=(size_t)count*sizeof(*plans);ibytes=(size_t)count*sizeof(*ids);
 if(!slcli_arena_live_block(a,c,sizeof(*c))||!slcli_arena_live_block(a,slots,bytes)||
  !slcli_arena_live_block(a,plans,pbytes)||!slcli_arena_live_block(a,ids,ibytes)||
  !separate(c,sizeof(*c),slots,bytes)||!separate(c,sizeof(*c),plans,pbytes)||
  !separate(c,sizeof(*c),ids,ibytes)||!separate(slots,bytes,plans,pbytes)||
  !separate(slots,bytes,ids,ibytes)||!separate(plans,pbytes,ids,ibytes)||
  !zero(c,sizeof(*c))||!zero(slots,bytes))return 0;
 owned[0]=s;owned_bytes[0]=sizeof(*s);owned[1]=s->book;owned_bytes[1]=sizeof(*s->book);
 owned[2]=s->references;owned_bytes[2]=g.references_bytes;
 owned[3]=s->ids;owned_bytes[3]=g.ids_bytes;owned[4]=s->indices;owned_bytes[4]=g.indices_bytes;
 owned[5]=s->bitmap;owned_bytes[5]=g.bitmap_bytes;owned[6]=s->snapshot;owned_bytes[6]=g.snapshot_bytes;
 owned[7]=s->claims;owned_bytes[7]=g.claims_bytes;
 input[0]=c;input_bytes[0]=sizeof(*c);input[1]=slots;input_bytes[1]=bytes;
 input[2]=plans;input_bytes[2]=pbytes;input[3]=ids;input_bytes[3]=ibytes;
 /* Exact-start arena membership alone does not exclude equal-sized aliasing
  * with an existing book block (notably a zero terminal bitmap). */
 for(i=0;i<8;i++) {
  if(!slcli_arena_live_block(a,owned[i],owned_bytes[i]))return 0;
  for(j=0;j<4;j++)if(!separate(owned[i],owned_bytes[i],input[j],input_bytes[j]))return 0;
 }
 /* Validate the entire immutable plan before consuming any claim. No native
  * authority follows from these structural identities. */
 for(i=0;i<count;i++) {
  if(!ids[i]||(i&&ids[i]<=ids[i-1])||
   (!i&&s->attempted&&ids[i]<=s->ids[s->attempted-1])||
   !plans[i].version||plans[i].native_original||plans[i].native_class||
   plans[i].service_request_original||plans[i].handle||plans[i].native_rights||
   plans[i].kind>22||plans[i].state!=2||plans[i].effect_flags!=1)return 0;
  for(j=0;j<32;j++)if(plans[i].identity[j])return 0;
 }
 c->storage=s;c->slots=slots;c->plans=plans;c->ids=ids;c->count=count;
 c->first_ordinal=s->attempted;c->started=1;c->failed=1;
 for(i=0;i<count;i++) {
  struct slcli_bq_file_observation_slot *slot=&slots[i];
  c->next=i+1;slot->ordinal=s->attempted;slot->id=ids[i];slot->claim_attempted=1;
  slot->claim_result=slcli_binding_storage_claim_pre_effect_original(s,&plans[i],ids[i],
   slots,bytes,slot,sizeof(*slot),SLCLI_NATIVE_ORIGINAL_BQ_RECEIVED_FILE_OBSERVATION);
  slot->claim_returned=1;if(!slot->claim_result)return 0;
 }
 c->prepared=1;c->failed=0;return 1;
}
static int slot_original(const struct slcli_bq_observation_preparation *c,uint32_t index) {
 const struct slcli_binding_storage_original *s=c->storage;
 const struct slcli_bq_file_observation_slot *slot;
 const struct slcli_binding_row_claim_original *claim;size_t bytes;
 if(!s||!s->arena||!slcli_arena_live_block(s->arena,c,sizeof(*c))||!c->started||
  !c->prepared||c->failed||!c->count||index>=c->count||c->count>SIZE_MAX/sizeof(*c->slots)||
  !slcli_arena_live_block(s->arena,s,sizeof(*s))||s->enrolled||!s->prepared||
  !slcli_arena_live_block(s->arena,s->book,sizeof(*s->book))||s->book->storage_original!=s||
  s->book->count!=s->attempted||s->book->original_ids!=s->ids||
  s->capacity>SIZE_MAX/sizeof(*s->ids)||
  !slcli_arena_live_block(s->arena,s->ids,(size_t)s->capacity*sizeof(*s->ids))||
  c->first_ordinal>s->attempted||c->count>s->attempted-c->first_ordinal||
  s->capacity>SIZE_MAX/sizeof(*s->claims)||
  !slcli_arena_live_block(s->arena,s->claims,(size_t)s->capacity*sizeof(*s->claims)))return 0;
 bytes=(size_t)c->count*sizeof(*c->slots);
 if(!slcli_arena_live_block(s->arena,c->slots,bytes)||
  c->count>SIZE_MAX/sizeof(*c->plans)||c->count>SIZE_MAX/sizeof(*c->ids)||
  !slcli_arena_live_block(s->arena,c->plans,(size_t)c->count*sizeof(*c->plans))||
  !slcli_arena_live_block(s->arena,c->ids,(size_t)c->count*sizeof(*c->ids)))return 0;
 slot=&c->slots[index];claim=&s->claims[c->first_ordinal+index];
 return slot->claim_attempted&&slot->claim_returned&&slot->claim_result&&
  slot->id==c->ids[index]&&slot->id==s->ids[c->first_ordinal+index]&&
  slot->ordinal==c->first_ordinal+index&&claim->claimed&&claim->ordinal==slot->ordinal&&
  claim->capture==slot&&claim->capture_bytes==sizeof(*slot)&&
  claim->containing_block==c->slots&&claim->containing_bytes==bytes&&
  claim->capture_offset==(size_t)index*sizeof(*slot)&&
  claim->native_class==SLCLI_NATIVE_ORIGINAL_BQ_RECEIVED_FILE_OBSERVATION;
}
static int observation_raw(const struct slcli_bq_observation_preparation *c,uint32_t index) {
 const struct slcli_bq_file_observation_slot *r;const struct slcli_windows_file_capture *f;
 const void *outputs[4];size_t output_bytes[4];unsigned i;
 static const uint64_t information_classes[4]={FileIdInfo,FileStandardInfo,FileAttributeTagInfo,FileBasicInfo};
 if(!slot_original(c,index))return 0;r=&c->slots[index];f=&r->raw;
 if(!r->receiver_started||!r->deadline||
  !slcli_source_deadline_original_matches(c->storage->arena,r->deadline,r->caller)||
  r->deadline->origin!=r->origin||r->origin->tick!=r->origin_tick||r->deadline->expires!=r->expires||
  !slcli_arena_live_block(c->storage->arena,r->caller,r->caller_bytes)||
  !r->prior_original||!slcli_arena_live_range(c->storage->arena,r->prior_original,sizeof(*r->prior_original))||
  !r->prior_original->complete||r->prior_original->tick!=r->prior_tick||
  !r->sender_process||r->sender_process==INVALID_HANDLE_VALUE||!r->sender_file||r->sender_file==INVALID_HANDLE_VALUE||
  !r->duplicate_called||!r->duplicate_returned||!r->duplicate_status||!r->received||r->received==INVALID_HANDLE_VALUE||
  r->duplicate_args[0]!=(uintptr_t)r->sender_process||r->duplicate_args[1]!=(uintptr_t)r->sender_file||
  r->duplicate_args[2]!=(uintptr_t)(HANDLE)(intptr_t)-1||r->duplicate_args[3]!=(uintptr_t)&r->received||
  r->duplicate_args[4]!=(FILE_READ_DATA|FILE_READ_ATTRIBUTES|READ_CONTROL|SYNCHRONIZE)||
  r->duplicate_args[5]!=FALSE||r->duplicate_args[6]!=0||
  !r->before_returned||!r->before_result||!r->before.complete||
  !r->after_returned||!r->after_result||!r->after.complete||
  !r->observe_before_returned||!r->observe_before_result||!r->observe_before.complete||
  !r->observe_returned||!r->observe_result||!r->observe_after_returned||!r->observe_after_result||!r->observe_after.complete||
  r->prior_tick<r->origin_tick||r->before.tick<r->prior_tick||r->after.tick<r->before.tick||
  r->observe_before.tick<r->after.tick||r->observe_after.tick<r->observe_before.tick||
  r->observe_after.tick>=r->expires||f->original!=r->received||f->operation!=1||
  !f->observed||!f->completed||f->retained_native_io||f->tick<r->observe_before.tick||f->tick>r->observe_after.tick||
  f->access_status!=0||f->access_bytes!=sizeof(f->access_raw)||f->access_raw.GrantedAccess!=(DWORD)r->duplicate_args[4]||
  f->access_args[0]!=(uintptr_t)r->received||f->access_args[1]!=0||
  f->access_args[2]!=(uintptr_t)&f->access_raw||f->access_args[3]!=sizeof(f->access_raw)||
  f->access_args[4]!=(uintptr_t)&f->access_bytes||f->type_args[0]!=(uintptr_t)r->received||
  f->handle_args[0]!=(uintptr_t)r->received||f->handle_args[1]!=(uintptr_t)&f->handle_flags||
  !f->handle_status||(f->handle_flags&HANDLE_FLAG_INHERIT)||f->file_type!=FILE_TYPE_DISK||
  !f->identity_status||!f->standard_status||!f->attributes_status||!f->basic_status||
  f->mode_args[0]!=(uintptr_t)r->received||f->mode_args[1]!=(uintptr_t)&f->mode_io||
  f->mode_args[2]!=(uintptr_t)&f->mode||f->mode_args[3]!=sizeof(f->mode)||f->mode_args[4]!=16||
  f->mode_status!=0||f->mode_io.Status!=0||f->mode_io.Information!=sizeof(f->mode)||(f->mode&0x30)==0)return 0;
 outputs[0]=&f->identity;output_bytes[0]=sizeof(f->identity);
 outputs[1]=&f->standard;output_bytes[1]=sizeof(f->standard);
 outputs[2]=&f->attributes;output_bytes[2]=sizeof(f->attributes);
 outputs[3]=&f->basic;output_bytes[3]=sizeof(f->basic);
 for(i=0;i<4;i++)if(f->information_args[i][0]!=(uintptr_t)r->received||
  f->information_args[i][1]!=information_classes[i]||
  f->information_args[i][2]!=(uintptr_t)outputs[i]||f->information_args[i][3]!=output_bytes[i])return 0;
 return 1;
}
int slcli_bq_file_observation_matches_original(
 const struct slcli_bq_observation_preparation *c,uint32_t index) {
 const struct slcli_bq_file_observation_slot *r;
 if(!c||!observation_raw(c,index))return 0;r=&c->slots[index];
 return r->observation_complete&&!r->receiver_failed&&r->validation_returned&&r->validation_result&&
  r->validation_clock_returned&&r->validation_clock_result&&r->validation_after.complete&&
  r->validation_after.tick>=r->observe_after.tick&&r->validation_after.tick<r->expires&&
  r->previous_tick==r->validation_after.tick;
}
int slcli_bq_file_observe_duplicate_original(
 struct slcli_bq_observation_preparation *c,uint32_t index,HANDLE sender,HANDLE file,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior) {
 struct slcli_bq_file_observation_slot *r;struct slcli_arena *a;
 if(!c||!slot_original(c,index)||c->plans[index].kind!=2||!sender||sender==INVALID_HANDLE_VALUE||
  !file||file==INVALID_HANDLE_VALUE)return 0;
 a=c->storage->arena;r=&c->slots[index];
 if(r->receiver_started||!caller_bytes||!slcli_arena_live_block(a,caller,caller_bytes)||
  !slcli_source_deadline_original_matches(a,d,caller)||
  !prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  prior->tick<d->origin->tick||prior->tick>=d->expires||
  !separate(r,sizeof(*r),caller,caller_bytes)||!separate(r,sizeof(*r),prior,sizeof(*prior))||
  !slcli_source_deadline_away(d,r,sizeof(*r))||!zero(&r->raw,sizeof(r->raw)))return 0;
 r->receiver_started=1;r->receiver_failed=1;r->sender_process=sender;r->sender_file=file;
 r->deadline=d;r->caller=caller;r->caller_bytes=caller_bytes;r->previous_tick=prior->tick;
 r->prior_original=prior;r->prior_tick=prior->tick;
 r->origin=d->origin;r->origin_tick=d->origin->tick;r->expires=d->expires;
 r->duplicate_args[0]=(uintptr_t)sender;r->duplicate_args[1]=(uintptr_t)file;
 r->duplicate_args[2]=(uintptr_t)(HANDLE)(intptr_t)-1;r->duplicate_args[3]=(uintptr_t)&r->received;
 r->duplicate_args[4]=FILE_READ_DATA|FILE_READ_ATTRIBUTES|READ_CONTROL|SYNCHRONIZE;
 r->before_result=slcli_native_clock_read(&r->before);r->before_returned=1;
 if(!r->before_result||r->before.tick<r->previous_tick||r->before.tick>=r->expires||
  d->origin!=r->origin||r->origin->tick!=r->origin_tick||d->expires!=r->expires||
  !slot_original(c,index))return 0;
 r->duplicate_called=1;
 r->duplicate_status=DuplicateHandle(sender,file,(HANDLE)(intptr_t)-1,&r->received,
  (DWORD)r->duplicate_args[4],FALSE,0);
 r->duplicate_returned=1;if(!r->duplicate_status)r->duplicate_error=GetLastError();
 r->after_result=slcli_native_clock_read(&r->after);r->after_returned=1;
 if(!r->after_result||r->after.tick<r->before.tick||r->after.tick>=r->expires||
  d->origin!=r->origin||r->origin->tick!=r->origin_tick||d->expires!=r->expires||
  !r->duplicate_status||!r->received||r->received==INVALID_HANDLE_VALUE||!slot_original(c,index))return 0;
 r->previous_tick=r->after.tick;
 r->observe_before_result=slcli_native_clock_read(&r->observe_before);r->observe_before_returned=1;
 if(!r->observe_before_result||r->observe_before.tick<r->previous_tick||r->observe_before.tick>=r->expires||
  d->origin!=r->origin||r->origin->tick!=r->origin_tick||d->expires!=r->expires||
  !slot_original(c,index)||!slcli_source_deadline_original_matches(a,d,caller))return 0;
 r->observe_result=slcli_windows_file_observe(r->received,&r->raw);r->observe_returned=1;
 r->observe_after_result=slcli_native_clock_read(&r->observe_after);r->observe_after_returned=1;
 if(!r->observe_after_result||r->observe_after.tick<r->observe_before.tick||
  r->observe_after.tick>=r->expires||d->origin!=r->origin||r->origin->tick!=r->origin_tick||
  d->expires!=r->expires||!r->observe_result||!slot_original(c,index)||
  r->raw.original!=r->received||!r->raw.completed||r->raw.retained_native_io||
  r->raw.access_status!=0||r->raw.access_bytes!=sizeof(r->raw.access_raw)||
  r->raw.access_raw.GrantedAccess!=(DWORD)r->duplicate_args[4]||
  !r->raw.handle_status||(r->raw.handle_flags&HANDLE_FLAG_INHERIT)||r->raw.file_type!=FILE_TYPE_DISK||
  r->raw.tick<r->observe_before.tick||r->raw.tick>r->observe_after.tick)return 0;
 r->validation_result=observation_raw(c,index);r->validation_returned=1;
 r->validation_clock_result=slcli_native_clock_read(&r->validation_after);r->validation_clock_returned=1;
 if(!r->validation_result||!r->validation_clock_result||
  r->validation_after.tick<r->observe_after.tick||r->validation_after.tick>=r->expires||
  d->origin!=r->origin||r->origin->tick!=r->origin_tick||d->expires!=r->expires)return 0;
 r->previous_tick=r->validation_after.tick;r->observation_complete=1;r->receiver_failed=0;return 1;
}
#endif
