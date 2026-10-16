#include "route_write_windows.h"
#if defined(_WIN32)
#include "launcher_native.h"
static int separate(const void *p,size_t n,const void *q,size_t m){
 uintptr_t x=(uintptr_t)p,y=(uintptr_t)q;
 return p&&q&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int claimed_original(const struct slcli_route_write_windows *c,struct slcli_arena *a){
 const struct slcli_binding_storage_original *s=c->storage_original;
 const struct slcli_binding_originals *b=c->root->binding_original;
 const struct slcli_route_read_windows *read;const struct slcli_binding_row_claim_original *claim;
 struct slcli_binding_storage_geometry g;const void *container;size_t i,n,bytes,offset;uint32_t ordinal;
 if(!s)return !c->plans_original&&!c->ids_original&&!c->ordinals_original;
 if(c->capacity>(SIZE_MAX-2)/3)return 0;n=c->capacity*3+2;
 if(!slcli_arena_live_block(a,s,sizeof(*s))||!s->prepared||s->arena!=a||s->book!=b||
  b->storage_original!=s||s->attempted!=b->count||s->attempted>s->capacity||
  !slcli_binding_storage_geometry_derive(s->capacity,&g)||s->references!=b->references||s->ids!=b->original_ids||
  !slcli_arena_live_block(a,s->references,g.references_bytes)||!slcli_arena_live_block(a,s->ids,g.ids_bytes)||
  !slcli_arena_live_block(a,s->claims,g.claims_bytes)||n>SIZE_MAX/sizeof(*c->plans_original)||n>SIZE_MAX/sizeof(uint32_t)||
  !slcli_arena_live_block(a,c->plans_original,n*sizeof(*c->plans_original))||
  !slcli_arena_live_block(a,c->ids_original,n*sizeof(uint32_t))||
  !slcli_arena_live_block(a,c->ordinals_original,n*sizeof(uint32_t)))return 0;
 for(i=0;i<n;i++){
  read=i<c->capacity*3?&c->rows[i/3].reads[i%3]:i==n-2?&c->initial:&c->readback;
  container=i<c->capacity*3?(const void *)c->rows:(const void *)c->root;
  bytes=i<c->capacity*3?c->capacity*sizeof(*c->rows):sizeof(*c->root);
  offset=(size_t)((uintptr_t)read-(uintptr_t)container);ordinal=c->ordinals_original[i];
  if(ordinal>=s->attempted||read->ordinal!=ordinal)return 0;claim=&s->claims[ordinal];
  if(!claim->claimed||claim->ordinal!=ordinal||claim->capture!=read||claim->capture_bytes!=sizeof(*read)||
   claim->containing_block!=container||claim->containing_bytes!=bytes||claim->capture_offset!=offset||
   claim->native_class!=SLCLI_NATIVE_ORIGINAL_CHILD_ROUTE_BUFFER||b->original_ids[ordinal]!=c->ids_original[i]||
   b->references[ordinal].version!=c->plans_original[i].version)return 0;
 }
 return 1;
}
static int same(struct slcli_route_write_windows *c,struct slcli_arena *a) {
 const struct slcli_runtime_containing_windows *r=c->root;
 const struct slcli_launcher_windows_capture *b=c->birth;size_t i;
 unsigned char encoded[128];
 if(!r||!slcli_arena_live_block(a,r,sizeof(*r))||!c->prepared||
  !r->capture_prepared||r->first_failure||!r->layout||
  !slcli_arena_live_block(a,r->layout,sizeof(*r->layout))||
  !r->alias||!slcli_arena_live_block(a,r->alias,sizeof(*r->alias))||
  !r->alias->qualified||!r->alias->validated||r->alias->unmap_attempted||
  c->capacity!=r->layout->regions||!c->capacity||c->capacity>SIZE_MAX/sizeof(*c->rows)||
  !slcli_arena_live_block(a,c->rows,c->capacity*sizeof(*c->rows))||
  !slcli_source_file_windows_matches_original(c->source,a)||c->source->book!=r->binding_original||
  c->source->deadline!=r->deadline||c->source->caller!=r->caller||
  !slcli_source_deadline_original_matches(a,r->deadline,r->caller)||!claimed_original(c,a))return 0;
 if(!b)return 1;
 if(!slcli_arena_live_block(a,b,sizeof(*b)))return 0;
 if(!b->route_encode_returned||!b->route_encode_result||
  b->route_original.transport!=b->transport_handle_original||!b->transport_original||
  b->route_original.parent!=b->transport_original->parent||
  b->route_original.base!=r->alias->view||b->route_original.bytes!=r->original.runtime_bytes||
  b->route_original.capture_bytes!=r->layout->capture_bytes||
  b->route_original.threads!=r->layout->threads||b->route_original.events!=r->layout->events||
  b->route_original.regions!=r->layout->regions||b->route_original.modules!=r->layout->modules||
  b->route_original.name_units!=r->layout->name_units||
  b->route_original.page_bytes!=r->layout->geometry.dwPageSize||
  b->route_original.granularity!=r->layout->geometry.dwAllocationGranularity||
  !slcli_node_route_windows_encode(encoded,sizeof(encoded),&b->route_original))return 0;
 for(i=0;i<128;i++)if(encoded[i]!=b->route_bytes[i]||encoded[i]!=c->route_bytes_original[i])return 0;
 return slcli_arena_live_block(a,b,sizeof(*b))&&b->containing_original==r&&
  b->create_called&&b->create_returned&&b->create_status&&b->created&&
  b->times_called&&b->times_returned&&b->times_status&&!b->resume_attempted&&
  r->child.birth_original==b&&r->child.qualified&&
  r->child.process_original==b->process.hProcess&&r->child.process_id_original==b->process.dwProcessId&&
  r->child.thread_id_original==b->process.dwThreadId&&r->child.base==b->route_original.base&&
  r->child.bytes==b->route_original.bytes&&
  r->child.creation_original.dwLowDateTime==b->creation_time.dwLowDateTime&&
  r->child.creation_original.dwHighDateTime==b->creation_time.dwHighDateTime;
}
static int before(struct slcli_route_write_windows *c,struct slcli_arena *a,
 struct slcli_native_clock *out,const struct slcli_native_clock *prior) {
 return same(c,a)&&prior&&prior->complete&&slcli_native_clock_read(out)&&
  out->tick>=prior->tick&&out->tick<c->root->deadline->expires&&same(c,a);
}
static int after(struct slcli_route_write_windows *c,struct slcli_arena *a,
 struct slcli_native_clock *out,const struct slcli_native_clock *prior) {
 return slcli_native_clock_read(out)&&out->tick>=prior->tick&&
  out->tick<c->root->deadline->expires&&same(c,a);
}
static int allocate(struct slcli_route_write_windows *c,struct slcli_arena *a,
 struct slcli_route_read_windows *read,uint32_t ordinal,size_t bytes,size_t index) {
 struct slcli_native_reference *r;const void *container;size_t container_bytes;
 if(c->storage_original){
  container=index<c->capacity*3?(const void *)c->rows:(const void *)c->root;
  container_bytes=index<c->capacity*3?c->capacity*sizeof(*c->rows):sizeof(*c->root);
  if(!slcli_binding_storage_claim_pre_effect_original(c->storage_original,&c->plans_original[index],
   c->ids_original[index],container,container_bytes,read,sizeof(*read),SLCLI_NATIVE_ORIGINAL_CHILD_ROUTE_BUFFER))return 0;
 }
 r=&c->root->binding_original->references[ordinal];
 read->ordinal=ordinal;read->version=r->version;read->bytes=bytes;
 r->native_original=read;r->native_class=SLCLI_NATIVE_ORIGINAL_CHILD_ROUTE_BUFFER;
 r->effect_flags=3;read->body=slcli_arena_alloc(a,bytes);read->allocated=1;
 r->effect_flags=read->body?15:7;r->state=read->body?0:2;return read->body!=NULL;
}
static int prepare_original(struct slcli_route_write_windows *c,
 struct slcli_runtime_containing_windows *root,struct slcli_arena *a,
 const struct slcli_source_file_windows *source,const uint32_t *ordinals,size_t count,
 struct slcli_binding_storage_original *storage,const struct slcli_native_reference *plans,const uint32_t *ids) {
 size_t i,j,n,p;const struct slcli_native_reference *row;
 if(!a||!root||!slcli_arena_live_block(a,root,sizeof(*root))||c!=&root->route_writer||
  !root->layout||!slcli_arena_live_block(a,root->layout,sizeof(*root->layout))||
  !root->layout->ready||!slcli_source_file_windows_matches_original(source,a)||
  source->book!=root->binding_original||source->deadline!=root->deadline||source->caller!=root->caller||
  !root->binding_original||!root->binding_original->references||!root->layout->regions||
  root->layout->regions>(SIZE_MAX-2)/3)return 0;
 p=root->layout->regions;n=p*3+2;
 if(count!=n||n>SIZE_MAX/sizeof(*ordinals)||!ordinals||
  !slcli_arena_live_range(a,ordinals,n*sizeof(*ordinals))||p>SIZE_MAX/sizeof(*c->rows))return 0;
 for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
 if(storage&&(!slcli_arena_live_block(a,storage,sizeof(*storage))||!storage->prepared||storage->arena!=a||
  storage->book!=root->binding_original||source->storage_original!=storage||
  root->binding_original->storage_original!=storage||
  storage->attempted!=root->binding_original->count||storage->attempted>storage->capacity||
  n>storage->capacity-storage->attempted||n>SIZE_MAX/sizeof(*plans)||
  !slcli_arena_live_block(a,plans,n*sizeof(*plans))||!slcli_arena_live_block(a,ids,n*sizeof(*ids))||
  !separate(root,sizeof(*root),plans,n*sizeof(*plans))||!separate(root,sizeof(*root),ids,n*sizeof(*ids))||
  !separate(root,sizeof(*root),ordinals,n*sizeof(*ordinals))))return 0;
 for(i=0;i<n;i++) {
  if(storage){if(ordinals[i]!=storage->attempted+i)return 0;}else if(ordinals[i]>=root->binding_original->count)return 0;
  for(j=0;j<i;j++)if(ordinals[i]==ordinals[j])return 0;
  row=storage?&plans[i]:&root->binding_original->references[ordinals[i]];
  if(row->kind!=6||!row->version||row->effect_flags!=1||row->state!=2||row->handle||
   row->native_original||row->service_request_original||row->native_class!=SLCLI_NATIVE_ORIGINAL_UNCONSTRUCTED||
   row->physical_extent!=a->extent||row->alias_of!=UINT16_MAX)return 0;
 }
 c->started=1;c->root=root;c->source=source;c->capacity=p;
 c->storage_original=storage;c->plans_original=plans;c->ids_original=ids;c->ordinals_original=storage?ordinals:NULL;
 c->minimum=(uintptr_t)root->layout->geometry.lpMinimumApplicationAddress;
 c->maximum=(uintptr_t)root->layout->geometry.lpMaximumApplicationAddress;
 if(!c->minimum||c->maximum<c->minimum||c->maximum==UINTPTR_MAX)return 0;
 c->rows=slcli_arena_calloc(a,p,sizeof(*c->rows));if(!c->rows)return 0;
 for(i=0;i<p;i++)for(j=0;j<3;j++)if(!allocate(c,a,&c->rows[i].reads[j],ordinals[i*3+j],source->reads[j].bytes,i*3+j))return 0;
 if(!allocate(c,a,&c->initial,ordinals[p*3],128,p*3)||!allocate(c,a,&c->readback,ordinals[p*3+1],128,p*3+1))return 0;
 c->prepared=1;return 1;
}
int slcli_route_write_windows_prepare_original(struct slcli_route_write_windows *c,
 struct slcli_runtime_containing_windows *root,struct slcli_arena *a,
 const struct slcli_source_file_windows *source,const uint32_t *ordinals,size_t count){
 return prepare_original(c,root,a,source,ordinals,count,NULL,NULL,NULL);
}
int slcli_route_write_windows_prepare_claimed_original(struct slcli_route_write_windows *c,
 struct slcli_runtime_containing_windows *root,struct slcli_arena *a,
 const struct slcli_source_file_windows *source,const uint32_t *ordinals,size_t count,
 struct slcli_binding_storage_original *storage,const struct slcli_native_reference *plans,const uint32_t *ids){
 if(!storage)return 0;return prepare_original(c,root,a,source,ordinals,count,storage,plans,ids);
}
static int read_original(struct slcli_route_write_windows *c,struct slcli_arena *a,
 struct slcli_route_read_windows *r,const void *address,const struct slcli_native_clock *prior) {
 const struct slcli_native_reference *row;
 if(!r->allocated||!r->body||!r->bytes||r->called||
  !slcli_arena_live_block(a,r->body,r->bytes)||
  r->ordinal>=c->root->binding_original->count)return 0;
 row=&c->root->binding_original->references[r->ordinal];
 if(row->native_original!=r||row->native_class!=SLCLI_NATIVE_ORIGINAL_CHILD_ROUTE_BUFFER||
  row->version!=r->version||!row->version||row->kind!=6||row->effect_flags!=15||row->state||row->handle||
  row->physical_extent!=a->extent||row->alias_of!=UINT16_MAX||
  !address||(uintptr_t)address>UINTPTR_MAX-r->bytes||!before(c,a,&r->before,prior))return 0;
 r->address=address;r->args[0]=(uintptr_t)c->birth->process.hProcess;r->args[1]=(uintptr_t)address;
 r->args[2]=(uintptr_t)r->body;r->args[3]=r->bytes;r->args[4]=(uintptr_t)&r->received;r->called=1;
 r->status=ReadProcessMemory(c->birth->process.hProcess,address,r->body,r->bytes,&r->received);r->returned=1;
 if(!r->status)r->error=GetLastError();
 if(!after(c,a,&r->after,&r->before)||!r->status||r->received!=r->bytes)return 0;
 r->complete=1;return 1;
}
static int equal(const unsigned char *a,const unsigned char *b,size_t n) {
 size_t i;for(i=0;i<n;i++)if(a[i]!=b[i])return 0;return 1;
}
int slcli_route_write_windows_write_original(struct slcli_runtime_containing_windows *root,
 struct slcli_arena *a,const struct slcli_launcher_windows_capture *birth) {
 struct slcli_route_write_windows *c;struct slcli_route_image_windows *r;
 const struct slcli_native_clock *prior;size_t i,j;uintptr_t base,end,address;uint64_t offset;
 if(!root||!a||!slcli_arena_live_block(a,root,sizeof(*root))||!birth||
  !slcli_arena_live_block(a,birth,sizeof(*birth)))return 0;c=&root->route_writer;
 if(!c->prepared||c->birth||c->used||c->write_called||birth->containing_original!=root)return 0;
 for(i=0;i<128;i++)c->route_bytes_original[i]=birth->route_bytes[i];
 c->birth=birth;if(!same(c,a))return 0;
 c->cursor=c->minimum;prior=&root->child.query_after;
 while(c->cursor<=c->maximum) {
  if(c->used>=c->capacity)return 0;r=&c->rows[c->used++];
  if(!before(c,a,&r->before,prior))return 0;
  r->args[0]=(uintptr_t)birth->process.hProcess;r->args[1]=c->cursor;
  r->args[2]=(uintptr_t)&r->geometry;r->args[3]=sizeof(r->geometry);r->called=1;
  r->status=VirtualQueryEx(birth->process.hProcess,(const void *)c->cursor,&r->geometry,sizeof(r->geometry));r->returned=1;
  if(!r->status)r->error=GetLastError();
  if(!after(c,a,&r->after,&r->before)||r->status!=sizeof(r->geometry))return 0;
  base=(uintptr_t)r->geometry.BaseAddress;
  if(base!=c->cursor||!r->geometry.RegionSize||base>UINTPTR_MAX-r->geometry.RegionSize||
   !root->layout->geometry.dwPageSize||base%root->layout->geometry.dwPageSize||
   r->geometry.RegionSize%root->layout->geometry.dwPageSize)return 0;
  end=base+r->geometry.RegionSize;if(end<=c->cursor||end>c->maximum+1)return 0;
  prior=&r->after;
  if(r->geometry.State==MEM_COMMIT&&r->geometry.Type==MEM_IMAGE&&r->geometry.AllocationBase==r->geometry.BaseAddress) {
   for(j=0;j<3;j++) {
    offset=j==0?0:j==1?c->source->pe_offset:(uint64_t)c->source->pe_offset+24+c->source->optional_bytes;
    if(offset>c->source->image_bytes||r->reads[j].bytes>c->source->image_bytes-offset||
     base>UINTPTR_MAX-offset||!read_original(c,a,&r->reads[j],(const void *)(base+(uintptr_t)offset),prior))return 0;
    prior=&r->reads[j].after;
    if(!equal(r->reads[j].body,c->source->reads[j].body,r->reads[j].bytes))break;
   }
   if(j==3){r->headers_match=1;c->matches++;c->image_base=base;if(c->matches>1)return 0;}
  }
  c->cursor=end;
 }
 c->scan_complete=1;if(c->matches!=1||c->image_base>UINTPTR_MAX-c->source->image_bytes||
  c->source->route_rva>c->source->image_bytes||128>c->source->image_bytes-c->source->route_rva)return 0;
 address=c->image_base+c->source->route_rva;
 if(!before(c,a,&c->query_before,prior))return 0;
 c->query_args[0]=(uintptr_t)birth->process.hProcess;c->query_args[1]=address;
 c->query_args[2]=(uintptr_t)&c->geometry;c->query_args[3]=sizeof(c->geometry);c->query_called=1;
 c->query_status=VirtualQueryEx(birth->process.hProcess,(const void *)address,&c->geometry,sizeof(c->geometry));c->query_returned=1;
 if(!c->query_status)c->query_error=GetLastError();
 if(!after(c,a,&c->query_after,&c->query_before)||c->query_status!=sizeof(c->geometry)||
  (uintptr_t)c->geometry.AllocationBase!=c->image_base||c->geometry.State!=MEM_COMMIT||c->geometry.Type!=MEM_IMAGE||
  (c->geometry.Protect!=PAGE_READWRITE&&c->geometry.Protect!=PAGE_WRITECOPY)||
  (uintptr_t)c->geometry.BaseAddress>UINTPTR_MAX-c->geometry.RegionSize||
  (uintptr_t)c->geometry.BaseAddress>address||c->geometry.RegionSize<128||
  address-(uintptr_t)c->geometry.BaseAddress>c->geometry.RegionSize-128)return 0;
 if(!read_original(c,a,&c->initial,(const void *)address,&c->query_after))return 0;
 for(i=0;i<128;i++)if(c->initial.body[i])return 0;
 if(!birth->route_encode_returned||!birth->route_encode_result||
  !before(c,a,&c->write_before,&c->initial.after))return 0;
 c->write_args[0]=(uintptr_t)birth->process.hProcess;c->write_args[1]=address;
 c->write_args[2]=(uintptr_t)birth->route_bytes;c->write_args[3]=sizeof(birth->route_bytes);
 c->write_args[4]=(uintptr_t)&c->written;c->possibly_written=1;c->write_called=1;
 c->write_status=WriteProcessMemory(birth->process.hProcess,(void *)address,birth->route_bytes,sizeof(birth->route_bytes),&c->written);c->write_returned=1;
 if(!c->write_status)c->write_error=GetLastError();
 if(!after(c,a,&c->write_after,&c->write_before))return 0;
 if(!read_original(c,a,&c->readback,(const void *)address,&c->write_after)||
  !equal(c->readback.body,birth->route_bytes,sizeof(birth->route_bytes)))return 0;
 c->readback_matches=1;return c->write_status&&c->written==sizeof(birth->route_bytes);
}
#endif
