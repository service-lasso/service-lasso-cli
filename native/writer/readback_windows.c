#include "readback_windows.h"
#if defined(_WIN32)
static int separate(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int zero(const void *p,size_t n){
 size_t i;const unsigned char *b=p;for(i=0;i<n;i++)if(b[i])return 0;return 1;
}
static int same(const struct slcli_writer_windows_readback_original *c){
 const struct slcli_writer_windows_path *p=c->creator;
 return c->started&&c->arena&&p&&slcli_arena_live_block(c->arena,c,sizeof(*c))&&
  slcli_arena_live_range(c->arena,p,sizeof(*p))&&p->readback_borrower==c&&
  p->complete&&!p->failed&&!p->directory&&!p->retained_native_io&&
  p->create_status==0&&p->create_io.Status==0&&p->create_io.Information==2&&
  p->original&&p->original!=INVALID_HANDLE_VALUE&&p->mode.complete&&!p->mode.failed&&
  p->mode.original==p->original&&p->mode.file.original==p->original&&
  p->mode.file.completed&&p->mode.file.identity_status&&
  p->evidence_plan&&p->evidence_original&&
  slcli_arena_live_range(c->arena,p->evidence_plan,sizeof(*p->evidence_plan))&&
  slcli_arena_live_range(c->arena,p->evidence_original,sizeof(*p->evidence_original))&&
  p->evidence_file&&slcli_evidence_deadline_ready(p->evidence_file)&&
  p->evidence_plan->deadline==c->deadline&&p->evidence_original->deadline==c->deadline&&
  p->evidence_plan->caller==c->caller&&p->evidence_plan->caller_bytes==c->caller_bytes&&
  p->evidence_original->caller==c->caller&&p->evidence_original->caller_bytes==c->caller_bytes&&
  p->evidence_original->maximum==p->evidence_plan->maximum&&
  p->evidence_original->bq_profile==p->evidence_plan->bq_profile&&
  p->evidence_plan->bq_profile==p->evidence_file->deadline_owner->bq_profile&&
  slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)&&
  slcli_arena_live_block(c->arena,c->caller,c->caller_bytes)&&
  c->duplicate_args[0]==(uintptr_t)(HANDLE)(intptr_t)-1&&
  c->duplicate_args[1]==(uintptr_t)p->original&&
  c->duplicate_args[2]==(uintptr_t)(HANDLE)(intptr_t)-1&&
  c->duplicate_args[3]==(uintptr_t)&c->original&&
  c->duplicate_args[4]==(FILE_READ_DATA|FILE_READ_ATTRIBUTES|READ_CONTROL|SYNCHRONIZE)&&
  c->duplicate_args[5]==FALSE&&c->duplicate_args[6]==0;
}
int slcli_writer_duplicate_windows_readback_original(
 struct slcli_writer_windows_readback_original *c,struct slcli_arena *a,
 struct slcli_writer_windows_path *p,const struct slcli_source_deadline_original *d,
 const void *caller,size_t bytes,const struct slcli_native_clock *prior){
 if(!c||!a||!p||!bytes||!slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !slcli_arena_live_range(a,p,sizeof(*p))||p->readback_borrower||
  !slcli_arena_live_block(a,caller,bytes)||!prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||
  !prior->complete||!slcli_source_deadline_original_matches(a,d,caller)||
  prior->tick<d->origin->tick||prior->tick>=d->expires||
  !separate(c,sizeof(*c),p,sizeof(*p))||!separate(c,sizeof(*c),caller,bytes)||
  !separate(c,sizeof(*c),prior,sizeof(*prior))||!slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
 c->started=1;c->failed=1;c->arena=a;c->creator=p;c->deadline=d;c->caller=caller;c->caller_bytes=bytes;
 c->previous_tick=prior->tick;p->readback_borrower=c;
 c->duplicate_args[0]=(uintptr_t)(HANDLE)(intptr_t)-1;c->duplicate_args[1]=(uintptr_t)p->original;
 c->duplicate_args[2]=(uintptr_t)(HANDLE)(intptr_t)-1;c->duplicate_args[3]=(uintptr_t)&c->original;
 c->duplicate_args[4]=FILE_READ_DATA|FILE_READ_ATTRIBUTES|READ_CONTROL|SYNCHRONIZE;
 if(!same(c))return 0;
 c->before_result=slcli_native_clock_read(&c->before);c->before_returned=1;
 if(!c->before_result||c->before.tick<c->previous_tick||c->before.tick>=d->expires||!same(c)||
  !slcli_evidence_deadline_precede(p->evidence_file,&c->before))return 0;
 c->duplicate_called=1;
 c->duplicate_status=DuplicateHandle((HANDLE)(intptr_t)-1,p->original,(HANDLE)(intptr_t)-1,
  &c->original,(DWORD)c->duplicate_args[4],FALSE,0);
 c->duplicate_returned=1;if(!c->duplicate_status)c->duplicate_error=GetLastError();
 c->after_result=slcli_native_clock_read(&c->after);c->after_returned=1;
 if(!c->after_result||c->after.tick<c->before.tick||c->after.tick>=d->expires||!same(c)||
  !slcli_evidence_deadline_precede(p->evidence_file,&c->after)||
  !c->duplicate_status||!c->original||c->original==INVALID_HANDLE_VALUE||c->original==p->original)return 0;
 c->previous_tick=c->after.tick;
 c->observe_before_result=slcli_native_clock_read(&c->observe_before);c->observe_before_returned=1;
 if(!c->observe_before_result||c->observe_before.tick<c->previous_tick||
  c->observe_before.tick>=d->expires||!same(c)||
  !slcli_evidence_deadline_precede(p->evidence_file,&c->observe_before))return 0;
 c->observe_result=slcli_windows_source_observe(c->original,&c->observed);c->observe_returned=1;
 c->observe_after_result=slcli_native_clock_read(&c->observe_after);c->observe_after_returned=1;
 if(!c->observe_after_result||c->observe_after.tick<c->observe_before.tick||
  c->observe_after.tick<c->observed.tick||c->observe_after.tick>=d->expires||!same(c)||
  !slcli_evidence_deadline_precede(p->evidence_file,&c->observe_after)||
  !c->observe_result||c->observed.original!=c->original||
  c->observed.tick<c->observe_before.tick||
  c->observed.identity.VolumeSerialNumber!=p->mode.file.identity.VolumeSerialNumber||
  !slcli_equal(c->observed.identity.FileId.Identifier,p->mode.file.identity.FileId.Identifier,16))return 0;
 c->previous_tick=c->observe_after.tick;c->complete=1;c->failed=0;return 1;
}
int slcli_writer_windows_readback_matches_original(
 const struct slcli_writer_windows_readback_original *c){
 const struct slcli_windows_file_capture *r;
 if(!c||!same(c)||!c->complete||c->failed||!c->duplicate_called||!c->duplicate_returned||
  !c->duplicate_status||!c->original||c->original==INVALID_HANDLE_VALUE||
  c->original==c->creator->original||!c->before_returned||!c->before_result||
  !c->after_returned||!c->after_result||!c->observe_before_returned||!c->observe_before_result||
  !c->observe_returned||!c->observe_result||!c->observe_after_returned||!c->observe_after_result||
  !c->before.complete||!c->after.complete||!c->observe_before.complete||!c->observe_after.complete||
  c->before.tick<c->deadline->origin->tick||
  c->after.tick<c->before.tick||c->observe_before.tick<c->after.tick||
  c->observe_after.tick<c->observe_before.tick||c->observe_after.tick>=c->deadline->expires||
  c->previous_tick!=c->observe_after.tick)return 0;
 r=&c->observed;
 return r->original==c->original&&r->operation==1&&r->completed&&!r->retained_native_io&&
  r->tick>=c->observe_before.tick&&r->tick<=c->observe_after.tick&&
  r->identity_status&&r->standard_status&&r->attributes_status&&r->basic_status&&
  r->access_status==0&&r->access_bytes==sizeof(r->access_raw)&&
  r->access_raw.GrantedAccess==(FILE_READ_DATA|FILE_READ_ATTRIBUTES|READ_CONTROL|SYNCHRONIZE)&&
  r->handle_status&&!(r->handle_flags&HANDLE_FLAG_INHERIT)&&r->file_type==FILE_TYPE_DISK&&
  r->identity.VolumeSerialNumber==c->creator->mode.file.identity.VolumeSerialNumber&&
  slcli_equal(r->identity.FileId.Identifier,c->creator->mode.file.identity.FileId.Identifier,16);
}
#endif
