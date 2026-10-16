#include "preentry_backing_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int same(struct slcli_preentry_backing_windows *c,struct slcli_arena *a) {
  return slcli_source_deadline_original_matches(a,c->deadline,c->caller)&&
    c->deadline->origin==c->origin&&c->origin->tick==c->origin_tick&&
    c->deadline->expires==c->expires&&c->predecessor->complete&&
    c->predecessor->tick==c->predecessor_tick;
}
static int before(struct slcli_preentry_backing_windows *c,struct slcli_arena *a,size_t n) {
  c->before_result[n]=slcli_native_clock_read(&c->before[n])&&same(c,a)&&
    c->before[n].tick>=(n?c->after[n-1].tick:c->predecessor_tick)&&
    c->before[n].tick<c->expires;
  return c->before_result[n];
}
static int after(struct slcli_preentry_backing_windows *c,struct slcli_arena *a,size_t n) {
  c->after_result[n]=slcli_native_clock_read(&c->after[n])&&same(c,a)&&
    c->after[n].tick>=c->before[n].tick&&c->after[n].tick<c->expires;
  return c->after_result[n];
}
static int create_original(struct slcli_preentry_backing_windows *c,
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,
  size_t caller_bytes,const struct slcli_native_clock *prior,uint64_t requested,
  const void *descriptor,size_t descriptor_bytes,struct slcli_memory_owner *physical,uint32_t section_only) {
  size_t i;uint64_t granularity,remainder;
  if(!c||!a||!slcli_memory_owner_mutable(a->owner)||!slcli_arena_live_block(a,c,sizeof(*c))||!caller_bytes||
     !slcli_arena_live_block(a,caller,caller_bytes)||!slcli_source_deadline_original_matches(a,d,caller)||
     !prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
     prior->tick<d->origin->tick||prior->tick>=d->expires||!descriptor_bytes||
     !slcli_arena_live_range(a,descriptor,descriptor_bytes)||
     !apart(c,sizeof(*c),caller,caller_bytes)||!apart(c,sizeof(*c),prior,sizeof(*prior))||
     !apart(c,sizeof(*c),descriptor,descriptor_bytes)||!slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  if(!slcli_memory_owner_mutable(physical)||section_only>1||
     (physical!=a->owner&&(!slcli_arena_live_block(a,physical,sizeof(*physical))||
      !apart(c,sizeof(*c),physical,sizeof(*physical))||
      !apart(physical,sizeof(*physical),caller,caller_bytes)||
      !apart(physical,sizeof(*physical),prior,sizeof(*prior))||
      !apart(physical,sizeof(*physical),descriptor,descriptor_bytes)||
      !slcli_source_deadline_away(d,physical,sizeof(*physical)))))return 0;
  c->physical_owner=physical;c->section_only=section_only;
  c->started=1;c->operation=1;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
  c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
  c->predecessor=prior;c->predecessor_tick=prior->tick;
  c->descriptor_original=descriptor;c->descriptor_bytes=descriptor_bytes;
  c->descriptor=slcli_arena_alloc(a,descriptor_bytes);if(!c->descriptor)return 0;
  for(i=0;i<descriptor_bytes;i++)((unsigned char *)c->descriptor)[i]=((const unsigned char *)descriptor)[i];
  c->security.nLength=sizeof(c->security);c->security.lpSecurityDescriptor=c->descriptor;
  c->security.bInheritHandle=TRUE;
  if(!before(c,a,0))return 0;
  c->args[0][0]=(uintptr_t)&c->page_geometry;c->called[0]=1;
  GetSystemInfo(&c->page_geometry);c->returned[0]=1;
  if(!after(c,a,0))return 0;
  granularity=c->page_geometry.dwAllocationGranularity;
  if(!requested||!granularity||!c->page_geometry.dwPageSize||
     granularity%c->page_geometry.dwPageSize||requested>UINT64_MAX-granularity+1)return 0;
  remainder=requested%granularity;c->bytes=remainder?requested+granularity-remainder:requested;
  if(c->bytes>SIZE_MAX)return 0;
  c->prepare_result=slcli_memory_prepare(c->physical_owner,SLCLI_RUNTIME,c->bytes,&c->request);
  c->prepare_returned=1;if(!c->prepare_result)return 0;c->prepared=1;
  if(!before(c,a,1))goto retained;
  c->args[1][0]=(uintptr_t)INVALID_HANDLE_VALUE;c->args[1][1]=(uintptr_t)&c->security;
  c->args[1][2]=PAGE_READWRITE|SEC_COMMIT;c->args[1][3]=c->bytes>>32;
  c->args[1][4]=c->bytes&UINT32_MAX;c->called[1]=1;
  c->backing=CreateFileMappingW(INVALID_HANDLE_VALUE,&c->security,PAGE_READWRITE|SEC_COMMIT,
    (DWORD)(c->bytes>>32),(DWORD)c->bytes,NULL);
  c->returned[1]=1;if(!c->backing)c->create_error=GetLastError();
  if(c->backing) {
    c->backing_result=slcli_memory_backing_acquired(c->physical_owner,&c->request);
    c->backing_returned=1;
  }
  if(!after(c,a,1)||!c->backing||!c->backing_returned||!c->backing_result)goto retained;
  if(c->section_only){c->qualified=1;return 1;}
  if(!before(c,a,2))goto retained;
  c->args[2][0]=(uintptr_t)c->backing;c->args[2][1]=FILE_MAP_READ|FILE_MAP_WRITE;
  c->args[2][4]=c->bytes;c->called[2]=1;
  c->mapping=MapViewOfFile(c->backing,FILE_MAP_READ|FILE_MAP_WRITE,0,0,(SIZE_T)c->bytes);
  c->returned[2]=1;if(!c->mapping)c->map_error=GetLastError();
  if(c->mapping) {
    c->mapped=1;c->partial_result=slcli_memory_backing_mapped(c->physical_owner,&c->request,(uintptr_t)c->mapping);
    c->partial_returned=1;
  }
  if(!after(c,a,2)||!c->mapped||!c->partial_returned||!c->partial_result)goto retained;
  if(!before(c,a,3))goto retained;
  c->args[3][0]=(uintptr_t)c->mapping;c->args[3][1]=(uintptr_t)&c->geometry;
  c->args[3][2]=sizeof(c->geometry);c->called[3]=1;
  c->geometry_status=VirtualQuery(c->mapping,&c->geometry,sizeof(c->geometry));
  c->returned[3]=1;if(!c->geometry_status)c->geometry_error=GetLastError();
  if(!after(c,a,3)||c->geometry_status!=sizeof(c->geometry)||
     c->geometry.BaseAddress!=c->mapping||c->geometry.AllocationBase!=c->mapping||
     c->geometry.RegionSize!=c->bytes||c->geometry.State!=MEM_COMMIT||
     c->geometry.Type!=MEM_MAPPED||c->geometry.Protect!=PAGE_READWRITE)goto retained;
  c->acquired_result=slcli_memory_acquired(c->physical_owner,&c->request,(uintptr_t)c->mapping,c->bytes,0,0);
  c->acquired_returned=1;if(!c->acquired_result)goto retained;
  c->qualified=1;return 1;
retained:
  c->retained_result=slcli_memory_unobserved(c->physical_owner,&c->request);c->retained_returned=1;return 0;
}
int slcli_preentry_backing_windows_create(struct slcli_preentry_backing_windows *c,
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,
  size_t caller_bytes,const struct slcli_native_clock *prior,uint64_t requested,
  const void *descriptor,size_t descriptor_bytes) {
  return create_original(c,a,d,caller,caller_bytes,prior,requested,descriptor,
                         descriptor_bytes,a?a->owner:NULL,0);
}
int slcli_preentry_backing_windows_create_section(struct slcli_preentry_backing_windows *c,
  struct slcli_arena *a,struct slcli_memory_owner *physical,
  const struct slcli_source_deadline_original *d,const void *caller,
  size_t caller_bytes,const struct slcli_native_clock *prior,uint64_t requested,
  const void *descriptor,size_t descriptor_bytes) {
  return create_original(c,a,d,caller,caller_bytes,prior,requested,descriptor,
                         descriptor_bytes,physical,1);
}
int slcli_preentry_backing_windows_duplicate(struct slcli_preentry_backing_windows *c,
  struct slcli_arena *a,const struct slcli_native_clock *prior) {
  HANDLE process;
  if(!c||!a||!slcli_memory_owner_mutable(a->owner)||!slcli_arena_live_block(a,c,sizeof(*c))||!c->qualified||c->operation!=1||
     !c->backing||c->duplicate_attempted||!prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||
     !prior->complete||prior->tick<c->after[c->section_only?1:3].tick||prior->tick>=c->expires||!same(c,a))return 0;
  c->duplicate_attempted=1;
  if(!slcli_native_clock_read(&c->duplicate_before)||!same(c,a)||
     c->duplicate_before.tick<prior->tick||c->duplicate_before.tick>=c->expires)return 0;
  process=GetCurrentProcess();c->duplicate_args[0]=(uintptr_t)process;
  c->duplicate_args[1]=(uintptr_t)c->backing;c->duplicate_args[2]=(uintptr_t)process;
  c->duplicate_args[3]=(uintptr_t)&c->duplicate;c->duplicate_args[5]=TRUE;
  c->duplicate_args[6]=DUPLICATE_SAME_ACCESS;
  c->duplicate_called=1;
  c->duplicate_status=DuplicateHandle(process,c->backing,process,&c->duplicate,0,TRUE,DUPLICATE_SAME_ACCESS);
  c->duplicate_returned=1;if(!c->duplicate_status)c->duplicate_error=GetLastError();
  if(!slcli_native_clock_read(&c->duplicate_after)||!same(c,a)||
     c->duplicate_after.tick<c->duplicate_before.tick||c->duplicate_after.tick>=c->expires||
     !c->duplicate_status||!c->duplicate||c->duplicate==c->backing)return 0;
  c->duplicate_qualified=1;return 1;
}
int slcli_preentry_backing_windows_recheck_mapped(struct slcli_preentry_backing_windows *c,
 struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,
 size_t caller_bytes,const struct slcli_native_clock *prior,HANDLE backing,HANDLE expected,
 uint64_t bytes,const struct slcli_extent_request *request) {
 size_t i;const struct slcli_extent *e;
 if(!c||!a||!slcli_memory_owner_mutable(a->owner)||!a->allocation_floor||!slcli_arena_live_block(a,c,sizeof(*c))||
  !caller_bytes||!slcli_arena_live_block(a,caller,caller_bytes)||
  !slcli_source_deadline_original_matches(a,d,caller)||
  !prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  prior->tick<d->origin->tick||prior->tick>=d->expires||
  !request||!slcli_arena_live_range(a,request,sizeof(*request))||
  request->index!=a->extent||request->sequence!=a->acquisition||
  !backing||backing==INVALID_HANDLE_VALUE||!expected||expected==INVALID_HANDLE_VALUE||
  backing==expected||!apart(c,sizeof(*c),caller,caller_bytes)||
  !apart(c,sizeof(*c),prior,sizeof(*prior))||!apart(c,sizeof(*c),request,sizeof(*request))||
  !slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
 e=&a->owner->extents[a->extent];
 if(e->state!=SLCLI_EXTENT_LIVE||e->category!=SLCLI_RUNTIME||e->reserved!=bytes||
  !bytes||bytes>SIZE_MAX||!e->base)return 0;
 for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
 c->started=1;c->operation=3;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
 c->predecessor=prior;c->predecessor_tick=prior->tick;c->backing=backing;c->expected_backing=expected;
 c->bytes=bytes;c->physical_owner=a->owner;c->request=*request;c->mapping=(void *)e->base;
 if(!before(c,a,0))return 0;
 c->args[0][0]=(uintptr_t)&c->page_geometry;c->called[0]=1;
 GetSystemInfo(&c->page_geometry);c->returned[0]=1;
 if(!after(c,a,0)||!c->page_geometry.dwPageSize||!c->page_geometry.dwAllocationGranularity||
  bytes%c->page_geometry.dwAllocationGranularity)return 0;
 for(i=0;i<2;i++) {
  HANDLE original=i?expected:backing;
  if(!before(c,a,i+1))return 0;
  c->args[i+1][0]=(uintptr_t)original;c->args[i+1][1]=(uintptr_t)&c->handle_flags[i];c->called[i+1]=1;
  c->handle_status[i]=GetHandleInformation(original,&c->handle_flags[i]);c->returned[i+1]=1;
  if(!c->handle_status[i])c->handle_error[i]=GetLastError();
  if(!after(c,a,i+1)||!c->handle_status[i]||!(c->handle_flags[i]&HANDLE_FLAG_INHERIT))return 0;
 }
 if(!before(c,a,3))return 0;
 c->args[3][0]=(uintptr_t)backing;c->args[3][1]=(uintptr_t)expected;c->called[3]=1;
 c->compare_status=CompareObjectHandles(backing,expected);c->returned[3]=1;
 if(!c->compare_status)c->compare_error=GetLastError();
 if(!after(c,a,3)||!c->compare_status||!before(c,a,4))return 0;
 c->args[4][0]=(uintptr_t)c->mapping;c->args[4][1]=(uintptr_t)&c->geometry;
 c->args[4][2]=sizeof(c->geometry);c->called[4]=1;
 c->geometry_status=VirtualQuery(c->mapping,&c->geometry,sizeof(c->geometry));c->returned[4]=1;
 if(!c->geometry_status)c->geometry_error=GetLastError();
 if(!after(c,a,4)||c->geometry_status!=sizeof(c->geometry)||
  c->geometry.BaseAddress!=c->mapping||c->geometry.AllocationBase!=c->mapping||
  c->geometry.RegionSize!=bytes||c->geometry.State!=MEM_COMMIT||
  c->geometry.Type!=MEM_MAPPED||c->geometry.Protect!=PAGE_READWRITE||
  e->state!=SLCLI_EXTENT_LIVE||e->base!=(uintptr_t)c->mapping||e->reserved!=bytes||
  e->acquisition!=request->sequence||a->acquisition!=request->sequence||
  a->allocation_floor!=((uint64_t)e->arena_initialized-1)*16)return 0;
 c->qualified=1;return 1;
}
int slcli_preentry_backing_windows_receive(struct slcli_preentry_backing_windows *c,
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,
  size_t caller_bytes,const struct slcli_native_clock *prior,HANDLE backing,HANDLE expected,uint64_t bytes,
  const struct slcli_extent_request *request_original) {
  size_t i;
  if(!c||!a||!slcli_memory_owner_mutable(a->owner)||!slcli_arena_live_block(a,c,sizeof(*c))||!caller_bytes||
     !slcli_arena_live_block(a,caller,caller_bytes)||!slcli_source_deadline_original_matches(a,d,caller)||
     !prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
     prior->tick<d->origin->tick||prior->tick>=d->expires||!bytes||bytes>SIZE_MAX||
     !backing||backing==INVALID_HANDLE_VALUE||!expected||expected==INVALID_HANDLE_VALUE||expected==backing||
     !apart(c,sizeof(*c),caller,caller_bytes)||!apart(c,sizeof(*c),prior,sizeof(*prior))||
     !request_original||!slcli_arena_live_range(a,request_original,sizeof(*request_original))||
     !apart(c,sizeof(*c),request_original,sizeof(*request_original))||
     !slcli_memory_backing_request_matches(a->owner,request_original->index,request_original->sequence,bytes)||
     !slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  c->started=1;c->operation=2;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
  c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
  c->predecessor=prior;c->predecessor_tick=prior->tick;c->backing=backing;c->expected_backing=expected;c->bytes=bytes;
  c->physical_owner=a->owner;c->request=*request_original;c->prepared=1;
  if(!before(c,a,0))goto retained_receive;
  c->args[0][0]=(uintptr_t)&c->page_geometry;c->called[0]=1;
  GetSystemInfo(&c->page_geometry);c->returned[0]=1;
  if(!after(c,a,0)||!c->page_geometry.dwAllocationGranularity||
     !c->page_geometry.dwPageSize||bytes%c->page_geometry.dwAllocationGranularity)goto retained_receive;
  for(i=0;i<2;i++) {
    HANDLE original=i?expected:backing;
    if(!before(c,a,i+1))goto retained_receive;
    c->args[i+1][0]=(uintptr_t)original;c->args[i+1][1]=(uintptr_t)&c->handle_flags[i];c->called[i+1]=1;
    c->handle_status[i]=GetHandleInformation(original,&c->handle_flags[i]);
    c->returned[i+1]=1;if(!c->handle_status[i])c->handle_error[i]=GetLastError();
    if(!after(c,a,i+1)||!c->handle_status[i]||!(c->handle_flags[i]&HANDLE_FLAG_INHERIT))goto retained_receive;
  }
  if(!before(c,a,3))goto retained_receive;
  c->args[3][0]=(uintptr_t)backing;c->args[3][1]=(uintptr_t)expected;c->called[3]=1;
  c->compare_status=CompareObjectHandles(backing,expected);
  c->returned[3]=1;if(!c->compare_status)c->compare_error=GetLastError();
  if(!after(c,a,3)||!c->compare_status)goto retained_receive;
  if(!before(c,a,4))goto retained_receive;
  c->args[4][0]=(uintptr_t)backing;c->args[4][1]=FILE_MAP_READ|FILE_MAP_WRITE;c->args[4][4]=bytes;c->called[4]=1;
  c->mapping=MapViewOfFile(backing,FILE_MAP_READ|FILE_MAP_WRITE,0,0,(SIZE_T)bytes);
  c->returned[4]=1;if(!c->mapping)c->map_error=GetLastError();
  if(c->mapping) {c->mapped=1;c->partial_result=slcli_memory_backing_mapped(a->owner,&c->request,(uintptr_t)c->mapping);c->partial_returned=1;}
  if(!after(c,a,4)||!c->mapped||!c->partial_returned||!c->partial_result)goto retained_receive;
  if(!before(c,a,5))goto retained_receive;
  c->args[5][0]=(uintptr_t)c->mapping;c->args[5][1]=(uintptr_t)&c->geometry;c->args[5][2]=sizeof(c->geometry);c->called[5]=1;
  c->geometry_status=VirtualQuery(c->mapping,&c->geometry,sizeof(c->geometry));
  c->returned[5]=1;if(!c->geometry_status)c->geometry_error=GetLastError();
  if(!after(c,a,5)||c->geometry_status!=sizeof(c->geometry)||c->geometry.BaseAddress!=c->mapping||
     c->geometry.AllocationBase!=c->mapping||c->geometry.RegionSize!=bytes||c->geometry.State!=MEM_COMMIT||
     c->geometry.Type!=MEM_MAPPED||c->geometry.Protect!=PAGE_READWRITE)goto retained_receive;
  c->acquired_result=slcli_memory_acquired(a->owner,&c->request,(uintptr_t)c->mapping,bytes,0,0);
  c->acquired_returned=1;if(!c->acquired_result)goto retained_receive;
  c->qualified=1;return 1;
retained_receive:
  c->retained_result=slcli_memory_unobserved(a->owner,&c->request);c->retained_returned=1;return 0;
}
#endif
