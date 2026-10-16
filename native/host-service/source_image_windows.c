#include "source_image_windows.h"
#if defined(_WIN32)
static int separate(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int same(struct slcli_source_image_windows *c,struct slcli_arena *a) {
 size_t i,call;int born;uintptr_t handle;const struct slcli_native_reference *r;
 if(!slcli_source_deadline_original_matches(a,c->deadline,c->caller)||
  !c->book||!slcli_arena_live_range(a,c->book,sizeof(*c->book))||
  c->book->deadline_original!=c->deadline||!c->book->references||!c->book->count||
  c->book->count>SIZE_MAX/sizeof(*r)||
  !slcli_arena_live_range(a,c->book->references,c->book->count*sizeof(*r))||
  !c->owner||!slcli_arena_live_block(a,c->owner,sizeof(*c->owner))||
  !slcli_memory_owner_writable(c->owner)||
  !slcli_arena_live_block(a,c->path,c->path_bytes)||
  !slcli_arena_live_range(a,c->path_original,c->path_bytes))return 0;
 for(i=0;i<c->path_bytes;i++)if(((const unsigned char *)c->path)[i]!=((const unsigned char *)c->path_original)[i])return 0;
 for(i=0;i<3;i++) {
  if(c->ordinals[i]>=c->book->count)return 0;r=&c->book->references[c->ordinals[i]];
  call=i==0?0:i==1?3:4;
  born=i==0?(c->file&&c->file!=INVALID_HANDLE_VALUE):i==1?c->section!=NULL:c->view!=NULL;
  handle=i==0?(born?(uintptr_t)c->file:0):i==1?(uintptr_t)c->section:0;
  if(r->native_original!=c||r->version!=c->versions[i]||!r->version||
   r->native_class!=(i==0?SLCLI_NATIVE_ORIGINAL_SOURCE_IMAGE_FILE:
    i==1?SLCLI_NATIVE_ORIGINAL_SOURCE_IMAGE_SECTION:SLCLI_NATIVE_ORIGINAL_SOURCE_IMAGE_VIEW)||
   r->kind!=(i==0?4:i==1?22:9)||r->handle!=handle||
   r->effect_flags!=(c->called[call]?(c->returned[call]?(born?15:7):3):1)||
   r->state!=(born?0:2)||r->physical_extent!=(i==0?UINT16_MAX:c->physical_index_original)||
   r->alias_of!=(i==2?c->ordinals[1]:UINT16_MAX))return 0;
 }
 if(c->reserved) {
  const struct slcli_extent *e;
  if(c->request.index!=c->physical_index_original||c->request.index>=c->owner->count)return 0;
  e=&c->owner->extents[c->request.index];
  if(e->acquisition!=c->request.sequence||e->reserved!=c->rounded_bytes||
   e->state!=(c->acquired_recorded?SLCLI_EXTENT_LIVE:c->view_recorded?SLCLI_EXTENT_PARTIAL_ACQUIRED:
     c->backing_recorded?SLCLI_EXTENT_BACKING_ACQUIRED:SLCLI_EXTENT_RESERVED)||
   (c->view_recorded&&e->base!=(uintptr_t)c->view))return 0;
 }
 return 1;
}
static int before(struct slcli_source_image_windows *c,struct slcli_arena *a,size_t i,
 const struct slcli_native_clock *prior) {
 return same(c,a)&&prior&&prior->complete&&slcli_native_clock_read(&c->before[i])&&
  c->before[i].tick>=prior->tick&&c->before[i].tick<c->deadline->expires&&same(c,a);
}
static int after(struct slcli_source_image_windows *c,struct slcli_arena *a,size_t i) {
 return slcli_native_clock_read(&c->after[i])&&c->after[i].tick>=c->before[i].tick&&
  c->after[i].tick<c->deadline->expires&&same(c,a);
}
static void row_called(struct slcli_source_image_windows *c,size_t i) {
 c->book->references[c->ordinals[i]].effect_flags=3;
}
static void row_returned(struct slcli_source_image_windows *c,size_t i,uintptr_t handle,int born) {
 struct slcli_native_reference *r=&c->book->references[c->ordinals[i]];
 r->effect_flags=born?15:7;r->state=born?0:2;r->handle=handle;
}
int slcli_source_image_windows_acquire_original(struct slcli_source_image_windows *c,
 struct slcli_arena *a,const struct slcli_binding_originals *b,struct slcli_memory_owner *o,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const WCHAR *path,size_t path_bytes,
 uint32_t file_ordinal,uint32_t section_ordinal,uint32_t view_ordinal) {
 size_t i,j;uint32_t ordinals[3]={file_ordinal,section_ordinal,view_ordinal};
 struct slcli_native_reference *r;BOOL ok;SIZE_T queried;uint64_t bytes,page,remainder;
 if(!a||!c||!slcli_arena_live_block(a,c,sizeof(*c))||!b||
  !slcli_arena_live_range(a,b,sizeof(*b))||b->deadline_original!=d||!b->references||
  !b->count||b->count>SIZE_MAX/sizeof(*r)||
  !slcli_arena_live_range(a,b->references,b->count*sizeof(*r))||
  !o||o==a->owner||!slcli_arena_live_block(a,o,sizeof(*o))||o->count>=256||
  !slcli_memory_owner_mutable(o)||!slcli_source_deadline_original_matches(a,d,caller)||
  !caller_bytes||!slcli_arena_live_block(a,caller,caller_bytes)||!prior||
  !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  !path||path_bytes<2*sizeof(WCHAR)||path_bytes%sizeof(WCHAR)||
  !slcli_arena_live_range(a,path,path_bytes)||
  !separate(c,sizeof(*c),path,path_bytes)||!separate(c,sizeof(*c),o,sizeof(*o))||
  !separate(c,sizeof(*c),b,sizeof(*b))||!separate(c,sizeof(*c),b->references,b->count*sizeof(*r))||
  !separate(c,sizeof(*c),caller,caller_bytes)||!separate(c,sizeof(*c),prior,sizeof(*prior))||
  !slcli_source_deadline_away(d,c,sizeof(*c))||file_ordinal>=section_ordinal||section_ordinal>=view_ordinal||
  section_ordinal>=UINT16_MAX)return 0;
 for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
 for(i=0;i<path_bytes/sizeof(WCHAR);i++)if((i+1==path_bytes/sizeof(WCHAR))?path[i]!=0:path[i]==0)return 0;
 for(i=0;i<3;i++) {
  if(ordinals[i]>=b->count)return 0;r=&b->references[ordinals[i]];
  if(r->kind!=(i==0?4:i==1?22:9)||!r->version||r->effect_flags!=1||r->state!=2||
   r->handle||r->native_original||r->service_request_original||
   r->native_class!=SLCLI_NATIVE_ORIGINAL_UNCONSTRUCTED||
   r->physical_extent!=(i==0?UINT16_MAX:o->count)||
   r->alias_of!=(i==2?section_ordinal:UINT16_MAX))return 0;
 }
 c->started=1;c->book=b;c->owner=o;c->physical_index_original=o->count;
 c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->path_original=path;c->path_bytes=path_bytes;
 c->path=slcli_arena_alloc(a,path_bytes);if(!c->path)return 0;
 for(i=0;i<path_bytes;i++)((unsigned char *)c->path)[i]=((const unsigned char *)path)[i];
 for(i=0;i<3;i++) {
  r=&b->references[ordinals[i]];c->ordinals[i]=ordinals[i];c->versions[i]=r->version;
  r->native_original=c;r->native_class=i==0?SLCLI_NATIVE_ORIGINAL_SOURCE_IMAGE_FILE:
   i==1?SLCLI_NATIVE_ORIGINAL_SOURCE_IMAGE_SECTION:SLCLI_NATIVE_ORIGINAL_SOURCE_IMAGE_VIEW;
 }
 if(!before(c,a,0,prior))return 0;
 c->args[0][0]=(uintptr_t)c->path;c->args[0][1]=GENERIC_READ;c->args[0][2]=FILE_SHARE_READ;
 c->args[0][4]=OPEN_EXISTING;c->args[0][5]=FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN;
 row_called(c,0);c->called[0]=1;
 c->file=CreateFileW(c->path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
  FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
 c->raw[0]=(uintptr_t)c->file;c->returned[0]=1;
 row_returned(c,0,c->file&&c->file!=INVALID_HANDLE_VALUE?(uintptr_t)c->file:0,c->file&&c->file!=INVALID_HANDLE_VALUE);
 if(!c->file||c->file==INVALID_HANDLE_VALUE){c->errors[0]=GetLastError();c->error_observed[0]=1;}
 if(!after(c,a,0)||!c->file||c->file==INVALID_HANDLE_VALUE)return 0;
 if(!before(c,a,1,&c->after[0]))return 0;
 c->args[1][0]=(uintptr_t)c->file;c->args[1][1]=(uintptr_t)&c->file_bytes;c->called[1]=1;
 ok=GetFileSizeEx(c->file,&c->file_bytes);c->raw[1]=(uint32_t)ok;c->returned[1]=1;
 if(!ok){c->errors[1]=GetLastError();c->error_observed[1]=1;}
 if(!after(c,a,1)||!ok||c->file_bytes.QuadPart<=0)return 0;
 if(!before(c,a,2,&c->after[1]))return 0;
 c->args[2][0]=(uintptr_t)&c->system;c->called[2]=1;GetSystemInfo(&c->system);c->returned[2]=1;
 if(!after(c,a,2)||!c->system.dwPageSize||!c->system.dwAllocationGranularity||
  c->system.dwAllocationGranularity%c->system.dwPageSize||
  o->native_page_size!=c->system.dwPageSize)return 0;
 bytes=(uint64_t)c->file_bytes.QuadPart;page=c->system.dwPageSize;remainder=bytes%page;
 if(remainder){if(bytes>UINT64_MAX-(page-remainder))return 0;bytes+=page-remainder;}
 if(bytes>SIZE_MAX||!slcli_memory_prepare(o,SLCLI_RUNTIME,bytes,&c->request))return 0;
 c->reserved=1;c->rounded_bytes=bytes;
 if(c->request.index!=b->references[section_ordinal].physical_extent)return 0;
 if(!before(c,a,3,&c->after[2]))return 0;
 c->args[3][0]=(uintptr_t)c->file;c->args[3][2]=PAGE_READONLY;row_called(c,1);c->called[3]=1;
 c->section=CreateFileMappingW(c->file,NULL,PAGE_READONLY,0,0,NULL);
 c->raw[3]=(uintptr_t)c->section;c->returned[3]=1;row_returned(c,1,(uintptr_t)c->section,c->section!=NULL);
 if(!c->section){c->errors[3]=GetLastError();c->error_observed[3]=1;}
 if(c->section)c->backing_recorded=slcli_memory_backing_acquired(o,&c->request);
 if(!after(c,a,3)||!c->section||!c->backing_recorded)return 0;
 if(!before(c,a,4,&c->after[3]))return 0;
 c->args[4][0]=(uintptr_t)c->section;c->args[4][1]=FILE_MAP_READ;
 c->args[4][4]=(uint64_t)c->file_bytes.QuadPart;row_called(c,2);c->called[4]=1;
 c->view=MapViewOfFile(c->section,FILE_MAP_READ,0,0,(SIZE_T)c->file_bytes.QuadPart);
 c->raw[4]=(uintptr_t)c->view;c->returned[4]=1;row_returned(c,2,0,c->view!=NULL);
 if(!c->view){c->errors[4]=GetLastError();c->error_observed[4]=1;}
 if(c->view)c->view_recorded=slcli_memory_backing_mapped(o,&c->request,(uintptr_t)c->view);
 if(!after(c,a,4)||!c->view||!c->view_recorded)return 0;
 if(!before(c,a,5,&c->after[4]))return 0;
 c->args[5][0]=(uintptr_t)c->view;c->args[5][1]=(uintptr_t)&c->geometry;
 c->args[5][2]=sizeof(c->geometry);c->called[5]=1;
 queried=VirtualQuery(c->view,&c->geometry,sizeof(c->geometry));c->raw[5]=queried;c->returned[5]=1;
 if(!queried){c->errors[5]=GetLastError();c->error_observed[5]=1;}
 if(!after(c,a,5)||queried!=sizeof(c->geometry)||c->geometry.BaseAddress!=c->view||
  c->geometry.AllocationBase!=c->view||c->geometry.RegionSize!=bytes||
  c->geometry.State!=MEM_COMMIT||c->geometry.Type!=MEM_MAPPED||c->geometry.Protect!=PAGE_READONLY)return 0;
 c->acquired_result=slcli_memory_acquired(o,&c->request,(uintptr_t)c->view,bytes,0,0);
 c->acquired_returned=1;
 if(!c->acquired_result)return 0;
 c->acquired_recorded=1;
 /* Native source namespace/byte identity and independent last-borrower
  * unmap/close remain unqualified; mapped is raw custody, never authority. */
 for(j=0;j<2;j++) {
  i=6+j;if(!before(c,a,i,&c->after[i-1]))return 0;
  c->args[i][0]=(uintptr_t)c->file;c->args[i][1]=(uintptr_t)&c->information[j];c->called[i]=1;
  ok=GetFileInformationByHandle(c->file,&c->information[j]);c->raw[i]=(uint32_t)ok;c->returned[i]=1;
  if(!ok){c->errors[i]=GetLastError();c->error_observed[i]=1;}
  if(!after(c,a,i)||!ok)return 0;
 }
 for(i=0;i<sizeof(c->information[0]);i++)if(((unsigned char *)&c->information[0])[i]!=((unsigned char *)&c->information[1])[i])return 0;
 c->mapped=1;return 1;
}
#endif
