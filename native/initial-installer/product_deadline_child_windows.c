#include "product_deadline_child_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const volatile void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);
}
static int equal(const void *a,const void *b,size_t n) {
 const unsigned char *x=a,*y=b;size_t i;for(i=0;i<n;i++)if(x[i]!=y[i])return 0;return 1;
}
static int same(const struct slcli_product_windows_deadline_acquisition *c) {
 struct slcli_product_storage_windows *s=c->storage_original;
 return s&&!s->failed&&s->prefix_prepared==1&&!s->storage_constructed&&!s->origin_attempted&&
 s->deadline_acquisition_original==c&&s->source_original==c->initial_source_original&&
 s->source_original->initial_origin_original==c->initial_origin_original&&
 c->initial_origin_original->tick==c->initial_origin_tick&&
 s->source_original->initial_expires==c->initial_expires&&
 slcli_native_clock_windows_matches_original(c->initial_origin_original)&&
 slcli_memory_owner_mutable(&s->memory_original)&&s->arena_original.owner==&s->memory_original&&
 slcli_arena_live_block(&s->arena_original,c,sizeof(*c));
}
static int clock_cut(struct slcli_product_windows_deadline_acquisition *c,struct slcli_native_clock *out) {
 if(!slcli_native_clock_read(out)||!same(c)||out->tick<c->prior_tick||out->tick>=c->initial_expires)return 0;
 c->prior_tick=out->tick;return 1;
}
static int before(struct slcli_product_windows_deadline_acquisition *c,size_t i) {
 struct slcli_build_windows_call *r=&c->calls[i];
 if(r->called||r->returned||!clock_cut(c,&r->before))return 0;r->called=1;return 1;
}
static int after(struct slcli_product_windows_deadline_acquisition *c,size_t i) {
 return c->calls[i].returned&&clock_cut(c,&c->calls[i].after);
}
static int fail(struct slcli_product_windows_deadline_acquisition *c) {
 c->failed=1;c->storage_original->failed=1;return 0;
}
static int stats(const TOKEN_STATISTICS *a,const TOKEN_STATISTICS *b) {
 return a->TokenId.LowPart==b->TokenId.LowPart&&a->TokenId.HighPart==b->TokenId.HighPart&&
 a->AuthenticationId.LowPart==b->AuthenticationId.LowPart&&a->AuthenticationId.HighPart==b->AuthenticationId.HighPart&&
 a->TokenType==b->TokenType&&a->ImpersonationLevel==b->ImpersonationLevel&&
 a->DynamicCharged==b->DynamicCharged&&a->DynamicAvailable==b->DynamicAvailable&&
 a->GroupCount==b->GroupCount&&a->PrivilegeCount==b->PrivilegeCount&&
 a->ModifiedId.LowPart==b->ModifiedId.LowPart&&a->ModifiedId.HighPart==b->ModifiedId.HighPart;
}
#define RAW(call) do{if(!before(c,i))return fail(c);r=&c->calls[i];r->raw=(uintptr_t)(call);r->returned=1;}while(0)
#define BOOLRAW(call) do{RAW(call);if(!r->raw){r->error=GetLastError();r->error_observed=1;}}while(0)
#define END() do{if(!after(c,i))return fail(c);}while(0)
int slcli_product_windows_deadline_acquire_original(struct slcli_product_storage_windows *s) {
 struct slcli_product_windows_deadline_acquisition *c;struct slcli_extent *e;
 struct slcli_build_windows_call *r;size_t i,n;uintptr_t sid,base;
 uint64_t page,remainder;NTSTATUS status;
 if(!s||s->failed||s->prefix_prepared!=1||s->storage_constructed||s->origin_attempted||
 s->deadline_acquisition_original||s->deadline_acquisition_entry_original.started||
 !slcli_memory_owner_mutable(&s->memory_original)||s->arena_original.owner!=&s->memory_original||
 !s->source_original||!slcli_native_clock_windows_matches_original(s->source_original->initial_origin_original)||
 !slcli_build_principal_windows_matches_original(s->effective_original)||
 !slcli_build_principal_windows_matches_original(s->primary_original)||
 s->arena_original.extent>=s->memory_original.count||s->arena_original.extent>=256)return 0;
 e=&s->memory_original.extents[s->arena_original.extent];
 /* Covers allocator header/padding/split writes BEFORE allocation. The fixed
  * route export and ALL initial inputs must be outside the complete arena. */
 if(e->state!=SLCLI_EXTENT_LIVE||e->acquisition!=s->arena_original.acquisition||
 !e->base||!e->reserved||e->reserved>SIZE_MAX||
 !apart((void *)e->base,(size_t)e->reserved,slcli_product_route_cell_v1,SLCLI_PRODUCT_ROUTE_BYTES)||
 !apart(s,sizeof(*s),slcli_product_route_cell_v1,SLCLI_PRODUCT_ROUTE_BYTES)||
 !slcli_product_windows_initial_source_excludes_output(s->source_original,(void *)e->base,(size_t)e->reserved)||
 !slcli_arena_allocation_bytes(sizeof(*c)))return 0;
 if(!slcli_native_clock_read(&s->deadline_acquisition_entry_original)||
 s->deadline_acquisition_entry_original.tick<s->prior_tick||
 s->deadline_acquisition_entry_original.tick>=s->source_original->initial_expires){s->failed=1;return 0;}
 c=slcli_arena_alloc(&s->arena_original,sizeof(*c));if(!c){s->failed=1;return 0;}
 if(!slcli_product_storage_windows_prefix_output_away_original(s,c,sizeof(*c))){s->failed=1;return 0;}
 for(i=0;i<sizeof(*c);i++)((unsigned char *)c)[i]=0;
 s->deadline_acquisition_original=c;c->storage_original=s;c->initial_source_original=s->source_original;
 c->initial_origin_original=s->source_original->initial_origin_original;
 c->initial_origin_tick=c->initial_origin_original->tick;c->initial_expires=s->source_original->initial_expires;
 c->prior_tick=s->deadline_acquisition_entry_original.tick;c->started=1;
 if(!clock_cut(c,&c->entry))return fail(c);
 for(i=0;i<SLCLI_PRODUCT_ROUTE_BYTES;i++)c->route_before[i]=slcli_product_route_cell_v1[i];
 if(!slcli_product_windows_route_decode(&c->route,c->route_before,SLCLI_PRODUCT_ROUTE_BYTES)||
 !clock_cut(c,&c->route_cut))return fail(c);
 i=SLCLI_PRODUCT_CHILD_SELF_PID;RAW(GetCurrentProcessId());END();
 if(r->raw!=s->primary_original->process_original)return fail(c);
 i=SLCLI_PRODUCT_CHILD_SELF_TID;RAW(GetCurrentThreadId());END();
 if(r->raw!=s->primary_original->thread_original)return fail(c);
 i=SLCLI_PRODUCT_CHILD_SELF_PROCESS;RAW(GetCurrentProcess());c->self_process_original=(HANDLE)r->raw;END();
 if(!c->self_process_original)return fail(c);
 i=SLCLI_PRODUCT_CHILD_SECTION_FLAGS;r=&c->calls[i];r->args[0]=(uintptr_t)c->route.readonly_section;r->args[1]=(uintptr_t)&c->section_flags;
 BOOLRAW(GetHandleInformation(c->route.readonly_section,&c->section_flags));END();
 if(!r->raw||c->section_flags!=HANDLE_FLAG_INHERIT)return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_FLAGS;r=&c->calls[i];r->args[0]=(uintptr_t)c->route.parent_process;r->args[1]=(uintptr_t)&c->parent_flags;
 BOOLRAW(GetHandleInformation(c->route.parent_process,&c->parent_flags));END();
 if(!r->raw||c->parent_flags!=HANDLE_FLAG_INHERIT)return fail(c);
 i=SLCLI_PRODUCT_CHILD_SECTION_ACCESS;r=&c->calls[i];r->args[0]=(uintptr_t)c->route.readonly_section;
 r->args[1]=ObjectBasicInformation;r->args[2]=(uintptr_t)&c->section_access;
 r->args[3]=sizeof(c->section_access);r->args[4]=(uintptr_t)&c->section_access_bytes;
 if(!before(c,i))return fail(c);status=NtQueryObject(c->route.readonly_section,ObjectBasicInformation,&c->section_access,sizeof(c->section_access),&c->section_access_bytes);
 r->raw=(uint32_t)status;r->returned=1;END();
 if(status!=0||c->section_access_bytes!=sizeof(c->section_access)||c->section_access.GrantedAccess!=FILE_MAP_READ)return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_ACCESS;r=&c->calls[i];r->args[0]=(uintptr_t)c->route.parent_process;
 r->args[1]=ObjectBasicInformation;r->args[2]=(uintptr_t)&c->parent_access;
 r->args[3]=sizeof(c->parent_access);r->args[4]=(uintptr_t)&c->parent_access_bytes;
 if(!before(c,i))return fail(c);status=NtQueryObject(c->route.parent_process,ObjectBasicInformation,&c->parent_access,sizeof(c->parent_access),&c->parent_access_bytes);
 r->raw=(uint32_t)status;r->returned=1;END();
 if(status!=0||c->parent_access_bytes!=sizeof(c->parent_access)||
 c->parent_access.GrantedAccess!=(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE))return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_PID;r=&c->calls[i];r->args[0]=(uintptr_t)c->route.parent_process;
 BOOLRAW(GetProcessId(c->route.parent_process));END();if(!r->raw)return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_TIMES;r=&c->calls[i];r->args[0]=(uintptr_t)c->route.parent_process;
 r->args[1]=(uintptr_t)&c->parent_creation;r->args[2]=(uintptr_t)&c->parent_exit;r->args[3]=(uintptr_t)&c->parent_kernel;r->args[4]=(uintptr_t)&c->parent_user_time;
 BOOLRAW(GetProcessTimes(c->route.parent_process,&c->parent_creation,&c->parent_exit,&c->parent_kernel,&c->parent_user_time));END();if(!r->raw)return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_RELATION;r=&c->calls[i];r->args[0]=(uintptr_t)c->self_process_original;
 r->args[1]=ProcessBasicInformation;r->args[2]=(uintptr_t)c->pbi;r->args[3]=sizeof(c->pbi);r->args[4]=(uintptr_t)&c->pbi_bytes;
 if(!before(c,i))return fail(c);status=NtQueryInformationProcess(c->self_process_original,ProcessBasicInformation,c->pbi,sizeof(c->pbi),&c->pbi_bytes);
 r->raw=(uint32_t)status;r->returned=1;END();
 if(status!=0||c->pbi_bytes!=sizeof(c->pbi)||c->pbi[4]!=c->calls[SLCLI_PRODUCT_CHILD_SELF_PID].raw||
 c->pbi[5]!=c->calls[SLCLI_PRODUCT_CHILD_PARENT_PID].raw)return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_LIVE;r=&c->calls[i];r->args[0]=(uintptr_t)c->route.parent_process;
 RAW(WaitForSingleObject(c->route.parent_process,0));
 if(r->raw==WAIT_FAILED){r->error=GetLastError();r->error_observed=1;}END();if(r->raw!=WAIT_TIMEOUT)return fail(c);
 i=SLCLI_PRODUCT_CHILD_MAP;r=&c->calls[i];r->args[0]=(uintptr_t)c->route.readonly_section;
 r->args[1]=FILE_MAP_READ;r->args[4]=SLCLI_BOOTSTRAP_TRANSPORT_BYTES;
 BOOLRAW(MapViewOfFile(c->route.readonly_section,FILE_MAP_READ,0,0,SLCLI_BOOTSTRAP_TRANSPORT_BYTES));
 c->view_original=(void *)r->raw;END();if(!c->view_original)return fail(c);
 page=s->pages_original.dwPageSize;if(!page||SLCLI_BOOTSTRAP_TRANSPORT_BYTES>UINT64_MAX-page+1)return fail(c);
 remainder=SLCLI_BOOTSTRAP_TRANSPORT_BYTES%page;
 c->physical_bytes=remainder?SLCLI_BOOTSTRAP_TRANSPORT_BYTES+page-remainder:SLCLI_BOOTSTRAP_TRANSPORT_BYTES;
 i=SLCLI_PRODUCT_CHILD_GEOMETRY;r=&c->calls[i];r->args[0]=(uintptr_t)c->view_original;
 r->args[1]=(uintptr_t)&c->geometry;r->args[2]=sizeof(c->geometry);
 BOOLRAW(VirtualQuery(c->view_original,&c->geometry,sizeof(c->geometry)));END();
 if(r->raw!=sizeof(c->geometry)||c->geometry.BaseAddress!=c->view_original||
 c->geometry.AllocationBase!=c->view_original||c->geometry.RegionSize!=c->physical_bytes||
 c->geometry.State!=MEM_COMMIT||c->geometry.Type!=MEM_MAPPED||c->geometry.Protect!=PAGE_READONLY)return fail(c);
 for(i=0;i<SLCLI_BOOTSTRAP_TRANSPORT_BYTES;i++)c->body_original[i]=((const volatile unsigned char *)c->view_original)[i];
 for(i=0;i<SLCLI_BOOTSTRAP_TRANSPORT_BYTES;i++)c->readback_original[i]=((const volatile unsigned char *)c->view_original)[i];
 if(!equal(c->body_original,c->readback_original,SLCLI_BOOTSTRAP_TRANSPORT_BYTES)||
 !slcli_product_windows_deadline_decode(&c->values,c->body_original,SLCLI_PRODUCT_DEADLINE_BYTES))return fail(c);
 for(i=SLCLI_PRODUCT_DEADLINE_BYTES;i<SLCLI_BOOTSTRAP_TRANSPORT_BYTES;i++)if(c->body_original[i])return fail(c);
 if(c->values.parent_pid!=c->calls[SLCLI_PRODUCT_CHILD_PARENT_PID].raw||
 c->values.parent_creation[0]!=c->parent_creation.dwLowDateTime||
 c->values.parent_creation[1]!=c->parent_creation.dwHighDateTime||
 c->values.qpc_frequency!=(uint64_t)c->initial_origin_original->native_windows_frequency.QuadPart||
 !clock_cut(c,&c->body_cut))return fail(c);
 if(c->body_cut.tick>=c->values.active_phase_expires||c->body_cut.tick>=c->values.enclosing_expires)return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_TOKEN;r=&c->calls[i];r->args[0]=(uintptr_t)c->route.parent_process;
 r->args[1]=TOKEN_QUERY;r->args[2]=(uintptr_t)&c->parent_token_original;
 BOOLRAW(OpenProcessToken(c->route.parent_process,TOKEN_QUERY,&c->parent_token_original));END();
 if(!r->raw||!c->parent_token_original||c->parent_token_original==INVALID_HANDLE_VALUE)return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_USER;r=&c->calls[i];r->args[0]=(uintptr_t)c->parent_token_original;
 r->args[1]=TokenUser;r->args[2]=(uintptr_t)&c->parent_user;r->args[3]=sizeof(c->parent_user);r->args[4]=(uintptr_t)&c->parent_user_bytes;
 BOOLRAW(GetTokenInformation(c->parent_token_original,TokenUser,&c->parent_user,sizeof(c->parent_user),&c->parent_user_bytes));END();
 base=(uintptr_t)&c->parent_user;sid=(uintptr_t)c->parent_user.header.User.Sid;
 if(!r->raw||c->parent_user_bytes<sizeof(TOKEN_USER)||c->parent_user_bytes>sizeof(c->parent_user)||
 sid<base+sizeof(TOKEN_USER)||sid>base+c->parent_user_bytes||base+c->parent_user_bytes-sid<8)return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_STATISTICS;r=&c->calls[i];r->args[0]=(uintptr_t)c->parent_token_original;
 r->args[1]=TokenStatistics;r->args[2]=(uintptr_t)&c->parent_statistics;r->args[3]=sizeof(c->parent_statistics);r->args[4]=(uintptr_t)&c->parent_statistics_bytes;
 BOOLRAW(GetTokenInformation(c->parent_token_original,TokenStatistics,&c->parent_statistics,sizeof(c->parent_statistics),&c->parent_statistics_bytes));END();
 if(!r->raw||c->parent_statistics_bytes!=sizeof(c->parent_statistics)||c->parent_statistics.TokenType!=TokenPrimary)return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_SID_VALID;r=&c->calls[i];r->args[0]=sid;RAW(IsValidSid((PSID)sid));END();if(!r->raw)return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_SID_LENGTH;r=&c->calls[i];r->args[0]=sid;RAW(GetLengthSid((PSID)sid));END();n=(size_t)r->raw;
 if(n<8||n>SECURITY_MAX_SID_SIZE||n>base+c->parent_user_bytes-sid||
 n!=s->primary_original->expected_original->windows_sid_bytes||
 !equal((void *)sid,s->primary_original->user_original.header.User.Sid,n)||
 c->parent_statistics.AuthenticationId.LowPart!=s->primary_original->statistics_original.AuthenticationId.LowPart||
 c->parent_statistics.AuthenticationId.HighPart!=s->primary_original->statistics_original.AuthenticationId.HighPart)return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_STATISTICS_AFTER;r=&c->calls[i];r->args[0]=(uintptr_t)c->parent_token_original;
 r->args[1]=TokenStatistics;r->args[2]=(uintptr_t)&c->parent_statistics_after;r->args[3]=sizeof(c->parent_statistics_after);r->args[4]=(uintptr_t)&c->parent_statistics_after_bytes;
 BOOLRAW(GetTokenInformation(c->parent_token_original,TokenStatistics,&c->parent_statistics_after,sizeof(c->parent_statistics_after),&c->parent_statistics_after_bytes));END();
 if(!r->raw||c->parent_statistics_after_bytes!=sizeof(c->parent_statistics_after)||!stats(&c->parent_statistics,&c->parent_statistics_after))return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_TIMES_AFTER;r=&c->calls[i];r->args[0]=(uintptr_t)c->route.parent_process;
 r->args[1]=(uintptr_t)&c->parent_creation_after;r->args[2]=(uintptr_t)&c->parent_exit_after;r->args[3]=(uintptr_t)&c->parent_kernel_after;r->args[4]=(uintptr_t)&c->parent_user_time_after;
 BOOLRAW(GetProcessTimes(c->route.parent_process,&c->parent_creation_after,&c->parent_exit_after,&c->parent_kernel_after,&c->parent_user_time_after));END();
 if(!r->raw||c->parent_creation.dwLowDateTime!=c->parent_creation_after.dwLowDateTime||c->parent_creation.dwHighDateTime!=c->parent_creation_after.dwHighDateTime)return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_PID_AFTER;r=&c->calls[i];r->args[0]=(uintptr_t)c->route.parent_process;BOOLRAW(GetProcessId(c->route.parent_process));END();
 if(r->raw!=c->values.parent_pid)return fail(c);
 i=SLCLI_PRODUCT_CHILD_SELF_PID_AFTER;RAW(GetCurrentProcessId());END();if(r->raw!=s->primary_original->process_original)return fail(c);
 i=SLCLI_PRODUCT_CHILD_SELF_TID_AFTER;RAW(GetCurrentThreadId());END();if(r->raw!=s->primary_original->thread_original)return fail(c);
 i=SLCLI_PRODUCT_CHILD_PARENT_LIVE_AFTER;r=&c->calls[i];r->args[0]=(uintptr_t)c->route.parent_process;
 RAW(WaitForSingleObject(c->route.parent_process,0));
 if(r->raw==WAIT_FAILED){r->error=GetLastError();r->error_observed=1;}END();if(r->raw!=WAIT_TIMEOUT)return fail(c);
 for(i=0;i<SLCLI_BOOTSTRAP_TRANSPORT_BYTES;i++)c->final_body_original[i]=((const volatile unsigned char *)c->view_original)[i];
 for(i=0;i<SLCLI_PRODUCT_ROUTE_BYTES;i++)c->route_after[i]=slcli_product_route_cell_v1[i];
 if(!equal(c->body_original,c->final_body_original,SLCLI_BOOTSTRAP_TRANSPORT_BYTES)||
 !equal(c->route_before,c->route_after,SLCLI_PRODUCT_ROUTE_BYTES)||!clock_cut(c,&c->finish)||
 c->finish.tick>=c->values.active_phase_expires||c->finish.tick>=c->values.enclosing_expires)return fail(c);
 s->prior_tick=c->finish.tick;c->native_capture_completed=1;return 1;
}
#undef RAW
#undef BOOLRAW
#undef END
#endif
