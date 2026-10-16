#include "transport_windows.h"
#if defined(_WIN32)
size_t slcli_node_transport_windows_control_bytes(size_t descriptor_bytes) {
  size_t book=slcli_arena_allocation_bytes(sizeof(struct slcli_node_transport_windows));
  size_t descriptor=slcli_arena_allocation_bytes(descriptor_bytes);
  size_t encoded=slcli_arena_allocation_bytes(SLCLI_BOOTSTRAP_TRANSPORT_BYTES);
  size_t readback=slcli_arena_allocation_bytes(SLCLI_BOOTSTRAP_TRANSPORT_BYTES);
  if(!descriptor_bytes||!book||!descriptor||!encoded||!readback||
     book>SIZE_MAX-descriptor||book+descriptor>SIZE_MAX-encoded||
     book+descriptor+encoded>SIZE_MAX-readback)return 0;
  return book+descriptor+encoded+readback;
}
static int apart(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int same(struct slcli_node_transport_windows *c,struct slcli_arena *a) {
  return slcli_source_deadline_original_matches(a,c->deadline,c->caller)&&
    c->deadline->origin==c->origin&&c->origin->tick==c->origin_tick&&
    c->deadline->expires==c->expires&&c->predecessor->complete&&
    c->predecessor->tick==c->predecessor_tick;
}
static int before(struct slcli_node_transport_windows *c,struct slcli_arena *a,size_t i) {
  struct slcli_node_transport_windows_call *r=&c->calls[i];
  uint64_t previous=i?c->calls[i-1].after.tick:c->constructor[3].tick;
  if(i==5)previous=c->publication[2].tick;
  if(i==7)previous=c->calls[4].after.tick;
  if(r->called||r->returned||!slcli_native_clock_read(&r->before)||!same(c,a)||
     r->before.tick<previous||r->before.tick>=c->expires)return 0;
  r->before_result=1;return 1;
}
static int after(struct slcli_node_transport_windows *c,struct slcli_arena *a,size_t i) {
  struct slcli_node_transport_windows_call *r=&c->calls[i];
  r->after_result=r->returned&&slcli_native_clock_read(&r->after)&&same(c,a)&&
    r->after.tick>=r->before.tick&&r->after.tick<c->expires;
  return r->after_result;
}
/* Cleanup records the original clock failure/expiry but does not use useful
 * operation timing as permission to retain an independently closable handle. */
static int before_writer_close(struct slcli_node_transport_windows *c) {
  struct slcli_node_transport_windows_call *r=&c->calls[6];
  uint64_t previous=c->calls[5].before.tick;
  if(c->calls[5].after.complete&&c->calls[5].after.tick>previous)
    previous=c->calls[5].after.tick;
  if(r->called||r->returned||!c->calls[1].returned||
     c->calls[1].result!=(uintptr_t)c->writable||!c->writable||
     c->writable==INVALID_HANDLE_VALUE||!c->calls[5].returned)return 0;
  r->before_result=slcli_native_clock_read(&r->before)&&
    r->before.tick>=previous&&r->before.tick<c->expires;
  return 1;
}
static int dispose_parent_token(struct slcli_node_transport_windows *c) {
  struct slcli_node_transport_windows_call *r=&c->calls[13];BOOL closed;
  if(c->parent_token_disposal_attempted)return c->parent_token_closed;
  c->parent_token_disposal_attempted=1;
  if((c->calls[10].called&&!c->calls[10].returned)||
     (c->calls[11].called&&!c->calls[11].returned)||
     (c->calls[12].called&&!c->calls[12].returned))goto pending;
  if(!c->calls[10].returned||!c->calls[10].result)return 1;
  if(!c->parent_token||c->parent_token==INVALID_HANDLE_VALUE||
     c->parent_token!=c->parent_token_raw)goto pending;
  r->before_result=slcli_native_clock_read(&r->before);
  r->args[0]=(uintptr_t)c->parent_token;r->called=1;
  closed=CloseHandle(c->parent_token);r->result=(uint32_t)closed;r->returned=1;
  if(closed)c->parent_token_closed=1;else{r->error=GetLastError();r->error_observed=1;}
  r->after_result=slcli_native_clock_read(&r->after);
  if(!closed)goto pending;
  return 1;
pending:c->parent_token_disposal_pending=1;return 0;
}
int slcli_node_transport_windows_create(struct slcli_node_transport_windows *c,
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,
  size_t caller_bytes,const struct slcli_native_clock *prior,const void *descriptor,size_t descriptor_bytes) {
  size_t i;uint64_t page,remainder;struct slcli_node_transport_windows_call *r;
  uintptr_t sid,begin,end;size_t sid_bytes;
  HANDLE handle;void *view;SIZE_T query;BOOL duplicated;
  if(!c||!a||!slcli_memory_owner_mutable(a->owner)||!caller_bytes||!descriptor_bytes||!slcli_arena_live_block(a,c,sizeof(*c))||
     !slcli_arena_live_block(a,caller,caller_bytes)||!slcli_source_deadline_original_matches(a,d,caller)||
     !prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
     prior->tick<d->origin->tick||prior->tick>=d->expires||
     !slcli_arena_live_range(a,descriptor,descriptor_bytes)||
     !apart(c,sizeof(*c),caller,caller_bytes)||!apart(c,sizeof(*c),prior,sizeof(*prior))||
     !apart(c,sizeof(*c),descriptor,descriptor_bytes)||!slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  c->started=1;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
  c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
  c->predecessor=prior;c->predecessor_tick=prior->tick;
  c->descriptor_original=descriptor;c->descriptor_bytes=descriptor_bytes;
  if(!slcli_native_clock_read(&c->constructor[0])||!same(c,a)||
     c->constructor[0].tick<prior->tick||c->constructor[0].tick>=c->expires)goto failed;
  c->descriptor=slcli_arena_alloc(a,descriptor_bytes);if(!c->descriptor)goto failed;
  for(i=0;i<descriptor_bytes;i++)((unsigned char *)c->descriptor)[i]=((const unsigned char *)descriptor)[i];
  if(!slcli_native_clock_read(&c->constructor[1])||!same(c,a)||
     c->constructor[1].tick<c->constructor[0].tick||c->constructor[1].tick>=c->expires)goto failed;
  c->encoded=(unsigned char *)slcli_arena_calloc(a,1,SLCLI_BOOTSTRAP_TRANSPORT_BYTES);
  if(!c->encoded)goto failed;
  if(!slcli_native_clock_read(&c->constructor[2])||!same(c,a)||
     c->constructor[2].tick<c->constructor[1].tick||c->constructor[2].tick>=c->expires)goto failed;
  c->readback=(unsigned char *)slcli_arena_calloc(a,1,SLCLI_BOOTSTRAP_TRANSPORT_BYTES);
  if(!c->readback)goto failed;
  page=a->owner->native_page_size;
  if(!page||SLCLI_BOOTSTRAP_TRANSPORT_BYTES>UINT64_MAX-page+1)goto failed;
  remainder=SLCLI_BOOTSTRAP_TRANSPORT_BYTES%page;
  c->physical_bytes=remainder?SLCLI_BOOTSTRAP_TRANSPORT_BYTES+page-remainder:SLCLI_BOOTSTRAP_TRANSPORT_BYTES;
  if(!slcli_native_clock_read(&c->constructor[3])||!same(c,a)||
     c->constructor[3].tick<c->constructor[2].tick||c->constructor[3].tick>=c->expires)goto failed;
  c->prepare_result=slcli_memory_prepare(a->owner,SLCLI_IO,c->physical_bytes,&c->request);
  c->prepare_returned=1;if(!c->prepare_result)goto failed;
  c->security.nLength=sizeof(c->security);c->security.lpSecurityDescriptor=c->descriptor;
  /* Original writable section handle is NOT inherited; only explicit readonly
   * DuplicateHandle output is admitted to the native launcher roster. */
  c->security.bInheritHandle=FALSE;
  if(!before(c,a,0))goto failed;
  r=&c->calls[0];r->called=1;handle=GetCurrentProcess();r->result=(uintptr_t)handle;r->returned=1;
  c->current_process=handle;if(!after(c,a,0)||!handle)goto failed;
  if(!before(c,a,1))goto failed;
  r=&c->calls[1];r->args[0]=(uintptr_t)INVALID_HANDLE_VALUE;r->args[1]=(uintptr_t)&c->security;
  r->args[2]=PAGE_READWRITE;r->args[4]=SLCLI_BOOTSTRAP_TRANSPORT_BYTES;r->called=1;
  handle=CreateFileMappingW(INVALID_HANDLE_VALUE,&c->security,PAGE_READWRITE,0,SLCLI_BOOTSTRAP_TRANSPORT_BYTES,NULL);
  r->result=(uintptr_t)handle;r->returned=1;c->writable=handle;
  if(handle){c->backing_result=slcli_memory_backing_acquired(a->owner,&c->request);c->backing_returned=1;}
  else{r->error=GetLastError();r->error_observed=1;}
  if(!after(c,a,1)||!handle||!c->backing_returned||!c->backing_result)goto failed;
  if(!before(c,a,2))goto failed;
  r=&c->calls[2];r->args[0]=(uintptr_t)c->writable;r->args[1]=FILE_MAP_WRITE;
  r->args[4]=SLCLI_BOOTSTRAP_TRANSPORT_BYTES;r->called=1;
  view=MapViewOfFile(c->writable,FILE_MAP_WRITE,0,0,SLCLI_BOOTSTRAP_TRANSPORT_BYTES);
  r->result=(uintptr_t)view;r->returned=1;c->writer_view=view;
  if(view){c->partial_result=slcli_memory_backing_mapped(a->owner,&c->request,(uintptr_t)view);c->partial_returned=1;}
  else{r->error=GetLastError();r->error_observed=1;}
  if(!after(c,a,2)||!view||!c->partial_returned||!c->partial_result)goto failed;
  if(!before(c,a,3))goto failed;
  r=&c->calls[3];r->args[0]=(uintptr_t)view;r->args[1]=(uintptr_t)&c->geometry;
  r->args[2]=sizeof(c->geometry);r->called=1;
  query=VirtualQuery(view,&c->geometry,sizeof(c->geometry));r->result=query;r->returned=1;
  if(!query){r->error=GetLastError();r->error_observed=1;}
  if(!after(c,a,3)||query!=sizeof(c->geometry)||c->geometry.BaseAddress!=view||
     c->geometry.AllocationBase!=view||c->geometry.RegionSize!=c->physical_bytes||
     c->geometry.State!=MEM_COMMIT||c->geometry.Type!=MEM_MAPPED||
     (c->geometry.Protect&0xff)!=PAGE_READWRITE)goto failed;
  c->acquired_result=slcli_memory_acquired(a->owner,&c->request,(uintptr_t)view,c->physical_bytes,0,0);
  c->acquired_returned=1;if(!c->acquired_result)goto failed;
  if(!before(c,a,4))goto failed;
  r=&c->calls[4];r->args[0]=(uintptr_t)c->current_process;r->args[1]=(uintptr_t)c->writable;
  r->args[2]=(uintptr_t)c->current_process;r->args[3]=(uintptr_t)&c->readonly_raw;
  r->args[4]=FILE_MAP_READ;r->args[5]=TRUE;r->called=1;
  duplicated=DuplicateHandle(c->current_process,c->writable,c->current_process,&c->readonly_raw,FILE_MAP_READ,TRUE,0);
  r->result=(uint32_t)duplicated;r->returned=1;
  if(duplicated)c->readonly=c->readonly_raw;else{r->error=GetLastError();r->error_observed=1;}
  if(!after(c,a,4)||!duplicated||!c->readonly||c->readonly==INVALID_HANDLE_VALUE||c->readonly==c->writable)goto failed;
  if(!before(c,a,7))goto failed;
  r=&c->calls[7];r->args[0]=(uintptr_t)c->current_process;
  r->args[1]=(uintptr_t)c->current_process;r->args[2]=(uintptr_t)c->current_process;
  r->args[3]=(uintptr_t)&c->parent_raw;
  r->args[4]=PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE;r->args[5]=TRUE;r->called=1;
  duplicated=DuplicateHandle(c->current_process,c->current_process,c->current_process,
    &c->parent_raw,PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,TRUE,0);
  r->result=(uint32_t)duplicated;r->returned=1;
  if(duplicated)c->parent=c->parent_raw;else{r->error=GetLastError();r->error_observed=1;}
  if(!after(c,a,7)||!duplicated||!c->parent||c->parent==INVALID_HANDLE_VALUE||
     c->parent==c->writable||c->parent==c->readonly)goto failed;
  if(!before(c,a,8))goto failed;
  r=&c->calls[8];r->args[0]=(uintptr_t)c->parent;r->called=1;
  c->parent_pid=GetProcessId(c->parent);r->result=c->parent_pid;r->returned=1;
  if(!c->parent_pid){r->error=GetLastError();r->error_observed=1;}
  if(!after(c,a,8)||!c->parent_pid)goto failed;
  if(!before(c,a,9))goto failed;
  r=&c->calls[9];r->args[0]=(uintptr_t)c->parent;
  r->args[1]=(uintptr_t)&c->parent_creation;r->args[2]=(uintptr_t)&c->parent_exit;
  r->args[3]=(uintptr_t)&c->parent_kernel;r->args[4]=(uintptr_t)&c->parent_user;r->called=1;
  duplicated=GetProcessTimes(c->parent,&c->parent_creation,&c->parent_exit,
    &c->parent_kernel,&c->parent_user);r->result=(uint32_t)duplicated;r->returned=1;
  if(!duplicated){r->error=GetLastError();r->error_observed=1;}
  if(!after(c,a,9)||!duplicated)goto failed;
  if(!before(c,a,10))goto failed;
  r=&c->calls[10];r->args[0]=(uintptr_t)c->parent;r->args[1]=TOKEN_QUERY;
  r->args[2]=(uintptr_t)&c->parent_token_raw;r->called=1;
  duplicated=OpenProcessToken(c->parent,TOKEN_QUERY,&c->parent_token_raw);
  r->result=(uint32_t)duplicated;r->returned=1;
  if(duplicated)c->parent_token=c->parent_token_raw;else{r->error=GetLastError();r->error_observed=1;}
  if(!after(c,a,10)||!duplicated||!c->parent_token||c->parent_token==INVALID_HANDLE_VALUE)goto failed;
  if(!before(c,a,11))goto failed;
  r=&c->calls[11];r->args[0]=(uintptr_t)c->parent_token;r->args[1]=TokenUser;
  r->args[2]=(uintptr_t)c->parent_user_buffer.bytes;r->args[3]=sizeof(c->parent_user_buffer.bytes);
  r->args[4]=(uintptr_t)&c->parent_user_bytes;r->called=1;
  duplicated=GetTokenInformation(c->parent_token,TokenUser,c->parent_user_buffer.bytes,
    sizeof(c->parent_user_buffer.bytes),&c->parent_user_bytes);
  r->result=(uint32_t)duplicated;r->returned=1;
  if(!duplicated){r->error=GetLastError();r->error_observed=1;}
  if(!after(c,a,11)||!duplicated||c->parent_user_bytes<sizeof(TOKEN_USER)||
     c->parent_user_bytes>sizeof(c->parent_user_buffer.bytes))goto failed;
  begin=(uintptr_t)c->parent_user_buffer.bytes;
  if(begin>UINTPTR_MAX-c->parent_user_bytes)goto failed;
  end=begin+c->parent_user_bytes;
  sid=(uintptr_t)((const TOKEN_USER *)(const void *)c->parent_user_buffer.bytes)->User.Sid;
  if(sid<begin+sizeof(TOKEN_USER)||sid>end||end-sid<8||((const unsigned char *)sid)[0]!=SID_REVISION||
     ((const unsigned char *)sid)[1]>SID_MAX_SUB_AUTHORITIES)goto failed;
  sid_bytes=8u+4u*((const unsigned char *)sid)[1];
  if(sid_bytes>end-sid||sid_bytes>sizeof(c->parent_sid))goto failed;
  for(i=0;i<sid_bytes;i++)c->parent_sid[i]=((const unsigned char *)sid)[i];
  c->parent_sid_bytes=(uint32_t)sid_bytes;
  if(!before(c,a,12))goto failed;
  r=&c->calls[12];r->args[0]=(uintptr_t)c->parent_token;r->args[1]=TokenStatistics;
  r->args[2]=(uintptr_t)&c->parent_statistics;r->args[3]=sizeof(c->parent_statistics);
  r->args[4]=(uintptr_t)&c->parent_statistics_bytes;r->called=1;
  duplicated=GetTokenInformation(c->parent_token,TokenStatistics,&c->parent_statistics,
    sizeof(c->parent_statistics),&c->parent_statistics_bytes);
  r->result=(uint32_t)duplicated;r->returned=1;
  if(!duplicated){r->error=GetLastError();r->error_observed=1;}
  if(!after(c,a,12)||!duplicated||c->parent_statistics_bytes!=sizeof(c->parent_statistics))goto failed;
  if(!dispose_parent_token(c)||!same(c,a)||!c->calls[13].before_result||!c->calls[13].after_result||
     c->calls[13].before.tick<c->calls[12].after.tick||
     c->calls[13].after.tick<c->calls[13].before.tick||c->calls[13].after.tick>=c->expires)goto failed;
  c->object_prepared=1;return 1;
failed:
  c->terminal=1;
  (void)dispose_parent_token(c);
  (void)slcli_node_transport_windows_dispose_writer(c,a);
  return 0;
}
int slcli_node_transport_windows_publish(struct slcli_node_transport_windows *c,
  struct slcli_arena *a,const struct slcli_native_clock *prior,
  const struct slcli_node_bootstrap_transport *t) {
  size_t i;BOOL unmapped,closed;int unmap_after;
  struct slcli_node_transport_windows_call *r;
  if(!c||!a||!slcli_memory_owner_mutable(a->owner)||!t||!prior||!slcli_arena_live_block(a,c,sizeof(*c))||
     !c->started||!c->object_prepared||c->publication_attempted||c->terminal||!same(c,a)||
     !slcli_arena_live_range(a,t,sizeof(*t))||!slcli_arena_live_range(a,prior,sizeof(*prior))||
     !prior->complete||prior->tick<c->calls[13].after.tick||prior->tick>=c->expires||
     !apart(c,sizeof(*c),t,sizeof(*t))||!apart(c,sizeof(*c),prior,sizeof(*prior))||
     !slcli_arena_live_block(a,c->encoded,SLCLI_BOOTSTRAP_TRANSPORT_BYTES)||
     !slcli_arena_live_block(a,c->readback,SLCLI_BOOTSTRAP_TRANSPORT_BYTES)||
     c->calls[1].result!=(uintptr_t)c->writable||c->calls[2].result!=(uintptr_t)c->writer_view||
     !c->calls[4].result||c->readonly!=c->readonly_raw)return 0;
  c->publication_attempted=1;c->transport_original=t;
  if(t->expires!=c->expires)goto failed;
  /* Publication consumes only this constructor's copied parent observations.
   * This local correspondence does not authenticate their external foundation. */
  if(!c->parent_token_closed||c->parent_token_disposal_pending||
     t->capabilities[3]!=(uintptr_t)c->parent||t->parent_pid!=c->parent_pid||
     t->parent_creation[0]!=c->parent_creation.dwLowDateTime||
     t->parent_creation[1]!=c->parent_creation.dwHighDateTime||
     t->parent_sid_bytes!=c->parent_sid_bytes||
     t->parent_token_id[0]!=c->parent_statistics.TokenId.LowPart||
     t->parent_token_id[1]!=(uint32_t)c->parent_statistics.TokenId.HighPart||
     t->parent_authentication_id[0]!=c->parent_statistics.AuthenticationId.LowPart||
     t->parent_authentication_id[1]!=(uint32_t)c->parent_statistics.AuthenticationId.HighPart)goto failed;
  for(i=0;i<SECURITY_MAX_SID_SIZE;i++)if(t->parent_sid[i]!=c->parent_sid[i])goto failed;
  for(i=0;i<2;i++)if(t->origin.args[i]!=c->origin->args[i]||
    t->origin.raw[i]!=c->origin->raw[i]||t->origin.status[i]!=c->origin->status[i])goto failed;
  if(t->origin.tick!=c->origin->tick||t->origin.started!=c->origin->started||
     t->origin.observed!=c->origin->observed||t->origin.complete!=c->origin->complete||
     t->origin.calls_called!=c->origin->calls_called||t->origin.calls_returned!=c->origin->calls_returned)goto failed;
  if(!slcli_native_clock_read(&c->publication[0])||!same(c,a)||
     c->publication[0].tick<prior->tick||c->publication[0].tick>=c->expires)goto failed;
  c->encode_result=slcli_node_bootstrap_transport_encode(c->encoded,SLCLI_BOOTSTRAP_TRANSPORT_BYTES,t);
  c->encode_returned=1;if(!c->encode_result)goto failed;
  if(!slcli_native_clock_read(&c->publication[1])||!same(c,a)||
     c->publication[1].tick<c->publication[0].tick||c->publication[1].tick>=c->expires)goto failed;
  for(i=0;i<SLCLI_BOOTSTRAP_TRANSPORT_BYTES;i++)((unsigned char *)c->writer_view)[i]=c->encoded[i];
  for(i=0;i<SLCLI_BOOTSTRAP_TRANSPORT_BYTES;i++)c->readback[i]=((const unsigned char *)c->writer_view)[i];
  for(i=0;i<SLCLI_BOOTSTRAP_TRANSPORT_BYTES;i++) {
    c->compared=i+1;if(c->readback[i]!=c->encoded[i])goto failed;
  }
  c->bytes_equal=1;
  if(!slcli_native_clock_read(&c->publication[2])||!same(c,a)||
     c->publication[2].tick<c->publication[1].tick||c->publication[2].tick>=c->expires)goto failed;
  if(!before(c,a,5))goto failed;
  c->release_prepare_result=slcli_memory_prepare_release(a->owner,c->request.index,
    &c->release_request,&c->release_related);
  c->release_prepare_returned=1;if(!c->release_prepare_result)goto failed;
  r=&c->calls[5];r->args[0]=(uintptr_t)c->writer_view;r->called=1;
  unmapped=UnmapViewOfFile(c->writer_view);r->result=(uint32_t)unmapped;r->returned=1;
  if(unmapped) {
    c->writer_unmapped=1;
    c->view_transition_result=slcli_memory_backing_view_unmapped(a->owner,&c->release_request,
      c->release_related,(uintptr_t)c->writer_view,c->physical_bytes);
    c->view_transition_returned=1;
  } else {r->error=GetLastError();r->error_observed=1;}
  unmap_after=after(c,a,5);
  /* The known-returned view operation has no outstanding handle borrower.
   * Independently attempt the original writable handle close, even when view
   * qualification failed. No repeat of either original operation is allowed. */
  if(!before_writer_close(c))goto failed;
  r=&c->calls[6];r->args[0]=(uintptr_t)c->writable;r->called=1;
  closed=CloseHandle(c->writable);r->result=(uint32_t)closed;r->returned=1;
  if(closed)c->writable_closed=1;else{r->error=GetLastError();r->error_observed=1;}
  if(!after(c,a,6)||!r->before_result||!closed||!unmap_after||!unmapped||!c->view_transition_returned||
     !c->view_transition_result)goto failed;
  c->returned=1;c->terminal=1;return 1;
failed:
  c->terminal=1;
  (void)slcli_node_transport_windows_dispose_writer(c,a);
  return 0;
}
int slcli_node_transport_windows_dispose_writer(struct slcli_node_transport_windows *c,
  struct slcli_arena *a) {
  size_t i;BOOL result;struct slcli_node_transport_windows_call *r;
  if(!c||!a||!slcli_memory_owner_mutable(a->owner)||!slcli_arena_live_block(a,c,sizeof(*c))||!c->started||
     !c->terminal||c->writer_disposal_attempted)return 0;
  c->writer_disposal_attempted=1;
  /* A native call entered without return retains its original possible borrow.
   * No slot is retried, including failed known-returned cleanup calls. */
  for(i=0;i<7;i++)if(c->calls[i].called&&!c->calls[i].returned) {
    c->writer_disposal_pending=1;goto terminal;
  }
  if(c->calls[2].returned&&c->calls[2].result&&!c->calls[5].called) {
    if(c->calls[2].result!=(uintptr_t)c->writer_view) {
      c->writer_disposal_pending=1;goto terminal;
    }
    r=&c->calls[5];
    if(!r->before.started)r->before_result=slcli_native_clock_read(&r->before);
    if(c->acquired_returned&&c->acquired_result&&!c->release_prepare_returned) {
      c->release_prepare_result=slcli_memory_prepare_release(a->owner,c->request.index,
        &c->release_request,&c->release_related);
      c->release_prepare_returned=1;
    }
    r->args[0]=(uintptr_t)c->writer_view;r->called=1;
    result=UnmapViewOfFile(c->writer_view);r->result=(uint32_t)result;r->returned=1;
    if(result) {
      c->writer_unmapped=1;
      if(c->release_prepare_returned&&c->release_prepare_result&&!c->view_transition_returned) {
        c->view_transition_result=slcli_memory_backing_view_unmapped(a->owner,
          &c->release_request,c->release_related,(uintptr_t)c->writer_view,c->physical_bytes);
        c->view_transition_returned=1;
      }
    } else {r->error=GetLastError();r->error_observed=1;}
    r->after_result=slcli_native_clock_read(&r->after);
  }
  /* MapViewOfFile's returned mapping no longer borrows the section HANDLE.
   * Even failed known-returned unmap cannot prevent this independent close. */
  if(c->calls[1].returned&&c->calls[1].result&&!c->calls[6].called) {
    if(c->calls[1].result!=(uintptr_t)c->writable||c->writable==INVALID_HANDLE_VALUE) {
      c->writer_disposal_pending=1;goto terminal;
    }
    r=&c->calls[6];
    if(!r->before.started)r->before_result=slcli_native_clock_read(&r->before);
    r->args[0]=(uintptr_t)c->writable;r->called=1;
    result=CloseHandle(c->writable);r->result=(uint32_t)result;r->returned=1;
    if(result)c->writable_closed=1;else{r->error=GetLastError();r->error_observed=1;}
    r->after_result=slcli_native_clock_read(&r->after);
  }
  if((c->calls[2].result&&!c->writer_unmapped)||
     (c->calls[1].result&&!c->writable_closed))c->writer_disposal_pending=1;
terminal:
  c->writer_disposal_terminal=1;return !c->writer_disposal_pending;
}
#endif
