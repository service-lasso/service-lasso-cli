#include "source_file_windows.h"
#if defined(_WIN32)
static uint16_t le16(const unsigned char *p) {return (uint16_t)(p[0]|(uint16_t)p[1]<<8);}
static uint32_t le32(const unsigned char *p) {return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int separate(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int claimed_original(const struct slcli_source_file_windows *c,struct slcli_arena *a){
 const struct slcli_binding_storage_original *s=c->storage_original;
 struct slcli_binding_storage_geometry g;const struct slcli_binding_row_claim_original *claim;
 const void *capture;size_t i,bytes,offset;uint32_t ordinal;enum slcli_native_original_class tag;
 if(!s)return !c->plans_original&&!c->ids_original;
 if(!slcli_arena_live_block(a,s,sizeof(*s))||!s->prepared||s->arena!=a||s->book!=c->book||
  c->book->storage_original!=s||s->attempted!=c->book->count||s->attempted>s->capacity||
  !slcli_binding_storage_geometry_derive(s->capacity,&g)||
  s->references!=c->book->references||s->ids!=c->book->original_ids||s->indices!=c->book->original_indices||
  !slcli_arena_live_block(a,s->references,g.references_bytes)||!slcli_arena_live_block(a,s->ids,g.ids_bytes)||
  !slcli_arena_live_block(a,s->indices,g.indices_bytes)||!slcli_arena_live_block(a,s->claims,g.claims_bytes)||
  !slcli_arena_live_block(a,c->plans_original,11*sizeof(*c->plans_original))||
  !slcli_arena_live_block(a,c->ids_original,11*sizeof(*c->ids_original)))return 0;
 for(i=0;i<11;i++){
  ordinal=c->ordinals[i];if(ordinal>=s->attempted)return 0;claim=&s->claims[ordinal];
  capture=i?(const void *)&c->reads[(i-1)/2]:(const void *)c;
  bytes=i?sizeof(*c->reads):sizeof(*c);offset=(size_t)((uintptr_t)capture-(uintptr_t)c);
  tag=i?(i%2?SLCLI_NATIVE_ORIGINAL_SOURCE_FILE_BUFFER:SLCLI_NATIVE_ORIGINAL_SOURCE_FILE_READ):SLCLI_NATIVE_ORIGINAL_SOURCE_FILE;
  if(!claim->claimed||claim->ordinal!=ordinal||claim->capture!=capture||claim->capture_bytes!=bytes||
   claim->containing_block!=c||claim->containing_bytes!=sizeof(*c)||claim->capture_offset!=offset||
   claim->native_class!=tag||c->book->original_ids[ordinal]!=c->ids_original[i]||
   c->book->references[ordinal].version!=c->plans_original[i].version)return 0;
 }
 return 1;
}
static int same(struct slcli_source_file_windows *c,struct slcli_arena *a) {
 size_t i;int born;uint16_t effects;const struct slcli_native_reference *r;
 const struct slcli_source_file_read_windows *read;
 if(!slcli_source_deadline_original_matches(a,c->deadline,c->caller)||
  !c->book||!slcli_arena_live_range(a,c->book,sizeof(*c->book))||
  c->book->deadline_original!=c->deadline||!c->book->count||!c->book->references||
  c->book->count>SIZE_MAX/sizeof(*r)||
  !slcli_arena_live_range(a,c->book->references,c->book->count*sizeof(*r))||
  !slcli_arena_live_block(a,c->path,c->path_bytes)||
  !slcli_arena_live_range(a,c->path_original,c->path_bytes)||!claimed_original(c,a))return 0;
 if(c->namespace_original&&(!slcli_installed_namespace_windows_matches_original(c->namespace_original,a)||
  c->namespace_original->book!=c->book||c->namespace_original->deadline!=c->deadline||
  c->namespace_original->caller!=c->caller||c->namespace_original->caller_bytes!=c->caller_bytes||
  c->namespace_original->leaf_borrow_attempted!=1||c->namespace_original->leaf_borrower_original!=c||
  c->namespace_original->leaf_borrower_bytes!=sizeof(*c)||
  c->path_original!=c->namespace_original->path||c->path_bytes!=c->namespace_original->path_bytes))return 0;
 for(i=0;i<c->path_bytes;i++)if(((const unsigned char *)c->path)[i]!=((const unsigned char *)c->path_original)[i])return 0;
 for(i=0;i<11;i++) {
  if(c->ordinals[i]>=c->book->count)return 0;r=&c->book->references[c->ordinals[i]];
  if(!i) {
   born=(!c->namespace_original||c->duplicate_status)&&c->file&&c->file!=INVALID_HANDLE_VALUE;
   effects=c->called[0]?(c->returned[0]?(born?15:7):3):1;
  } else {
   read=&c->reads[(i-1)/2];
   born=i%2?read->body!=NULL:read->read_returned&&read->read_status;
   effects=i%2?(read->allocated?(born?15:7):1):
    (read->read_called?(read->read_returned?(born?15:23):3):1);
   if(read->body&&(!read->bytes||!slcli_arena_live_block(a,read->body,read->bytes)))return 0;
  }
  if(r->version!=c->versions[i]||!r->version||r->native_original!=(i? (const void *)&c->reads[(i-1)/2]:(const void *)c)||
   r->native_class!=(i? (i%2?SLCLI_NATIVE_ORIGINAL_SOURCE_FILE_BUFFER:SLCLI_NATIVE_ORIGINAL_SOURCE_FILE_READ):SLCLI_NATIVE_ORIGINAL_SOURCE_FILE)||
   r->kind!=(i?(i%2?6:5):4)||r->effect_flags!=effects||r->state!=(born?0:2)||
   r->handle!=(!i&&born?(uintptr_t)c->file:0)||r->physical_extent!=(i?a->extent:UINT16_MAX)||
   r->alias_of!=(!i&&c->namespace_original?
    c->namespace_original->components[c->namespace_original->count-1].ordinal:UINT16_MAX))return 0;
 }
 return 1;
}
static int tick_before(struct slcli_source_file_windows *c,struct slcli_arena *a,
 struct slcli_native_clock *out,const struct slcli_native_clock *prior) {
 return same(c,a)&&prior&&prior->complete&&slcli_native_clock_read(out)&&
  out->tick>=prior->tick&&out->tick<c->deadline->expires&&same(c,a);
}
static int tick_after(struct slcli_source_file_windows *c,struct slcli_arena *a,
 struct slcli_native_clock *out,const struct slcli_native_clock *prior) {
 return slcli_native_clock_read(out)&&out->tick>=prior->tick&&
  out->tick<c->deadline->expires&&same(c,a);
}
/* Closed callers below derive every offset/size from actual retained headers.
 * No function pointer or externally supplied range can issue a native read. */
static int read_original(struct slcli_source_file_windows *c,struct slcli_arena *a,
 size_t slot,uint64_t offset,DWORD bytes,const struct slcli_native_clock *prior) {
 struct slcli_source_file_read_windows *r;struct slcli_native_reference *buffer,*io;
 if(slot>=5)return 0;r=&c->reads[slot];
 if(r->allocated||r->seek_called||!bytes||offset>INT64_MAX||
  c->bytes.QuadPart<=0||offset>(uint64_t)c->bytes.QuadPart||
  bytes>(uint64_t)c->bytes.QuadPart-offset||!same(c,a))return 0;
 r->offset=offset;r->bytes=bytes;r->requested.QuadPart=(LONGLONG)offset;
 buffer=&c->book->references[c->ordinals[1+slot*2]];io=&c->book->references[c->ordinals[2+slot*2]];
 buffer->effect_flags=3;r->body=slcli_arena_alloc(a,bytes);r->allocated=1;
 buffer->effect_flags=r->body?15:7;buffer->state=r->body?0:2;
 if(!r->body)return 0;
 if(!tick_before(c,a,&r->seek_before,prior))return 0;
 r->seek_args[0]=(uintptr_t)c->file;r->seek_args[1]=offset;
 r->seek_args[2]=(uintptr_t)&r->position;r->seek_args[3]=FILE_BEGIN;r->seek_called=1;
 r->seek_status=SetFilePointerEx(c->file,r->requested,&r->position,FILE_BEGIN);r->seek_returned=1;
 if(!r->seek_status)r->seek_error=GetLastError();
 if(!tick_after(c,a,&r->seek_after,&r->seek_before)||!r->seek_status||r->position.QuadPart!=r->requested.QuadPart)return 0;
 if(!tick_before(c,a,&r->read_before,&r->seek_after))return 0;
 r->read_args[0]=(uintptr_t)c->file;r->read_args[1]=(uintptr_t)r->body;r->read_args[2]=bytes;
 r->read_args[3]=(uintptr_t)&r->received;io->effect_flags=3;r->read_called=1;
 r->read_status=ReadFile(c->file,r->body,bytes,&r->received,NULL);r->read_returned=1;
 /* A negative return leaves an uncertain attempted IO original, including
  * partial bytes. Never infer no effect or release from a zero count. */
 io->effect_flags=r->read_status?15:23;io->state=r->read_status?0:2;
 if(!r->read_status)r->read_error=GetLastError();
 if(!tick_after(c,a,&r->read_after,&r->read_before)||!r->read_status||r->received!=bytes)return 0;
 r->complete=1;return 1;
}
/* Unique initialized-file range; actual section bytes are retained separately. */
static int range(const struct slcli_source_file_windows *c,uint32_t rva,uint32_t bytes,
 uint32_t *raw,uint32_t *ordinal) {
 size_t i;unsigned found=0;uint32_t va,virtual_bytes,raw_bytes,raw_at,delta;
 const unsigned char *s;
 if(rva>c->image_bytes||bytes>c->image_bytes-rva)return 0;
 for(i=0;i<c->section_count;i++) {
  s=c->reads[2].body+i*40;virtual_bytes=le32(s+8);va=le32(s+12);
  raw_bytes=le32(s+16);raw_at=le32(s+20);
  if(rva<va)continue;delta=rva-va;
  if(bytes>virtual_bytes||delta>virtual_bytes-bytes||bytes>raw_bytes||delta>raw_bytes-bytes)continue;
  if(raw_at>UINT32_MAX-delta||raw_at+delta>(uint64_t)c->bytes.QuadPart||
   bytes>(uint64_t)c->bytes.QuadPart-(raw_at+delta))return 0;
  *raw=raw_at+delta;*ordinal=(uint32_t)i;found++;
 }
 return found==1;
}
static const unsigned char *export_at(const struct slcli_source_file_windows *c,uint32_t rva,uint64_t bytes) {
 uint64_t delta;if(rva<c->export_rva)return NULL;delta=rva-c->export_rva;
 if(bytes>c->export_bytes||delta>c->export_bytes-bytes)return NULL;
 return c->reads[3].body+(size_t)delta;
}
int slcli_source_file_windows_matches_original(const struct slcli_source_file_windows *c,struct slcli_arena *a) {
 size_t i,j;uint32_t raw,ordinal,functions_count,names_count,name_rva,route,flags;
 uint32_t headers,va,span,previous_va,previous_span;
 unsigned found=0;uint16_t index;
 const unsigned char *exp,*functions,*names,*indices,*name,*section;
 const struct slcli_source_file_read_windows *read;
 const struct slcli_native_clock *previous;
 static const unsigned char wanted[]="slcli_node_route_cell_v1";
 if(!c||!a||!slcli_arena_live_block(a,c,sizeof(*c))||!c->started||!c->geometry_complete||
  !same((struct slcli_source_file_windows *)c,a)||!c->file||c->file==INVALID_HANDLE_VALUE||
  !c->called[0]||!c->returned[0]||c->raw[0]!=(uintptr_t)c->file||
  !c->called[1]||!c->returned[1]||!c->raw[1]||!c->called[2]||!c->returned[2]||!c->raw[2]||
  !c->called[3]||!c->returned[3]||!c->raw[3])return 0;
 if(c->namespace_original){
  const struct slcli_installed_namespace_windows *ns=c->namespace_original;
  if(!slcli_installed_namespace_windows_matches_original(ns,a)||ns->book!=c->book||
   ns->deadline!=c->deadline||ns->caller!=c->caller||ns->caller_bytes!=c->caller_bytes||
   c->path_original!=ns->path||c->path_bytes!=ns->path_bytes||!c->duplicate_status||
   c->file==ns->components[ns->count-1].handle||
   c->args[0][0]!=(uintptr_t)(HANDLE)(LONG_PTR)-1||
   c->args[0][1]!=(uintptr_t)ns->components[ns->count-1].handle||
   c->args[0][2]!=(uintptr_t)(HANDLE)(LONG_PTR)-1||
   c->args[0][3]!=(uintptr_t)&c->file||c->args[0][4]||c->args[0][5]||
   c->args[0][6]!=DUPLICATE_SAME_ACCESS||
   c->before[0].tick<ns->components[ns->count-1].query_after.tick)return 0;
 }else if(c->args[0][0]!=(uintptr_t)c->path||c->args[0][1]!=GENERIC_READ||
  c->args[0][2]!=FILE_SHARE_READ||c->args[0][3]||c->args[0][4]!=OPEN_EXISTING||
  c->args[0][5]!=(FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN)||c->args[0][6])return 0;
 if(c->args[1][0]!=(uintptr_t)c->file||c->args[1][1]!=(uintptr_t)&c->bytes||
  c->args[2][0]!=(uintptr_t)c->file||c->args[2][1]!=(uintptr_t)&c->information[0]||
  c->args[3][0]!=(uintptr_t)c->file||c->args[3][1]!=(uintptr_t)&c->information[1]||
  (c->information[0].dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)||
  (((uint64_t)c->information[0].nFileSizeHigh<<32)|c->information[0].nFileSizeLow)!=(uint64_t)c->bytes.QuadPart)return 0;
 previous=c->deadline->origin;
 for(i=0;i<3;i++) {
  if(!c->before[i].complete||!c->after[i].complete||
   c->before[i].tick<previous->tick||c->after[i].tick<c->before[i].tick||
   c->after[i].tick>=c->deadline->expires)return 0;
  previous=&c->after[i];
 }
 previous=&c->after[2];
 for(i=0;i<5;i++) {
  read=&c->reads[i];
  if(!read->complete||!read->read_called||!read->read_returned||!read->read_status||
   read->received!=read->bytes||!read->seek_called||!read->seek_returned||!read->seek_status||
   read->offset>INT64_MAX||read->requested.QuadPart!=(LONGLONG)read->offset||
   read->position.QuadPart!=(LONGLONG)read->offset||
   read->seek_args[0]!=(uintptr_t)c->file||read->seek_args[1]!=read->offset||
   read->seek_args[2]!=(uintptr_t)&read->position||read->seek_args[3]!=FILE_BEGIN||
   read->read_args[0]!=(uintptr_t)c->file||read->read_args[1]!=(uintptr_t)read->body||
   read->read_args[2]!=read->bytes||read->read_args[3]!=(uintptr_t)&read->received||read->read_args[4]||
   !previous->complete||!read->seek_before.complete||!read->seek_after.complete||
   !read->read_before.complete||!read->read_after.complete||
   read->seek_before.tick<previous->tick||read->seek_after.tick<read->seek_before.tick||
   read->read_before.tick<read->seek_after.tick||read->read_after.tick<read->read_before.tick||
   read->read_after.tick>=c->deadline->expires)return 0;
  previous=&read->read_after;
 }
 if(!c->before[3].complete||!c->after[3].complete||
  c->before[3].tick<previous->tick||c->after[3].tick<c->before[3].tick||
  c->after[3].tick>=c->deadline->expires)return 0;
 if(c->reads[0].bytes!=64||c->reads[1].bytes!=264||c->reads[4].bytes!=128||
  c->reads[0].offset||c->reads[1].offset!=c->pe_offset||
  c->reads[2].offset!=(uint64_t)c->pe_offset+24+c->optional_bytes||
  le16(c->reads[0].body)!=0x5a4d||le32(c->reads[1].body)!=0x4550||
  le16(c->reads[1].body+4)!=0x8664||le16(c->reads[1].body+24)!=0x20b||
  !c->section_count||c->optional_bytes<240||le32(c->reads[1].body+132)<1||
  !c->image_bytes||c->export_rva>c->image_bytes||c->export_bytes>c->image_bytes-c->export_rva||
  c->pe_offset!=le32(c->reads[0].body+60)||c->section_count!=le16(c->reads[1].body+6)||
  c->optional_bytes!=le16(c->reads[1].body+20)||c->image_bytes!=le32(c->reads[1].body+80)||
  c->export_rva!=le32(c->reads[1].body+136)||c->export_bytes!=le32(c->reads[1].body+140)||
  c->reads[2].bytes!=c->section_count*40||c->reads[3].bytes!=c->export_bytes||
  !range(c,c->route_rva,128,&raw,&ordinal)||raw!=c->route_raw_offset||ordinal!=c->route_section)return 0;
 headers=le32(c->reads[1].body+84);
 if(!headers||headers>c->image_bytes||c->bytes.QuadPart<=0)return 0;
 for(i=0;i<c->section_count;i++) {
  section=c->reads[2].body+i*40;va=le32(section+12);span=le32(section+8);raw=le32(section+20);
  if(!span||va<headers||va>c->image_bytes||span>c->image_bytes-va||
   raw>(uint64_t)c->bytes.QuadPart||le32(section+16)>(uint64_t)c->bytes.QuadPart-raw)return 0;
  for(j=0;j<i;j++) {
   previous_va=le32(c->reads[2].body+j*40+12);previous_span=le32(c->reads[2].body+j*40+8);
   if((uint64_t)va<(uint64_t)previous_va+previous_span&&(uint64_t)previous_va<(uint64_t)va+span)return 0;
  }
 }
 /* Re-derive the unique locator from the retained export original. A cached
  * RVA is not a substitute for the bytes that selected it. This is a source
  * geometry check only, not native image identity or namespace authority. */
 if(c->export_bytes<40||!range(c,c->export_rva,c->export_bytes,&raw,&ordinal)||
  c->reads[3].offset!=raw||c->reads[4].offset!=c->route_raw_offset)return 0;
 exp=c->reads[3].body;functions_count=le32(exp+20);names_count=le32(exp+24);
 functions=export_at(c,le32(exp+28),(uint64_t)functions_count*4);
 names=export_at(c,le32(exp+32),(uint64_t)names_count*4);
 indices=export_at(c,le32(exp+36),(uint64_t)names_count*2);
 if(!functions_count||!names_count||!functions||!names||!indices)return 0;
 for(i=0;i<names_count;i++) {
  name_rva=le32(names+i*4);name=export_at(c,name_rva,1);index=le16(indices+i*2);
  if(index>=functions_count||!name)return 0;
  for(j=0;j<c->export_bytes-(name_rva-c->export_rva)&&name[j];j++){}
  if(j==c->export_bytes-(name_rva-c->export_rva))return 0;
  if(j+1!=sizeof(wanted))continue;
  for(j=0;j<sizeof(wanted)&&name[j]==wanted[j];j++){}
  if(j!=sizeof(wanted))continue;
  if(++found!=1)return 0;route=le32(functions+index*4);
  if(route!=c->route_rva||(route>=c->export_rva&&route-c->export_rva<c->export_bytes)||
   !range(c,route,128,&raw,&ordinal)||raw!=c->route_raw_offset||ordinal!=c->route_section)return 0;
  section=c->reads[2].body+ordinal*40;flags=le32(section+36);
  if(!(flags&0x80000000u)||(flags&0x20000000u))return 0;
 }
 if(found!=1)return 0;
 for(i=0;i<128;i++)if(c->reads[4].body[i])return 0;
 for(i=0;i<sizeof(c->information[0]);i++)if(((const unsigned char *)&c->information[0])[i]!=((const unsigned char *)&c->information[1])[i])return 0;
 return 1;
}
static int probe_original(struct slcli_source_file_windows *c,
 struct slcli_arena *a,const struct slcli_binding_originals *b,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const WCHAR *path,size_t path_bytes,const uint32_t *ordinals,
 struct slcli_installed_namespace_windows *ns,struct slcli_binding_storage_original *storage,
 const struct slcli_native_reference *plans,const uint32_t *ids) {
 size_t i,j;struct slcli_native_reference *r;BOOL ok;
 const struct slcli_native_reference *planned;const void *capture;size_t capture_bytes;
 enum slcli_native_original_class tag;
 const unsigned char *nt,*exp,*names,*functions,*indices,*name,*section;
 uint32_t raw,ordinal,names_count,functions_count,name_rva,route,flags,headers,va,span;
 uint32_t previous_va,previous_span;uint16_t index;
 static const unsigned char wanted[]="slcli_node_route_cell_v1";
 if(!a||!c||!slcli_arena_live_block(a,c,sizeof(*c))||!b||
  !slcli_arena_live_range(a,b,sizeof(*b))||b->deadline_original!=d||!b->count||!b->references||
  b->count>SIZE_MAX/sizeof(*r)||!slcli_arena_live_range(a,b->references,b->count*sizeof(*r))||
  !slcli_source_deadline_original_matches(a,d,caller)||!caller_bytes||
  !slcli_arena_live_block(a,caller,caller_bytes)||!prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||
  !prior->complete||!ordinals||!slcli_arena_live_range(a,ordinals,11*sizeof(*ordinals))||
  !path||path_bytes<2*sizeof(WCHAR)||path_bytes%sizeof(WCHAR)||!slcli_arena_live_range(a,path,path_bytes)||
  !separate(c,sizeof(*c),b,sizeof(*b))||!separate(c,sizeof(*c),b->references,b->count*sizeof(*r))||
  !separate(c,sizeof(*c),caller,caller_bytes)||!separate(c,sizeof(*c),prior,sizeof(*prior))||
  !separate(c,sizeof(*c),path,path_bytes)||!separate(c,sizeof(*c),ordinals,11*sizeof(*ordinals))||
  !slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
 if(ns&&(!slcli_installed_namespace_windows_matches_original(ns,a)||ns->book!=b||
  ns->deadline!=d||ns->caller!=caller||ns->caller_bytes!=caller_bytes||
  path!=ns->path||path_bytes!=ns->path_bytes||ns->leaf_borrow_attempted||
  ns->leaf_borrower_original||ns->leaf_borrower_bytes||
  prior->tick<ns->components[ns->count-1].query_after.tick||
  ns->components[ns->count-1].ordinal>=UINT16_MAX||
  ns->components[ns->count-1].ordinal>=ordinals[0]||
  !separate(c,sizeof(*c),ns,sizeof(*ns))||
  !separate(c,sizeof(*c),ns->components,ns->count*sizeof(*ns->components))))return 0;
 for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
 if(storage&&(!ns||!slcli_arena_live_block(a,storage,sizeof(*storage))||!storage->prepared||
  storage->arena!=a||storage->book!=b||b->storage_original!=storage||storage->attempted!=b->count||
  storage->attempted>storage->capacity||11>storage->capacity-storage->attempted||
  !slcli_arena_live_block(a,plans,11*sizeof(*plans))||!slcli_arena_live_block(a,ids,11*sizeof(*ids))||
  !separate(c,sizeof(*c),plans,11*sizeof(*plans))||!separate(c,sizeof(*c),ids,11*sizeof(*ids))))return 0;
 for(i=0;i<path_bytes/sizeof(WCHAR);i++)if(i+1==path_bytes/sizeof(WCHAR)?path[i]!=0:path[i]==0)return 0;
 for(i=0;i<11;i++) {
  if(storage){if(ordinals[i]!=storage->attempted+i)return 0;}else if(ordinals[i]>=b->count)return 0;
  for(j=0;j<i;j++)if(ordinals[j]==ordinals[i])return 0;
  planned=storage?&plans[i]:&b->references[ordinals[i]];
  if(planned->kind!=(i?(i%2?6:5):4)||!planned->version||planned->effect_flags!=1||planned->state!=2||planned->handle||
   planned->native_original||planned->service_request_original||planned->native_class!=SLCLI_NATIVE_ORIGINAL_UNCONSTRUCTED||
   planned->physical_extent!=(i?a->extent:UINT16_MAX)||planned->alias_of!=(!i&&ns?
    ns->components[ns->count-1].ordinal:UINT16_MAX))return 0;
 }
 c->started=1;c->book=b;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;c->namespace_original=ns;
 c->storage_original=storage;c->plans_original=plans;c->ids_original=ids;
 if(ns){ns->leaf_borrower_original=c;ns->leaf_borrower_bytes=sizeof(*c);ns->leaf_borrow_attempted=1;}
 if(storage)for(i=0;i<11;i++){
  capture=i?(const void *)&c->reads[(i-1)/2]:(const void *)c;
  capture_bytes=i?sizeof(*c->reads):sizeof(*c);
  tag=i?(i%2?SLCLI_NATIVE_ORIGINAL_SOURCE_FILE_BUFFER:SLCLI_NATIVE_ORIGINAL_SOURCE_FILE_READ):SLCLI_NATIVE_ORIGINAL_SOURCE_FILE;
  if(!slcli_binding_storage_claim_pre_effect_original(storage,&plans[i],ids[i],c,sizeof(*c),capture,capture_bytes,tag))return 0;
 }
 c->path_original=path;c->path_bytes=path_bytes;c->path=slcli_arena_alloc(a,path_bytes);if(!c->path)return 0;
 for(i=0;i<path_bytes;i++)((unsigned char *)c->path)[i]=((const unsigned char *)path)[i];
 for(i=0;i<11;i++) {
  r=&b->references[ordinals[i]];c->ordinals[i]=ordinals[i];c->versions[i]=r->version;
  r->native_original=i?(const void *)&c->reads[(i-1)/2]:(const void *)c;
  r->native_class=i?(i%2?SLCLI_NATIVE_ORIGINAL_SOURCE_FILE_BUFFER:SLCLI_NATIVE_ORIGINAL_SOURCE_FILE_READ):SLCLI_NATIVE_ORIGINAL_SOURCE_FILE;
 }
 if(!tick_before(c,a,&c->before[0],prior))return 0;
 r=&b->references[ordinals[0]];r->effect_flags=3;c->called[0]=1;
 if(ns){
  c->args[0][0]=(uintptr_t)(HANDLE)(LONG_PTR)-1;c->args[0][1]=(uintptr_t)ns->components[ns->count-1].handle;
  c->args[0][2]=(uintptr_t)(HANDLE)(LONG_PTR)-1;c->args[0][3]=(uintptr_t)&c->file;
  c->args[0][6]=DUPLICATE_SAME_ACCESS;
  c->duplicate_status=DuplicateHandle((HANDLE)(LONG_PTR)-1,ns->components[ns->count-1].handle,
   (HANDLE)(LONG_PTR)-1,&c->file,0,FALSE,DUPLICATE_SAME_ACCESS);c->returned[0]=1;
 }else{
  c->args[0][0]=(uintptr_t)c->path;c->args[0][1]=GENERIC_READ;c->args[0][2]=FILE_SHARE_READ;
  c->args[0][4]=OPEN_EXISTING;c->args[0][5]=FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN;
  c->file=CreateFileW(c->path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
   FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);c->returned[0]=1;
 }
 c->raw[0]=(uintptr_t)c->file;
 r->effect_flags=(!ns||c->duplicate_status)&&c->file&&c->file!=INVALID_HANDLE_VALUE?15:7;r->state=r->effect_flags==15?0:2;
 r->handle=r->effect_flags==15?(uintptr_t)c->file:0;
 if(r->state==2)c->errors[0]=GetLastError();
 if(!tick_after(c,a,&c->after[0],&c->before[0])||r->state==2)return 0;
 if(!tick_before(c,a,&c->before[1],&c->after[0]))return 0;
 c->args[1][0]=(uintptr_t)c->file;c->args[1][1]=(uintptr_t)&c->bytes;c->called[1]=1;
 ok=GetFileSizeEx(c->file,&c->bytes);c->raw[1]=(uint32_t)ok;c->returned[1]=1;
 if(!ok)c->errors[1]=GetLastError();
 if(!tick_after(c,a,&c->after[1],&c->before[1])||!ok||c->bytes.QuadPart<64)return 0;
 if(!tick_before(c,a,&c->before[2],&c->after[1]))return 0;
 c->args[2][0]=(uintptr_t)c->file;c->args[2][1]=(uintptr_t)&c->information[0];c->called[2]=1;
 ok=GetFileInformationByHandle(c->file,&c->information[0]);c->raw[2]=(uint32_t)ok;c->returned[2]=1;
 if(!ok)c->errors[2]=GetLastError();
 if(!tick_after(c,a,&c->after[2],&c->before[2])||!ok||
  (c->information[0].dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)||
  (((uint64_t)c->information[0].nFileSizeHigh<<32)|c->information[0].nFileSizeLow)!=(uint64_t)c->bytes.QuadPart)return 0;
 if(!read_original(c,a,0,0,64,&c->after[2])||le16(c->reads[0].body)!=0x5a4d)return 0;
 c->pe_offset=le32(c->reads[0].body+60);
 if(!read_original(c,a,1,c->pe_offset,264,&c->reads[0].read_after))return 0;
 nt=c->reads[1].body;c->section_count=le16(nt+6);c->optional_bytes=le16(nt+20);
 if(le32(nt)!=0x4550||le16(nt+4)!=0x8664||!c->section_count||c->optional_bytes<240||
  le16(nt+24)!=0x20b||le32(nt+24+108)<1)return 0;
 c->image_bytes=le32(nt+24+56);c->export_rva=le32(nt+24+112);c->export_bytes=le32(nt+24+116);
 if(!c->image_bytes||c->export_bytes<40||c->export_rva>c->image_bytes||
  c->export_bytes>c->image_bytes-c->export_rva)return 0;
 if(!read_original(c,a,2,(uint64_t)c->pe_offset+24+c->optional_bytes,c->section_count*40,&c->reads[1].read_after)||
  !c->reads[2].complete)return 0;
 headers=le32(nt+24+60);
 if(!headers||headers>c->image_bytes)return 0;
 for(i=0;i<c->section_count;i++) {
  section=c->reads[2].body+i*40;va=le32(section+12);span=le32(section+8);
  raw=le32(section+20);
  if(!span||va<headers||va>c->image_bytes||span>c->image_bytes-va||
   raw>(uint64_t)c->bytes.QuadPart||le32(section+16)>(uint64_t)c->bytes.QuadPart-raw)return 0;
  for(j=0;j<i;j++) {
   previous_va=le32(c->reads[2].body+j*40+12);previous_span=le32(c->reads[2].body+j*40+8);
   if((uint64_t)va<(uint64_t)previous_va+previous_span&&(uint64_t)previous_va<(uint64_t)va+span)return 0;
  }
 }
 if(!range(c,c->export_rva,c->export_bytes,&raw,&ordinal)||
  !read_original(c,a,3,raw,c->export_bytes,&c->reads[2].read_after))return 0;
 exp=c->reads[3].body;functions_count=le32(exp+20);names_count=le32(exp+24);
 functions=export_at(c,le32(exp+28),(uint64_t)functions_count*4);
 names=export_at(c,le32(exp+32),(uint64_t)names_count*4);
 indices=export_at(c,le32(exp+36),(uint64_t)names_count*2);
 if(!functions_count||!names_count||!functions||!names||!indices)return 0;
 for(i=0;i<names_count;i++) {
  name_rva=le32(names+i*4);name=export_at(c,name_rva,1);index=le16(indices+i*2);
  if(index>=functions_count||!name)return 0;
  for(j=0;j<c->export_bytes-(name_rva-c->export_rva)&&name[j];j++){}
  if(j==c->export_bytes-(name_rva-c->export_rva))return 0;
  if(j+1!=sizeof(wanted))continue;
  for(j=0;j<sizeof(wanted)&&name[j]==wanted[j];j++){}if(j!=sizeof(wanted))continue;
  if(c->route_rva)return 0;route=le32(functions+index*4);
  if(route>=c->export_rva&&route-c->export_rva<c->export_bytes)return 0;
  if(!range(c,route,128,&raw,&ordinal))return 0;
  section=c->reads[2].body+ordinal*40;flags=le32(section+36);
  if(!(flags&0x80000000u)||(flags&0x20000000u))return 0;
  c->route_rva=route;c->route_raw_offset=raw;c->route_section=ordinal;
 }
 if(!c->route_rva||!read_original(c,a,4,c->route_raw_offset,128,&c->reads[3].read_after))return 0;
 for(i=0;i<128;i++)if(c->reads[4].body[i])return 0;
 if(!tick_before(c,a,&c->before[3],&c->reads[4].read_after))return 0;
 c->args[3][0]=(uintptr_t)c->file;c->args[3][1]=(uintptr_t)&c->information[1];c->called[3]=1;
 ok=GetFileInformationByHandle(c->file,&c->information[1]);c->raw[3]=(uint32_t)ok;c->returned[3]=1;
 if(!ok)c->errors[3]=GetLastError();
 if(!tick_after(c,a,&c->after[3],&c->before[3])||!ok)return 0;
 for(i=0;i<sizeof(c->information[0]);i++)if(((const unsigned char *)&c->information[0])[i]!=((const unsigned char *)&c->information[1])[i])return 0;
 c->geometry_complete=1;return 1;
}
int slcli_source_file_windows_probe_original(struct slcli_source_file_windows *c,
 struct slcli_arena *a,const struct slcli_binding_originals *b,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const WCHAR *path,size_t path_bytes,const uint32_t *ordinals){
 return probe_original(c,a,b,d,caller,caller_bytes,prior,path,path_bytes,ordinals,NULL,NULL,NULL,NULL);
}
int slcli_source_file_windows_probe_held_original(struct slcli_source_file_windows *c,
 struct slcli_arena *a,const struct slcli_binding_originals *b,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,struct slcli_installed_namespace_windows *ns,
 const uint32_t *ordinals){
 if(!slcli_installed_namespace_windows_matches_original(ns,a))return 0;
 return probe_original(c,a,b,d,caller,caller_bytes,prior,ns->path,ns->path_bytes,ordinals,ns,NULL,NULL,NULL);
}
int slcli_source_file_windows_probe_held_claimed_original(struct slcli_source_file_windows *c,
 struct slcli_arena *a,struct slcli_binding_storage_original *storage,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,struct slcli_installed_namespace_windows *ns,
 const uint32_t *ordinals,const struct slcli_native_reference *plans,const uint32_t *ids){
 if(!storage||!a||!slcli_arena_live_block(a,storage,sizeof(*storage))||
  !slcli_installed_namespace_windows_matches_original(ns,a)||ns->storage_original!=storage)return 0;
 return probe_original(c,a,storage->book,d,caller,caller_bytes,prior,ns->path,ns->path_bytes,ordinals,ns,storage,plans,ids);
}
#endif
