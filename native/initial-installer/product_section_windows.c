#include "product_section_windows.h"
#include <limits.h>
#if defined(_WIN32)
/* Fixed private new-object policy; never a caller-selected access mask. */
#define SLCLI_PRODUCT_EFFECTIVE_SECTION (SECTION_QUERY|SECTION_MAP_READ|SECTION_MAP_WRITE|READ_CONTROL)
#define SLCLI_PRODUCT_PRIMARY_SECTION (SECTION_MAP_READ|READ_CONTROL)
static int range(const void *base,size_t n,const void *value,size_t bytes) {
 uintptr_t a=(uintptr_t)base,b=(uintptr_t)value;
 return base&&value&&n&&bytes&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-bytes&&b>=a&&b+bytes<=a+n;
}
static int equal(const void *a,const void *b,size_t n) {
 const unsigned char *x=a,*y=b;size_t i;for(i=0;i<n;i++)if(x[i]!=y[i])return 0;return 1;
}
static int apart(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);
}
static int same(struct slcli_product_section_windows *c) {
 struct slcli_product_storage_windows *s=c->storage_original;
 return s&&s->section_original==c&&s->storage_constructed==1&&!s->failed&&
 slcli_memory_owner_mutable(&s->memory_original)&&s->arena_original.owner==&s->memory_original&&
 slcli_arena_live_block(&s->arena_original,c,sizeof(*c))&&
 slcli_source_deadline_original_matches(&s->arena_original,c->deadline_original,s->caller_original)&&
 s->deadline_original==c->deadline_original&&c->deadline_original->origin==c->origin_original&&
 c->origin_original==s->operation_origin_original&&c->origin_original->tick==c->origin_tick&&
 c->deadline_original->expires==c->expires;
}
static int before(struct slcli_product_section_windows *c,size_t i) {
 return same(c)&&slcli_native_clock_read(&c->calls[i].before)&&
 c->calls[i].before.tick>=c->prior_tick&&c->calls[i].before.tick<c->expires;
}
static int after(struct slcli_product_section_windows *c,size_t i,BOOL raw) {
 struct slcli_build_windows_call *r=&c->calls[i];r->raw=(uint32_t)raw;r->returned=1;
 if(!raw){r->error=GetLastError();r->error_observed=1;}
 if(!slcli_native_clock_read(&r->after)||!same(c)||r->after.tick<r->before.tick||
 r->after.tick>=c->expires||!raw)return 0;c->prior_tick=r->after.tick;return 1;
}
static int stop(struct slcli_product_section_windows *c) {
 c->failed=1;if(c->storage_original)c->storage_original->failed=1;return 0;
}
static int ace(const unsigned char *p,size_t available,DWORD mask,const unsigned char *sid,size_t bytes) {
 const ACCESS_ALLOWED_ACE *a=(const ACCESS_ALLOWED_ACE *)p;
 return available>=8&&bytes<=USHRT_MAX-8&&available>=8+bytes&&
 a->Header.AceType==ACCESS_ALLOWED_ACE_TYPE&&!a->Header.AceFlags&&
 a->Header.AceSize==8+bytes&&a->Mask==mask&&equal(p+8,sid,bytes);
}
int slcli_product_windows_descriptor_bytes_correspond_original(const struct slcli_product_section_windows *c) {
 if(!c||!c->relative_original||!c->effective_original.expected_original||
 !c->primary_original.expected_original)return 0;
 const SECURITY_DESCRIPTOR_RELATIVE *d=(const SECURITY_DESCRIPTOR_RELATIVE *)c->relative_original;
 const ACL *a=c->observed_acl_original;
 size_t es=c->effective_original.expected_original->windows_sid_bytes;
 size_t ps=c->primary_original.expected_original->windows_sid_bytes;
 const unsigned char *acl=(const unsigned char *)a;
 if(!es||es>SECURITY_MAX_SID_SIZE||!ps||ps>SECURITY_MAX_SID_SIZE||
 c->acl_bytes!=sizeof(ACL)+16+es+ps||
 !range(&c->effective_original.user_original,sizeof(c->effective_original.user_original),
 c->effective_original.user_original.header.User.Sid,es)||
 !range(&c->primary_original.user_original,sizeof(c->primary_original.user_original),
 c->primary_original.user_original.header.User.Sid,ps))return 0;
 if(!c->relative_bytes||c->relative_bytes>c->relative_capacity||
 c->relative_bytes<sizeof(*d)||!d->Owner||d->Owner>=c->relative_bytes||
 !d->Dacl||d->Dacl>=c->relative_bytes||d->Group||d->Sacl||d->Revision!=SECURITY_DESCRIPTOR_REVISION||
 d->Sbz1||d->Control!=(SE_SELF_RELATIVE|SE_DACL_PRESENT|SE_DACL_PROTECTED)||
 c->control!=d->Control||c->revision!=SECURITY_DESCRIPTOR_REVISION||
 c->owner_defaulted||!c->dacl_present||c->dacl_defaulted||
 c->observed_owner_original!=(void *)(c->relative_original+d->Owner)||
 a!=(const ACL *)(c->relative_original+d->Dacl)||
 !range(c->relative_original,c->relative_bytes,c->observed_owner_original,es)||
 !range(c->relative_original,c->relative_bytes,a,c->acl_bytes)||
 !equal(c->observed_owner_original,c->effective_original.user_original.header.User.Sid,es))return 0;
 if(a->AclRevision!=ACL_REVISION||a->Sbz1||a->Sbz2||a->AceCount!=2||a->AclSize!=c->acl_bytes)return 0;
 return ace(acl+sizeof(ACL),c->acl_bytes-sizeof(ACL),SLCLI_PRODUCT_EFFECTIVE_SECTION,
 c->effective_original.user_original.header.User.Sid,es)&&
 ace(acl+sizeof(ACL)+8+es,c->acl_bytes-sizeof(ACL)-8-es,SLCLI_PRODUCT_PRIMARY_SECTION,
 c->primary_original.user_original.header.User.Sid,ps);
}
int slcli_product_windows_section_create_original(struct slcli_product_storage_windows *s) {
 struct slcli_product_section_windows *c;struct slcli_build_windows_call *r;
 size_t es,ps,total,relative;BOOL raw;DWORD value;
 if(!s||s->failed||s->storage_constructed!=1||s->section_attempted||s->section_original||
 !slcli_memory_owner_mutable(&s->memory_original)||s->arena_original.owner!=&s->memory_original||
 !slcli_source_deadline_original_matches(&s->arena_original,s->deadline_original,s->caller_original)||
 s->deadline_original->origin!=s->operation_origin_original||
 !slcli_build_principal_windows_matches_original(s->effective_original)||
 !slcli_build_principal_windows_matches_original(s->primary_original))return 0;
 es=s->source_original->effective_original->windows_sid_bytes;
 ps=s->source_original->primary_original->windows_sid_bytes;
 if(!es||es>SECURITY_MAX_SID_SIZE||!ps||ps>SECURITY_MAX_SID_SIZE)return 0;
 total=sizeof(ACL)+16+es+ps;
 relative=sizeof(SECURITY_DESCRIPTOR_RELATIVE)+es+total;
 if(total>USHRT_MAX||relative>MAXDWORD||
 !slcli_arena_allocation_bytes(sizeof(*c))||!slcli_arena_allocation_bytes(total)||
 !slcli_arena_allocation_bytes(relative))return 0;
 s->section_attempted=1;
 if(!slcli_native_clock_read(&s->section_entry_original)||
 s->section_entry_original.tick<s->prior_tick||
 s->section_entry_original.tick>=s->deadline_original->expires){s->failed=1;return 0;}
 c=slcli_arena_calloc(&s->arena_original,1,sizeof(*c));s->section_original=c;
 if(!c){s->failed=1;return 0;}
 if(!slcli_product_storage_windows_output_away_original(s,c,sizeof(*c))){s->failed=1;return 0;}
 c->storage_original=s;c->deadline_original=s->deadline_original;c->origin_original=s->operation_origin_original;
 c->origin_tick=c->origin_original->tick;c->expires=c->deadline_original->expires;
 c->prior_tick=s->section_entry_original.tick;c->started=1;c->failed=1;
 c->acl_bytes=(DWORD)total;c->relative_capacity=(DWORD)relative;c->relative_bytes=(DWORD)relative;
 c->acl_original=slcli_arena_calloc(&s->arena_original,1,total);
 c->relative_original=slcli_arena_calloc(&s->arena_original,1,relative);
 if(!c->acl_original||!c->relative_original||
 !slcli_arena_live_block(&s->arena_original,c->acl_original,total)||
 !slcli_arena_live_block(&s->arena_original,c->relative_original,relative)||
 !apart(c,sizeof(*c),c->acl_original,total)||!apart(c,sizeof(*c),c->relative_original,relative)||
 !apart(c->acl_original,total,c->relative_original,relative)||
 !slcli_product_storage_windows_output_away_original(s,c->acl_original,total)||
 !slcli_product_storage_windows_output_away_original(s,c->relative_original,relative)||
 !slcli_native_clock_read(&c->start)||!same(c)||c->start.tick<c->prior_tick||c->start.tick>=c->expires)return stop(c);
 if(!slcli_build_principal_windows_capture_original(&c->effective_original,s->source_original->effective_original,
 c->origin_original,&c->start,c->expires)||
 !slcli_build_process_primary_windows_capture_original(&c->primary_original,&c->effective_original,
 s->source_original->primary_original)||
 !slcli_build_principal_windows_issuing_relation_original(&c->effective_original,&c->primary_original)||
 c->effective_original.process_original!=s->effective_original->process_original||
 c->effective_original.thread_original!=s->effective_original->thread_original)return stop(c);
 c->prior_tick=c->primary_original.finish.tick;
 if(!before(c,SLCLI_PRODUCT_SD_INITIALIZE))return stop(c);r=&c->calls[SLCLI_PRODUCT_SD_INITIALIZE];
 r->args[0]=(uintptr_t)&c->absolute_original;r->args[1]=SECURITY_DESCRIPTOR_REVISION;r->called=1;
 raw=InitializeSecurityDescriptor(&c->absolute_original,SECURITY_DESCRIPTOR_REVISION);
 if(!after(c,SLCLI_PRODUCT_SD_INITIALIZE,raw))return stop(c);
 if(!before(c,SLCLI_PRODUCT_ACL_INITIALIZE))return stop(c);r=&c->calls[SLCLI_PRODUCT_ACL_INITIALIZE];
 r->args[0]=(uintptr_t)c->acl_original;r->args[1]=c->acl_bytes;r->args[2]=ACL_REVISION;r->called=1;
 raw=InitializeAcl(c->acl_original,c->acl_bytes,ACL_REVISION);
 if(!after(c,SLCLI_PRODUCT_ACL_INITIALIZE,raw))return stop(c);
 if(!before(c,SLCLI_PRODUCT_ACE_EFFECTIVE))return stop(c);r=&c->calls[SLCLI_PRODUCT_ACE_EFFECTIVE];
 r->args[0]=(uintptr_t)c->acl_original;r->args[1]=ACL_REVISION;r->args[2]=0;
 r->args[3]=SLCLI_PRODUCT_EFFECTIVE_SECTION;r->args[4]=(uintptr_t)c->effective_original.user_original.header.User.Sid;r->called=1;
 raw=AddAccessAllowedAceEx(c->acl_original,ACL_REVISION,0,SLCLI_PRODUCT_EFFECTIVE_SECTION,
 c->effective_original.user_original.header.User.Sid);
 if(!after(c,SLCLI_PRODUCT_ACE_EFFECTIVE,raw))return stop(c);
 if(!before(c,SLCLI_PRODUCT_ACE_PRIMARY))return stop(c);r=&c->calls[SLCLI_PRODUCT_ACE_PRIMARY];
 r->args[0]=(uintptr_t)c->acl_original;r->args[1]=ACL_REVISION;r->args[2]=0;
 r->args[3]=SLCLI_PRODUCT_PRIMARY_SECTION;r->args[4]=(uintptr_t)c->primary_original.user_original.header.User.Sid;r->called=1;
 raw=AddAccessAllowedAceEx(c->acl_original,ACL_REVISION,0,SLCLI_PRODUCT_PRIMARY_SECTION,
 c->primary_original.user_original.header.User.Sid);
 if(!after(c,SLCLI_PRODUCT_ACE_PRIMARY,raw))return stop(c);
 if(!before(c,SLCLI_PRODUCT_SD_OWNER))return stop(c);r=&c->calls[SLCLI_PRODUCT_SD_OWNER];
 r->args[0]=(uintptr_t)&c->absolute_original;r->args[1]=(uintptr_t)c->effective_original.user_original.header.User.Sid;
 r->args[2]=FALSE;r->called=1;
 raw=SetSecurityDescriptorOwner(&c->absolute_original,c->effective_original.user_original.header.User.Sid,FALSE);
 if(!after(c,SLCLI_PRODUCT_SD_OWNER,raw))return stop(c);
 if(!before(c,SLCLI_PRODUCT_SD_DACL))return stop(c);r=&c->calls[SLCLI_PRODUCT_SD_DACL];
 r->args[0]=(uintptr_t)&c->absolute_original;r->args[1]=TRUE;r->args[2]=(uintptr_t)c->acl_original;r->args[3]=FALSE;r->called=1;
 raw=SetSecurityDescriptorDacl(&c->absolute_original,TRUE,c->acl_original,FALSE);
 if(!after(c,SLCLI_PRODUCT_SD_DACL,raw))return stop(c);
 if(!before(c,SLCLI_PRODUCT_SD_CONTROL))return stop(c);r=&c->calls[SLCLI_PRODUCT_SD_CONTROL];
 r->args[0]=(uintptr_t)&c->absolute_original;r->args[1]=SE_DACL_PROTECTED;r->args[2]=SE_DACL_PROTECTED;r->called=1;
 raw=SetSecurityDescriptorControl(&c->absolute_original,SE_DACL_PROTECTED,SE_DACL_PROTECTED);
 if(!after(c,SLCLI_PRODUCT_SD_CONTROL,raw))return stop(c);
 if(!before(c,SLCLI_PRODUCT_SD_RELATIVE))return stop(c);r=&c->calls[SLCLI_PRODUCT_SD_RELATIVE];
 r->args[0]=(uintptr_t)&c->absolute_original;r->args[1]=(uintptr_t)c->relative_original;
 r->args[2]=(uintptr_t)&c->relative_bytes;r->called=1;
 raw=MakeSelfRelativeSD(&c->absolute_original,c->relative_original,&c->relative_bytes);
 if(!after(c,SLCLI_PRODUCT_SD_RELATIVE,raw)||c->relative_bytes>c->relative_capacity)return stop(c);
 if(!before(c,SLCLI_PRODUCT_SD_VALID))return stop(c);r=&c->calls[SLCLI_PRODUCT_SD_VALID];
 r->args[0]=(uintptr_t)c->relative_original;r->called=1;raw=IsValidSecurityDescriptor(c->relative_original);
 if(!after(c,SLCLI_PRODUCT_SD_VALID,raw))return stop(c);
 if(!before(c,SLCLI_PRODUCT_SD_GET_OWNER))return stop(c);r=&c->calls[SLCLI_PRODUCT_SD_GET_OWNER];
 r->args[0]=(uintptr_t)c->relative_original;r->args[1]=(uintptr_t)&c->observed_owner_original;
 r->args[2]=(uintptr_t)&c->owner_defaulted;r->called=1;
 raw=GetSecurityDescriptorOwner(c->relative_original,&c->observed_owner_original,&c->owner_defaulted);
 if(!after(c,SLCLI_PRODUCT_SD_GET_OWNER,raw))return stop(c);
 if(!before(c,SLCLI_PRODUCT_SD_GET_DACL))return stop(c);r=&c->calls[SLCLI_PRODUCT_SD_GET_DACL];
 r->args[0]=(uintptr_t)c->relative_original;r->args[1]=(uintptr_t)&c->dacl_present;
 r->args[2]=(uintptr_t)&c->observed_acl_original;r->args[3]=(uintptr_t)&c->dacl_defaulted;r->called=1;
 raw=GetSecurityDescriptorDacl(c->relative_original,&c->dacl_present,&c->observed_acl_original,&c->dacl_defaulted);
 if(!after(c,SLCLI_PRODUCT_SD_GET_DACL,raw))return stop(c);
 if(!before(c,SLCLI_PRODUCT_SD_GET_CONTROL))return stop(c);r=&c->calls[SLCLI_PRODUCT_SD_GET_CONTROL];
 r->args[0]=(uintptr_t)c->relative_original;r->args[1]=(uintptr_t)&c->control;r->args[2]=(uintptr_t)&c->revision;r->called=1;
 raw=GetSecurityDescriptorControl(c->relative_original,&c->control,&c->revision);
 if(!after(c,SLCLI_PRODUCT_SD_GET_CONTROL,raw))return stop(c);
 if(!before(c,SLCLI_PRODUCT_SD_LENGTH))return stop(c);r=&c->calls[SLCLI_PRODUCT_SD_LENGTH];
 r->args[0]=(uintptr_t)c->relative_original;r->called=1;
 value=GetSecurityDescriptorLength(c->relative_original);c->observed_length=value;
 r->raw=value;r->returned=1;
 if(!slcli_native_clock_read(&r->after)||!same(c)||r->after.tick<r->before.tick||
 r->after.tick>=c->expires||!value||value!=c->relative_bytes||
 !slcli_product_windows_descriptor_bytes_correspond_original(c))return stop(c);
 c->prior_tick=r->after.tick;
 if(!slcli_native_clock_read(&c->finish)||!same(c)||c->finish.tick<c->prior_tick||c->finish.tick>=c->expires)return stop(c);
 c->descriptor_constructed=1;
 if(!slcli_node_transport_windows_create(s->transport_original,&s->arena_original,c->deadline_original,
 s->caller_original,sizeof(*s->caller_original),&c->finish,c->relative_original,c->relative_bytes))return stop(c);
 if(s->transport_original->parent_pid!=c->primary_original.process_original||
 s->transport_original->parent_sid_bytes!=ps||
 !equal(s->transport_original->parent_sid,c->primary_original.user_original.header.User.Sid,ps)||
 s->transport_original->parent_creation.dwLowDateTime!=c->primary_original.process_creation.dwLowDateTime||
 s->transport_original->parent_creation.dwHighDateTime!=c->primary_original.process_creation.dwHighDateTime||
 s->transport_original->parent_statistics.TokenId.LowPart!=c->primary_original.statistics_original.TokenId.LowPart||
 s->transport_original->parent_statistics.TokenId.HighPart!=c->primary_original.statistics_original.TokenId.HighPart||
 s->transport_original->parent_statistics.AuthenticationId.LowPart!=c->primary_original.statistics_original.AuthenticationId.LowPart||
 s->transport_original->parent_statistics.AuthenticationId.HighPart!=c->primary_original.statistics_original.AuthenticationId.HighPart||
 s->transport_original->parent_statistics.TokenType!=TokenPrimary||!same(c))return stop(c);
 s->prior_tick=s->transport_original->calls[13].after.tick;
 c->transport_created=1;c->failed=0;return 1;
}
#endif
