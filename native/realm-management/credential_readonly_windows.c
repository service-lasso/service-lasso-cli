#include "credential_readonly_windows.h"
#if defined(_WIN32)
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
static int source(struct slcli_management_credential_windows_readonly *c) {
  struct slcli_management_credential_windows_creation *b;
  if(!c||!c->prepared||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||
     !slcli_management_credential_windows_factory_original_matches(c->factory))return 0;
  b=c->factory->creation;
  return c->factory->arena==c->arena&&c->deadline==b->deadline&&c->origin==b->origin&&
    c->caller_original==b->caller_original&&c->caller_bytes==b->caller_bytes&&
    c->expires==b->expires&&c->origin_tick==b->origin_tick&&
    slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller_original)&&
    c->body&&c->input_owner&&c->body->deadline_owner==c->input_owner&&
    c->input_owner->body==c->body&&c->input_owner->deadline==c->deadline&&
    c->input_owner->caller_original==c->caller_original&&
    c->input_owner->origin==c->origin&&c->input_owner->expires==c->expires&&
    c->input_owner->origin_tick==c->origin_tick&&slcli_input_deadline_ready(c->body)&&
    c->credential&&slcli_arena_live_block(c->arena,c->credential,sizeof(*c->credential))&&
    c->credential->arena==c->arena&&away(c,sizeof(*c),c->credential,sizeof(*c->credential));
}
struct slcli_management_credential_windows_readonly *slcli_management_credential_windows_readonly_allocate(
  struct slcli_arena *a,struct slcli_management_credential_windows_factory *f) {
  struct slcli_management_credential_windows_readonly *c;
  struct slcli_management_credential_windows_creation *b;
  size_t io_bytes;
  if(!a||!f||!slcli_management_credential_windows_factory_original_matches(f)||f->arena!=a||
     f->started||f->unresolved||f->complete)return 0;b=f->creation;
  c=slcli_arena_calloc(a,1,sizeof(*c));if(!c)return 0;
  c->arena=a;c->factory=f;c->deadline=b->deadline;c->origin=b->origin;
  c->caller_original=b->caller_original;c->caller_bytes=b->caller_bytes;
  c->expires=b->expires;c->origin_tick=b->origin_tick;c->previous_tick=b->origin_tick;
  if(!away(c,sizeof(*c),b,sizeof(*b))||!away(c,sizeof(*c),f,sizeof(*f))||
     !away(c,sizeof(*c),c->caller_original,c->caller_bytes)||
     !away(c,sizeof(*c),b->capture_original,b->capture_original_bytes)||
     !away(c,sizeof(*c),b->name_original,b->name_bytes)||
     !away(c,sizeof(*c),b->security_original,b->security_bytes)||
     !slcli_source_deadline_away(c->deadline,c,sizeof(*c)))return 0;
  c->body=slcli_input_body_allocate(a,64,134);if(!c->body)return 0;
  c->input_owner=slcli_input_deadline_bind(a,c->body,c->deadline,c->caller_original,
    c->caller_bytes,c->origin,b->sink,b->capture_original,b->capture_original_bytes);
  c->credential=slcli_management_credential_allocate(a);
  if(!c->input_owner||!c->credential)return 0;
  io_bytes=(sizeof(*c->input_owner)+_Alignof(struct slcli_native_clock)-1)/
    _Alignof(struct slcli_native_clock)*_Alignof(struct slcli_native_clock)+134*sizeof(struct slcli_native_clock);
  if(!away(c,sizeof(*c),c->body,c->input_owner->body_storage_bytes)||
     !away(c,sizeof(*c),c->input_owner,io_bytes)||
     !away(c->credential,sizeof(*c->credential),c->body,c->input_owner->body_storage_bytes)||
     !away(c->credential,sizeof(*c->credential),c->input_owner,io_bytes)||
     !slcli_source_deadline_away(c->deadline,c->credential,sizeof(*c->credential)))return 0;
  c->prepared=1;if(!source(c))return 0;return c;
}
static int clock_row(struct slcli_management_credential_windows_readonly *c,size_t row) {
  struct slcli_native_clock *r=&c->clocks[row];
  if(!source(c)||!slcli_native_clock_read(r)||r->tick<c->previous_tick||
     r->tick>=c->expires||!source(c))return 0;c->previous_tick=r->tick;return 1;
}
static int same_transfer(const struct slcli_windows_file_capture *a,
  const struct slcli_windows_file_capture *b) {
  /* Native access intentionally decreases. All SAME-file identity/content
   * custody remains exact; old granted write bit is never called readonly. */
  return a&&b&&a->completed&&b->completed&&a->owner_sid_bytes&&
    a->identity.VolumeSerialNumber==b->identity.VolumeSerialNumber&&
    slcli_equal(a->identity.FileId.Identifier,b->identity.FileId.Identifier,16)&&
    a->owner_sid_bytes==b->owner_sid_bytes&&slcli_equal(a->owner_sid,b->owner_sid,a->owner_sid_bytes)&&
    a->attributes.FileAttributes==b->attributes.FileAttributes&&a->mode==b->mode&&
    a->basic.LastWriteTime.QuadPart==b->basic.LastWriteTime.QuadPart&&
    a->basic.ChangeTime.QuadPart==b->basic.ChangeTime.QuadPart&&
    a->standard.EndOfFile.QuadPart==64&&b->standard.EndOfFile.QuadPart==64&&
    a->standard.NumberOfLinks==b->standard.NumberOfLinks&&
    b->access_raw.GrantedAccess==(FILE_READ_DATA|FILE_READ_ATTRIBUTES|READ_CONTROL|SYNCHRONIZE)&&
    !(b->handle_flags&HANDLE_FLAG_INHERIT);
}
int slcli_management_credential_windows_readonly_run(struct slcli_management_credential_windows_readonly *c) {
  struct slcli_management_credential_persistence *p;const struct slcli_windows_file_capture *last;
  ACCESS_MASK access=FILE_READ_DATA|FILE_READ_ATTRIBUTES|READ_CONTROL|SYNCHRONIZE;
  int ok;
  if(!source(c)||c->started||c->unresolved)return 0;
  if(c->factory->started||c->factory->complete)return 0;c->started=1;c->unresolved=1;
  if(!slcli_management_credential_windows_factory_run(c->factory)||!source(c))return 0;
  p=c->factory->persistence;c->previous_tick=p->previous_tick;
  if(!p->complete||p->unresolved||!p->second->capture_count)return 0;
  last=&p->second->captures[p->second->capture_count-1];c->writer_original=p->original;
  if(!clock_row(c,0))return 0;c->process_called=1;c->process=GetCurrentProcess();c->process_observed=1;
  if(!clock_row(c,1)||!c->process||!clock_row(c,2))return 0;
  c->duplicate_args[0]=(uintptr_t)c->process;c->duplicate_args[1]=(uintptr_t)c->writer_original;
  c->duplicate_args[2]=(uintptr_t)c->process;c->duplicate_args[3]=(uintptr_t)&c->readonly_original;
  c->duplicate_args[4]=access;c->duplicate_args[5]=FALSE;c->duplicate_args[6]=0;
  c->duplicate_called=1;
  c->duplicate_status=DuplicateHandle(c->process,c->writer_original,c->process,
    &c->readonly_original,access,FALSE,0);c->duplicate_observed=1;
  if(!c->duplicate_status){c->duplicate_error=GetLastError();c->duplicate_error_observed=1;}
  if(c->duplicate_status&&c->readonly_original&&c->readonly_original!=INVALID_HANDLE_VALUE)c->alias_acquired=1;
  /* Both alias/raw output facts precede diagnostics. No close-source option;
   * a failed duplicate/post-clock retains the old writer and every new output. */
  if(!clock_row(c,3)||!c->alias_acquired||!clock_row(c,4))return 0;
  c->close_args[0]=(uintptr_t)c->writer_original;c->close_called=1;
  c->close_status=CloseHandle(c->writer_original);c->close_observed=1;
  if(c->close_status)c->writer_closed=1;
  else {c->close_error=GetLastError();c->close_error_observed=1;}
  if(!clock_row(c,5)||!c->writer_closed||!clock_row(c,6))return 0;
  ok=slcli_windows_source_observe(c->readonly_original,&c->readonly_capture);
  if(!c->factory->creation->sink(c->factory->creation->capture_original,&c->readonly_capture,0,0)||
     !ok||!c->readonly_capture.tick||c->readonly_capture.tick<c->previous_tick||
     c->readonly_capture.tick>=c->expires)return 0;c->previous_tick=c->readonly_capture.tick;
  if(!clock_row(c,7)||!same_transfer(last,&c->readonly_capture)||
     !slcli_input_deadline_precede(c->input_owner,&c->clocks[7])||
     !slcli_input_body_read(c->body,c->readonly_original,64,p->second->digest)||
     !slcli_input_deadline_ready(c->body)||
     !slcli_input_original_same(&c->readonly_capture,&c->body->captures[0],64))return 0;
  c->body_read=1;
  if(!slcli_management_credential_consume(c->credential,c->body)||!source(c)||
     !slcli_input_deadline_ready(c->body)||c->body->deadline_owner!=c->input_owner||
     c->input_owner->predecessor!=&c->clocks[7])return 0;
  c->credential_consumed=1;c->complete=1;c->unresolved=0;return 1;
}
int slcli_management_credential_windows_readonly_native_positive(struct slcli_management_credential_windows_readonly *c) {
  size_t n;uint64_t tick;HANDLE alias;
  if(!c||!slcli_management_credential_windows_readonly_run(c)||!c->complete||c->unresolved||
     !c->alias_acquired||!c->writer_closed||!c->body_read||!c->credential_consumed||
     !c->credential->complete||c->credential->failed||!c->body->recheck_complete)return 0;
  n=c->body->capture_count;tick=c->input_owner->previous_tick;alias=c->readonly_original;
  if(slcli_management_credential_windows_readonly_run(c))return 0;
  return c->complete&&!c->unresolved&&c->body->capture_count==n&&
    c->input_owner->previous_tick==tick&&c->readonly_original==alias&&c->writer_closed;
}
#endif