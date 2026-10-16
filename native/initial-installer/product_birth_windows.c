#include "product_birth_windows.h"
#include "../host-service/arena_layout.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);
}
static int text(const WCHAR *p,size_t n,int env) {
 size_t i;if(!p||n<(env?2u:1u)||n>SIZE_MAX/sizeof(*p)||p[n-1]||(env&&p[n-2]))return 0;
 for(i=0;i+1<n;i++)if(!p[i]&&(!env||(i+2<n&&!p[i+1])))return 0;return 1;
}
static int equal(const void *v,const void *w,size_t n) {
 const unsigned char *a=v,*b=w;size_t i;for(i=0;i<n;i++)if(a[i]!=b[i])return 0;return 1;
}
/* No allocator effect: protect the complete current FREE write regions,
 * including headers/alignment/split tails, while allowing genuine held-file
 * principals already resident in LIVE blocks of this SAME arena. This body
 * only allocates; owning serialization excludes free/realloc/foreign writers. */
static int free_away(struct slcli_arena *a,const struct slcli_build_held_file_windows *file) {
 const struct slcli_extent *e;const struct slcli_arena_block_header *b;uint64_t offset=0;
 if(!slcli_arena_live_range(a,file->principal_original,sizeof(*file->principal_original)))return 0;
 e=&a->owner->extents[a->extent];
 for(;;) {
 if(offset%16||offset>e->reserved-sizeof(*b))return 0;
 b=(const struct slcli_arena_block_header *)(e->base+(uintptr_t)offset);
 if(b->bytes<48||b->bytes%16||b->bytes>e->reserved-offset||
 (b->state!=SLCLI_ARENA_BLOCK_FREE&&b->state!=SLCLI_ARENA_BLOCK_LIVE)||
 (b->state==SLCLI_ARENA_BLOCK_FREE&&b->requested)||
 (b->state==SLCLI_ARENA_BLOCK_LIVE&&(!b->requested||b->requested>b->bytes-sizeof(*b)))||
 (!b->next&&offset+b->bytes!=e->reserved)||
 (b->next&&(b->next!=offset+b->bytes||b->next>e->reserved-sizeof(*b))))return 0;
 if(offset>=a->allocation_floor&&b->state==SLCLI_ARENA_BLOCK_FREE&&
 !slcli_build_held_file_windows_output_away_original(file,b,(size_t)b->bytes))return 0;
 if(!b->next)return 1;offset=b->next;
 }
}
static unsigned word(const unsigned char *p) {return (unsigned)p[0]|((unsigned)p[1]<<8);}
static uint32_t dword(const unsigned char *p) {
 return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
/* Bound every self-relative native dereference before SD validation/length.
 * This is geometry, not ACL semantics or a principal/source authority. */
static int descriptor(const void *v,size_t n) {
 const unsigned char *p=v;size_t i,at,extent,j,cursor,count,ace;
 if(!p||(uintptr_t)p%4||n<20||!(word(p+2)&SE_SELF_RELATIVE))return 0;
 for(i=4;i<20;i+=4) {
 at=dword(p+i);if(!at)continue;
 if(at<20||at%4||at>n||n-at<8)return 0;
 extent=i<12?8u+4u*p[at+1]:word(p+at+2);
 if(extent<8||extent>n-at)return 0;
 if(i>=12) {
 count=word(p+at+4);cursor=8;
 for(j=0;j<count;j++) {
 if(cursor>extent||extent-cursor<4)return 0;
 ace=word(p+at+cursor+2);if(ace<4||ace%4||ace>extent-cursor)return 0;cursor+=ace;
 }
 }
 }
 return 1;
}
static int same(const struct slcli_product_windows_birth *c) {
 struct slcli_product_storage_windows *s=c->storage_original;
 return s&&!s->failed&&s->storage_constructed==1&&s->birth_original==c&&
 s->deadline_original==c->deadline_original&&c->deadline_original->origin==c->origin_original&&
 c->origin_original->tick==c->origin_tick&&c->deadline_original->expires==c->enclosing_expires&&
 c->phase_original&&c->phase_original->expires==c->expires&&c->expires<=c->enclosing_expires&&
 slcli_source_deadline_original_matches(&s->arena_original,c->phase_original,s->caller_original)&&
 slcli_source_deadline_original_matches(&s->arena_original,c->deadline_original,s->caller_original)&&
 slcli_arena_live_block(&s->arena_original,c,sizeof(*c))&&
 slcli_arena_live_block(&s->arena_original,c->source_original,sizeof(*c->source_original))&&
 equal(c->source_original,&c->source_snapshot,sizeof(c->source_snapshot))&&
 slcli_build_held_file_windows_matches_original(c->source_original->image_original)&&
 slcli_build_principal_windows_issuing_relation_original(
 c->source_original->image_original->principal_original,s->primary_original)&&
 slcli_build_principal_windows_matches_original(s->primary_original)&&
 s->deadline_publication_original&&s->deadline_publication_original->publication_completed==1&&
 !s->deadline_publication_original->failed&&s->deadline_publication_original->phase_original==c->phase_original&&
 c->phase_original->origin==s->deadline_publication_original->phase_origin_original&&
 c->phase_original->origin->tick==s->deadline_publication_original->phase_origin_tick&&
 s->deadline_publication_original->values.enclosing_expires==c->enclosing_expires&&
 s->deadline_publication_original->values.active_phase_expires==c->expires&&s->transport_original->writer_unmapped==1&&
 s->transport_original->writable_closed==1;
}
static int cut(struct slcli_product_windows_birth *c,struct slcli_native_clock *out) {
 if(!slcli_native_clock_read(out)||!same(c)||out->tick<c->prior_tick||out->tick>=c->expires)return 0;
 c->prior_tick=out->tick;return 1;
}
static int begin(struct slcli_product_windows_birth *c,size_t i) {
 struct slcli_build_windows_call *r=&c->calls[i];
 if(r->called||r->returned||!cut(c,&r->before))return 0;r->called=1;return 1;
}
static int end(struct slcli_product_windows_birth *c,size_t i) {
 return c->calls[i].returned&&cut(c,&c->calls[i].after);
}
static int fail(struct slcli_product_windows_birth *c) {c->failed=1;c->storage_original->failed=1;return 0;}
static int replay(const struct slcli_product_windows_birth *c,size_t index,
 const uintptr_t *args,size_t count,uint64_t *prior,int size_query) {
 const struct slcli_build_windows_call *r=&c->calls[index];size_t i;
 if(r->called!=1||r->returned!=1||!slcli_native_clock_windows_matches_original(&r->before)||
 !slcli_native_clock_windows_matches_original(&r->after)||r->before.tick<*prior||
 r->before.tick>=c->expires||r->after.tick<r->before.tick||r->after.tick>=c->expires)return 0;
 if(size_query) {if(r->raw||r->error_observed!=1||r->error!=ERROR_INSUFFICIENT_BUFFER)return 0;}
 else if(!r->raw||r->error||r->error_observed)return 0;
 for(i=0;i<12;i++)if(r->args[i]!=(i<count?args[i]:0))return 0;
 *prior=r->after.tick;return 1;
}
int slcli_product_windows_birth_matches_original(const struct slcli_product_windows_birth *c) {
 const struct slcli_product_windows_birth_source *s;const void *sd[2];size_t bytes[2],i,j;
 uintptr_t a[10]={0};uint64_t prior;
 if(!c||c->birth_captured!=1||c->failed||c->child_effects_possible!=1||!same(c)||
 !slcli_native_clock_windows_matches_original(&c->entry)||!slcli_native_clock_windows_matches_original(&c->finish)||
 c->entry.tick<c->storage_original->birth_entry_original.tick||c->entry.tick>=c->expires||
 !c->command_original||!c->attributes_original)return 0;
 s=&c->source_snapshot;prior=c->entry.tick;
 if(!slcli_native_clock_windows_matches_original(&c->storage_original->birth_entry_original)||
 !slcli_arena_live_block(&c->storage_original->arena_original,c->command_original,s->command_units*sizeof(WCHAR))||
 !slcli_arena_live_block(&c->storage_original->arena_original,c->attributes_original,s->attribute_capacity)||
 c->process_security.nLength!=sizeof(c->process_security)||c->process_security.lpSecurityDescriptor!=s->process_descriptor_original||c->process_security.bInheritHandle||
 c->thread_security.nLength!=sizeof(c->thread_security)||c->thread_security.lpSecurityDescriptor!=s->thread_descriptor_original||c->thread_security.bInheritHandle||
 c->startup_original.StartupInfo.cb!=sizeof(c->startup_original)||c->startup_original.StartupInfo.dwFlags!=STARTF_USESTDHANDLES||
 c->startup_original.StartupInfo.hStdInput!=c->inherited_original[0]||
 c->startup_original.StartupInfo.hStdOutput!=c->inherited_original[1]||
 c->startup_original.StartupInfo.hStdError!=c->inherited_original[2]||
 c->startup_original.lpAttributeList!=c->attributes_original)return 0;
 for(i=0;i<5;i++) {
 if(c->inherited_original[i]!=(i<3?s->standard_original[i]:i==3?c->storage_original->transport_original->readonly:c->storage_original->transport_original->parent)||
 c->flags_original[i]!=HANDLE_FLAG_INHERIT)return 0;
 a[0]=(uintptr_t)c->inherited_original[i];a[1]=(uintptr_t)&c->flags_original[i];
 if(!replay(c,i,a,2,&prior,0))return 0;
 }
 sd[0]=s->process_descriptor_original;sd[1]=s->thread_descriptor_original;
 bytes[0]=s->process_descriptor_bytes;bytes[1]=s->thread_descriptor_bytes;
 for(j=0;j<2;j++) {
 a[0]=(uintptr_t)sd[j];
 if(!descriptor(sd[j],bytes[j])||!replay(c,SLCLI_PRODUCT_BIRTH_SD_VALID+j,a,1,&prior,0)||
 !replay(c,SLCLI_PRODUCT_BIRTH_SD_LENGTH+j,a,1,&prior,0)||c->calls[SLCLI_PRODUCT_BIRTH_SD_LENGTH+j].raw!=bytes[j])return 0;
 }
 if(!replay(c,SLCLI_PRODUCT_BIRTH_CREATOR_PID,a,0,&prior,0)||
 c->calls[SLCLI_PRODUCT_BIRTH_CREATOR_PID].raw!=c->storage_original->primary_original->process_original||
 !replay(c,SLCLI_PRODUCT_BIRTH_CREATOR_TID,a,0,&prior,0)||
 c->calls[SLCLI_PRODUCT_BIRTH_CREATOR_TID].raw!=c->storage_original->primary_original->thread_original)return 0;
 a[0]=0;a[1]=1;a[2]=0;a[3]=(uintptr_t)&c->attribute_required;
 if(!replay(c,SLCLI_PRODUCT_BIRTH_ATTRIBUTE_SIZE,a,4,&prior,1)||!c->attribute_required||c->attribute_required>s->attribute_capacity)return 0;
 a[0]=(uintptr_t)c->attributes_original;a[3]=(uintptr_t)&c->attribute_returned;
 if(!replay(c,SLCLI_PRODUCT_BIRTH_ATTRIBUTE_INIT,a,4,&prior,0)||c->attribute_returned>s->attribute_capacity)return 0;
 a[1]=0;a[2]=PROC_THREAD_ATTRIBUTE_HANDLE_LIST;a[3]=(uintptr_t)c->inherited_original;a[4]=sizeof(c->inherited_original);a[5]=0;a[6]=0;
 if(!replay(c,SLCLI_PRODUCT_BIRTH_ATTRIBUTE_HANDLES,a,7,&prior,0))return 0;
 a[0]=(uintptr_t)s->image_original->member_original->leaf.original;a[1]=(uintptr_t)c->command_original;
 a[2]=(uintptr_t)&c->process_security;a[3]=(uintptr_t)&c->thread_security;a[4]=TRUE;
 a[5]=CREATE_SUSPENDED|CREATE_UNICODE_ENVIRONMENT|EXTENDED_STARTUPINFO_PRESENT;
 a[6]=(uintptr_t)s->environment_original;a[7]=(uintptr_t)s->directory_original;
 a[8]=(uintptr_t)&c->startup_original.StartupInfo;a[9]=(uintptr_t)&c->process_original;
 if(!replay(c,SLCLI_PRODUCT_BIRTH_CREATE,a,10,&prior,0)||!c->process_original.hProcess||
 !c->process_original.hThread||!c->process_original.dwProcessId||!c->process_original.dwThreadId)return 0;
 a[0]=(uintptr_t)c->process_original.hProcess;a[1]=(uintptr_t)&c->creation_original;a[2]=(uintptr_t)&c->exit_original;
 a[3]=(uintptr_t)&c->kernel_original;a[4]=(uintptr_t)&c->user_original;
 if(!replay(c,SLCLI_PRODUCT_BIRTH_TIMES,a,5,&prior,0)||!replay(c,SLCLI_PRODUCT_BIRTH_CHILD_PID,a,1,&prior,0)||
 c->calls[SLCLI_PRODUCT_BIRTH_CHILD_PID].raw!=c->process_original.dwProcessId)return 0;
 a[0]=(uintptr_t)c->process_original.hThread;
 if(!replay(c,SLCLI_PRODUCT_BIRTH_CHILD_TID,a,1,&prior,0)||c->calls[SLCLI_PRODUCT_BIRTH_CHILD_TID].raw!=c->process_original.dwThreadId)return 0;
 return c->finish.tick>=prior&&c->finish.tick<c->expires;
}
int slcli_product_windows_birth_output_away_original(
 const struct slcli_product_windows_birth *c,const void *out,size_t n) {
 const struct slcli_product_windows_birth_source *s;
 if(!c||!c->source_original||!apart(out,n,c,sizeof(*c)))return 0;s=&c->source_snapshot;
 return equal(c->source_original,s,sizeof(*s))&&apart(out,n,c->source_original,sizeof(*s))&&
 apart(out,n,s->command_original,s->command_units*sizeof(WCHAR))&&
 apart(out,n,s->environment_original,s->environment_units*sizeof(WCHAR))&&
 apart(out,n,s->directory_original,s->directory_units*sizeof(WCHAR))&&
 apart(out,n,s->process_descriptor_original,s->process_descriptor_bytes)&&
 apart(out,n,s->thread_descriptor_original,s->thread_descriptor_bytes)&&
 slcli_build_held_file_windows_output_away_original(s->image_original,out,n)&&
 (!c->command_original||apart(out,n,c->command_original,s->command_units*sizeof(WCHAR)))&&
 (!c->attributes_original||apart(out,n,c->attributes_original,s->attribute_capacity));
}
#define CALL(expr) do {if(!begin(c,i))return fail(c);r=&c->calls[i];r->raw=(uintptr_t)(expr);r->returned=1; \
 if(!r->raw){r->error=GetLastError();r->error_observed=1;}if(!end(c,i))return fail(c);}while(0)
