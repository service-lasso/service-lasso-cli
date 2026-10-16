#include "launcher_native.h"
#if defined(_WIN32)
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static void bytes_copy(void *to,const void *from,size_t n) {
  size_t i;for(i=0;i<n;i++)((unsigned char *)to)[i]=((const unsigned char *)from)[i];
}
static int text_original(struct slcli_arena *a,const WCHAR *p,size_t n,int environment) {
  size_t i,count;
  if(!p||n<sizeof(WCHAR)*(environment?2:1)||n%sizeof(WCHAR)||
     !slcli_arena_live_range(a,p,n))return 0;
  count=n/sizeof(WCHAR);
  if(p[count-1]||(environment&&p[count-2]))return 0;
  if(!environment)for(i=0;i+1<count;i++)if(!p[i])return 0;
  return 1;
}
static int target_lineage(const struct slcli_launcher_windows_capture *c) {
  const struct slcli_memory_owner *o=c->target_owner_original;size_t i;
  const struct slcli_preentry_backing_windows *b=c->runtime_backing_original;
  const struct slcli_runtime_containing_windows *root=c->containing_original;
  if(!root||!b||!root->capture_prepared||root->first_failure||root->backing!=b||
     root->runtime_owner_original!=o||root->deadline!=c->deadline||root->caller!=c->caller||
     !root->layout||!root->initializer||!root->alias||
     !root->initializer->initialized||!root->initializer->unmapped||
     !root->alias->qualified||!root->alias->view||root->alias->unmap_attempted||
     !root->validate_returned||!root->validate_result||!root->alias->validated||
     !root->route_prepare_returned||!root->route_prepare_result||!root->route_writer.prepared||
     root->alias->capture_bytes!=root->layout->capture_bytes||
     root->layout->threads!=root->original.threads||root->layout->events!=root->original.events||
     root->layout->regions!=root->original.regions||root->layout->modules!=root->original.modules||
     root->layout->name_units!=root->original.name_units||root->original.runtime_bytes!=b->bytes||
     root->original.threads!=c->original.thread_capture_attempts||
     root->original.events!=c->original.allocator_capacity)return 0;
  if(root->route_writer.source!=root->original.source_file||!root->original.source_file||
    root->original.source_file->path_bytes!=c->original.application_bytes)return 0;
  for(i=0;i<c->original.application_bytes;i++)if(((const unsigned char *)root->original.source_file->path)[i]!=
    ((const unsigned char *)c->original.application)[i])return 0;
  if(c->route_encode_returned) {
    const struct slcli_node_route_windows *r=&c->route_original;
    unsigned char bytes[SLCLI_NODE_ROUTE_CELL_BYTES];
    if(!c->route_encode_result||r->transport!=c->transport_handle_original||
      !c->transport_original||r->parent!=c->transport_original->parent||
      r->base!=root->alias->view||r->bytes!=root->original.runtime_bytes||
      r->capture_bytes!=root->layout->capture_bytes||r->threads!=root->layout->threads||
      r->events!=root->layout->events||r->regions!=root->layout->regions||
      r->modules!=root->layout->modules||r->name_units!=root->layout->name_units||
      r->page_bytes!=root->layout->geometry.dwPageSize||
      r->granularity!=root->layout->geometry.dwAllocationGranularity||
      !slcli_node_route_windows_encode(bytes,sizeof(bytes),r))return 0;
    for(i=0;i<sizeof(bytes);i++)if(bytes[i]!=c->route_bytes[i])return 0;
  }
  if(!o||(!slcli_memory_owner_prebirth_shape(o)&&!slcli_memory_owner_frozen(o))||
     o->charged!=c->target_charged_original||o->count!=c->target_count_original||
     o->next_sequence!=c->target_next_sequence_original)return 0;
  for(i=0;i<32;i++)if(o->original_birth[i]!=c->target_associations_original[0][i]||
    o->original_image[i]!=c->target_associations_original[1][i]||
    o->original_owner[i]!=c->target_associations_original[2][i]||
    o->original_source[i]!=c->target_associations_original[3][i])return 0;
  if(!b||b->physical_owner!=o||!b->section_only||b->mapped||!b->qualified||
     b->operation!=1||!b->backing_returned||!b->backing_result||
     !b->backing||b->backing==INVALID_HANDLE_VALUE||!b->duplicate_qualified||
     !b->duplicate_returned||!b->duplicate_status||!b->duplicate||
     b->duplicate==INVALID_HANDLE_VALUE||b->duplicate==b->backing||
     b->request.index>=o->count||!b->request.sequence||
     o->extents[b->request.index].state!=SLCLI_EXTENT_BACKING_ACQUIRED||
     o->extents[b->request.index].acquisition!=b->request.sequence||
     o->extents[b->request.index].reserved!=b->bytes)return 0;
  return 1;
}
static int same(struct slcli_launcher_windows_capture *c,struct slcli_arena *a) {
  return target_lineage(c)&&slcli_source_deadline_original_matches(a,c->deadline,c->caller)&&
    c->deadline->origin==c->origin&&c->origin->tick==c->origin_tick&&
    c->deadline->expires==c->expires&&c->predecessor->complete&&
    c->predecessor->tick==c->predecessor_tick;
}
static WCHAR *text_copy(struct slcli_arena *a,const WCHAR *p,size_t n) {
  WCHAR *r=(WCHAR *)slcli_arena_alloc(a,n);if(r)bytes_copy(r,p,n);return r;
}
static int selector_environment(struct slcli_launcher_windows_capture *c,struct slcli_arena *a) {
  static const WCHAR name[]=SLCLI_NODE_SELECTOR_NAME;
  const WCHAR *source=c->original.environment;
  size_t count=c->original.environment_bytes/sizeof(WCHAR),i=0,j,start,insert,total;
  if(count<2)return 0;
  /* Reject every case-insensitive spelling of the reserved original selector;
   * do not replace caller data or let Windows choose one of duplicate names. */
  while(i+1<count&&source[i]) {
    start=i;while(i<count&&source[i])i++;
    if(i>=count-1)return 0;
    for(j=0;j+1<sizeof(name)/sizeof(*name)&&start+j<i;j++) {
      WCHAR ch=source[start+j];if(ch>=L'a'&&ch<=L'z')ch-=L'a'-L'A';
      if(ch!=name[j])break;
    }
    if(j+1==sizeof(name)/sizeof(*name)&&start+j<i&&source[start+j]==L'=')return 0;
    i++;
  }
  if(source[0]){if(i!=count-1)return 0;insert=count-1;}
  else{if(count!=2)return 0;insert=0;}
  if(insert>SIZE_MAX-SLCLI_NODE_SELECTOR_ENTRY_CHARS-1u)return 0;
  total=insert+SLCLI_NODE_SELECTOR_ENTRY_CHARS+1u;
  if(total>SIZE_MAX/sizeof(WCHAR))return 0;
  c->selector_encoded=slcli_node_selector_windows_encode(c->bootstrap_selector,
    SLCLI_NODE_SELECTOR_ENTRY_CHARS,c->transport_handle_original,c->transport_original->parent,
    c->containing_original->alias->view,c->containing_original->original.runtime_bytes);
  if(!c->selector_encoded)return 0;
  c->environment_constructed_bytes=total*sizeof(WCHAR);
  c->environment=(WCHAR *)slcli_arena_calloc(a,total,sizeof(WCHAR));
  if(!c->environment)return 0;
  for(j=0;j<insert;j++)c->environment[j]=source[j];
  for(j=0;j<SLCLI_NODE_SELECTOR_ENTRY_CHARS;j++)c->environment[insert+j]=c->bootstrap_selector[j];
  c->selector_environment_returned=1;return 1;
}
/* Existing exclusive launcher ownership closes future publication before
 * independently disposing its writer. Child/readonly aliases stay retained. */
