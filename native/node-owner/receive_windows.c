#include "receive_windows.h"
#if defined(_WIN32)
static int before(struct slcli_node_receive_windows *c,size_t i) {
  c->before_result[i]=slcli_native_clock_read(&c->before[i])&&
    (!i||c->before[i].tick>=c->after[i-1].tick);
  return c->before_result[i];
}
static int after(struct slcli_node_receive_windows *c,size_t i) {
  c->after_result[i]=slcli_native_clock_read(&c->after[i])&&
    c->after[i].tick>=c->before[i].tick;
  return c->after_result[i];
}
int slcli_node_receive_windows_original(struct slcli_node_receive_windows *c,
  HANDLE transport,HANDLE parent,const void *entry) {
  BOOL ok;SIZE_T queried;size_t i;uintptr_t mapping,book;
  if(!c||!transport||transport==INVALID_HANDLE_VALUE||!parent||
     parent==INVALID_HANDLE_VALUE||transport==parent||!entry)return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  c->started=1;c->transport_original=transport;c->parent_original=parent;c->entry_original=entry;
  if(!before(c,0))goto failed;
  c->args[0][0]=(uintptr_t)transport;c->args[0][1]=(uintptr_t)&c->handle_flags;c->called[0]=1;
  ok=GetHandleInformation(transport,&c->handle_flags);c->results[0]=(uint32_t)ok;c->returned[0]=1;
  if(!ok){c->errors[0]=GetLastError();c->error_observed[0]=1;}
  if(!after(c,0)||!ok)goto failed;
  /* Flags are raw custody observations, not a granted-access or object check. */
  if(!before(c,1))goto failed;
  c->args[1][0]=(uintptr_t)transport;c->args[1][1]=FILE_MAP_READ;
  c->args[1][2]=0;c->args[1][3]=0;c->args[1][4]=sizeof(c->raw);c->called[1]=1;
  c->mapping=MapViewOfFile(transport,FILE_MAP_READ,0,0,sizeof(c->raw));
  c->results[1]=(uintptr_t)c->mapping;c->returned[1]=1;
  if(c->mapping)c->mapping_observed=1;else{c->errors[1]=GetLastError();c->error_observed[1]=1;}
  if(!after(c,1)||!c->mapping)goto failed;
  if(!before(c,2))goto failed;
  c->args[2][0]=(uintptr_t)c->mapping;c->args[2][1]=(uintptr_t)&c->geometry;
  c->args[2][2]=sizeof(c->geometry);c->called[2]=1;
  queried=VirtualQuery(c->mapping,&c->geometry,sizeof(c->geometry));
  c->results[2]=queried;c->returned[2]=1;
  if(!queried){c->errors[2]=GetLastError();c->error_observed[2]=1;}
  if(!after(c,2)||queried!=sizeof(c->geometry)||c->geometry.BaseAddress!=c->mapping||
     c->geometry.AllocationBase!=c->mapping||c->geometry.RegionSize<sizeof(c->raw)||
     c->geometry.State!=MEM_COMMIT||c->geometry.Type!=MEM_MAPPED||
     c->geometry.Protect!=PAGE_READONLY)goto failed;
  mapping=(uintptr_t)c->mapping;book=(uintptr_t)c;
  if(mapping>UINTPTR_MAX-sizeof(c->raw)||book>UINTPTR_MAX-sizeof(*c)||
     (mapping<book+sizeof(*c)&&book<mapping+sizeof(c->raw)))goto failed;
  if(!slcli_native_clock_read(&c->copy_before)||c->copy_before.tick<c->after[2].tick)goto failed;
  for(i=0;i<sizeof(c->raw);i++){c->raw[i]=((const unsigned char *)c->mapping)[i];c->copied=i+1;}
  c->copy_returned=1;
  if(!slcli_native_clock_read(&c->copy_after)||c->copy_after.tick<c->copy_before.tick)goto failed;
  c->decode_result=slcli_node_bootstrap_transport_decode(&c->decode,c->raw,sizeof(c->raw));
  c->decode_returned=1;
  if(!c->decode_result||c->before[0].tick<c->decode.transport.origin.tick||
     c->before[0].tick<c->decode.transport.mapped_view.query_tick||
     c->copy_after.tick>=c->decode.transport.expires)goto failed;
  c->parent_result=slcli_node_parent_windows_observe(&c->parent,&c->decode,parent,&c->copy_after);
  c->parent_returned=1;
  if(!c->parent_result)goto failed;
  /* Readonly mapping and inherited handles stay resident through independently
   * observed complete child/tree closure. No local FREE/close of parent aliases. */
  c->custody_returned=1;c->terminal=1;return 1;
failed:c->terminal=1;return 0;
}
#endif
