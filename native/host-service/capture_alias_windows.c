#include "capture_alias_windows.h"
#if defined(_WIN32)
static int separate(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int row_in(struct slcli_arena *a,const struct slcli_binding_originals *b,
 const struct slcli_native_reference *r) {
 uintptr_t first,at;size_t bytes;
 if(!b->count||b->count>SIZE_MAX/sizeof(*r)||!b->references)return 0;
 bytes=b->count*sizeof(*r);first=(uintptr_t)b->references;at=(uintptr_t)r;
 return slcli_arena_live_range(a,b->references,bytes)&&at>=first&&at-first<bytes&&
  !((at-first)%sizeof(*r));
}
static int alias_same(struct slcli_capture_alias_windows *c,struct slcli_arena *a) {
 const struct slcli_preentry_backing_windows *s=c->backing_original;
 return slcli_source_deadline_original_matches(a,c->deadline,c->caller)&&
  c->deadline->origin==c->origin&&c->origin->tick==c->origin_tick&&
  c->deadline->expires==c->expires&&c->predecessor->complete&&
  c->predecessor->tick==c->predecessor_tick&&c->binding_original->deadline_original==c->deadline&&
  row_in(a,c->binding_original,c->section_reference)&&row_in(a,c->binding_original,c->view_reference)&&
  s->qualified&&s->section_only&&s->operation==1&&s->deadline==c->deadline&&
  s->backing==c->backing_handle_original&&s->bytes==c->backing_bytes_original&&
  s->request.index==c->backing_extent_original&&s->request.sequence==c->backing_acquisition_original&&
  s->backing_returned&&s->backing_result&&
  c->section_reference->native_original==s&&
  c->section_reference->native_class==SLCLI_NATIVE_ORIGINAL_SECTION_BACKING&&
  c->section_reference->kind==22&&c->section_reference->version&&
  c->section_reference->handle==(uintptr_t)c->backing_handle_original&&c->section_reference->effect_flags==15&&
  c->view_reference->native_original==c&&
  c->view_reference->native_class==c->native_class_original&&
  ((c->native_class_original==SLCLI_NATIVE_ORIGINAL_CAPTURE_ALIAS&&c->access_original==FILE_MAP_READ)||
   (c->native_class_original==SLCLI_NATIVE_ORIGINAL_CAPTURE_INITIALIZER_VIEW&&c->access_original==(FILE_MAP_READ|FILE_MAP_WRITE)))&&
  c->view_reference->kind==9&&c->view_reference->version&&!c->view_reference->handle&&
  c->view_reference->alias_of==c->section_ordinal_original&&
  c->view_reference->physical_extent==c->section_reference->physical_extent&&
  c->section_reference->physical_extent==s->request.index&&
  c->capture_bytes&&c->capture_bytes<=s->bytes;
}
int slcli_preentry_section_windows_create_enrolled_original(
 struct slcli_preentry_backing_windows *backing,struct slcli_arena *a,
 const struct slcli_binding_originals *b,struct slcli_native_reference *section,
 struct slcli_memory_owner *physical,const struct slcli_source_deadline_original *d,
 const void *caller,size_t caller_bytes,const struct slcli_native_clock *prior,
 uint64_t bytes,const void *descriptor,size_t descriptor_bytes) {
 size_t i;int result;
 if(!a||!backing||!slcli_arena_live_block(a,backing,sizeof(*backing))||
  !b||!slcli_arena_live_range(a,b,sizeof(*b))||!row_in(a,b,section)||
  b->deadline_original!=d||!slcli_source_deadline_original_matches(a,d,caller)||
  !physical||!slcli_arena_live_block(a,physical,sizeof(*physical))||
  !slcli_memory_owner_writable(physical)||physical->count>=256||
  !separate(backing,sizeof(*backing),b->references,b->count*sizeof(*section))||
  section->kind!=22||!section->version||section->effect_flags!=1||section->handle||
  section->native_original||section->service_request_original||
  section->native_class!=SLCLI_NATIVE_ORIGINAL_UNCONSTRUCTED||
  section->physical_extent!=physical->count)return 0;
 for(i=0;i<sizeof(*backing);i++)if(((const unsigned char *)backing)[i])return 0;
 /* The source's exact already reserved physical ordinal must be the next
  * actual prepare. No incoming frame supplies it and no second book is made.
  * The raw capture owns attempted/returned cells independently of this outer
  * function's return; unknown or partial construction never clears this link. */
 section->native_original=backing;section->native_class=SLCLI_NATIVE_ORIGINAL_SECTION_BACKING;
 result=slcli_preentry_backing_windows_create_section(backing,a,physical,d,caller,
  caller_bytes,prior,bytes,descriptor,descriptor_bytes);
 if(backing->called[1])section->effect_flags|=2;
 if(backing->returned[1])section->effect_flags|=4;
 if(backing->backing){section->effect_flags|=8;section->handle=(uintptr_t)backing->backing;}
 /* No identity/digest or positive fixed verifier is fabricated here. Until
  * complete typed identity/lifetime construction this tag remains denied by
  * primary dispatch, even after the actual API returned success. */
 return result;
}
int slcli_preentry_section_windows_duplicate_enrolled_original(
 struct slcli_preentry_backing_windows *backing,struct slcli_arena *a,
 const struct slcli_binding_originals *b,struct slcli_native_reference *section,
 struct slcli_native_reference *duplicate,const struct slcli_native_clock *prior) {
 uint32_t ordinal;int result;
 if(!a||!backing||!slcli_arena_live_block(a,backing,sizeof(*backing))||
  !b||!slcli_arena_live_range(a,b,sizeof(*b))||!row_in(a,b,section)||!row_in(a,b,duplicate)||
  section>=duplicate||section->kind!=22||section->native_original!=backing||
  section->native_class!=SLCLI_NATIVE_ORIGINAL_SECTION_BACKING||
  section->handle!=(uintptr_t)backing->backing||section->effect_flags!=15||
  section->physical_extent!=backing->request.index||
  backing->deadline!=b->deadline_original||!backing->qualified||backing->duplicate_attempted||
  duplicate->kind!=22||!duplicate->version||duplicate->effect_flags!=1||duplicate->handle||
  duplicate->native_original||duplicate->service_request_original||
  duplicate->native_class!=SLCLI_NATIVE_ORIGINAL_UNCONSTRUCTED||
  duplicate->physical_extent!=section->physical_extent||
  !separate(backing,sizeof(*backing),b->references,b->count*sizeof(*section)))return 0;
 ordinal=(uint32_t)(section-b->references);
 if(ordinal>=0xffffu||duplicate->alias_of!=ordinal)return 0;
 duplicate->native_original=backing;duplicate->native_class=SLCLI_NATIVE_ORIGINAL_SECTION_DUPLICATE;
 result=slcli_preentry_backing_windows_duplicate(backing,a,prior);
 /* The original duplicate capture retains raw BOOL/HANDLE/return marker.
  * duplicate_attempted also records pre-call denial, so is not a called bit. */
 if(backing->duplicate_called)duplicate->effect_flags|=2;
 if(backing->duplicate_returned)duplicate->effect_flags|=4;
 if(backing->duplicate){duplicate->handle=(uintptr_t)backing->duplicate;duplicate->effect_flags|=8;}
 return result;
}
static int bind_original(struct slcli_capture_alias_windows *c,
 struct slcli_arena *a,const struct slcli_binding_originals *b,
 struct slcli_native_reference *section,struct slcli_native_reference *view,
 const struct slcli_preentry_backing_windows *backing,enum slcli_native_original_class tag) {
 size_t i;uint32_t ordinal;
 if(!a||!c||!slcli_arena_live_block(a,c,sizeof(*c))||!b||
  !slcli_arena_live_range(a,b,sizeof(*b))||!row_in(a,b,section)||!row_in(a,b,view)||
  section==view||!backing||!slcli_arena_live_block(a,backing,sizeof(*backing))||
  !separate(c,sizeof(*c),backing,sizeof(*backing))||
  !separate(c,sizeof(*c),b->references,b->count*sizeof(*view)))return 0;
 for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
 ordinal=(uint32_t)(section-b->references);
 if(ordinal>=0xffffu||section>=view||section->kind!=22||section->native_original!=backing||
  section->native_class!=SLCLI_NATIVE_ORIGINAL_SECTION_BACKING||
  !section->version||!backing->qualified||!backing->section_only||
  backing->deadline!=b->deadline_original||!backing->backing||
  section->handle!=(uintptr_t)backing->backing||section->effect_flags!=15||
  section->physical_extent!=backing->request.index||view->kind!=9||!view->version||
  view->effect_flags!=1||view->handle||view->native_original||view->service_request_original||
  view->native_class!=SLCLI_NATIVE_ORIGINAL_UNCONSTRUCTED||
  view->physical_extent!=section->physical_extent||view->alias_of!=ordinal)return 0;
 view->native_original=c;view->native_class=tag;return 1;
}
int slcli_capture_alias_windows_bind_original(struct slcli_capture_alias_windows *c,
 struct slcli_arena *a,const struct slcli_binding_originals *b,
 struct slcli_native_reference *section,struct slcli_native_reference *view,
 const struct slcli_preentry_backing_windows *backing) {
 return bind_original(c,a,b,section,view,backing,SLCLI_NATIVE_ORIGINAL_CAPTURE_ALIAS);
}
int slcli_capture_initializer_windows_bind_original(struct slcli_capture_alias_windows *c,
 struct slcli_arena *a,const struct slcli_binding_originals *b,
 struct slcli_native_reference *section,struct slcli_native_reference *view,
 const struct slcli_preentry_backing_windows *backing) {
 return bind_original(c,a,b,section,view,backing,SLCLI_NATIVE_ORIGINAL_CAPTURE_INITIALIZER_VIEW);
}
static int map_original(struct slcli_capture_alias_windows *c,
 struct slcli_arena *a,const struct slcli_binding_originals *b,
 struct slcli_native_reference *section,struct slcli_native_reference *view,
 const struct slcli_preentry_backing_windows *backing,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,size_t bytes,DWORD access,enum slcli_native_original_class tag) {
 size_t i;uint32_t section_ordinal;
 if(!a||!c||!slcli_arena_live_block(a,c,sizeof(*c))||
  !b||!slcli_arena_live_range(a,b,sizeof(*b))||!row_in(a,b,section)||!row_in(a,b,view)||
  section==view||!backing||!slcli_arena_live_block(a,backing,sizeof(*backing))||
  !slcli_source_deadline_original_matches(a,d,caller)||b->deadline_original!=d||
  !caller_bytes||!slcli_arena_live_block(a,caller,caller_bytes)||
  !prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  prior->tick<d->origin->tick||prior->tick>=d->expires||
  !separate(c,sizeof(*c),backing,sizeof(*backing))||
  !separate(c,sizeof(*c),b->references,b->count*sizeof(*section))||
  !separate(c,sizeof(*c),caller,caller_bytes)||!separate(c,sizeof(*c),prior,sizeof(*prior))||
  !slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
 for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
 section_ordinal=(uint32_t)(section-b->references);
 /* This explicit typed association is supplied by the source constructor,
  * never by incoming snapshot claims. Missing construction remains denial. */
 if(!backing->qualified||!backing->section_only||backing->operation!=1||
  backing->deadline!=d||!backing->backing||!backing->backing_returned||!backing->backing_result||
  !bytes||bytes>backing->bytes||!backing->page_geometry.dwPageSize||
  bytes%backing->page_geometry.dwPageSize||
  section->kind!=22||section->native_original!=backing||!section->version||
  section->native_class!=SLCLI_NATIVE_ORIGINAL_SECTION_BACKING||
  section->handle!=(uintptr_t)backing->backing||section->effect_flags!=15||
  section->physical_extent!=backing->request.index||
  view->kind!=9||view->native_original!=c||!view->version||view->effect_flags!=1||
  view->native_class!=tag||
  view->handle||view->physical_extent!=section->physical_extent||
  view->alias_of!=section_ordinal)return 0;
 c->started=1;c->backing_original=backing;c->binding_original=b;
 c->section_reference=section;c->view_reference=view;c->deadline=d;
 c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
 c->predecessor=prior;c->predecessor_tick=prior->tick;
 c->backing_handle_original=backing->backing;c->backing_bytes_original=backing->bytes;
 c->backing_extent_original=backing->request.index;c->backing_acquisition_original=backing->request.sequence;
 c->section_ordinal_original=section_ordinal;
 c->caller=caller;c->caller_bytes=caller_bytes;c->capture_bytes=bytes;
 c->access_original=access;c->native_class_original=tag;
 if(!slcli_native_clock_read(&c->before)||!alias_same(c,a)||
  c->before.tick<c->predecessor_tick||c->before.tick>=c->expires)return 0;
 c->map_args[0]=(uintptr_t)backing->backing;c->map_args[1]=access;c->map_args[4]=bytes;
 /* Parent original reserved frontier precedes the native effect. No later
  * clock/diagnostic failure erases a returned mapping or its raw cells. */
 view->effect_flags|=2;c->map_called=1;
 c->view=MapViewOfFile(backing->backing,access,0,0,bytes);
 c->map_returned=1;view->effect_flags|=4;
 if(c->view)view->effect_flags|=8;
 else c->map_error=GetLastError();
 if(!slcli_native_clock_read(&c->after)||!alias_same(c,a)||
  c->after.tick<c->before.tick||c->after.tick>=c->expires||!c->view)return 0;
 if(!slcli_native_clock_read(&c->query_before)||!alias_same(c,a)||
  c->query_before.tick<c->after.tick||c->query_before.tick>=c->expires)return 0;
 c->query_args[0]=(uintptr_t)c->view;c->query_args[1]=(uintptr_t)&c->geometry;c->query_args[2]=sizeof(c->geometry);
 c->query_called=1;c->query_status=VirtualQuery(c->view,&c->geometry,sizeof(c->geometry));
 c->query_returned=1;if(!c->query_status)c->query_error=GetLastError();
 if(!slcli_native_clock_read(&c->query_after)||!alias_same(c,a)||c->query_after.tick<c->query_before.tick||
  c->query_after.tick>=c->expires||c->query_status!=sizeof(c->geometry)||
  c->geometry.BaseAddress!=c->view||c->geometry.AllocationBase!=c->view||
  c->geometry.RegionSize!=bytes||c->geometry.State!=MEM_COMMIT||
  c->geometry.Type!=MEM_MAPPED||c->geometry.Protect!=(access==FILE_MAP_READ?PAGE_READONLY:PAGE_READWRITE))return 0;
 c->qualified=1;return 1;
}
int slcli_capture_alias_windows_map_original(struct slcli_capture_alias_windows *c,
 struct slcli_arena *a,const struct slcli_binding_originals *b,
 struct slcli_native_reference *section,struct slcli_native_reference *view,
 const struct slcli_preentry_backing_windows *backing,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,size_t bytes) {
 return map_original(c,a,b,section,view,backing,d,caller,caller_bytes,prior,bytes,
  FILE_MAP_READ,SLCLI_NATIVE_ORIGINAL_CAPTURE_ALIAS);
}
int slcli_capture_initializer_windows_map_original(struct slcli_capture_alias_windows *c,
 struct slcli_arena *a,const struct slcli_binding_originals *b,
 struct slcli_native_reference *section,struct slcli_native_reference *view,
 const struct slcli_preentry_backing_windows *backing,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,size_t bytes) {
 return map_original(c,a,b,section,view,backing,d,caller,caller_bytes,prior,bytes,
  FILE_MAP_READ|FILE_MAP_WRITE,SLCLI_NATIVE_ORIGINAL_CAPTURE_INITIALIZER_VIEW);
}
int slcli_capture_initializer_windows_unmap_original(struct slcli_capture_alias_windows *c,
 struct slcli_arena *a) {
 struct slcli_native_reference *r;
 if(!a||!c||!slcli_arena_live_block(a,c,sizeof(*c))||c->unmap_attempted||
  c->native_class_original!=SLCLI_NATIVE_ORIGINAL_CAPTURE_INITIALIZER_VIEW||
  c->access_original!=(FILE_MAP_READ|FILE_MAP_WRITE)||!c->map_called||!c->map_returned||!c->view||
  (c->initialize_started&&!c->initialize_returned)||!c->binding_original||
  !slcli_arena_live_range(a,c->binding_original,sizeof(*c->binding_original))||
  !(r=c->view_reference)||!row_in(a,c->binding_original,r)||
  r->native_original!=c||r->native_class!=SLCLI_NATIVE_ORIGINAL_CAPTURE_INITIALIZER_VIEW||
  r->kind!=9||(r->effect_flags&15)!=15)return 0;
 c->unmap_attempted=1;
 /* Preserve both useful-work failure and disposal. Failed before/after clocks
  * neither suppress the once-only real unmap nor manufacture row retirement. */
 c->unmap_before_result=slcli_native_clock_read(&c->unmap_before);
 c->unmap_args[0]=(uintptr_t)c->view;r->state=2;r->effect_flags|=16|32;
 c->unmap_called=1;c->unmap_status=UnmapViewOfFile(c->view);
 c->unmap_returned=1;r->effect_flags|=64;
 if(!c->unmap_status)c->unmap_error=GetLastError();
 else c->unmapped=1;
 c->unmap_after_result=slcli_native_clock_read(&c->unmap_after);
 return c->unmapped!=0;
}
#endif