static int failed_transport(struct slcli_launcher_windows_capture *c,struct slcli_arena *a) {
  struct slcli_node_transport_windows *t=c->transport_original;
  if(t&&slcli_arena_live_block(a,t,sizeof(*t))) {
    t->terminal=1;
    if(!t->writer_disposal_attempted)(void)slcli_node_transport_windows_dispose_writer(t,a);
  }
  return 0;
}
int slcli_launcher_windows_create(struct slcli_launcher_windows_capture *c,
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,
  size_t caller_bytes,const struct slcli_native_clock *prior,const struct slcli_launcher_windows_inputs *in,const struct slcli_memory_owner *target,
  const struct slcli_preentry_backing_windows *runtime) {
  size_t i,handles_bytes;
  if(!c||!a||!in||!slcli_arena_live_block(a,c,sizeof(*c))||
     !slcli_source_deadline_original_matches(a,d,caller)||
     !target||target==a->owner||!slcli_arena_live_block(a,target,sizeof(*target))||
     !slcli_memory_owner_prebirth_shape(target)||
     !runtime||!slcli_arena_live_block(a,runtime,sizeof(*runtime))||
     !separate(c,sizeof(*c),runtime,sizeof(*runtime))||
     runtime->deadline!=d||runtime->caller!=caller||runtime->origin!=d->origin||
     runtime->expires!=d->expires||
     !separate(c,sizeof(*c),target,sizeof(*target))||
     !slcli_arena_live_range(a,in,sizeof(*in))||!caller_bytes||
     !in->containing||!slcli_arena_live_block(a,in->containing,sizeof(*in->containing))||
     !separate(c,sizeof(*c),in->containing,sizeof(*in->containing))||
     !slcli_arena_live_block(a,in->containing->layout,sizeof(*in->containing->layout))||
     !slcli_arena_live_block(a,in->containing->initializer,sizeof(*in->containing->initializer))||
     !slcli_arena_live_block(a,in->containing->alias,sizeof(*in->containing->alias))||
     !slcli_arena_live_block(a,caller,caller_bytes)||
     !slcli_source_deadline_original_matches(a,d,caller)||!prior||
     !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
     !in->containing->validate_returned||!in->containing->validate_result||
     !in->containing->constructor_after_returned||!in->containing->constructor_after_result||
     !in->containing->constructor_after.complete||
     in->containing->constructor_after.tick<in->containing->alias->validate_after.tick||
     prior->tick<in->containing->constructor_after.tick||
     prior->tick<d->origin->tick||prior->tick>=d->expires||
     !slcli_source_deadline_away(d,c,sizeof(*c))||
     !separate(c,sizeof(*c),caller,caller_bytes)||
     !separate(c,sizeof(*c),in,sizeof(*in))||
     !separate(c,sizeof(*c),prior,sizeof(*prior)))return 0;
  if(!separate(c,sizeof(*c),in->application,in->application_bytes)||
     !separate(c,sizeof(*c),in->command,in->command_bytes)||
     !separate(c,sizeof(*c),in->environment,in->environment_bytes)||
     !separate(c,sizeof(*c),in->directory,in->directory_bytes)||
     !separate(c,sizeof(*c),in->process_descriptor,in->process_descriptor_bytes)||
     !separate(c,sizeof(*c),in->thread_descriptor,in->thread_descriptor_bytes)||
     !in->inherited_count||in->inherited_count>SIZE_MAX/sizeof(HANDLE)||
     !separate(c,sizeof(*c),in->inherited,in->inherited_count*sizeof(HANDLE)))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  c->started=1;c->original=*in;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
  c->target_owner_original=target;c->runtime_backing_original=runtime;
  c->containing_original=in->containing;
  c->target_charged_original=target->charged;
  c->target_next_sequence_original=target->next_sequence;c->target_count_original=target->count;
  for(i=0;i<32;i++) {
    c->target_associations_original[0][i]=target->original_birth[i];
    c->target_associations_original[1][i]=target->original_image[i];
    c->target_associations_original[2][i]=target->original_owner[i];
    c->target_associations_original[3][i]=target->original_source[i];
  }
  c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
  c->predecessor=prior;c->predecessor_tick=prior->tick;
  in=&c->original;
  c->prefix_original=in->prefix;
  if(!slcli_node_prefix_original_matches(c->prefix_original,a,d,caller,target,&runtime->request)||
     prior->tick<c->prefix_original->after.tick)return 0;
  if(!in->transport||!slcli_arena_live_block(a,in->transport,sizeof(*in->transport))||
     !separate(c,sizeof(*c),in->transport,sizeof(*in->transport))||
     !in->transport->object_prepared||in->transport->terminal||
     in->transport->publication_attempted||in->transport->deadline!=d||
     in->transport->caller!=caller||in->transport->origin!=d->origin||
     in->transport->expires!=d->expires||!in->bootstrap_bytes||
     !in->allocator_capacity||in->allocator_capacity>SIZE_MAX||
     !in->thread_capture_attempts||in->thread_capture_attempts>SIZE_MAX||
     prior->tick<in->transport->calls[13].after.tick)return 0;
  c->transport_original=in->transport;
  c->transport_handle_original=in->transport->readonly;
  if(!slcli_native_clock_read(&c->transport_setup)||!same(c,a)||
     c->transport_setup.tick<prior->tick||c->transport_setup.tick>=c->expires)return failed_transport(c,a);
  c->transport_payload=slcli_arena_calloc(a,1,sizeof(*c->transport_payload));
  if(!c->transport_payload)return failed_transport(c,a);
  if(!text_original(a,in->application,in->application_bytes,0)||
     !text_original(a,in->command,in->command_bytes,0)||
     !text_original(a,in->environment,in->environment_bytes,1)||
     !text_original(a,in->directory,in->directory_bytes,0)||
     !in->inherited_count||in->inherited_count>SIZE_MAX/sizeof(HANDLE))return failed_transport(c,a);
  if(!in->process_descriptor_bytes||!in->thread_descriptor_bytes||
     !slcli_arena_live_range(a,in->process_descriptor,in->process_descriptor_bytes)||
     !slcli_arena_live_range(a,in->thread_descriptor,in->thread_descriptor_bytes))return failed_transport(c,a);
  handles_bytes=in->inherited_count*sizeof(HANDLE);
  if(!slcli_arena_live_range(a,in->inherited,handles_bytes))return failed_transport(c,a);
  for(i=0;i<in->inherited_count;i++) {
    size_t j;if(!in->inherited[i]||in->inherited[i]==INVALID_HANDLE_VALUE)return failed_transport(c,a);
    for(j=0;j<i;j++)if(in->inherited[j]==in->inherited[i])return failed_transport(c,a);
  }
  c->application=text_copy(a,in->application,in->application_bytes);
  c->command_before=text_copy(a,in->command,in->command_bytes);
  c->command_returned=text_copy(a,in->command,in->command_bytes);
  if(!selector_environment(c,a))return failed_transport(c,a);
  /* SAME already prepared original geometry, not incoming locator metadata. */
  c->route_original.transport=c->transport_handle_original;
  c->route_original.parent=c->transport_original->parent;
  c->route_original.base=c->containing_original->alias->view;
  c->route_original.bytes=c->containing_original->original.runtime_bytes;
  c->route_original.capture_bytes=c->containing_original->layout->capture_bytes;
  c->route_original.threads=c->containing_original->layout->threads;
  c->route_original.events=c->containing_original->layout->events;
  c->route_original.regions=c->containing_original->layout->regions;
  c->route_original.modules=c->containing_original->layout->modules;
  c->route_original.name_units=c->containing_original->layout->name_units;
  c->route_original.page_bytes=c->containing_original->layout->geometry.dwPageSize;
  c->route_original.granularity=c->containing_original->layout->geometry.dwAllocationGranularity;
  c->route_encode_result=slcli_node_route_windows_encode(c->route_bytes,sizeof(c->route_bytes),&c->route_original);
  c->route_encode_returned=1;if(!c->route_encode_result)return failed_transport(c,a);
  c->directory=text_copy(a,in->directory,in->directory_bytes);
  c->inherited=(HANDLE *)slcli_arena_alloc(a,handles_bytes);
  if(!c->application||!c->command_before||!c->command_returned||!c->environment||
     !c->directory||!c->inherited)return failed_transport(c,a);
  bytes_copy(c->inherited,in->inherited,handles_bytes);
  { int backing=0,duplicate=0,transport=0,parent=0;
    for(i=0;i<in->inherited_count;i++) {
      if(c->inherited[i]==runtime->backing)backing=1;
      if(c->inherited[i]==runtime->duplicate)duplicate=1;
      if(c->inherited[i]==c->transport_original->writable)return failed_transport(c,a);
      if(c->inherited[i]==c->transport_handle_original)transport=1;
      if(c->inherited[i]==c->transport_original->parent)parent=1;
    }
    if(!backing||!duplicate||!transport||!parent||prior->tick<runtime->duplicate_after.tick)return failed_transport(c,a);
  }
  c->process_descriptor=slcli_arena_alloc(a,in->process_descriptor_bytes);
  c->thread_descriptor=slcli_arena_alloc(a,in->thread_descriptor_bytes);
  if(!c->process_descriptor||!c->thread_descriptor)return failed_transport(c,a);
  bytes_copy(c->process_descriptor,in->process_descriptor,in->process_descriptor_bytes);
  bytes_copy(c->thread_descriptor,in->thread_descriptor,in->thread_descriptor_bytes);
  c->process_security.nLength=sizeof(c->process_security);
  c->process_security.lpSecurityDescriptor=c->process_descriptor;
  c->thread_security.nLength=sizeof(c->thread_security);
  c->thread_security.lpSecurityDescriptor=c->thread_descriptor;
  if(!slcli_native_clock_read(&c->before)||c->before.tick<c->transport_setup.tick||
     c->before.tick>=c->expires||!same(c,a))return failed_transport(c,a);
  c->creator_process=GetCurrentProcessId();c->creator_thread=GetCurrentThreadId();c->creator_observed=1;
  if(!c->creator_process||!c->creator_thread||
     c->creator_process!=c->transport_original->parent_pid||
     !c->transport_original->calls[7].returned||!c->transport_original->calls[7].result||
     c->transport_original->parent!=c->transport_original->parent_raw)goto post;
  c->query_status=InitializeProcThreadAttributeList(NULL,1,0,&c->query_bytes);
  c->query_returned=1;c->query_error=GetLastError();
  if(c->query_status||c->query_error!=ERROR_INSUFFICIENT_BUFFER||!c->query_bytes)goto post;
  c->allocated_bytes=c->query_bytes;
  c->attributes=(LPPROC_THREAD_ATTRIBUTE_LIST)slcli_arena_alloc(a,c->allocated_bytes);
  if(!c->attributes)goto post;
  if(!slcli_native_clock_read(&c->setup_clock[0])||!same(c,a)||
     c->setup_clock[0].tick<c->before.tick||c->setup_clock[0].tick>=c->expires)goto post;
  c->initialize_bytes_before=c->allocated_bytes;
  c->initialize_bytes_returned=c->initialize_bytes_before;
  c->initialize_status=InitializeProcThreadAttributeList(c->attributes,1,0,&c->initialize_bytes_returned);
  c->initialize_returned=1;if(!c->initialize_status)c->initialize_error=GetLastError();
  if(!c->initialize_status)goto post;
  if(!slcli_native_clock_read(&c->setup_clock[1])||!same(c,a)||
     c->setup_clock[1].tick<c->setup_clock[0].tick||c->setup_clock[1].tick>=c->expires)goto post;
  c->update_status=UpdateProcThreadAttribute(c->attributes,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
    c->inherited,handles_bytes,NULL,NULL);
  c->update_returned=1;if(!c->update_status)c->update_error=GetLastError();
  if(!c->update_status)goto post;
  c->startup.StartupInfo.cb=sizeof(c->startup);c->startup.lpAttributeList=c->attributes;
  c->creation_flags=CREATE_SUSPENDED|CREATE_UNICODE_ENVIRONMENT|EXTENDED_STARTUPINFO_PRESENT;
  if(!slcli_native_clock_read(&c->setup_clock[2])||!same(c,a)||
     c->setup_clock[2].tick<c->setup_clock[1].tick||c->setup_clock[2].tick>=c->expires)goto post;
  if(!slcli_memory_owner_prebirth_shape(target)||target->charged!=c->target_charged_original||
     target->next_sequence!=c->target_next_sequence_original||target->count!=c->target_count_original||
     !slcli_node_prefix_original_matches(c->prefix_original,a,d,caller,target,&runtime->request))return failed_transport(c,a);
  c->possible_thread_attempts=c->original.thread_capture_attempts;
  c->possible_event_capacity=c->original.allocator_capacity;
  c->child_effects_possible=1;c->create_called=1;
  c->create_status=CreateProcessW(c->application,c->command_returned,&c->process_security,&c->thread_security,TRUE,
    c->creation_flags,c->environment,c->directory,&c->startup.StartupInfo,&c->process);
  c->create_returned=1;if(!c->create_status)c->create_error=GetLastError();
  c->created=c->create_status!=0;
  if(c->created) {
    if(!slcli_native_clock_read(&c->birth_before)||!same(c,a)||
       c->birth_before.tick<c->setup_clock[2].tick||c->birth_before.tick>=c->expires)goto post;
    c->times_args[0]=(uintptr_t)c->process.hProcess;c->times_args[1]=(uintptr_t)&c->creation_time;
    c->times_args[2]=(uintptr_t)&c->exit_time;c->times_args[3]=(uintptr_t)&c->kernel_time;
    c->times_args[4]=(uintptr_t)&c->user_time;c->times_called=1;
    c->times_status=GetProcessTimes(c->process.hProcess,&c->creation_time,&c->exit_time,
      &c->kernel_time,&c->user_time);
    c->times_returned=1;if(!c->times_status)c->times_error=GetLastError();
  }
post:
  if(!slcli_native_clock_read(&c->after)||!same(c,a)||
     c->after.tick<c->before.tick||c->after.tick>=c->expires)return failed_transport(c,a);
  if(!c->created||!c->times_returned||!c->times_status||
     c->after.tick<c->birth_before.tick||!c->process.hProcess||!c->process.hThread||
     !c->process.dwProcessId||!c->process.dwThreadId)return failed_transport(c,a);
  c->child_mapping_result=slcli_child_mapping_windows_map_original(c->containing_original,a,c);
  c->child_mapping_returned=1;
  if(!c->child_mapping_result)return failed_transport(c,a);
  c->route_write_result=slcli_route_write_windows_write_original(c->containing_original,a,c);
  c->route_write_returned=1;if(!c->route_write_result)return failed_transport(c,a);
  c->qualified=1;return 1;
}
int slcli_launcher_windows_resume(struct slcli_launcher_windows_capture *c,
  struct slcli_arena *a,const struct slcli_native_clock *prior,const struct slcli_memory_owner *target,
  const unsigned char *owner_export,size_t owner_export_bytes) {
  if(!c||!a||!slcli_arena_live_block(a,c,sizeof(*c))||!c->qualified||
     !c->created||c->resume_attempted||!prior||
     !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
     !c->child_mapping_returned||!c->child_mapping_result||!c->containing_original->child.qualified||
     !slcli_child_mapping_windows_matches_original(c->containing_original,a,c)||
     !c->route_write_returned||!c->route_write_result||!c->containing_original->route_writer.readback_matches||
     prior->tick<c->containing_original->route_writer.readback.after.tick||
     prior->tick<c->containing_original->child.query_after.tick||prior->tick>=c->expires||!same(c,a)||
     !target||target!=c->target_owner_original||target==a->owner||
     !slcli_arena_live_block(a,target,sizeof(*target))||
     target->charged!=c->target_charged_original||target->count!=c->target_count_original||
     target->next_sequence!=c->target_next_sequence_original||
     !slcli_arena_live_range(a,owner_export,owner_export_bytes)||
     !slcli_owner_handoff_matches(owner_export,owner_export_bytes,target)||
     !separate(c,sizeof(*c),target,sizeof(*target))||
     !separate(c,sizeof(*c),owner_export,owner_export_bytes))return 0;
  c->resume_attempted=1;
  if(!slcli_node_prefix_original_matches(c->prefix_original,a,c->deadline,c->caller,target,
     &c->runtime_backing_original->request))return failed_transport(c,a);
  c->target_owner_original=target;c->owner_export_original=owner_export;
  c->owner_export_bytes=owner_export_bytes;
  {
    size_t i;struct slcli_node_bootstrap_transport *t=c->transport_payload;
    const struct slcli_preentry_backing_windows *b=c->runtime_backing_original;
    if(!t||!slcli_arena_live_block(a,t,sizeof(*t))||
       c->transport_original->readonly!=c->transport_handle_original)return failed_transport(c,a);
    t->origin=*c->origin;t->expires=c->expires;t->frozen_owner=*target;
    t->runtime_request=b->request;t->owner_limit=target->limit;
    t->prefix=c->prefix_original->snapshot;
    t->bootstrap_bytes=c->original.bootstrap_bytes;t->runtime_bytes=b->bytes;
    t->capacity=c->original.allocator_capacity;
    t->thread_capture_attempts=c->original.thread_capture_attempts;
    t->capabilities[0]=(uintptr_t)b->backing;t->capabilities[1]=(uintptr_t)b->duplicate;
    t->capabilities[2]=(uintptr_t)c->transport_handle_original;
    t->capabilities[3]=(uintptr_t)c->transport_original->parent;
    t->parent_pid=c->transport_original->parent_pid;
    t->parent_creation[0]=c->transport_original->parent_creation.dwLowDateTime;
    t->parent_creation[1]=c->transport_original->parent_creation.dwHighDateTime;
    t->parent_sid_bytes=c->transport_original->parent_sid_bytes;
    bytes_copy(t->parent_sid,c->transport_original->parent_sid,sizeof(t->parent_sid));
    t->parent_token_id[0]=c->transport_original->parent_statistics.TokenId.LowPart;
    t->parent_token_id[1]=(uint32_t)c->transport_original->parent_statistics.TokenId.HighPart;
    t->parent_authentication_id[0]=c->transport_original->parent_statistics.AuthenticationId.LowPart;
    t->parent_authentication_id[1]=(uint32_t)c->transport_original->parent_statistics.AuthenticationId.HighPart;
    t->process_original=c->process.dwProcessId;t->thread_original=c->process.dwThreadId;
    {
      const struct slcli_child_mapping_windows *m=&c->containing_original->child;
      if(m->birth_original!=c||m->root_original!=c->containing_original||
         !m->map_called||!m->map_returned||m->status<0||!m->possibly_mapped||
         !m->query_called||!m->query_returned||!m->qualified||
         m->query_status!=sizeof(m->geometry)||!m->query_after.complete||
         m->process_original!=c->process.hProcess||m->process_id_original!=c->process.dwProcessId||
         m->thread_id_original!=c->process.dwThreadId||m->section_original!=b->backing||
         m->base!=c->route_original.base||m->bytes!=t->runtime_bytes||
         m->geometry.BaseAddress!=m->base||m->geometry.AllocationBase!=m->base||
         m->geometry.RegionSize!=m->bytes||m->geometry.Protect!=PAGE_READWRITE||
         m->geometry.State!=MEM_COMMIT||m->geometry.Type!=MEM_MAPPED||
         prior->tick<m->query_after.tick)return failed_transport(c,a);
      t->mapped_view.base=(uintptr_t)m->base;t->mapped_view.bytes=m->bytes;
      t->mapped_view.section=(uintptr_t)m->section_original;
      t->mapped_view.view_version=m->row_version_original;
      t->mapped_view.section_version=m->section_version_original;
      t->mapped_view.query_tick=m->query_after.tick;t->mapped_view.status=(uint32_t)m->status;
      t->mapped_view.protection=m->geometry.Protect;t->mapped_view.state=m->geometry.State;
      t->mapped_view.type=m->geometry.Type;
    }
    for(i=0;i<2;i++)t->birth_raw[i]=c->birth_before.raw[i];
    for(i=0;i<4;i++)bytes_copy(t->associations[i],c->target_associations_original[i],32);
    bytes_copy(t->expected_native,&c->creation_time,sizeof(c->creation_time));
    c->transport_publication_result=slcli_node_transport_windows_publish(c->transport_original,a,prior,t);
    c->transport_publication_returned=1;
    if(!c->transport_publication_result||!c->transport_original->returned||
       !slcli_native_clock_read(&c->transport_ready)||!same(c,a)||
       c->transport_ready.tick<c->transport_original->calls[6].after.tick||
       c->transport_ready.tick>=c->expires)return failed_transport(c,a);
  }
  if(!slcli_native_clock_read(&c->resume_before)||c->resume_before.tick<c->transport_ready.tick||
     c->resume_before.tick>=c->expires||!same(c,a)||
     !slcli_owner_handoff_matches(owner_export,owner_export_bytes,target))return failed_transport(c,a);
  c->resume_called=1;c->resume_status=ResumeThread(c->process.hThread);
  c->resume_returned=1;
  if(c->resume_status==(DWORD)-1)c->resume_error=GetLastError();
  if(!slcli_native_clock_read(&c->resume_after)||!same(c,a)||
     c->resume_after.tick<c->resume_before.tick||c->resume_after.tick>=c->expires||
     c->resume_status!=1||
     !slcli_owner_handoff_matches(owner_export,owner_export_bytes,target))return failed_transport(c,a);
  c->resume_qualified=1;return 1;
}
#endif
