#include "capture_plan_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_writer_capture_plan_windows_geometry_original(struct slcli_arena *a,
 const struct slcli_writer_capture_view_plan_windows *p,size_t count,uint64_t file_bytes,
 const SYSTEM_INFO *system,uint64_t *mapped_bytes){
 size_t i,j,plan_bytes;uint64_t total=0;
 if(!a||!p||!count||count>SIZE_MAX/sizeof(*p)||!system||!mapped_bytes||
  !file_bytes||file_bytes>INT64_MAX)return 0;
 plan_bytes=count*sizeof(*p);
 if(!slcli_arena_live_block(a,p,plan_bytes)||
  !slcli_arena_live_range(a,system,sizeof(*system))||
  !slcli_arena_live_range(a,mapped_bytes,sizeof(*mapped_bytes))||
  !apart(p,plan_bytes,mapped_bytes,sizeof(*mapped_bytes))||
  !apart(system,sizeof(*system),mapped_bytes,sizeof(*mapped_bytes))||
  !system->dwPageSize||!system->dwAllocationGranularity||
  system->dwAllocationGranularity%system->dwPageSize)return 0;
 for(i=0;i<count;i++){
  if(!p[i].bytes||p[i].bytes>=file_bytes||p[i].bytes>SIZE_MAX||
   p[i].file_offset>file_bytes-p[i].bytes||
   p[i].file_offset%system->dwAllocationGranularity||p[i].bytes%system->dwPageSize||
   !p[i].capture||!slcli_arena_live_block(a,p[i].capture,sizeof(*p[i].capture))||
   !apart(p,plan_bytes,p[i].capture,sizeof(*p[i].capture))||
   !apart(system,sizeof(*system),p[i].capture,sizeof(*p[i].capture))||
   !apart(mapped_bytes,sizeof(*mapped_bytes),p[i].capture,sizeof(*p[i].capture))||
   total>file_bytes-p[i].bytes)return 0;
  for(j=0;j<i;j++){
   if(!apart(p[i].capture,sizeof(*p[i].capture),p[j].capture,sizeof(*p[j].capture))||
    (p[i].file_offset<p[j].file_offset+p[j].bytes&&
     p[j].file_offset<p[i].file_offset+p[i].bytes))return 0;
  }
  total+=p[i].bytes;
 }
 if(total>=file_bytes)return 0;
 *mapped_bytes=total;return 1;
}
#endif
