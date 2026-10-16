#include "prefix_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_node_prefix_windows_observe(struct slcli_node_prefix_windows *c,
  struct slcli_node_prefix_windows_region *rows,size_t capacity,
  const struct slcli_node_receive_windows *receiver,const void *entry,const void *stack) {
  size_t i,bytes;uintptr_t base,end;SIZE_T result;DWORD protection;
  struct slcli_node_prefix_windows_region *r;
  if(!c||!rows||!capacity||capacity>SIZE_MAX/sizeof(*rows)||!receiver||
     !receiver->custody_returned||!receiver->terminal||!receiver->parent_returned||
     !receiver->parent_result||!receiver->parent.observations_match||!entry||!stack||
     receiver->entry_original!=entry)return 0;
  bytes=capacity*sizeof(*rows);
  if(!apart(c,sizeof(*c),rows,bytes)||!apart(c,sizeof(*c),receiver,sizeof(*receiver))||
     !apart(rows,bytes,receiver,sizeof(*receiver)))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  /* Entire supplied backing must be fresh before the first native effect;
   * rows are never cleared, recycled or grown after a failed observation. */
  for(i=0;i<bytes;i++)if(((const unsigned char *)rows)[i])return 0;
  c->started=1;c->rows=rows;c->capacity=capacity;c->receiver_original=receiver;
  c->prior_original=&receiver->parent.calls[6].after;
  c->previous_tick=c->prior_original->tick;c->expires=receiver->decode.transport.expires;
  c->entry_original=entry;c->stack_original=stack;
  if(!slcli_native_clock_read(&c->system_before)||c->system_before.tick<c->previous_tick||
     c->system_before.tick>=c->expires)goto failed;
  c->system_args[0]=(uintptr_t)&c->system;c->system_called=1;
  GetSystemInfo(&c->system);c->system_returned=1;
  if(!slcli_native_clock_read(&c->system_after)||c->system_after.tick<c->system_before.tick||
     c->system_after.tick>=c->expires||!c->system.dwPageSize||
     !c->system.dwAllocationGranularity||c->system.dwAllocationGranularity%c->system.dwPageSize)goto failed;
  c->minimum=(uintptr_t)c->system.lpMinimumApplicationAddress;
  c->maximum=(uintptr_t)c->system.lpMaximumApplicationAddress;
  if(!c->minimum||c->maximum<c->minimum||c->maximum==UINTPTR_MAX||
     receiver->decode.transport.frozen_owner.native_page_size!=c->system.dwPageSize)goto failed;
  c->cursor=c->minimum;c->previous_tick=c->system_after.tick;
  while(c->cursor<=c->maximum) {
    if(c->used==c->capacity)goto failed;
    r=&rows[c->used++];
    r->before_result=slcli_native_clock_read(&r->before)&&
      r->before.tick>=c->previous_tick&&r->before.tick<c->expires;
    if(!r->before_result)goto failed;
    r->args[0]=c->cursor;r->args[1]=(uintptr_t)&r->geometry;r->args[2]=sizeof(r->geometry);
    r->called=1;result=VirtualQuery((const void *)c->cursor,&r->geometry,sizeof(r->geometry));
    r->result=result;r->returned=1;
    if(!result){r->error=GetLastError();r->error_observed=1;}
    r->after_result=slcli_native_clock_read(&r->after)&&
      r->after.tick>=r->before.tick&&r->after.tick<c->expires;
    if(!r->after_result||result!=sizeof(r->geometry))goto failed;
    base=(uintptr_t)r->geometry.BaseAddress;
    if(base!=c->cursor||!r->geometry.RegionSize||r->geometry.RegionSize>UINTPTR_MAX-base||
       base%c->system.dwPageSize||r->geometry.RegionSize%c->system.dwPageSize)goto failed;
    end=base+r->geometry.RegionSize;
    if(end<=base||end>c->maximum+1u)goto failed;
    if(r->geometry.State==MEM_FREE)c->free_regions++;
    else if(r->geometry.State==MEM_COMMIT||r->geometry.State==MEM_RESERVE) {
      if(!r->geometry.AllocationBase||(uintptr_t)r->geometry.AllocationBase>base)goto failed;
      c->allocated_regions++;
      if((uintptr_t)entry>=base&&(uintptr_t)entry<end) {
        protection=r->geometry.Protect&0xffu;
        if(c->image_observed||r->geometry.State!=MEM_COMMIT||r->geometry.Type!=MEM_IMAGE||
          (r->geometry.Protect&(PAGE_NOACCESS|PAGE_GUARD))||
          (protection!=PAGE_EXECUTE&&protection!=PAGE_EXECUTE_READ&&
           protection!=PAGE_EXECUTE_READWRITE&&protection!=PAGE_EXECUTE_WRITECOPY))goto failed;
        c->image_original=r->geometry.AllocationBase;c->image_region_original=r;
        c->image_observed=1;
      }
      if((uintptr_t)stack>=base&&(uintptr_t)stack<end&&r->geometry.State==MEM_COMMIT&&
         r->geometry.Type==MEM_PRIVATE&&!(r->geometry.Protect&(PAGE_NOACCESS|PAGE_GUARD)))c->stack_observed=1;
      if(r->geometry.AllocationBase==receiver->mapping&&r->geometry.Type==MEM_MAPPED)c->ingress_observed=1;
      if(base==receiver->decode.transport.mapped_view.base) {
        const struct slcli_node_mapped_view_observation *m=&receiver->decode.transport.mapped_view;
        if(c->runtime_observed||r->geometry.AllocationBase!=(void *)(uintptr_t)m->base||
          r->geometry.RegionSize!=m->bytes||r->geometry.Protect!=m->protection||
          r->geometry.State!=m->state||r->geometry.Type!=m->type||
          r->before.tick<m->query_tick)goto failed;
        c->runtime_region_original=r;c->runtime_observed=1;
      }
    } else goto failed;
    c->cursor=end;c->previous_tick=r->after.tick;
  }
  if(!c->image_observed||!c->stack_observed||!c->ingress_observed||!c->runtime_observed)goto failed;
  c->complete_scan=1;c->terminal=1;return 1;
failed:c->terminal=1;return 0;
}
#endif
