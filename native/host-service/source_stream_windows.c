#include "source_stream_windows.h"
#if defined(_WIN32)
int slcli_source_stream_geometry_windows_derive(uint64_t bytes,size_t workspace,
 struct slcli_source_stream_geometry_windows *out) {
 struct slcli_source_stream_geometry_windows g={0};uint64_t n,scans,refs,own,unique;size_t charges[4],i,total=0;
 if(!out||!bytes||bytes>INT64_MAX||!workspace||workspace>MAXDWORD)return 0;
 n=bytes/workspace+(bytes%workspace!=0);
 if(n>SIZE_MAX-2||n>SIZE_MAX/sizeof(struct slcli_source_stream_read_windows)||
  n+2>SIZE_MAX/sizeof(uint32_t)||n>(UINT64_MAX-25)/6)return 0;
 scans=6*n+25;if(n+2>UINT64_MAX/scans||n>UINT64_MAX-6||n>(UINT64_MAX-12)/3)return 0;
 g.reads=(size_t)n;g.references=g.reads+2;
 g.read_bytes=g.reads*sizeof(struct slcli_source_stream_read_windows);
 g.ordinal_bytes=g.references*sizeof(uint32_t);
 g.stream_charge=slcli_arena_allocation_bytes(sizeof(struct slcli_source_stream_windows));
 g.reads_charge=slcli_arena_allocation_bytes(g.read_bytes);
 g.workspace_charge=slcli_arena_allocation_bytes(workspace);
 g.ordinals_charge=slcli_arena_allocation_bytes(g.ordinal_bytes);
 charges[0]=g.stream_charge;charges[1]=g.reads_charge;charges[2]=g.workspace_charge;charges[3]=g.ordinals_charge;
 for(i=0;i<4;i++){if(!charges[i]||charges[i]>SIZE_MAX-total)return 0;total+=charges[i];}
 g.total_control_charge=total;g.native_calls=n+6;g.clock_calls=3*n+12;
 refs=n+2;own=refs*scans;
 if(own>UINT64_MAX/2||refs-1>UINT64_MAX/refs)return 0;
 unique=refs*(refs-1);
 if(unique>UINT64_MAX-2*own)return 0;
 /* Own-row upper subtotal for the claimed route: two per same() scan plus
  * constructor duplicate checks and its own earlier claim prefix. Add
  * prior_claim_multiplier*actual prior binding attempts in the outer bound.
  * Nested source/namespace scans, byte scans and native work are separate. */
 g.stream_row_comparisons=2*own+unique;g.prior_claim_multiplier=refs;*out=g;return 1;
}
static int separate(const void *p,size_t n,const void *q,size_t m) {
 uintptr_t x=(uintptr_t)p,y=(uintptr_t)q;
 return p&&q&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int claimed_original(const struct slcli_source_stream_windows *c,struct slcli_arena *a){
 const struct slcli_binding_storage_original *s=c->storage_original;
 const struct slcli_binding_originals *b=c->source->book;
 struct slcli_binding_storage_geometry g;const struct slcli_binding_row_claim_original *claim;
 const void *capture,*container;size_t i,count,bytes,container_bytes,offset;uint32_t ordinal;
 enum slcli_native_original_class tag;
 if(!s)return !c->plans_original&&!c->ids_original&&!c->ordinals_original;
 if(c->count>SIZE_MAX-2)return 0;count=c->count+2;
 if(!slcli_arena_live_block(a,s,sizeof(*s))||!s->prepared||s->arena!=a||s->book!=b||
  b->storage_original!=s||c->source->storage_original!=s||s->attempted!=b->count||s->attempted>s->capacity||
  !slcli_binding_storage_geometry_derive(s->capacity,&g)||s->references!=b->references||s->ids!=b->original_ids||
  !slcli_arena_live_block(a,s->references,g.references_bytes)||!slcli_arena_live_block(a,s->ids,g.ids_bytes)||
  !slcli_arena_live_block(a,s->claims,g.claims_bytes)||count>SIZE_MAX/sizeof(*c->plans_original)||
  count>SIZE_MAX/sizeof(uint32_t)||!slcli_arena_live_block(a,c->plans_original,count*sizeof(*c->plans_original))||
  !slcli_arena_live_block(a,c->ids_original,count*sizeof(uint32_t))||
  !slcli_arena_live_block(a,c->ordinals_original,count*sizeof(uint32_t)))return 0;
 for(i=0;i<count;i++){
  ordinal=c->ordinals_original[i];if(ordinal>=s->attempted)return 0;claim=&s->claims[ordinal];
  capture=!i?(const void *)c:i==count-1?(const void *)&c->eof:(const void *)&c->reads[i-1];
  container=(!i||i==count-1)?(const void *)c:(const void *)c->reads;
  bytes=!i?sizeof(*c):sizeof(c->eof);container_bytes=(!i||i==count-1)?sizeof(*c):c->count*sizeof(*c->reads);
  offset=(size_t)((uintptr_t)capture-(uintptr_t)container);
  tag=i?SLCLI_NATIVE_ORIGINAL_SOURCE_STREAM_READ:SLCLI_NATIVE_ORIGINAL_SOURCE_STREAM_BUFFER;
  if(!claim->claimed||claim->ordinal!=ordinal||claim->capture!=capture||claim->capture_bytes!=bytes||
   claim->containing_block!=container||claim->containing_bytes!=container_bytes||claim->capture_offset!=offset||
   claim->native_class!=tag||b->original_ids[ordinal]!=c->ids_original[i]||
   b->references[ordinal].version!=c->plans_original[i].version)return 0;
 }
 return 1;
}
static int same(const struct slcli_source_stream_windows *c,struct slcli_arena *a) {
 const struct slcli_source_file_windows *s=c->source;
 const struct slcli_native_reference *r;const struct slcli_source_stream_read_windows *read;size_t i;
 struct slcli_source_stream_geometry_windows g;
 if(!s||!slcli_source_file_windows_matches_original(s,a)||s->stream_original!=c||!c->count||
  c->used>c->count||!c->workspace_bytes||c->file_bytes!=(uint64_t)s->bytes.QuadPart||
  c->count!=c->file_bytes/c->workspace_bytes+(c->file_bytes%c->workspace_bytes!=0)||
  !c->reads||c->count>SIZE_MAX/sizeof(*c->reads)||
  !slcli_arena_live_block(a,c->reads,c->count*sizeof(*c->reads))||
  c->buffer_ordinal>=s->book->count||
  !slcli_source_stream_geometry_windows_derive(c->file_bytes,c->workspace_bytes,&g)||
  c->geometry.reads!=g.reads||c->geometry.references!=g.references||
  c->geometry.read_bytes!=g.read_bytes||c->geometry.ordinal_bytes!=g.ordinal_bytes||
  c->geometry.stream_charge!=g.stream_charge||c->geometry.reads_charge!=g.reads_charge||
  c->geometry.workspace_charge!=g.workspace_charge||c->geometry.ordinals_charge!=g.ordinals_charge||
  c->geometry.total_control_charge!=g.total_control_charge||c->geometry.native_calls!=g.native_calls||
  c->geometry.clock_calls!=g.clock_calls||c->geometry.stream_row_comparisons!=g.stream_row_comparisons||
  c->geometry.prior_claim_multiplier!=g.prior_claim_multiplier||
  !claimed_original(c,a))return 0;
 r=&s->book->references[c->buffer_ordinal];
 if(r->native_original!=c||r->native_class!=SLCLI_NATIVE_ORIGINAL_SOURCE_STREAM_BUFFER||
  r->kind!=6||r->version!=c->buffer_version||r->handle||r->alias_of!=UINT16_MAX||
  r->physical_extent!=a->extent||r->effect_flags!=(c->buffer_returned?(c->workspace?15:7):c->buffer_attempted?3:1)||
  r->state!=(c->workspace?0:2)||
  (c->workspace&&!slcli_arena_live_block(a,c->workspace,c->workspace_bytes)))return 0;
 for(i=0;i<=c->count;i++) {
  read=i==c->count?&c->eof:&c->reads[i];if(read->ordinal>=s->book->count)return 0;
  r=&s->book->references[read->ordinal];
  if(r->native_original!=read||r->native_class!=SLCLI_NATIVE_ORIGINAL_SOURCE_STREAM_READ||
   r->kind!=5||r->version!=read->version||r->handle||r->alias_of!=UINT16_MAX||
   r->physical_extent!=a->extent||r->effect_flags!=(read->called?(read->returned?(read->status?15:23):3):1)||
   r->state!=(read->returned&&read->status?0:2))return 0;
 }
 return 1;
}
static int tick(struct slcli_source_stream_windows *c,struct slcli_arena *a,
 struct slcli_native_clock *out,const struct slcli_native_clock *prior) {
 return same(c,a)&&prior&&prior->complete&&slcli_native_clock_read(out)&&
  out->tick>=prior->tick&&out->tick<c->source->deadline->expires&&same(c,a);
}
static int identity(struct slcli_source_stream_windows *c,struct slcli_arena *a,
 size_t i,const struct slcli_native_clock *prior) {
 if(i>=2||c->identity_called[i]||!tick(c,a,&c->identity_before[i],prior))return 0;
 c->identity_args[i][0]=(uintptr_t)c->source->file;c->identity_args[i][1]=FileIdInfo;
 c->identity_args[i][2]=(uintptr_t)&c->identity[i];c->identity_args[i][3]=sizeof(c->identity[i]);
 c->identity_called[i]=1;
 c->identity_status[i]=GetFileInformationByHandleEx(c->source->file,FileIdInfo,&c->identity[i],sizeof(c->identity[i]));
 c->identity_returned[i]=1;if(!c->identity_status[i])c->identity_error[i]=GetLastError();
 return tick(c,a,&c->identity_after[i],&c->identity_before[i])&&c->identity_status[i];
}
int slcli_source_stream_windows_matches_original(const struct slcli_source_stream_windows *c,
 struct slcli_arena *a,const struct slcli_source_file_windows *s) {
 size_t i;uint64_t offset=0,previous;const struct slcli_source_stream_read_windows *r;
 if(!a||!c||!slcli_arena_live_block(a,c,sizeof(*c))||c->source!=s||!c->complete||
  !same(c,a)||c->used!=c->count||c->consumed!=c->file_bytes||!c->hash_finished||
  !c->hash_finish_returned||!c->hash_finish_status||
  !c->hash.finished||c->hash.failed||c->hash.bytes!=c->file_bytes||
  !c->seek_called||!c->seek_returned||!c->seek_status||c->zero.QuadPart||c->position.QuadPart||
  c->seek_args[0]!=(uintptr_t)s->file||c->seek_args[1]||
  c->seek_args[2]!=(uintptr_t)&c->position||c->seek_args[3]!=FILE_BEGIN||
  !c->size_called||!c->size_returned||!c->size_status||c->final_bytes.QuadPart!=s->bytes.QuadPart||
  c->size_args[0]!=(uintptr_t)s->file||c->size_args[1]!=(uintptr_t)&c->final_bytes||
  !c->information_called||!c->information_returned||!c->information_status||
  c->information_args[0]!=(uintptr_t)s->file||
  c->information_args[1]!=(uintptr_t)&c->final_information||!c->final_after.complete||
  c->final_after.tick>=s->deadline->expires)return 0;
 for(i=0;i<2;i++)if(!c->identity_called[i]||!c->identity_returned[i]||!c->identity_status[i]||
  c->identity_args[i][0]!=(uintptr_t)s->file||c->identity_args[i][1]!=FileIdInfo||
  c->identity_args[i][2]!=(uintptr_t)&c->identity[i]||c->identity_args[i][3]!=sizeof(c->identity[i])||
  !c->identity_before[i].complete||!c->identity_after[i].complete||
  c->identity_after[i].tick<c->identity_before[i].tick||c->identity_after[i].tick>=s->deadline->expires)return 0;
 for(i=0;i<sizeof(c->identity[0]);i++)if(((const unsigned char *)&c->identity[0])[i]!=
  ((const unsigned char *)&c->identity[1])[i])return 0;
 for(i=0;i<sizeof(c->final_information);i++)if(((const unsigned char *)&c->final_information)[i]!=
  ((const unsigned char *)&s->information[0])[i])return 0;
 if(!c->seek_before.complete||!c->seek_after.complete||
  c->identity_before[0].tick<s->after[3].tick||c->seek_before.tick<c->identity_after[0].tick||
  c->seek_after.tick<c->seek_before.tick)return 0;previous=c->seek_after.tick;
 for(i=0;i<c->count;i++) {
  r=&c->reads[i];if(!r->called||!r->returned||!r->status||!r->hash_returned||!r->consumed||
   !r->hash_status||r->offset!=offset||!r->requested||r->received!=r->requested||
   r->requested>c->workspace_bytes||r->requested>c->file_bytes-offset||
   !r->before.complete||!r->after.complete||!r->consumed_after.complete||
   r->before.tick<previous||r->after.tick<r->before.tick||
   r->consumed_after.tick<r->after.tick||r->consumed_after.tick>=s->deadline->expires||
   r->args[0]!=(uintptr_t)s->file||r->args[1]!=(uintptr_t)c->workspace||
   r->args[2]!=r->requested||r->args[3]!=(uintptr_t)&r->received||r->args[4])return 0;
  offset+=r->requested;previous=r->consumed_after.tick;
 }
 r=&c->eof;
 if(!r->called||!r->returned||r->offset!=c->file_bytes||r->requested!=1||r->received||
  (!r->status&&r->error!=ERROR_HANDLE_EOF)||r->hash_returned||r->hash_status||r->consumed||
  !r->before.complete||!r->after.complete||r->before.tick<previous||r->after.tick<r->before.tick||
  r->args[0]!=(uintptr_t)s->file||r->args[1]!=(uintptr_t)c->workspace||r->args[2]!=1||
  r->args[3]!=(uintptr_t)&r->received||r->args[4])return 0;
 return offset==c->file_bytes&&c->final_before.complete&&c->size_after.complete&&
  c->information_before.complete&&c->final_before.tick>=r->after.tick&&
  c->size_after.tick>=c->final_before.tick&&c->information_before.tick>=c->size_after.tick&&
  c->final_after.tick>=c->information_before.tick&&
  c->identity_before[1].tick>=c->final_after.tick;
}
static int stream_original(struct slcli_source_stream_windows *c,
 struct slcli_arena *a,struct slcli_source_file_windows *s,
 const struct slcli_native_clock *prior,size_t workspace_bytes,const uint32_t *ordinals,size_t count,
 struct slcli_binding_storage_original *storage,const struct slcli_native_reference *plans,const uint32_t *ids) {
 size_t i,j,reads;struct slcli_native_reference *r;struct slcli_source_stream_read_windows *read;
 struct slcli_source_stream_geometry_windows geometry;
 const struct slcli_native_clock *previous;uint64_t remaining;
 const struct slcli_native_reference *planned;const void *capture,*container;size_t bytes,container_bytes;
 enum slcli_native_original_class tag;
 if(!a||!c||!slcli_arena_live_block(a,c,sizeof(*c))||!s||
  !slcli_source_file_windows_matches_original(s,a)||s->stream_original||!prior||
  !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  prior->tick<s->after[3].tick||prior->tick>=s->deadline->expires||
  !workspace_bytes||workspace_bytes>MAXDWORD||s->bytes.QuadPart<=0)return 0;
 if(!slcli_source_stream_geometry_windows_derive((uint64_t)s->bytes.QuadPart,workspace_bytes,&geometry))return 0;
 reads=geometry.reads;
 if(!reads||count!=geometry.references||count>SIZE_MAX/sizeof(*ordinals)||
  reads>SIZE_MAX/sizeof(*read)||!ordinals||!slcli_arena_live_block(a,ordinals,count*sizeof(*ordinals))||
  !separate(c,sizeof(*c),s,sizeof(*s))||!separate(c,sizeof(*c),prior,sizeof(*prior))||
  !separate(c,sizeof(*c),ordinals,count*sizeof(*ordinals))||
  !separate(c,sizeof(*c),s->book,sizeof(*s->book))||
  !separate(c,sizeof(*c),s->book->references,s->book->count*sizeof(*r))||
  !slcli_source_deadline_away(s->deadline,c,sizeof(*c)))return 0;
 for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
 if(storage&&(!slcli_arena_live_block(a,storage,sizeof(*storage))||!storage->prepared||storage->arena!=a||
  storage->book!=s->book||s->book->storage_original!=storage||s->storage_original!=storage||
  storage->attempted!=s->book->count||storage->attempted>storage->capacity||count>storage->capacity-storage->attempted||
  count>SIZE_MAX/sizeof(*plans)||!slcli_arena_live_block(a,plans,count*sizeof(*plans))||
  !slcli_arena_live_block(a,ids,count*sizeof(*ids))||!separate(c,sizeof(*c),plans,count*sizeof(*plans))||
  !separate(c,sizeof(*c),ids,count*sizeof(*ids))))return 0;
 for(i=0;i<count;i++) {
  if(storage){if(ordinals[i]!=storage->attempted+i)return 0;}else if(ordinals[i]>=s->book->count)return 0;
  for(j=0;j<i;j++)if(ordinals[i]==ordinals[j])return 0;
  planned=storage?&plans[i]:&s->book->references[ordinals[i]];
  if(planned->kind!=(i?5:6)||!planned->version||planned->effect_flags!=1||planned->state!=2||planned->handle||
   planned->native_original||planned->service_request_original||planned->native_class!=SLCLI_NATIVE_ORIGINAL_UNCONSTRUCTED||
   planned->physical_extent!=a->extent||planned->alias_of!=UINT16_MAX)return 0;
 }
 c->started=1;c->geometry=geometry;c->source=s;s->stream_original=c;
 c->storage_original=storage;c->plans_original=plans;c->ids_original=ids;c->ordinals_original=storage?ordinals:NULL;
 c->workspace_bytes=workspace_bytes;c->file_bytes=(uint64_t)s->bytes.QuadPart;
 c->count=reads;c->buffer_ordinal=ordinals[0];
 c->reads=slcli_arena_calloc(a,reads,sizeof(*read));if(!c->reads)return 0;
 if(storage)for(i=0;i<count;i++){
  capture=!i?(const void *)c:i==count-1?(const void *)&c->eof:(const void *)&c->reads[i-1];
  container=(!i||i==count-1)?(const void *)c:(const void *)c->reads;
  bytes=!i?sizeof(*c):sizeof(c->eof);container_bytes=(!i||i==count-1)?sizeof(*c):reads*sizeof(*read);
  tag=i?SLCLI_NATIVE_ORIGINAL_SOURCE_STREAM_READ:SLCLI_NATIVE_ORIGINAL_SOURCE_STREAM_BUFFER;
  if(!slcli_binding_storage_claim_pre_effect_original(storage,&plans[i],ids[i],container,container_bytes,capture,bytes,tag))return 0;
 }
 c->buffer_version=s->book->references[ordinals[0]].version;
 r=&s->book->references[ordinals[0]];r->native_original=c;r->native_class=SLCLI_NATIVE_ORIGINAL_SOURCE_STREAM_BUFFER;
 for(i=0;i<reads;i++) {
  read=&c->reads[i];read->ordinal=ordinals[i+1];r=&s->book->references[read->ordinal];
  read->version=r->version;r->native_original=read;r->native_class=SLCLI_NATIVE_ORIGINAL_SOURCE_STREAM_READ;
 }
 c->eof.ordinal=ordinals[reads+1];r=&s->book->references[c->eof.ordinal];
 c->eof.version=r->version;r->native_original=&c->eof;r->native_class=SLCLI_NATIVE_ORIGINAL_SOURCE_STREAM_READ;
 r=&s->book->references[c->buffer_ordinal];r->effect_flags=3;c->buffer_attempted=1;
 c->workspace=slcli_arena_alloc(a,workspace_bytes);c->buffer_returned=1;
 r->effect_flags=c->workspace?15:7;r->state=c->workspace?0:2;if(!c->workspace)return 0;
 if(!identity(c,a,0,prior)||!tick(c,a,&c->seek_before,&c->identity_after[0]))return 0;
 c->seek_args[0]=(uintptr_t)s->file;c->seek_args[2]=(uintptr_t)&c->position;c->seek_args[3]=FILE_BEGIN;
 c->seek_called=1;c->seek_status=SetFilePointerEx(s->file,c->zero,&c->position,FILE_BEGIN);c->seek_returned=1;
 if(!c->seek_status)c->seek_error=GetLastError();
 if(!tick(c,a,&c->seek_after,&c->seek_before)||!c->seek_status||c->position.QuadPart)return 0;
 slcli_sha256_init(&c->hash);previous=&c->seek_after;
 for(i=0;i<reads;i++) {
  read=&c->reads[i];read->offset=c->consumed;remaining=c->file_bytes-c->consumed;
  read->requested=(DWORD)(remaining>workspace_bytes?workspace_bytes:remaining);
  if(!read->requested||!tick(c,a,&read->before,previous))return 0;
  read->args[0]=(uintptr_t)s->file;read->args[1]=(uintptr_t)c->workspace;
  read->args[2]=read->requested;read->args[3]=(uintptr_t)&read->received;
  r=&s->book->references[read->ordinal];r->effect_flags=3;c->used=i+1;read->called=1;
  read->status=ReadFile(s->file,c->workspace,read->requested,&read->received,NULL);read->returned=1;
  r->effect_flags=read->status?15:23;r->state=read->status?0:2;
  if(!read->status)read->error=GetLastError();
  if(!tick(c,a,&read->after,&read->before)||!read->status||read->received!=read->requested)return 0;
  read->hash_status=slcli_sha256_update(&c->hash,c->workspace,read->received);read->hash_returned=1;
  if(!read->hash_status)return 0;
  read->consumed=1;c->consumed+=read->received;
  if(!tick(c,a,&read->consumed_after,&read->after))return 0;
  previous=&read->consumed_after;
 }
 read=&c->eof;read->offset=c->file_bytes;read->requested=1;
 if(!tick(c,a,&read->before,previous))return 0;
 read->args[0]=(uintptr_t)s->file;read->args[1]=(uintptr_t)c->workspace;
 read->args[2]=1;read->args[3]=(uintptr_t)&read->received;
 r=&s->book->references[read->ordinal];r->effect_flags=3;read->called=1;
 read->status=ReadFile(s->file,c->workspace,1,&read->received,NULL);read->returned=1;
 r->effect_flags=read->status?15:23;r->state=read->status?0:2;
 if(!read->status)read->error=GetLastError();
 if(!tick(c,a,&read->after,&read->before)||read->received||
  (!read->status&&read->error!=ERROR_HANDLE_EOF))return 0;
 previous=&read->after;
 c->hash_finish_status=slcli_sha256_finish(&c->hash,c->digest);c->hash_finish_returned=1;
 if(!c->hash_finish_status)return 0;c->hash_finished=1;
 if(!tick(c,a,&c->final_before,previous))return 0;
 c->size_args[0]=(uintptr_t)s->file;c->size_args[1]=(uintptr_t)&c->final_bytes;c->size_called=1;
 c->size_status=GetFileSizeEx(s->file,&c->final_bytes);c->size_returned=1;
 if(!c->size_status)c->size_error=GetLastError();
 if(!tick(c,a,&c->size_after,&c->final_before)||!c->size_status||
  c->final_bytes.QuadPart!=s->bytes.QuadPart||
  !tick(c,a,&c->information_before,&c->size_after))return 0;
 c->information_args[0]=(uintptr_t)s->file;c->information_args[1]=(uintptr_t)&c->final_information;
 c->information_called=1;c->information_status=GetFileInformationByHandle(s->file,&c->final_information);
 c->information_returned=1;if(!c->information_status)c->information_error=GetLastError();
 if(!tick(c,a,&c->final_after,&c->information_before)||!c->information_status)return 0;
 for(i=0;i<sizeof(c->final_information);i++)if(((unsigned char *)&c->final_information)[i]!=
  ((const unsigned char *)&s->information[0])[i])return 0;
 if(!identity(c,a,1,&c->final_after))return 0;
 c->complete=1;return slcli_source_stream_windows_matches_original(c,a,s);
}
int slcli_source_stream_windows_original(struct slcli_source_stream_windows *c,
 struct slcli_arena *a,struct slcli_source_file_windows *s,
 const struct slcli_native_clock *prior,size_t workspace,const uint32_t *ordinals,size_t count){
 return stream_original(c,a,s,prior,workspace,ordinals,count,NULL,NULL,NULL);
}
int slcli_source_stream_windows_claimed_original(struct slcli_source_stream_windows *c,
 struct slcli_arena *a,struct slcli_source_file_windows *s,
 const struct slcli_native_clock *prior,size_t workspace,const uint32_t *ordinals,size_t count,
 struct slcli_binding_storage_original *storage,const struct slcli_native_reference *plans,const uint32_t *ids){
 if(!storage)return 0;
 return stream_original(c,a,s,prior,workspace,ordinals,count,storage,plans,ids);
}
#endif
