#include "loader_windows.h"
#include "early_allocator.h"
#if defined(_WIN32)
/* These fixed ntdll declarations require exact selected native ABI/source
 * qualification before executable admission. Export-name presence alone is
 * insufficient. TRY_ONLY never waits on another loader owner. */
NTSYSAPI NTSTATUS NTAPI LdrLockLoaderLock(ULONG,ULONG *,ULONG_PTR *);
NTSYSAPI NTSTATUS NTAPI LdrUnlockLoaderLock(ULONG,ULONG_PTR);
/* Defined by the selected complete original ordinary static tlssup object;
 * they are source/link originals, never caller-supplied aliases. */
extern const IMAGE_TLS_DIRECTORY64 _tls_used;
extern ULONG _tls_index;
static int own_tls(struct slcli_node_loader_windows *);
static int entry_image_matches(const struct slcli_node_prefix_windows *p) {
 size_t i;uintptr_t base,entry;const struct slcli_node_prefix_windows_region *r;
 if(!p||!p->entry_original||!p->image_original||!p->image_region_original||
  !p->rows||p->capacity>SIZE_MAX/sizeof(*p->rows)||p->used>p->capacity||!p->receiver_original||
  p->entry_original!=p->receiver_original->entry_original)return 0;
 for(i=0;i<p->used;i++)if(&p->rows[i]==p->image_region_original)break;
 if(i==p->used)return 0;r=&p->rows[i];base=(uintptr_t)r->geometry.BaseAddress;
 entry=(uintptr_t)p->entry_original;
 return r->called&&r->returned&&r->result==sizeof(r->geometry)&&r->after_result&&
  r->geometry.State==MEM_COMMIT&&r->geometry.Type==MEM_IMAGE&&
  r->geometry.AllocationBase==p->image_original&&r->geometry.RegionSize<=UINTPTR_MAX-base&&
  entry>=base&&entry<base+r->geometry.RegionSize;
}
static int apart(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int readable(const struct slcli_node_prefix_windows *p,const void *address,size_t bytes) {
  uintptr_t at=(uintptr_t)address,end,base,limit;size_t i;
  if(!address||!bytes||at>UINTPTR_MAX-bytes)return 0;end=at+bytes;
  for(i=0;i<p->used&&at<end;i++) {
    const MEMORY_BASIC_INFORMATION *g=&p->rows[i].geometry;
    base=(uintptr_t)g->BaseAddress;if(g->RegionSize>UINTPTR_MAX-base)return 0;limit=base+g->RegionSize;
    if(at<base)return 0;if(at>=limit)continue;
    if(g->State!=MEM_COMMIT||(g->Protect&(PAGE_NOACCESS|PAGE_GUARD))||
       !(g->Protect&(PAGE_READONLY|PAGE_READWRITE|PAGE_WRITECOPY|
         PAGE_EXECUTE_READ|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY)))return 0;
    at=limit<end?limit:end;
  }
  return at==end;
}
static void copy(void *to,const void *from,size_t bytes) {
  size_t i;for(i=0;i<bytes;i++)((unsigned char *)to)[i]=((const unsigned char *)from)[i];
}
static int equal(const void *a,const void *b,size_t n) {
  size_t i;for(i=0;i<n;i++)if(((const unsigned char *)a)[i]!=((const unsigned char *)b)[i])return 0;return 1;
}
static int image_range(const struct slcli_node_prefix_windows *p,uintptr_t image,uintptr_t address,size_t bytes) {
  size_t i;uintptr_t base,end,at=address,limit;
  if(!bytes||address>UINTPTR_MAX-bytes)return 0;end=address+bytes;
  for(i=0;i<p->used&&at<end;i++) {
    const MEMORY_BASIC_INFORMATION *g=&p->rows[i].geometry;
    base=(uintptr_t)g->BaseAddress;if(g->RegionSize>UINTPTR_MAX-base)return 0;limit=base+g->RegionSize;
    if(at<base)return 0;if(at>=limit)continue;
    if((uintptr_t)g->AllocationBase!=image||g->Type!=MEM_IMAGE||
       (g->State!=MEM_COMMIT&&g->State!=MEM_RESERVE))return 0;
    at=limit<end?limit:end;
  }
  return at==end;
}
static int module_headers(struct slcli_node_loader_windows_module *m,
  const struct slcli_node_prefix_windows *p) {
  uintptr_t image=(uintptr_t)m->before.DllBase,nt,tls,index;DWORD rva,size;
  if(!image||!readable(p,(const void *)image,sizeof(m->dos)))return 0;
  copy(&m->dos,(const void *)image,sizeof(m->dos));
  if(m->dos.e_magic!=IMAGE_DOS_SIGNATURE||m->dos.e_lfanew<(LONG)sizeof(m->dos)||
     image>UINTPTR_MAX-(uint32_t)m->dos.e_lfanew)return 0;
  nt=image+(uint32_t)m->dos.e_lfanew;
  if(!readable(p,(const void *)nt,sizeof(m->nt)))return 0;
  copy(&m->nt,(const void *)nt,sizeof(m->nt));
  if(m->nt.Signature!=IMAGE_NT_SIGNATURE||m->nt.FileHeader.Machine!=IMAGE_FILE_MACHINE_AMD64||
     m->nt.FileHeader.SizeOfOptionalHeader!=sizeof(IMAGE_OPTIONAL_HEADER64)||
     m->nt.OptionalHeader.Magic!=IMAGE_NT_OPTIONAL_HDR64_MAGIC||
     m->nt.OptionalHeader.NumberOfRvaAndSizes!=IMAGE_NUMBEROF_DIRECTORY_ENTRIES||
     !m->nt.OptionalHeader.SizeOfImage||m->nt.OptionalHeader.SizeOfImage%p->system.dwPageSize||
     image>UINTPTR_MAX-m->nt.OptionalHeader.SizeOfImage)return 0;
  m->image_end=image+m->nt.OptionalHeader.SizeOfImage;
  if(nt>m->image_end||sizeof(m->nt)>m->image_end-nt||
     !image_range(p,image,image,m->nt.OptionalHeader.SizeOfImage))return 0;
  m->header_returned=1;
  rva=m->nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress;
  size=m->nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size;
  if(!rva&&!size)return 1;
  if(!rva||size<sizeof(m->tls)||rva>=m->nt.OptionalHeader.SizeOfImage||
     sizeof(m->tls)>m->nt.OptionalHeader.SizeOfImage-rva)return 0;
  tls=image+rva;if(!readable(p,(const void *)tls,sizeof(m->tls)))return 0;
  copy(&m->tls,(const void *)tls,sizeof(m->tls));m->tls_present=1;
  if(m->tls.StartAddressOfRawData>UINTPTR_MAX||m->tls.EndAddressOfRawData>UINTPTR_MAX||
     m->tls.EndAddressOfRawData<m->tls.StartAddressOfRawData)return 0;
  m->tls_template_begin=(uintptr_t)m->tls.StartAddressOfRawData;
  m->tls_template_end=(uintptr_t)m->tls.EndAddressOfRawData;
  m->tls_template_bytes=m->tls.EndAddressOfRawData-m->tls.StartAddressOfRawData;
  if(m->tls_template_bytes>UINT64_MAX-m->tls.SizeOfZeroFill)return 0;
  m->tls_storage_bytes=m->tls_template_bytes+m->tls.SizeOfZeroFill;
  if(m->tls_template_bytes&&
     (m->tls_template_bytes>SIZE_MAX||m->tls_template_begin<image||m->tls_template_end>m->image_end||
      !image_range(p,image,m->tls_template_begin,(size_t)m->tls_template_bytes)||
      !readable(p,(const void *)m->tls_template_begin,(size_t)m->tls_template_bytes)))return 0;
  m->tls_template_observed=1;
  index=(uintptr_t)m->tls.AddressOfIndex;
  if(m->tls.AddressOfIndex>UINTPTR_MAX||index<image||index>=m->image_end||
     sizeof(DWORD)>m->image_end-index||!readable(p,(const void *)index,sizeof(DWORD)))return 0;
  copy(&m->tls_index,(const void *)index,sizeof(m->tls_index));m->tls_index_observed=1;
  return 1;
}
int slcli_node_loader_windows_observe(struct slcli_node_loader_windows *c,
  struct slcli_node_loader_windows_module *modules,size_t capacity,WCHAR *names,
  size_t name_capacity,size_t per_name_units,const struct slcli_node_prefix_windows *p) {
  size_t i,j,module_bytes,name_bytes;uintptr_t next;struct slcli_node_loader_windows_module *m;
  if(!c||!modules||!capacity||capacity>SIZE_MAX/sizeof(*modules)||!names||!name_capacity||
     name_capacity>SIZE_MAX/sizeof(WCHAR)||!per_name_units||capacity>SIZE_MAX/per_name_units||
     name_capacity!=capacity*per_name_units||!p||!p->complete_scan||!p->terminal||!p->used||
     !entry_image_matches(p))return 0;
  module_bytes=capacity*sizeof(*modules);name_bytes=name_capacity*sizeof(WCHAR);
  if(!apart(c,sizeof(*c),modules,module_bytes)||!apart(c,sizeof(*c),names,name_bytes)||
     !apart(modules,module_bytes,names,name_bytes)||!apart(c,sizeof(*c),p,sizeof(*p))||
     !apart(modules,module_bytes,p,sizeof(*p))||!apart(names,name_bytes,p,sizeof(*p))||
     !apart(c,sizeof(*c),p->rows,p->capacity*sizeof(*p->rows))||
     !apart(modules,module_bytes,p->rows,p->capacity*sizeof(*p->rows))||
     !apart(names,name_bytes,p->rows,p->capacity*sizeof(*p->rows))||
     !apart(c,sizeof(*c),p->receiver_original,sizeof(*p->receiver_original))||
     !apart(modules,module_bytes,p->receiver_original,sizeof(*p->receiver_original))||
     !apart(names,name_bytes,p->receiver_original,sizeof(*p->receiver_original)))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  for(i=0;i<module_bytes;i++)if(((const unsigned char *)modules)[i])return 0;
  for(i=0;i<name_capacity;i++)if(names[i])return 0;
  c->started=1;c->capture_active=1;c->modules=modules;c->capacity=capacity;c->names=names;
  c->name_capacity=name_capacity;c->per_name_units=per_name_units;c->prefix_original=p;
  c->expires=p->expires;c->previous_tick=p->previous_tick;
  if(!slcli_native_clock_read(&c->before)||c->before.tick<c->previous_tick||c->before.tick>=c->expires)goto failed;
  c->basic_args[0]=(uintptr_t)(HANDLE)(LONG_PTR)-1;c->basic_args[1]=ProcessBasicInformation;
  c->basic_args[2]=(uintptr_t)&c->basic;c->basic_args[3]=sizeof(c->basic);
  c->basic_args[4]=(uintptr_t)&c->basic_bytes;c->basic_called=1;
  c->basic_status=NtQueryInformationProcess((HANDLE)(LONG_PTR)-1,ProcessBasicInformation,
    &c->basic,sizeof(c->basic),&c->basic_bytes);c->basic_returned=1;
  if(!slcli_native_clock_read(&c->after)||c->after.tick<c->before.tick||c->after.tick>=c->expires||
     c->basic_status!=0||c->basic_bytes!=sizeof(c->basic)||
     c->basic.UniqueProcessId!=p->receiver_original->decode.transport.process_original)goto failed;
  c->peb_original=c->basic.PebBaseAddress;
  if(!slcli_native_clock_read(&c->lock_before)||c->lock_before.tick<c->after.tick||
     c->lock_before.tick>=c->expires)goto failed;
  c->lock_args[0]=2;c->lock_args[1]=(uintptr_t)&c->lock_disposition;
  c->lock_args[2]=(uintptr_t)&c->lock_cookie;c->lock_called=1;
  c->lock_status=LdrLockLoaderLock(2,&c->lock_disposition,&c->lock_cookie);
  c->lock_returned=1;
  if(c->lock_status==0&&c->lock_disposition==1)c->lock_held=1;
  if(!slcli_native_clock_read(&c->lock_after)||c->lock_after.tick<c->lock_before.tick||
     c->lock_after.tick>=c->expires||!c->lock_held)goto failed;
  if(!readable(p,c->peb_original,sizeof(c->peb_before)))goto failed;
  copy(&c->peb_before,c->peb_original,sizeof(c->peb_before));c->ldr_original=c->peb_before.Ldr;
  if(!readable(p,c->ldr_original,sizeof(c->ldr_before)))goto failed;
  copy(&c->ldr_before,c->ldr_original,sizeof(c->ldr_before));
  c->head_original=&c->ldr_original->InMemoryOrderModuleList;
  c->previous_original=c->head_original;c->next_original=c->ldr_before.InMemoryOrderModuleList.Flink;
  c->previous_tick=c->lock_after.tick;
  while(c->next_original!=c->head_original) {
    if(c->used==c->capacity||!c->next_original)goto failed;
    next=(uintptr_t)c->next_original;
    if(next<offsetof(LDR_DATA_TABLE_ENTRY,InMemoryOrderLinks))goto failed;
    m=&modules[c->used++];m->started=1;
    m->original=(const LDR_DATA_TABLE_ENTRY *)(next - offsetof(LDR_DATA_TABLE_ENTRY,InMemoryOrderLinks));
    if(!slcli_native_clock_read(&m->begin)||m->begin.tick<c->previous_tick||m->begin.tick>=c->expires||
       !readable(p,m->original,sizeof(m->before)))goto failed;
    copy(&m->before,m->original,sizeof(m->before));
    if(m->before.InMemoryOrderLinks.Blink!=c->previous_original||!m->before.DllBase)goto failed;
    for(i=0;i+1<c->used;i++)if(modules[i].original==m->original||modules[i].before.DllBase==m->before.DllBase)goto failed;
    if(!slcli_native_clock_read(&m->reference_before)||m->reference_before.tick<m->begin.tick||
       m->reference_before.tick>=c->expires)goto failed;
    m->reference_args[0]=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS;
    m->reference_args[1]=(uintptr_t)m->before.DllBase;
    m->reference_args[2]=(uintptr_t)&m->native_reference;m->reference_called=1;
    m->reference_result=GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
      (LPCWSTR)m->before.DllBase,&m->native_reference);m->reference_returned=1;
    m->reference_error=m->reference_result?ERROR_SUCCESS:GetLastError();m->reference_error_observed=1;
    if(!slcli_native_clock_read(&m->reference_after)||m->reference_after.tick<m->reference_before.tick||
       m->reference_after.tick>=c->expires||!m->reference_result||
       (void *)m->native_reference!=m->before.DllBase)goto failed;
    /* A real successful reference remains retained on every later failure;
     * no UNCHANGED_REFCOUNT/PIN flag, fabricated unload or local FREE is used.
     * Full native accounting/source qualification precedes reaching this call. */
    if(!module_headers(m,p)||!m->before.FullDllName.Length||m->before.FullDllName.Length%sizeof(WCHAR)||
       m->before.FullDllName.MaximumLength<m->before.FullDllName.Length)goto failed;
    m->name_units=m->before.FullDllName.Length/sizeof(WCHAR);
    if(m->name_units>=per_name_units||!readable(p,m->before.FullDllName.Buffer,m->before.FullDllName.Length))goto failed;
    m->name=&names[(c->used-1)*per_name_units];
    for(j=0;j<m->name_units;j++){m->name[j]=m->before.FullDllName.Buffer[j];if(!m->name[j])goto failed;}
    m->name_returned=1;
    copy(&m->after,m->original,sizeof(m->after));
    if(!equal(&m->before,&m->after,sizeof(m->before)))goto failed;m->rechecked=1;
    if(!slcli_native_clock_read(&m->end)||m->end.tick<m->reference_after.tick||m->end.tick>=c->expires)goto failed;
    m->returned=1;c->previous_tick=m->end.tick;
    if(m->tls_present)c->static_tls_modules++;
    c->previous_original=&m->original->InMemoryOrderLinks;c->next_original=m->before.InMemoryOrderLinks.Flink;
  }
  if(!c->used||c->ldr_before.InMemoryOrderModuleList.Blink!=c->previous_original)goto failed;
  copy(&c->peb_after,c->peb_original,sizeof(c->peb_after));
  copy(&c->ldr_after,c->ldr_original,sizeof(c->ldr_after));
  if(!equal(&c->peb_before,&c->peb_after,sizeof(c->peb_before))||
     !equal(&c->ldr_before,&c->ldr_after,sizeof(c->ldr_before))||
     !slcli_native_clock_read(&c->finish)||c->finish.tick<c->previous_tick||c->finish.tick>=c->expires)goto failed;
  c->complete_list=1;c->own_tls_result=own_tls(c);c->own_tls_call_returned=1;
  c->capture_returned=1;c->capture_active=0;c->terminal=1;return c->own_tls_result;
failed:c->capture_returned=1;c->capture_active=0;c->terminal=1;return 0;
}
int slcli_node_loader_windows_release(struct slcli_node_loader_windows *c) {
  if(!c||!c->started||!c->terminal||c->capture_active||!c->capture_returned||!c->lock_called||!c->lock_returned||
     !c->lock_held||c->unlock_called)return 0;
  c->unlock_args[0]=0;c->unlock_args[1]=c->lock_cookie;c->unlock_called=1;
  c->unlock_status=LdrUnlockLoaderLock(0,c->lock_cookie);
  c->unlock_returned=1;
  if(c->unlock_status!=0)return 0;
  c->lock_held=0;return 1;
}
static int own_tls(struct slcli_node_loader_windows *c) {
  const struct slcli_node_loader_windows_module *m;size_t i,found=0;
  uintptr_t directory,image,slot,base,limit;size_t row;int geometry_match=0;
  if(!c||!c->complete_list||!c->capture_active||c->capture_returned||c->terminal||!c->lock_held||c->unlock_called||
     c->own_tls_started)return 0;
  c->own_tls_started=1;c->own_tls_directory_original=&_tls_used;
  c->own_tls_index_original=&_tls_index;
  c->own_tls_before_result=slcli_native_clock_read(&c->own_tls_before);
  if(!c->own_tls_before_result||c->own_tls_before.tick<c->finish.tick||c->own_tls_before.tick>=c->expires)return 0;
  image=(uintptr_t)c->prefix_original->image_original;
  for(i=0;i<c->used;i++) {
    m=&c->modules[i];
    if((uintptr_t)m->before.DllBase!=image)continue;
    if(found||!m->returned||!m->tls_present||!m->tls_index_observed)return 0;
    found=1;c->own_tls_module=i;
    directory=image+m->nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress;
    if(directory!=(uintptr_t)&_tls_used||m->tls.AddressOfIndex!=(uintptr_t)&_tls_index||
       !readable(c->prefix_original,&_tls_used,sizeof(_tls_used))||
       !readable(c->prefix_original,&_tls_index,sizeof(_tls_index))||
       !equal(&m->tls,&_tls_used,sizeof(m->tls)))return 0;
    copy(&c->own_tls_index,&_tls_index,sizeof(c->own_tls_index));
    if(c->own_tls_index!=m->tls_index)return 0;
  }
  if(!found)return 0;
  c->own_tls_symbols_match=1;
  c->own_thread_before_result=slcli_native_clock_read(&c->own_thread_before);
  if(!c->own_thread_before_result||c->own_thread_before.tick<c->own_tls_before.tick||
     c->own_thread_before.tick>=c->expires)return 0;
  c->own_thread_called=1;c->own_thread=GetCurrentThreadId();c->own_thread_returned=1;
  c->own_thread_after_result=slcli_native_clock_read(&c->own_thread_after);
  if(!c->own_thread_after_result||c->own_thread_after.tick<c->own_thread_before.tick||
     c->own_thread_after.tick>=c->expires||!c->own_thread||
     c->own_thread!=c->prefix_original->receiver_original->decode.transport.thread_original)return 0;
  c->own_tls_slot_original=slcli_node_allocator_tls_slot_original();c->own_tls_slot_returned=1;
  if(!c->own_tls_slot_original)return 0;
  c->own_tls_query_args[0]=(uintptr_t)c->own_tls_slot_original;
  c->own_tls_query_args[1]=(uintptr_t)&c->own_tls_geometry;
  c->own_tls_query_args[2]=sizeof(c->own_tls_geometry);
  c->own_tls_query_before_result=slcli_native_clock_read(&c->own_tls_query_before);
  if(!c->own_tls_query_before_result||c->own_tls_query_before.tick<c->own_thread_after.tick||
     c->own_tls_query_before.tick>=c->expires)return 0;
  c->own_tls_query_called=1;
  c->own_tls_query_result=VirtualQuery(c->own_tls_slot_original,&c->own_tls_geometry,
    sizeof(c->own_tls_geometry));c->own_tls_query_returned=1;
  c->own_tls_query_error=c->own_tls_query_result?ERROR_SUCCESS:GetLastError();c->own_tls_query_error_observed=1;
  c->own_tls_query_after_result=slcli_native_clock_read(&c->own_tls_query_after);
  if(!c->own_tls_query_after_result||c->own_tls_query_after.tick<c->own_tls_query_before.tick||
     c->own_tls_query_after.tick>=c->expires||c->own_tls_query_result!=sizeof(c->own_tls_geometry)||
     !c->own_tls_geometry.AllocationBase||c->own_tls_geometry.State!=MEM_COMMIT||
     c->own_tls_geometry.Type!=MEM_PRIVATE||
     (c->own_tls_geometry.Protect&(PAGE_NOACCESS|PAGE_GUARD))||
     !readable(c->prefix_original,c->own_tls_slot_original,sizeof(void *)))return 0;
  slot=(uintptr_t)c->own_tls_slot_original;base=(uintptr_t)c->own_tls_geometry.BaseAddress;
  if(!c->own_tls_geometry.RegionSize||c->own_tls_geometry.RegionSize>UINTPTR_MAX-base||
     base%c->prefix_original->system.dwPageSize||
     c->own_tls_geometry.RegionSize%c->prefix_original->system.dwPageSize)return 0;
  limit=base+c->own_tls_geometry.RegionSize;
  if(slot<base||slot>=limit||sizeof(void *)>limit-slot)return 0;
  for(row=0;row<c->prefix_original->used;row++) {
    const MEMORY_BASIC_INFORMATION *g=&c->prefix_original->rows[row].geometry;
    if(g->BaseAddress!=c->own_tls_geometry.BaseAddress)continue;
    if(g->AllocationBase!=c->own_tls_geometry.AllocationBase||g->RegionSize!=c->own_tls_geometry.RegionSize||
       g->State!=c->own_tls_geometry.State||g->Type!=c->own_tls_geometry.Type||
       g->Protect!=c->own_tls_geometry.Protect||g->AllocationProtect!=c->own_tls_geometry.AllocationProtect)return 0;
    geometry_match=1;break;
  }
  if(!geometry_match)return 0;
  c->own_tls_after_result=slcli_native_clock_read(&c->own_tls_after);
  if(!c->own_tls_after_result||c->own_tls_after.tick<c->own_tls_query_after.tick||c->own_tls_after.tick>=c->expires)return 0;
  c->own_tls_returned=1;return 1;
}
int slcli_node_loader_windows_decrement_references(struct slcli_node_loader_windows *c) {
  size_t i;int result=1;struct slcli_node_loader_windows_module *m;
  if(!c||!c->started||!c->terminal||c->capture_active||!c->capture_returned||c->lock_held||!c->unlock_called||
     !c->unlock_returned||c->unlock_status!=0||c->used>c->capacity)return 0;
  for(i=0;i<c->used;i++) {
    m=&c->modules[i];
    if(!m->reference_called)continue;
    if(!m->reference_returned) {result=0;continue;}
    if(!m->reference_result) {if(m->native_reference)result=0;continue;}
    if(!m->native_reference||(void *)m->native_reference!=m->before.DllBase) {result=0;continue;}
    if(m->decrement_called) {if(!m->decrement_returned||!m->reference_decremented)result=0;continue;}
    m->decrement_args[0]=(uintptr_t)m->native_reference;m->decrement_called=1;
    m->decrement_result=FreeLibrary(m->native_reference);m->decrement_returned=1;
    m->decrement_error=m->decrement_result?ERROR_SUCCESS:GetLastError();m->decrement_error_observed=1;
    if(m->decrement_result)m->reference_decremented=1;else result=0;
  }
  return result;
}
#endif
