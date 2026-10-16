#include "product_route_parent_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);
}
static int equal(const void *a,const void *b,size_t n) {
 const unsigned char *x=a,*y=b;size_t i;for(i=0;i<n;i++)if(x[i]!=y[i])return 0;return 1;
}
static int same(const struct slcli_product_windows_route_parent *c) {
 struct slcli_product_storage_windows *s=c->storage_original;
 return s&&!s->failed&&s->route_parent_original==c&&c->source_original&&
 slcli_arena_live_block(&s->arena_original,c,sizeof(*c))&&
 slcli_arena_live_block(&s->arena_original,c->source_original,sizeof(*c->source_original))&&
 equal(c->source_original,&c->source_snapshot,sizeof(c->source_snapshot))&&
 c->birth_original==s->birth_original&&c->birth_original==c->source_snapshot.birth_original&&
 slcli_product_windows_birth_matches_original(c->birth_original)&&c->expires==c->birth_original->expires&&
 c->scan_capacity==c->source_snapshot.scan_capacity&&c->image_original.file_original==c->birth_original->source_snapshot.image_original&&
 c->scan_capacity&&c->header_stride&&c->scan_capacity<=SIZE_MAX/sizeof(*c->scans_original)&&c->scan_capacity<=SIZE_MAX/c->header_stride&&
 (!c->scans_original||slcli_arena_live_block(&s->arena_original,c->scans_original,c->scan_capacity*sizeof(*c->scans_original)))&&
 (!c->headers_original||slcli_arena_live_block(&s->arena_original,c->headers_original,c->scan_capacity*c->header_stride));
}
static int cut(struct slcli_product_windows_route_parent *c,struct slcli_native_clock *out) {
 if(!slcli_native_clock_read(out)||!same(c)||out->tick<c->prior_tick||out->tick>=c->expires)return 0;
 c->prior_tick=out->tick;return 1;
}
static int before(struct slcli_product_windows_route_parent *c,struct slcli_build_windows_call *r) {
 if(r->called||r->returned||!cut(c,&r->before))return 0;r->called=1;return 1;
}
static int after(struct slcli_product_windows_route_parent *c,struct slcli_build_windows_call *r) {
 return r->returned&&cut(c,&r->after);
}
static int fail(struct slcli_product_windows_route_parent *c) {c->failed=1;c->storage_original->failed=1;return 0;}
static int read_original(struct slcli_product_windows_route_parent *c,struct slcli_build_windows_call *r,
 uintptr_t address,void *out,size_t bytes,SIZE_T *received) {
 if(!address||address>UINTPTR_MAX-bytes||!out||!bytes||!received)return 0;
 r->args[0]=(uintptr_t)c->birth_original->process_original.hProcess;r->args[1]=address;
 r->args[2]=(uintptr_t)out;r->args[3]=bytes;r->args[4]=(uintptr_t)received;
 if(!before(c,r))return 0;
 r->raw=ReadProcessMemory(c->birth_original->process_original.hProcess,(void *)address,out,bytes,received);r->returned=1;
 if(!r->raw){r->error=GetLastError();r->error_observed=1;}
 return after(c,r)&&r->raw&&*received==bytes;
}
int slcli_product_windows_route_parent_output_away_original(
 const struct slcli_product_windows_route_parent *c,const void *out,size_t n) {
 if(!c||!c->source_original||!c->scan_capacity||!c->header_stride||
 c->scan_capacity>SIZE_MAX/sizeof(*c->scans_original)||c->scan_capacity>SIZE_MAX/c->header_stride||
 !equal(c->source_original,&c->source_snapshot,sizeof(c->source_snapshot))||!apart(out,n,c,sizeof(*c))||
 !apart(out,n,c->source_original,sizeof(*c->source_original))||
 !apart(out,n,c->source_snapshot.cost_original,sizeof(*c->source_snapshot.cost_original))||
 !apart(out,n,c->source_snapshot.failed_overlap_original,sizeof(*c->source_snapshot.failed_overlap_original)))return 0;
 return (!c->scans_original||apart(out,n,c->scans_original,c->scan_capacity*sizeof(*c->scans_original)))&&
 (!c->headers_original||apart(out,n,c->headers_original,c->scan_capacity*c->header_stride));
}
int slcli_product_windows_route_parent_original(struct slcli_product_storage_windows *s,
 const struct slcli_product_windows_route_source *src) {
 struct slcli_product_windows_route_parent *c;struct slcli_product_windows_route_image image;
 struct slcli_product_windows_route_scan *row;struct slcli_build_windows_call *r;
 const unsigned char *expected;size_t i,j,k,stride,scan_bytes,header_bytes,bytes;uint64_t offset;
 uintptr_t base,end,address;HANDLE original;unsigned found=0;
 if(!s||s->failed||s->route_parent_original||s->route_parent_entry_original.started||!src||
 !slcli_arena_live_block(&s->arena_original,src,sizeof(*src))||src->birth_original!=s->birth_original||
 !slcli_product_windows_birth_matches_original(src->birth_original)||!src->scan_capacity||
 !slcli_arena_live_range(&s->arena_original,src->cost_original,sizeof(*src->cost_original))||
 !slcli_arena_live_range(&s->arena_original,src->failed_overlap_original,sizeof(*src->failed_overlap_original)))return 0;
 /* Semantic source membership only; constructor/admission must prove the actual
  * scan source/capacity/physical finance before entry, not this pointer check. */
 for(i=0;i<s->source_original->source_graph_count;i++)if(src->scan_source_original==&s->source_original->source_graph_original[i])found++;
 if(found!=1||!slcli_product_windows_route_image_original(&image,src->birth_original->source_snapshot.image_original))return 0;
 stride=64+24+(size_t)image.optional_bytes+(size_t)image.section_count*40;
 if(src->scan_capacity>SIZE_MAX/sizeof(*row)||src->scan_capacity>SIZE_MAX/stride)return 0;
 scan_bytes=src->scan_capacity*sizeof(*row);header_bytes=src->scan_capacity*stride;
 if(!slcli_native_clock_read(&s->route_parent_entry_original)||
 s->route_parent_entry_original.tick<s->prior_tick||s->route_parent_entry_original.tick>=src->birth_original->expires){s->failed=1;return 0;}
 c=slcli_arena_alloc(&s->arena_original,sizeof(*c));if(!c){s->failed=1;return 0;}
 if(!slcli_product_storage_windows_output_away_original(s,c,sizeof(*c))){s->failed=1;return 0;}
 for(i=0;i<sizeof(*c);i++)((unsigned char *)c)[i]=0;
 s->route_parent_original=c;c->storage_original=s;c->source_original=src;
 for(i=0;i<sizeof(*src);i++)((unsigned char *)&c->source_snapshot)[i]=((const unsigned char *)src)[i];
 c->birth_original=src->birth_original;c->image_original=image;c->scan_capacity=src->scan_capacity;c->header_stride=stride;
 c->expires=c->birth_original->expires;c->prior_tick=s->route_parent_entry_original.tick;c->started=1;
 if(!cut(c,&c->entry))return fail(c);
 c->scans_original=slcli_arena_alloc(&s->arena_original,scan_bytes);
 c->headers_original=slcli_arena_alloc(&s->arena_original,header_bytes);
 if(!c->scans_original||!c->headers_original)return fail(c);
 for(i=0;i<scan_bytes;i++)((unsigned char *)c->scans_original)[i]=0;
 for(i=0;i<header_bytes;i++)c->headers_original[i]=0;
 c->minimum=(uintptr_t)s->pages_original.lpMinimumApplicationAddress;c->maximum=(uintptr_t)s->pages_original.lpMaximumApplicationAddress;
 if(!s->pages_original.dwPageSize||!c->minimum||c->maximum==UINTPTR_MAX||c->minimum>c->maximum||
 c->minimum%s->pages_original.dwPageSize||(c->maximum+1)%s->pages_original.dwPageSize)return fail(c);
 c->cursor=c->minimum;
 while(c->cursor<=c->maximum) {
 if(c->used>=c->scan_capacity)return fail(c);row=&c->scans_original[c->used];
 row->headers_original=c->headers_original+c->used*c->header_stride;c->used++;
 r=&row->query;r->args[0]=(uintptr_t)c->birth_original->process_original.hProcess;r->args[1]=c->cursor;
 r->args[2]=(uintptr_t)&row->geometry;r->args[3]=sizeof(row->geometry);
 if(!before(c,r))return fail(c);
 r->raw=VirtualQueryEx(c->birth_original->process_original.hProcess,(void *)c->cursor,&row->geometry,sizeof(row->geometry));r->returned=1;
 if(!r->raw){r->error=GetLastError();r->error_observed=1;}
 if(!after(c,r)||r->raw!=sizeof(row->geometry))return fail(c);
 base=(uintptr_t)row->geometry.BaseAddress;
 if(base!=c->cursor||!row->geometry.RegionSize||base>UINTPTR_MAX-row->geometry.RegionSize||
 base%s->pages_original.dwPageSize||row->geometry.RegionSize%s->pages_original.dwPageSize)return fail(c);
 end=base+row->geometry.RegionSize;if(end<=base||end>c->maximum+1)return fail(c);
 if(row->geometry.State==MEM_COMMIT&&row->geometry.Type==MEM_IMAGE&&row->geometry.AllocationBase==row->geometry.BaseAddress) {
 k=0;
 for(j=0;j<3;j++) {
 offset=j==0?0:j==1?image.pe_offset:(uint64_t)image.pe_offset+24+image.optional_bytes;
 bytes=j==0?64:j==1?24+(size_t)image.optional_bytes:(size_t)image.section_count*40;
 if(offset>image.image_bytes||bytes>image.image_bytes-offset||base>UINTPTR_MAX-offset||
 !read_original(c,&row->reads[j],base+(uintptr_t)offset,row->headers_original+k,bytes,&row->received[j]))return fail(c);
 expected=image.file_original->body_original+(size_t)offset;
 if(!equal(row->headers_original+k,expected,bytes))break;k+=bytes;
 }
 if(j==3){c->matches++;c->image_base=base;if(c->matches>1)return fail(c);}
 }
 c->cursor=end;
 }
 c->scan_completed=1;
 if(c->matches!=1||c->image_base>UINTPTR_MAX-image.route_rva)return fail(c);
 c->route_address=c->image_base+image.route_rva;r=&c->route_query;
 r->args[0]=(uintptr_t)c->birth_original->process_original.hProcess;r->args[1]=c->route_address;
 r->args[2]=(uintptr_t)&c->route_geometry;r->args[3]=sizeof(c->route_geometry);
 if(!before(c,r))return fail(c);
 r->raw=VirtualQueryEx(c->birth_original->process_original.hProcess,(void *)c->route_address,&c->route_geometry,sizeof(c->route_geometry));r->returned=1;
 if(!r->raw){r->error=GetLastError();r->error_observed=1;}
 if(!after(c,r)||r->raw!=sizeof(c->route_geometry)||c->route_geometry.State!=MEM_COMMIT||c->route_geometry.Type!=MEM_IMAGE||
 (uintptr_t)c->route_geometry.AllocationBase!=c->image_base||
 (c->route_geometry.Protect!=PAGE_READWRITE&&c->route_geometry.Protect!=PAGE_WRITECOPY)||
 c->route_geometry.RegionSize<32||(uintptr_t)c->route_geometry.BaseAddress>c->route_address||
 c->route_address-(uintptr_t)c->route_geometry.BaseAddress>c->route_geometry.RegionSize-32)return fail(c);
 if(!read_original(c,&c->initial_read,c->route_address,c->initial,32,&c->received[0]))return fail(c);
 for(i=0;i<32;i++)if(c->initial[i])return fail(c);
 /* Each alias is an actual child-table acquisition, captured before its clock
  * check. They remain held for genuine child/nativecase and family borrowers. */
 for(i=0;i<2;i++) {
 original=c->birth_original->inherited_original[i+3];r=&c->duplicates[i];
 r->args[0]=(uintptr_t)c->birth_original->process_original.hProcess;r->args[1]=(uintptr_t)original;
 r->args[2]=(uintptr_t)s->transport_original->current_process;r->args[3]=(uintptr_t)&c->aliases_original[i];
 r->args[4]=i?PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE:FILE_MAP_READ;
 if(!before(c,r))return fail(c);
 r->raw=DuplicateHandle(c->birth_original->process_original.hProcess,original,s->transport_original->current_process,&c->aliases_original[i],(DWORD)r->args[4],FALSE,0);r->returned=1;
 if(!r->raw){r->error=GetLastError();r->error_observed=1;}
 if(!after(c,r)||!r->raw||!c->aliases_original[i]||c->aliases_original[i]==INVALID_HANDLE_VALUE||
 c->aliases_original[i]==original||(i&&c->aliases_original[i]==c->aliases_original[0]))return fail(c);
 r=&c->flags[i];r->args[0]=(uintptr_t)c->aliases_original[i];r->args[1]=(uintptr_t)&c->alias_flags[i];
 if(!before(c,r))return fail(c);r->raw=GetHandleInformation(c->aliases_original[i],&c->alias_flags[i]);r->returned=1;
 if(!r->raw){r->error=GetLastError();r->error_observed=1;}
 if(!after(c,r)||!r->raw||c->alias_flags[i])return fail(c);
 r=&c->comparisons[i];r->args[0]=(uintptr_t)original;r->args[1]=(uintptr_t)c->aliases_original[i];
 if(!before(c,r))return fail(c);r->raw=CompareObjectHandles(original,c->aliases_original[i]);r->returned=1;
 if(!r->raw){r->error=GetLastError();r->error_observed=1;}
 if(!after(c,r)||!r->raw)return fail(c);
 }
 c->route_original.readonly_section=c->birth_original->inherited_original[3];c->route_original.parent_process=c->birth_original->inherited_original[4];
 if(!slcli_product_windows_route_encode(c->encoded,32,&c->route_original))return fail(c);
 address=c->route_address;r=&c->write;r->args[0]=(uintptr_t)c->birth_original->process_original.hProcess;r->args[1]=address;
 r->args[2]=(uintptr_t)c->encoded;r->args[3]=32;r->args[4]=(uintptr_t)&c->written;
 if(!before(c,r))return fail(c);c->possibly_written=1;
 r->raw=WriteProcessMemory(c->birth_original->process_original.hProcess,(void *)address,c->encoded,32,&c->written);r->returned=1;
 if(!r->raw){r->error=GetLastError();r->error_observed=1;}
 if(!after(c,r)||!r->raw||c->written!=32)return fail(c);
 if(!read_original(c,&c->readback,address,c->observed,32,&c->received[1])||!equal(c->encoded,c->observed,32)||!cut(c,&c->finish))return fail(c);
 s->prior_tick=c->finish.tick;c->route_captured=1;return 1;
}
#endif
