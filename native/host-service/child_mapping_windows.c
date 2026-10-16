#include "child_mapping_windows.h"
#if defined(_WIN32)
#include "launcher_native.h"
/* Documented ZwMapViewOfSection selects NtMapViewOfSection for user mode.
 * Fixed 32-bit SECTION_INHERIT ViewUnmap=2; exact selected ntdll import/ABI,
 * native source and lifetime qualification remain required before execution. */
NTSYSAPI NTSTATUS NTAPI NtMapViewOfSection(HANDLE,HANDLE,PVOID *,ULONG_PTR,
 SIZE_T,PLARGE_INTEGER,PSIZE_T,ULONG,ULONG,ULONG);
_Static_assert(sizeof(ULONG)==4,"Native SECTION_INHERIT width");
static int same(struct slcli_runtime_containing_windows *r,struct slcli_arena *a) {
 struct slcli_child_mapping_windows *c=&r->child;
 const struct slcli_launcher_windows_capture *b=c->birth_original;
 const struct slcli_binding_originals *book=r->binding_original;
 const struct slcli_native_reference *row,*section;
 if(!r->capture_prepared||r->first_failure||!book||
  !slcli_arena_live_range(a,book,sizeof(*book))||!book->references||!book->count||
  book->count>SIZE_MAX/sizeof(*row)||
  !slcli_arena_live_range(a,book->references,book->count*sizeof(*row))||
  r->original.child_view_ordinal>=book->count||r->original.section_ordinal>=book->count||
  !b||!slcli_arena_live_block(a,b,sizeof(*b))||b->containing_original!=r||
  b->runtime_backing_original!=r->backing||b->target_owner_original!=r->runtime_owner_original||
  b->deadline!=r->deadline||b->caller!=r->caller||b->resume_attempted||
  b->creation_flags!=(CREATE_SUSPENDED|CREATE_UNICODE_ENVIRONMENT|EXTENDED_STARTUPINFO_PRESENT)||
  !b->create_called||!b->create_returned||!b->create_status||!b->created||
  !b->times_called||!b->times_returned||!b->times_status||
  b->process.hProcess!=c->process_original||b->process.dwProcessId!=c->process_id_original||
  b->process.dwThreadId!=c->thread_id_original||!b->process.hThread||
  b->creation_time.dwHighDateTime!=c->creation_original.dwHighDateTime||
  b->creation_time.dwLowDateTime!=c->creation_original.dwLowDateTime||
  !slcli_source_deadline_original_matches(a,r->deadline,r->caller)||
  !r->backing||!slcli_arena_live_block(a,r->backing,sizeof(*r->backing))||
  r->backing->backing!=c->section_original||!r->backing->qualified||
  !r->backing->section_only||r->backing->bytes!=c->bytes_before||
  !r->alias||!slcli_arena_live_block(a,r->alias,sizeof(*r->alias))||
  !r->alias->validated||!r->alias->qualified||r->alias->unmap_attempted||
  r->alias->view!=c->requested_base_original||
  !r->initializer||!slcli_arena_live_block(a,r->initializer,sizeof(*r->initializer))||
  !r->initializer->unmapped||!r->layout||!slcli_arena_live_block(a,r->layout,sizeof(*r->layout))||
  !r->layout->geometry.dwAllocationGranularity||r->layout->deadline!=r->deadline||
  r->layout->origin!=r->deadline->origin||r->layout->origin_tick!=r->deadline->origin->tick||
  r->layout->expires!=r->deadline->expires||!c->enrolled||c->root_original!=r)return 0;
 row=&book->references[r->original.child_view_ordinal];section=&book->references[r->original.section_ordinal];
 return row->native_original==c&&row->native_class==SLCLI_NATIVE_ORIGINAL_SUSPENDED_CHILD_VIEW&&
  row->kind==9&&row->version==c->row_version_original&&row->version&&!row->handle&&
  row->effect_flags==(c->map_called?(c->map_returned?(c->possibly_mapped?15:7):3):1)&&
  row->state==(c->possibly_mapped?0:2)&&row->alias_of==r->original.section_ordinal&&
  row->physical_extent==section->physical_extent&&section->native_original==r->backing&&
  section->native_class==SLCLI_NATIVE_ORIGINAL_SECTION_BACKING&&section->kind==22&&
  section->version==c->section_version_original&&section->version&&section->effect_flags==15&&
  section->handle==(uintptr_t)c->section_original&&section->physical_extent==r->backing->request.index;
}
int slcli_child_mapping_windows_matches_original(struct slcli_runtime_containing_windows *r,
 struct slcli_arena *a,const struct slcli_launcher_windows_capture *birth) {
 const struct slcli_child_mapping_windows *c;
 if(!r||!a||!slcli_arena_live_block(a,r,sizeof(*r))||!birth||
  r->child.birth_original!=birth||!same(r,a))return 0;
 c=&r->child;
 return c->attempted&&c->map_called&&c->map_returned&&c->possibly_mapped&&c->qualified&&
  c->status>=0&&c->base&&c->base==c->requested_base_original&&c->bytes==c->bytes_before&&
  !c->offset.QuadPart&&(uintptr_t)c->base%r->layout->geometry.dwAllocationGranularity==0&&
  c->map_args[0]==(uintptr_t)c->section_original&&c->map_args[1]==(uintptr_t)c->process_original&&
  c->map_args[2]==(uintptr_t)&c->base&&!c->map_args[3]&&!c->map_args[4]&&
  c->map_args[5]==(uintptr_t)&c->offset&&c->map_args[6]==(uintptr_t)&c->bytes&&
  c->map_args[7]==2&&!c->map_args[8]&&c->map_args[9]==PAGE_READWRITE&&
  c->query_called&&c->query_returned&&c->query_status==sizeof(c->geometry)&&
  c->query_args[0]==(uintptr_t)c->process_original&&c->query_args[1]==(uintptr_t)c->base&&
  c->query_args[2]==(uintptr_t)&c->geometry&&c->query_args[3]==sizeof(c->geometry)&&
  c->geometry.BaseAddress==c->base&&c->geometry.AllocationBase==c->base&&
  c->geometry.RegionSize==c->bytes&&c->geometry.State==MEM_COMMIT&&
  c->geometry.Type==MEM_MAPPED&&c->geometry.Protect==PAGE_READWRITE&&
  birth->after.complete&&c->before.complete&&c->after.complete&&
  c->query_before.complete&&c->query_after.complete&&c->before.tick>=birth->after.tick&&
  c->after.tick>=c->before.tick&&c->query_before.tick>=c->after.tick&&
  c->query_after.tick>=c->query_before.tick&&c->query_after.tick<r->deadline->expires;
}
int slcli_child_mapping_windows_map_original(struct slcli_runtime_containing_windows *r,
 struct slcli_arena *a,const struct slcli_launcher_windows_capture *birth) {
 struct slcli_child_mapping_windows *c;struct slcli_native_reference *row;
 if(!r||!a||!slcli_arena_live_block(a,r,sizeof(*r))||!birth||
  !slcli_arena_live_block(a,birth,sizeof(*birth))||r->child.attempted||
  !r->child.enrolled||birth->containing_original!=r||!r->backing||
  !slcli_arena_live_block(a,r->backing,sizeof(*r->backing))||
  r->backing->bytes>SIZE_MAX||!r->backing->bytes)return 0;
 c=&r->child;c->attempted=1;c->birth_original=birth;
 if(!r->binding_original||!slcli_arena_live_range(a,r->binding_original,sizeof(*r->binding_original))||
  !r->binding_original->references||!r->binding_original->count||
  r->binding_original->count>SIZE_MAX/sizeof(*row)||
  !slcli_arena_live_range(a,r->binding_original->references,r->binding_original->count*sizeof(*row))||
  r->original.child_view_ordinal>=r->binding_original->count||
  r->original.section_ordinal>=r->binding_original->count)return 0;
 row=&r->binding_original->references[r->original.child_view_ordinal];
 if(row->state!=2||row->effect_flags!=1)return 0;
 c->row_version_original=row->version;
 c->section_version_original=r->binding_original->references[r->original.section_ordinal].version;
 c->process_original=birth->process.hProcess;c->process_id_original=birth->process.dwProcessId;
 c->thread_id_original=birth->process.dwThreadId;c->creation_original=birth->creation_time;
 c->section_original=r->backing->backing;c->bytes_before=(SIZE_T)r->backing->bytes;c->bytes=c->bytes_before;
 if(!r->alias||!slcli_arena_live_block(a,r->alias,sizeof(*r->alias))||!r->alias->view||
  (uintptr_t)r->alias->view>UINTPTR_MAX-c->bytes_before)return 0;
 c->requested_base_original=r->alias->view;c->base=c->requested_base_original;
 if(!c->process_original||c->process_original==INVALID_HANDLE_VALUE||
  !c->process_id_original||!c->thread_id_original||!same(r,a)||!birth->after.complete||
  !slcli_native_clock_read(&c->before)||c->before.tick<birth->after.tick||
  c->before.tick>=r->layout->expires||!same(r,a))return 0;
 row=&r->binding_original->references[r->original.child_view_ordinal];
 c->map_args[0]=(uintptr_t)c->section_original;c->map_args[1]=(uintptr_t)c->process_original;
 c->map_args[2]=(uintptr_t)&c->base;c->map_args[3]=0;c->map_args[4]=0;
 c->map_args[5]=(uintptr_t)&c->offset;c->map_args[6]=(uintptr_t)&c->bytes;
 c->map_args[7]=2;c->map_args[8]=0;c->map_args[9]=PAGE_READWRITE;
 row->effect_flags|=2;c->map_called=1;
 c->status=NtMapViewOfSection(c->section_original,c->process_original,&c->base,0,0,
  &c->offset,&c->bytes,2,0,PAGE_READWRITE);
 c->map_returned=1;row->effect_flags|=4;
 if(c->status>=0&&c->base){c->possibly_mapped=1;row->effect_flags|=8;row->state=0;}
 if(!slcli_native_clock_read(&c->after)||!same(r,a)||c->after.tick<c->before.tick||
  c->after.tick>=r->layout->expires||c->status<0||!c->base||c->base!=c->requested_base_original||
  c->offset.QuadPart||c->bytes!=c->bytes_before||
  (uintptr_t)c->base%r->layout->geometry.dwAllocationGranularity)return 0;
 if(!slcli_native_clock_read(&c->query_before)||!same(r,a)||
  c->query_before.tick<c->after.tick||c->query_before.tick>=r->layout->expires)return 0;
 c->query_args[0]=(uintptr_t)c->process_original;c->query_args[1]=(uintptr_t)c->base;
 c->query_args[2]=(uintptr_t)&c->geometry;c->query_args[3]=sizeof(c->geometry);c->query_called=1;
 c->query_status=VirtualQueryEx(c->process_original,c->base,&c->geometry,sizeof(c->geometry));
 c->query_returned=1;if(!c->query_status)c->query_error=GetLastError();
 if(!slcli_native_clock_read(&c->query_after)||!same(r,a)||
  c->query_after.tick<c->query_before.tick||c->query_after.tick>=r->layout->expires||
  c->query_status!=sizeof(c->geometry)||c->geometry.BaseAddress!=c->base||
  c->geometry.AllocationBase!=c->base||c->geometry.RegionSize!=c->bytes_before||
  c->geometry.State!=MEM_COMMIT||c->geometry.Type!=MEM_MAPPED||
  c->geometry.Protect!=PAGE_READWRITE)return 0;
 c->qualified=1;return 1;
}
#endif