int slcli_product_windows_birth_original(struct slcli_product_storage_windows *s,
 const struct slcli_product_windows_birth_source *src) {
 struct slcli_product_windows_birth *c;struct slcli_arena *a;struct slcli_extent *e;
 struct slcli_build_windows_call *r;size_t i,j;const void *sd[2];size_t sd_bytes[2];
 if(!s||s->failed||s->storage_constructed!=1||s->birth_original||!src||
 s->birth_entry_original.started||!s->deadline_publication_original||s->deadline_publication_original->publication_completed!=1||
 s->deadline_publication_original->failed||!slcli_memory_owner_mutable(&s->memory_original))return 0;
 a=&s->arena_original;
 /* LIVE input ranges cannot be overwritten by allocator header/padding writes.
  * Held image's complete original graph excludes every potential FREE write. */
 if(!slcli_arena_live_block(a,src,sizeof(*src))||
 src->command_units<2||src->command_units>32767||src->environment_units>SIZE_MAX/sizeof(WCHAR)||
 src->directory_units>SIZE_MAX/sizeof(WCHAR)||
 !slcli_arena_live_range(a,src->command_original,src->command_units*sizeof(WCHAR))||
 !slcli_arena_live_range(a,src->environment_original,src->environment_units*sizeof(WCHAR))||
 !slcli_arena_live_range(a,src->directory_original,src->directory_units*sizeof(WCHAR))||
 !text(src->command_original,src->command_units,0)||!text(src->environment_original,src->environment_units,1)||
 !text(src->directory_original,src->directory_units,0)||
 src->directory_units<4||!((src->directory_original[0]>='A'&&src->directory_original[0]<='Z')||
 (src->directory_original[0]>='a'&&src->directory_original[0]<='z'))||
 src->directory_original[1]!=':'||src->directory_original[2]!='\\'||
 src->process_descriptor_bytes<sizeof(SECURITY_DESCRIPTOR_RELATIVE)||
 src->thread_descriptor_bytes<sizeof(SECURITY_DESCRIPTOR_RELATIVE)||
 !slcli_arena_live_range(a,src->process_descriptor_original,src->process_descriptor_bytes)||
 !slcli_arena_live_range(a,src->thread_descriptor_original,src->thread_descriptor_bytes)||
 !descriptor(src->process_descriptor_original,src->process_descriptor_bytes)||
 !descriptor(src->thread_descriptor_original,src->thread_descriptor_bytes)||
 !src->attribute_capacity||a->extent>=s->memory_original.count||a->extent>=256)return 0;
 e=&s->memory_original.extents[a->extent];
 if(e->state!=SLCLI_EXTENT_LIVE||e->acquisition!=a->acquisition||!e->base||!e->reserved||e->reserved>SIZE_MAX||
 !slcli_build_held_file_windows_matches_original(src->image_original)||
 !free_away(a,src->image_original)||
 !slcli_build_principal_windows_issuing_relation_original(src->image_original->principal_original,s->primary_original))return 0;
 if(!slcli_source_deadline_original_matches(a,s->deadline_original,s->caller_original)||
 !slcli_source_deadline_original_matches(a,s->deadline_publication_original->phase_original,s->caller_original)||
 !slcli_native_clock_read(&s->birth_entry_original)||s->birth_entry_original.tick<s->prior_tick||
 s->birth_entry_original.tick>=s->deadline_original->expires||
 s->birth_entry_original.tick>=s->deadline_publication_original->phase_original->expires){s->failed=1;return 0;}
 c=slcli_arena_alloc(a,sizeof(*c));if(!c){s->failed=1;return 0;}
 if(!slcli_product_storage_windows_output_away_original(s,c,sizeof(*c))){s->failed=1;return 0;}
 for(i=0;i<sizeof(*c);i++)((unsigned char *)c)[i]=0;
 s->birth_original=c;c->storage_original=s;c->source_original=src;c->deadline_original=s->deadline_original;
 c->phase_original=s->deadline_publication_original->phase_original;
 for(i=0;i<sizeof(*src);i++)((unsigned char *)&c->source_snapshot)[i]=((const unsigned char *)src)[i];
 c->origin_original=s->operation_origin_original;c->origin_tick=c->origin_original->tick;
 c->enclosing_expires=c->deadline_original->expires;c->expires=c->phase_original->expires;
 c->prior_tick=s->birth_entry_original.tick;c->started=1;
 if(!cut(c,&c->entry))return fail(c);
 c->command_original=slcli_arena_alloc(a,src->command_units*sizeof(WCHAR));
 c->attributes_original=slcli_arena_alloc(a,src->attribute_capacity);
 if(!c->command_original||!c->attributes_original)return fail(c);
 for(i=0;i<src->command_units;i++)c->command_original[i]=src->command_original[i];
 for(i=0;i<3;i++)c->inherited_original[i]=src->standard_original[i];
 c->inherited_original[3]=s->transport_original->readonly;c->inherited_original[4]=s->transport_original->parent;
 for(i=0;i<5;i++) {
 if(!c->inherited_original[i]||c->inherited_original[i]==INVALID_HANDLE_VALUE)return fail(c);
 for(j=0;j<i;j++)if(c->inherited_original[i]==c->inherited_original[j])return fail(c);
 r=&c->calls[i];r->args[0]=(uintptr_t)c->inherited_original[i];r->args[1]=(uintptr_t)&c->flags_original[i];
 CALL(GetHandleInformation(c->inherited_original[i],&c->flags_original[i]));
 if(!r->raw||c->flags_original[i]!=HANDLE_FLAG_INHERIT)return fail(c);
 }
 sd[0]=src->process_descriptor_original;sd[1]=src->thread_descriptor_original;
 sd_bytes[0]=src->process_descriptor_bytes;sd_bytes[1]=src->thread_descriptor_bytes;
 for(j=0;j<2;j++) {
 i=SLCLI_PRODUCT_BIRTH_SD_VALID+j;r=&c->calls[i];r->args[0]=(uintptr_t)sd[j];
 CALL(IsValidSecurityDescriptor((PSECURITY_DESCRIPTOR)sd[j]));if(!r->raw)return fail(c);
 i=SLCLI_PRODUCT_BIRTH_SD_LENGTH+j;r=&c->calls[i];r->args[0]=(uintptr_t)sd[j];
 CALL(GetSecurityDescriptorLength((PSECURITY_DESCRIPTOR)sd[j]));if(r->raw!=sd_bytes[j])return fail(c);
 }
 i=SLCLI_PRODUCT_BIRTH_CREATOR_PID;CALL(GetCurrentProcessId());if(r->raw!=s->primary_original->process_original||r->raw!=s->transport_original->parent_pid)return fail(c);
 i=SLCLI_PRODUCT_BIRTH_CREATOR_TID;CALL(GetCurrentThreadId());if(r->raw!=s->primary_original->thread_original)return fail(c);
 i=SLCLI_PRODUCT_BIRTH_ATTRIBUTE_SIZE;r=&c->calls[i];r->args[1]=1;r->args[3]=(uintptr_t)&c->attribute_required;
 CALL(InitializeProcThreadAttributeList(NULL,1,0,&c->attribute_required));
 if(r->raw||!r->error_observed||r->error!=ERROR_INSUFFICIENT_BUFFER||!c->attribute_required||c->attribute_required>src->attribute_capacity)return fail(c);
 c->attribute_returned=src->attribute_capacity;
 i=SLCLI_PRODUCT_BIRTH_ATTRIBUTE_INIT;r=&c->calls[i];r->args[0]=(uintptr_t)c->attributes_original;r->args[1]=1;r->args[3]=(uintptr_t)&c->attribute_returned;
 CALL(InitializeProcThreadAttributeList(c->attributes_original,1,0,&c->attribute_returned));if(!r->raw||c->attribute_returned>src->attribute_capacity)return fail(c);
 i=SLCLI_PRODUCT_BIRTH_ATTRIBUTE_HANDLES;r=&c->calls[i];r->args[0]=(uintptr_t)c->attributes_original;r->args[2]=PROC_THREAD_ATTRIBUTE_HANDLE_LIST;
 r->args[3]=(uintptr_t)c->inherited_original;r->args[4]=sizeof(c->inherited_original);
 CALL(UpdateProcThreadAttribute(c->attributes_original,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,c->inherited_original,sizeof(c->inherited_original),NULL,NULL));if(!r->raw)return fail(c);
 c->process_security.nLength=sizeof(c->process_security);c->process_security.lpSecurityDescriptor=(void *)sd[0];
 c->thread_security.nLength=sizeof(c->thread_security);c->thread_security.lpSecurityDescriptor=(void *)sd[1];
 c->startup_original.StartupInfo.cb=sizeof(c->startup_original);c->startup_original.StartupInfo.dwFlags=STARTF_USESTDHANDLES;
 c->startup_original.StartupInfo.hStdInput=c->inherited_original[0];c->startup_original.StartupInfo.hStdOutput=c->inherited_original[1];c->startup_original.StartupInfo.hStdError=c->inherited_original[2];
 c->startup_original.lpAttributeList=c->attributes_original;
 i=SLCLI_PRODUCT_BIRTH_CREATE;r=&c->calls[i];r->args[0]=(uintptr_t)src->image_original->member_original->leaf.original;r->args[1]=(uintptr_t)c->command_original;
 r->args[2]=(uintptr_t)&c->process_security;r->args[3]=(uintptr_t)&c->thread_security;r->args[4]=TRUE;
 r->args[5]=CREATE_SUSPENDED|CREATE_UNICODE_ENVIRONMENT|EXTENDED_STARTUPINFO_PRESENT;
 r->args[6]=(uintptr_t)src->environment_original;r->args[7]=(uintptr_t)src->directory_original;
 r->args[8]=(uintptr_t)&c->startup_original.StartupInfo;r->args[9]=(uintptr_t)&c->process_original;
 if(!begin(c,i))return fail(c);c->child_effects_possible=1;
 r->raw=CreateProcessW(src->image_original->member_original->leaf.original,c->command_original,&c->process_security,&c->thread_security,TRUE,
 (DWORD)r->args[5],(void *)src->environment_original,src->directory_original,&c->startup_original.StartupInfo,&c->process_original);
 r->returned=1;if(!r->raw){r->error=GetLastError();r->error_observed=1;}
 if(!end(c,i)||!r->raw||!c->process_original.hProcess||!c->process_original.hThread||
 !c->process_original.dwProcessId||!c->process_original.dwThreadId)return fail(c);
 i=SLCLI_PRODUCT_BIRTH_TIMES;r=&c->calls[i];r->args[0]=(uintptr_t)c->process_original.hProcess;
 r->args[1]=(uintptr_t)&c->creation_original;r->args[2]=(uintptr_t)&c->exit_original;r->args[3]=(uintptr_t)&c->kernel_original;r->args[4]=(uintptr_t)&c->user_original;
 CALL(GetProcessTimes(c->process_original.hProcess,&c->creation_original,&c->exit_original,&c->kernel_original,&c->user_original));if(!r->raw)return fail(c);
 i=SLCLI_PRODUCT_BIRTH_CHILD_PID;r=&c->calls[i];r->args[0]=(uintptr_t)c->process_original.hProcess;
 CALL(GetProcessId(c->process_original.hProcess));if(r->raw!=c->process_original.dwProcessId)return fail(c);
 i=SLCLI_PRODUCT_BIRTH_CHILD_TID;r=&c->calls[i];r->args[0]=(uintptr_t)c->process_original.hThread;
 CALL(GetThreadId(c->process_original.hThread));if(r->raw!=c->process_original.dwThreadId)return fail(c);
 if(!cut(c,&c->finish))return fail(c);s->prior_tick=c->finish.tick;c->birth_captured=1;
 if(!slcli_product_windows_birth_matches_original(c))return fail(c);return 1;
}
#undef CALL
#endif
