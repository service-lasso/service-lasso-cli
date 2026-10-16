#include "capture_extent_windows.h"
#include "capture_view_windows.h"
#if defined(_WIN32)
static int separate(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int zero(const void *p,size_t n){size_t i;const unsigned char *b=p;for(i=0;i<n;i++)if(b[i])return 0;return 1;}
static int same(const struct slcli_writer_capture_extent_windows *c){
 const struct slcli_extent *e;
 if(!c->started||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||
  !c->readback||!slcli_arena_live_block(c->arena,c->readback,sizeof(*c->readback))||
  c->readback->arena!=c->arena||!slcli_writer_windows_readback_matches_original(c->readback)||!c->owner||
  !slcli_arena_live_block(c->arena,c->owner,sizeof(*c->owner))||
  !slcli_memory_owner_writable(c->owner)||c->owner->failed||
  c->request.index>=c->owner->count||c->request.index>=256||!c->request.sequence||
  !c->file_bytes||c->file_bytes>INT64_MAX||!c->bytes||c->bytes>=c->file_bytes||
  c->file_offset>c->file_bytes-c->bytes||c->bytes>SIZE_MAX||!c->owner->native_page_size||
  c->bytes%c->owner->native_page_size)return 0;
 if(c->system_returned&&(c->system_output!=(uintptr_t)&c->system||
  c->system.dwPageSize!=c->owner->native_page_size||!c->system.dwAllocationGranularity||
  c->system.dwAllocationGranularity%c->system.dwPageSize||
  c->file_offset%c->system.dwAllocationGranularity))return 0;
 e=&c->owner->extents[c->request.index];
 return e->reserved==c->file_bytes&&e->category==SLCLI_IO&&e->acquisition==c->request.sequence&&
  e->latest==c->request.sequence&&!e->arena_initialized&&!e->retirement&&!e->base&&
  !c->view_registered&&!c->live_registered&&
  e->state==(c->backing_registered?SLCLI_EXTENT_BACKING_ACQUIRED:SLCLI_EXTENT_RESERVED);
}
static int before(struct slcli_writer_capture_extent_windows *c,unsigned i){
 const struct slcli_source_deadline_original *d=c->readback->deadline;
 if(i>=7||c->called[i]||c->returned[i]||!same(c))return 0;
 c->before_result[i]=slcli_native_clock_read(&c->clocks[i][0]);c->before_returned[i]=1;
 if(!c->before_result[i]||c->clocks[i][0].tick<c->previous_tick||
  c->clocks[i][0].tick>=d->expires||
  !slcli_evidence_deadline_precede(c->readback->creator->evidence_file,&c->clocks[i][0]))return 0;
 c->called[i]=1;return 1;
}
static int after(struct slcli_writer_capture_extent_windows *c,unsigned i){
 c->after_result[i]=slcli_native_clock_read(&c->clocks[i][1]);c->after_returned[i]=1;
 if(!c->returned[i]||!c->after_result[i]||c->clocks[i][1].tick<c->clocks[i][0].tick||
  c->clocks[i][1].tick>=c->readback->deadline->expires||!same(c)||
  !slcli_evidence_deadline_precede(c->readback->creator->evidence_file,&c->clocks[i][1]))return 0;
 c->previous_tick=c->clocks[i][1].tick;return 1;
}
int slcli_writer_capture_extent_windows_prepare_original(
 struct slcli_writer_capture_extent_windows *c,struct slcli_arena *a,
 struct slcli_writer_windows_readback_original *r,struct slcli_memory_owner *owner,
 const struct slcli_extent_request *request,uint64_t file_bytes,uint64_t file_offset,uint64_t bytes){
 unsigned i;uint64_t at;
 if(!c||!a||!r||!owner||!slcli_memory_owner_mutable(owner)||!request||!slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !slcli_arena_live_block(a,r,sizeof(*r))||r->arena!=a||!slcli_writer_windows_readback_matches_original(r)||
  !slcli_arena_live_block(a,owner,sizeof(*owner))||!slcli_arena_live_range(a,request,sizeof(*request))||
  !separate(c,sizeof(*c),r,sizeof(*r))||!separate(c,sizeof(*c),owner,sizeof(*owner))||
  !separate(c,sizeof(*c),request,sizeof(*request))||
  !separate(c,sizeof(*c),r->creator,sizeof(*r->creator))||
  !separate(c,sizeof(*c),r->caller,r->caller_bytes)||
  !slcli_source_deadline_away(r->deadline,c,sizeof(*c))||!bytes||bytes>SIZE_MAX||
  !file_bytes||file_bytes>INT64_MAX||bytes>=file_bytes||file_offset>file_bytes-bytes||
  file_bytes>r->creator->evidence_plan->maximum||r->observed.standard.EndOfFile.QuadPart)return 0;
 c->started=1;c->failed=1;c->arena=a;c->readback=r;c->owner=owner;c->request=*request;
 c->file_bytes=file_bytes;c->file_offset=file_offset;c->bytes=bytes;
 c->previous_tick=r->observe_after.tick;if(!same(c))return 0;
 c->system_output=(uintptr_t)&c->system;
 c->system_before_result=slcli_native_clock_read(&c->system_before);c->system_before_returned=1;
 if(!c->system_before_result||c->system_before.tick<c->previous_tick||
  c->system_before.tick>=r->deadline->expires||
  !slcli_evidence_deadline_precede(r->creator->evidence_file,&c->system_before))return 0;
 c->system_called=1;GetSystemInfo(&c->system);c->system_returned=1;
 c->system_after_result=slcli_native_clock_read(&c->system_after);c->system_after_returned=1;
 if(!c->system_after_result||c->system_after.tick<c->system_before.tick||
  c->system_after.tick>=r->deadline->expires||!same(c)||
  !slcli_evidence_deadline_precede(r->creator->evidence_file,&c->system_after)||
  c->system.dwPageSize!=owner->native_page_size||!c->system.dwAllocationGranularity||
  c->system.dwAllocationGranularity%c->system.dwPageSize||
  file_offset%c->system.dwAllocationGranularity)return 0;
 c->previous_tick=c->system_after.tick;
 for(i=0;i<2;i++){
  unsigned call=i?4:0;
  c->section_args[i][0]=(uintptr_t)(i?r->original:r->creator->original);
  c->section_args[i][1]=0;c->section_args[i][2]=i?PAGE_READONLY:PAGE_READWRITE;
  c->section_args[i][3]=i?0:(DWORD)(file_bytes>>32);c->section_args[i][4]=i?0:(DWORD)file_bytes;
  c->section_args[i][5]=0;
  if(!before(c,call))return 0;
  c->sections[i]=CreateFileMappingW((HANDLE)(uintptr_t)c->section_args[i][0],0,
   (DWORD)c->section_args[i][2],(DWORD)c->section_args[i][3],(DWORD)c->section_args[i][4],0);
  c->returned[call]=1;if(!c->sections[i])c->section_error[i]=GetLastError();
  if(!i&&c->sections[i]){
   c->backing_registered=(uint32_t)slcli_memory_backing_acquired(owner,&c->request);
  }
  if(!after(c,call)||!c->sections[i]||(!i&&!c->backing_registered))return 0;
  c->map_args[i][0]=(uintptr_t)c->sections[i];c->map_args[i][1]=i?FILE_MAP_READ:FILE_MAP_WRITE;
  c->map_args[i][2]=(DWORD)(file_offset>>32);c->map_args[i][3]=(DWORD)file_offset;c->map_args[i][4]=bytes;
  if(!before(c,call+1))return 0;
  c->views[i]=MapViewOfFile(c->sections[i],(DWORD)c->map_args[i][1],(DWORD)c->map_args[i][2],(DWORD)c->map_args[i][3],(SIZE_T)bytes);
  c->returned[call+1]=1;if(!c->views[i])c->map_error[i]=GetLastError();
  if(!after(c,call+1)||!c->views[i])return 0;
  c->query_args[i][0]=(uintptr_t)c->views[i];c->query_args[i][1]=(uintptr_t)&c->geometry[i];
  c->query_args[i][2]=sizeof(c->geometry[i]);
  if(!before(c,call+2))return 0;
  c->query_result[i]=VirtualQuery(c->views[i],&c->geometry[i],sizeof(c->geometry[i]));
  c->returned[call+2]=1;if(!c->query_result[i])c->query_error[i]=GetLastError();
  if(!after(c,call+2)||c->query_result[i]!=sizeof(c->geometry[i])||
   c->geometry[i].BaseAddress!=c->views[i]||c->geometry[i].AllocationBase!=c->views[i]||
   c->geometry[i].RegionSize!=bytes||c->geometry[i].State!=MEM_COMMIT||c->geometry[i].Type!=MEM_MAPPED||
   c->geometry[i].Protect!=(i?PAGE_READONLY:PAGE_READWRITE)||
   (uintptr_t)c->views[i]>UINTPTR_MAX-(uintptr_t)bytes||
   (i&&!separate(c->views[0],(size_t)bytes,c->views[1],(size_t)bytes)))return 0;
  for(at=0;at<bytes;at++){if(((volatile const unsigned char *)c->views[i])[at])return 0;c->zero_scanned[i]=at+1;}
  c->scan_after_result[i]=slcli_native_clock_read(&c->scan_after[i]);c->scan_after_returned[i]=1;
  if(!c->scan_after_result[i]||c->scan_after[i].tick<c->previous_tick||
   c->scan_after[i].tick>=r->deadline->expires||!same(c)||
   !slcli_evidence_deadline_precede(r->creator->evidence_file,&c->scan_after[i]))return 0;
  c->previous_tick=c->scan_after[i].tick;
  if(!i){
   if(!before(c,3))return 0;
   c->file_after_result=slcli_windows_source_observe(r->original,&c->file_after);c->returned[3]=1;
   if(!after(c,3)||!c->file_after_result||c->file_after.tick<c->clocks[3][0].tick||
    c->file_after.tick>c->clocks[3][1].tick||c->file_after.standard.EndOfFile.QuadPart<0||
    (uint64_t)c->file_after.standard.EndOfFile.QuadPart!=file_bytes||
    c->file_after.identity.VolumeSerialNumber!=r->observed.identity.VolumeSerialNumber||
    !slcli_equal(c->file_after.identity.FileId.Identifier,r->observed.identity.FileId.Identifier,16))return 0;
  }
 }
 c->complete=1;c->failed=0;return 1;
}
int slcli_writer_capture_extent_windows_matches_original(
 const struct slcli_writer_capture_extent_windows *c){
 unsigned i,j;uint64_t previous;
 if(c&&c->backing_original)return slcli_writer_capture_view_windows_matches_original(c);
 if(!c||!same(c)||!c->complete||c->failed||!c->backing_registered||
  c->view_registered||c->live_registered||c->sections[0]==c->sections[1]||
  !separate(c->views[0],(size_t)c->bytes,c->views[1],(size_t)c->bytes))return 0;
 if(!c->system_called||!c->system_returned||!c->system_before_returned||!c->system_after_returned||
  !c->system_before_result||!c->system_after_result||!c->system_before.complete||!c->system_after.complete||
  c->system_output!=(uintptr_t)&c->system||c->system_before.tick<c->readback->observe_after.tick||
  c->system_after.tick<c->system_before.tick||c->system_after.tick>=c->readback->deadline->expires||
  c->system.dwPageSize!=c->owner->native_page_size||!c->system.dwAllocationGranularity||
  c->system.dwAllocationGranularity%c->system.dwPageSize||c->file_offset%c->system.dwAllocationGranularity)return 0;
 previous=c->system_after.tick;
 for(j=0;j<7;j++){
  if(!c->called[j]||!c->returned[j]||!c->before_returned[j]||!c->after_returned[j]||
   !c->before_result[j]||!c->after_result[j]||!c->clocks[j][0].complete||
   !c->clocks[j][1].complete||c->clocks[j][0].tick<previous||
   c->clocks[j][1].tick<c->clocks[j][0].tick||
   c->clocks[j][1].tick>=c->readback->deadline->expires)return 0;
  previous=c->clocks[j][1].tick;
  if(j==2||j==6){i=j==2?0:1;
   if(!c->scan_after_returned[i]||!c->scan_after_result[i]||!c->scan_after[i].complete||
    c->scan_after[i].tick<previous||c->scan_after[i].tick>=c->readback->deadline->expires||
    c->zero_scanned[i]!=c->bytes)return 0;
   previous=c->scan_after[i].tick;
  }
 }
 for(i=0;i<2;i++){
  if(!c->sections[i]||!c->views[i]||c->section_args[i][0]!=(uintptr_t)(i?c->readback->original:c->readback->creator->original)||
   c->section_args[i][1]||c->section_args[i][2]!=(i?PAGE_READONLY:PAGE_READWRITE)||
   c->section_args[i][3]!=(i?0:(DWORD)(c->file_bytes>>32))||c->section_args[i][4]!=(i?0:(DWORD)c->file_bytes)||
   c->section_args[i][5]||c->map_args[i][0]!=(uintptr_t)c->sections[i]||
   c->map_args[i][1]!=(i?FILE_MAP_READ:FILE_MAP_WRITE)||c->map_args[i][2]!=(DWORD)(c->file_offset>>32)||c->map_args[i][3]!=(DWORD)c->file_offset||
   c->map_args[i][4]!=c->bytes||c->query_args[i][0]!=(uintptr_t)c->views[i]||
   c->query_args[i][1]!=(uintptr_t)&c->geometry[i]||c->query_args[i][2]!=sizeof(c->geometry[i])||
   c->query_result[i]!=sizeof(c->geometry[i])||c->geometry[i].BaseAddress!=c->views[i]||
   c->geometry[i].AllocationBase!=c->views[i]||c->geometry[i].RegionSize!=c->bytes||
   c->geometry[i].State!=MEM_COMMIT||c->geometry[i].Type!=MEM_MAPPED||
   c->geometry[i].Protect!=(i?PAGE_READONLY:PAGE_READWRITE))return 0;
 }
 return c->previous_tick==previous&&c->file_after_result&&c->file_after.completed&&
  c->file_after.original==c->readback->original&&c->file_after.tick>=c->clocks[3][0].tick&&
  c->file_after.tick<=c->clocks[3][1].tick&&c->file_after.standard.EndOfFile.QuadPart>=0&&
  (uint64_t)c->file_after.standard.EndOfFile.QuadPart==c->file_bytes&&
  c->file_after.identity.VolumeSerialNumber==c->readback->observed.identity.VolumeSerialNumber&&
  slcli_equal(c->file_after.identity.FileId.Identifier,c->readback->observed.identity.FileId.Identifier,16);
}
#endif
