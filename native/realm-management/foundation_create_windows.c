#include "foundation_create_windows.h"
#if defined(_WIN32)
#include <stddef.h>
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
static int role(uint32_t n){return n==97||n==98||n==21||n==15||n==6;}
static size_t sid_bytes(const unsigned char *raw,size_t bytes,DWORD offset) {
  const SID *sid;size_t n;
  if(!offset||offset%_Alignof(DWORD)||offset<sizeof(SECURITY_DESCRIPTOR_RELATIVE)||
     offset>bytes||offsetof(SID,SubAuthority)>bytes-offset)return 0;
  sid=(const SID *)(const void *)(raw+offset);
  if(sid->Revision!=SID_REVISION||sid->SubAuthorityCount>SID_MAX_SUB_AUTHORITIES)return 0;
  n=offsetof(SID,SubAuthority)+sid->SubAuthorityCount*sizeof(DWORD);
  return n<=bytes-offset?n:0;
}
static size_t acl_bytes(const unsigned char *raw,size_t bytes,DWORD offset) {
  const ACL *acl;const ACE_HEADER *ace;size_t n,i,at;
  if(!offset||offset%_Alignof(DWORD)||offset<sizeof(SECURITY_DESCRIPTOR_RELATIVE)||
     offset>bytes||sizeof(ACL)>bytes-offset)return 0;
  acl=(const ACL *)(const void *)(raw+offset);n=acl->AclSize;
  if((acl->AclRevision!=ACL_REVISION&&acl->AclRevision!=ACL_REVISION_DS)||
     n<sizeof(*acl)||n>bytes-offset||n%_Alignof(DWORD)||
     acl->AceCount>(n-sizeof(*acl))/sizeof(ACE_HEADER))return 0;
  at=sizeof(*acl);
  for(i=0;i<acl->AceCount;i++) {
    if(at>n||sizeof(*ace)>n-at)return 0;
    ace=(const ACE_HEADER *)(const void *)(raw+offset+at);
    if(ace->AceSize<sizeof(*ace)||ace->AceSize>n-at||ace->AceSize%_Alignof(DWORD))return 0;
    at+=ace->AceSize;
  }
  /* Container geometry only. Complete ACE semantics and permission policy
   * require the original independently admitted native SDK/policy producer. */
  return n;
}
static int security_shape(const unsigned char *raw,size_t bytes) {
  const SECURITY_DESCRIPTOR_RELATIVE *sd;
  if(!raw||bytes<sizeof(*sd)||bytes>512||(uintptr_t)raw%_Alignof(SECURITY_DESCRIPTOR_RELATIVE))return 0;
  sd=(const SECURITY_DESCRIPTOR_RELATIVE *)(const void *)raw;
  return sd->Revision==SECURITY_DESCRIPTOR_REVISION&&
    (sd->Control&(SE_SELF_RELATIVE|SE_DACL_PRESENT|SE_DACL_PROTECTED))==
      (SE_SELF_RELATIVE|SE_DACL_PRESENT|SE_DACL_PROTECTED)&&
    !(sd->Control&(SE_OWNER_DEFAULTED|SE_DACL_DEFAULTED))&&
    sid_bytes(raw,bytes,sd->Owner)&&(!sd->Group||sid_bytes(raw,bytes,sd->Group))&&
    acl_bytes(raw,bytes,sd->Dacl)&&(!sd->Sacl||acl_bytes(raw,bytes,sd->Sacl));
}
static int source(struct slcli_management_foundation_windows_creation *c) {
  size_t i;
  if(!c||!c->prepared||!c->arena||!c->sink||!c->parent_original||c->parent_original==INVALID_HANDLE_VALUE||
     c->name_bytes<2*sizeof(WCHAR)||c->name_bytes>65*sizeof(WCHAR)||c->name_bytes%sizeof(WCHAR)||
     !role(c->original_role)||c->parent_link_delta>1||
     !slcli_arena_live_block(c->arena,c,sizeof(*c))||
     !slcli_arena_live_block(c->arena,c->caller_original,c->caller_bytes)||
     !slcli_arena_live_block(c->arena,c->capture_original,c->capture_original_bytes)||
     !slcli_arena_live_block(c->arena,c->name_original,c->name_bytes)||
     !slcli_arena_live_block(c->arena,c->security_original,c->security_bytes)||
     !slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller_original)||
     c->deadline->origin!=c->origin||c->deadline->expires!=c->expires||
     c->origin->tick!=c->origin_tick||!security_shape(c->security.raw,c->security_bytes))return 0;
  for(i=0;i<c->name_bytes/sizeof(WCHAR);i++)if(c->name[i]!=c->name_original[i])return 0;
  for(i=0;i<c->security_bytes;i++)if(c->security.raw[i]!=c->security_original[i])return 0;
  return c->unicode_name.Buffer==c->name&&c->unicode_name.Length==c->name_bytes-sizeof(WCHAR)&&
    c->unicode_name.MaximumLength==c->name_bytes&&c->attributes.Length==sizeof(c->attributes)&&
    c->attributes.RootDirectory==c->parent_original&&c->attributes.ObjectName==&c->unicode_name&&
    !c->attributes.Attributes&&c->attributes.SecurityDescriptor==c->security.raw&&
    !c->attributes.SecurityQualityOfService;
}
struct slcli_management_foundation_windows_creation *slcli_management_foundation_windows_creation_allocate(
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,size_t bytes,
  HANDLE parent,uint32_t slot,uint32_t link_delta,const WCHAR *name,size_t name_bytes,
  const unsigned char *security,size_t security_bytes,
  slcli_store_capture_sink sink,void *owner,size_t owner_bytes) {
  struct slcli_management_foundation_windows_creation *c;size_t i,n;
  if(!a||!a->owner||!bytes||!owner_bytes||!parent||parent==INVALID_HANDLE_VALUE||!role(slot)||
     link_delta>1||!sink||name_bytes<2*sizeof(WCHAR)||name_bytes>65*sizeof(WCHAR)||name_bytes%sizeof(WCHAR)||
     !slcli_arena_live_block(a,name,name_bytes)||!slcli_arena_live_block(a,security,security_bytes)||
     !security_shape(security,security_bytes)||!slcli_arena_live_block(a,caller,bytes)||
     !slcli_arena_live_block(a,owner,owner_bytes)||!slcli_source_deadline_original_matches(a,d,caller)||
     !away(caller,bytes,owner,owner_bytes)||!away(name,name_bytes,owner,owner_bytes)||
     !away(security,security_bytes,owner,owner_bytes)||!away(name,name_bytes,security,security_bytes)||
     !slcli_source_deadline_away(d,caller,bytes)||!slcli_source_deadline_away(d,owner,owner_bytes)||
     !slcli_source_deadline_away(d,name,name_bytes)||!slcli_source_deadline_away(d,security,security_bytes)||
     !away(caller,bytes,a,sizeof(*a))||!away(caller,bytes,a->owner,sizeof(*a->owner))||
     !away(owner,owner_bytes,a,sizeof(*a))||!away(owner,owner_bytes,a->owner,sizeof(*a->owner))||
     !away(name,name_bytes,a,sizeof(*a))||!away(name,name_bytes,a->owner,sizeof(*a->owner))||
     !away(security,security_bytes,a,sizeof(*a))||!away(security,security_bytes,a->owner,sizeof(*a->owner)))return 0;
  n=name_bytes/sizeof(WCHAR);if(name[n-1]||name[0]==L'.')return 0;
  for(i=0;i+1<n;i++)if(!((name[i]>=L'a'&&name[i]<=L'z')||(name[i]>=L'A'&&name[i]<=L'Z')||
    (name[i]>=L'0'&&name[i]<=L'9')||name[i]==L'-'||name[i]==L'_'))return 0;
  c=slcli_arena_calloc(a,1,sizeof(*c));if(!c)return 0;
  if(!away(c,sizeof(*c),caller,bytes)||!away(c,sizeof(*c),owner,owner_bytes)||
     !away(c,sizeof(*c),name,name_bytes)||!away(c,sizeof(*c),security,security_bytes)||
     !slcli_source_deadline_away(d,c,sizeof(*c))||!away(c,sizeof(*c),a,sizeof(*a))||
     !away(c,sizeof(*c),a->owner,sizeof(*a->owner)))return 0;
  c->arena=a;c->deadline=d;c->origin=d->origin;c->caller_original=caller;c->caller_bytes=bytes;
  c->name_original=name;c->name_bytes=name_bytes;for(i=0;i<n;i++)c->name[i]=name[i];
  c->security_original=security;c->security_bytes=security_bytes;
  for(i=0;i<security_bytes;i++)c->security.raw[i]=security[i];
  c->parent_original=parent;c->original_role=slot;c->parent_link_delta=link_delta;
  c->unicode_name.Buffer=c->name;c->unicode_name.Length=(USHORT)(name_bytes-sizeof(WCHAR));
  c->unicode_name.MaximumLength=(USHORT)name_bytes;
  InitializeObjectAttributes(&c->attributes,&c->unicode_name,0,parent,c->security.raw);
  c->sink=sink;c->capture_original=owner;c->capture_original_bytes=owner_bytes;
  c->expires=d->expires;c->origin_tick=d->origin->tick;c->previous_tick=c->origin_tick;c->prepared=1;
  return c;
}
static int clock_row(struct slcli_management_foundation_windows_creation *c,size_t row) {
  struct slcli_native_clock *r=&c->clocks[row];
  if(!source(c)||!slcli_native_clock_read(r)||r->tick<c->previous_tick||
     r->tick>=c->expires||!source(c))return 0;c->previous_tick=r->tick;return 1;
}
static int observe(struct slcli_management_foundation_windows_creation *c,
  HANDLE h,struct slcli_windows_file_capture *r,size_t row) {
  int ok;if(!clock_row(c,row))return 0;ok=slcli_windows_directory_source_observe(h,r);
  if(!c->sink(c->capture_original,r,0,0)||!ok||!source(c)||!r->tick||
     r->tick<c->previous_tick||r->tick>=c->expires)return 0;
  c->previous_tick=r->tick;return clock_row(c,row+1);
}
static int parent_same_created(const struct slcli_management_foundation_windows_creation *c) {
  const struct slcli_windows_file_capture *a=&c->parent_before,*b=&c->parent_after;
  return a->completed&&b->completed&&a->owner_sid_bytes&&
    a->identity.VolumeSerialNumber==b->identity.VolumeSerialNumber&&
    slcli_equal(a->identity.FileId.Identifier,b->identity.FileId.Identifier,16)&&
    a->owner_sid_bytes==b->owner_sid_bytes&&slcli_equal(a->owner_sid,b->owner_sid,a->owner_sid_bytes)&&
    a->attributes.FileAttributes==b->attributes.FileAttributes&&a->mode==b->mode&&
    a->access_raw.GrantedAccess==b->access_raw.GrantedAccess&&a->handle_flags==b->handle_flags&&
    a->standard.NumberOfLinks<=MAXDWORD-c->parent_link_delta&&
    b->standard.NumberOfLinks==a->standard.NumberOfLinks+c->parent_link_delta;
}
static int security_same(const struct slcli_management_foundation_windows_creation *c) {
  const SECURITY_DESCRIPTOR_RELATIVE *a=(const void *)c->security.raw,*b=(const void *)c->security_readback.raw;
  size_t owner_bytes,dacl_bytes;
  if(!c->security_status||!security_shape(c->security_readback.raw,c->security_returned_bytes))return 0;
  owner_bytes=sid_bytes(c->security.raw,c->security_bytes,a->Owner);
  dacl_bytes=acl_bytes(c->security.raw,c->security_bytes,a->Dacl);
  return sid_bytes(c->security_readback.raw,c->security_returned_bytes,b->Owner)==owner_bytes&&
    acl_bytes(c->security_readback.raw,c->security_returned_bytes,b->Dacl)==dacl_bytes&&
    slcli_equal(c->security.raw+a->Owner,c->security_readback.raw+b->Owner,owner_bytes)&&
    slcli_equal(c->security.raw+a->Dacl,c->security_readback.raw+b->Dacl,dacl_bytes);
}
int slcli_management_foundation_windows_create(struct slcli_management_foundation_windows_creation *c) {
  ACCESS_MASK access=FILE_LIST_DIRECTORY|FILE_READ_ATTRIBUTES|READ_CONTROL|SYNCHRONIZE;
  ULONG options=FILE_DIRECTORY_FILE|FILE_SYNCHRONOUS_IO_NONALERT;
  if(!source(c)||c->started||c->unresolved)return 0;c->started=1;c->unresolved=1;
  if(!observe(c,c->parent_original,&c->parent_before,0)||
     c->parent_before.standard.NumberOfLinks>MAXDWORD-c->parent_link_delta||!clock_row(c,2))return 0;
  c->create_args[0]=(uintptr_t)&c->created_original;c->create_args[1]=access;
  c->create_args[2]=(uintptr_t)&c->attributes;c->create_args[3]=(uintptr_t)&c->create_io;
  c->create_args[4]=0;c->create_args[5]=FILE_ATTRIBUTE_NORMAL;c->create_args[6]=FILE_SHARE_READ;
  c->create_args[7]=FILE_CREATE;c->create_args[8]=options;c->create_args[9]=0;c->create_args[10]=0;
  c->native_called=1;c->native_pending_guard=1;
  c->create_status=NtCreateFile(&c->created_original,access,&c->attributes,&c->create_io,
    0,FILE_ATTRIBUTE_NORMAL,FILE_SHARE_READ,FILE_CREATE,options,0,0);
  c->native_return_observed=1;c->native_output_retained=1;
  if(c->create_status==0&&c->create_io.Status==0) {
    c->native_pending_guard=0;
    if(c->create_io.Information==FILE_CREATED)c->directory_created=1;
    if(c->created_original&&c->created_original!=INVALID_HANDLE_VALUE)c->descriptor_acquired=1;
  }
  /* Handle/IO output addresses and raw born effects are retained BEFORE clock.
   * Non-completed results retain SAME pending pointers and prohibit reuse. */
  if(!clock_row(c,3)||c->native_pending_guard||!c->directory_created||!c->descriptor_acquired||
     !observe(c,c->created_original,&c->created,4)||!clock_row(c,6))return 0;
  c->security_args[0]=(uintptr_t)c->created_original;c->security_args[1]=OWNER_SECURITY_INFORMATION|DACL_SECURITY_INFORMATION;
  c->security_args[2]=(uintptr_t)c->security_readback.raw;c->security_args[3]=512;
  c->security_args[4]=(uintptr_t)&c->security_returned_bytes;c->security_called=1;
  c->security_status=GetKernelObjectSecurity(c->created_original,(SECURITY_INFORMATION)c->security_args[1],
    c->security_readback.raw,512,&c->security_returned_bytes);
  c->security_observed=1;if(!c->security_status)c->security_error=GetLastError();
  if(!clock_row(c,7)||!security_same(c)||!observe(c,c->parent_original,&c->parent_after,8)||
     !parent_same_created(c))return 0;
  c->complete=1;c->unresolved=0;return 1;
}
#endif
