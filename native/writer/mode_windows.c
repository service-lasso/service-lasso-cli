#include "mode_windows.h"
#if defined(_WIN32)
#include <stddef.h>
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int equal(const unsigned char *a,const unsigned char *b,size_t n) {
  size_t i;unsigned different=0;for(i=0;i<n;i++)different|=a[i]^b[i];return !different;
}
int slcli_writer_mode_bound(const struct slcli_writer_mode_capture *c,struct slcli_evidence_file *f) {
  const struct slcli_image_deadline_owner *d=f&&f->deadline_owner?
    f->deadline_owner->rows.deadline_owner:0;
  return c&&d&&c->evidence_file==f&&c->birth_cell==d->birth_cell&&
    slcli_arena_live_range(d->arena,c,sizeof(*c))&&
    slcli_arena_live_range(d->arena,c->birth_cell,sizeof(*c->birth_cell))&&
    slcli_arena_live_range(d->arena,c->bound_token,sizeof(*c->bound_token))&&
    slcli_arena_live_range(d->arena,c->bound_owner,c->bound_owner_bytes)&&
    c->bound_owner_bytes&&c->bound_owner_bytes<=184&&
    c->bound_archive==f->capture_original&&c->bound_sink&&
    c->bound_effective&&c->bound_effective!=INVALID_HANDLE_VALUE&&
    !c->unresolved&&slcli_evidence_deadline_ready(f);
}
int slcli_writer_mode_prepare_unborn(struct slcli_arena *a,struct slcli_writer_mode_capture *c,
  struct slcli_evidence_file *f,HANDLE *birth,HANDLE effective,const TOKEN_STATISTICS *token_original,
  const unsigned char *owner,size_t bytes,slcli_writer_mode_sink sink,void *archive) {
  const struct slcli_image_deadline_owner *d=f&&f->deadline_owner?
    f->deadline_owner->rows.deadline_owner:0;
  const void *sources[10];size_t sizes[10],i;
  if(!a||!c||!d||d->arena!=a||!d->unborn||!slcli_evidence_deadline_ready(f)||
    !birth||birth!=d->birth_cell||*birth||!effective||effective==INVALID_HANDLE_VALUE||!token_original||
    !owner||!bytes||bytes>184||!sink||archive!=f->capture_original||
    !slcli_arena_live_range(a,c,sizeof(*c))||!slcli_arena_live_range(a,birth,sizeof(*birth))||
    !slcli_arena_live_range(a,token_original,sizeof(*token_original))||
    !slcli_arena_live_range(a,owner,bytes))return 0;
  sources[0]=f;sizes[0]=sizeof(*f);sources[1]=birth;sizes[1]=sizeof(*birth);
  sources[2]=token_original;sizes[2]=sizeof(*token_original);sources[3]=owner;sizes[3]=bytes;
  sources[4]=d->deadline;sizes[4]=sizeof(*d->deadline);
  sources[5]=d->origin;sizes[5]=sizeof(*d->origin);
  sources[6]=archive;sizes[6]=d->capture_owner_bytes;
  sources[7]=d;sizes[7]=sizeof(*d);sources[8]=f->deadline_owner;sizes[8]=sizeof(*f->deadline_owner);
  sources[9]=d->caller;sizes[9]=d->caller_bytes;
  for(i=0;i<10;i++)if(!separate(c,sizeof(*c),sources[i],sizes[i]))return 0;
  if(!separate(c,sizeof(*c),f->workspace,sizeof(*f->workspace))||
    !separate(c,sizeof(*c),f->captures,f->capture_capacity*sizeof(*f->captures))||
    !separate(c,sizeof(*c),d->clocks,d->capacity*sizeof(*d->clocks)))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  c->evidence_file=f;c->birth_cell=birth;c->bound_effective=effective;c->bound_token=token_original;
  c->bound_owner=owner;c->bound_owner_bytes=bytes;c->bound_sink=sink;c->bound_archive=archive;
  return slcli_writer_mode_bound(c,f);
}
static int unresolved(struct slcli_writer_mode_capture *c) {
  c->unresolved=1;
  if(c->evidence_file&&c->evidence_file->deadline_owner)c->evidence_file->deadline_owner->failed=1;
  return 0;
}
static int before(struct slcli_writer_mode_capture *c,unsigned kind) {
  size_t i;unsigned row=c->call_count;
  if(row>=20||c->unresolved)return unresolved(c);
  for(i=0;i<sizeof(c->call_clocks[row]);i++)
    if(((const unsigned char *)c->call_clocks[row])[i])return unresolved(c);
  if(c->call_called[row]||c->call_returned[row]||c->call_kind[row])return unresolved(c);
  c->call_count++;c->call_kind[row]=kind;
  if(c->evidence_file&&(!slcli_writer_mode_bound(c,c->evidence_file)||
    *c->birth_cell!=c->original||!c->original||c->original==INVALID_HANDLE_VALUE||
    !slcli_native_clock_read(&c->call_clocks[row][0])||
    !slcli_evidence_deadline_precede(c->evidence_file,&c->call_clocks[row][0]))) {
    return unresolved(c);
  }
  c->call_called[row]=1;return 1;
}
static int after(struct slcli_writer_mode_capture *c) {
  unsigned row;
  if(!c->call_count||c->call_count>20)return unresolved(c);
  row=c->call_count-1;
  if(!c->call_called[row]||!c->call_returned[row])return unresolved(c);
  if(c->evidence_file&&(!slcli_native_clock_read(&c->call_clocks[row][1])||
    !slcli_writer_mode_bound(c,c->evidence_file)||
    !slcli_evidence_deadline_precede(c->evidence_file,&c->call_clocks[row][1]))) {
    return unresolved(c);
  }
  return 1;
}
static int observe_sid(struct slcli_writer_mode_capture *c,unsigned row,PSID original,size_t bytes) {
  if(row>=3||!before(c,3))return 0;
  c->sid_valid[row]=IsValidSid(original);
  c->call_returned[c->call_count-1]=1;
  if(!after(c)||!c->sid_valid[row]||!before(c,4))return 0;
  c->sid_length[row]=GetLengthSid(original);
  c->call_returned[c->call_count-1]=1;
  return after(c)&&c->sid_length[row]==bytes;
}
static int same_token(const TOKEN_STATISTICS *a,const TOKEN_STATISTICS *b) {
  return a->TokenId.LowPart==b->TokenId.LowPart&&a->TokenId.HighPart==b->TokenId.HighPart&&
    a->AuthenticationId.LowPart==b->AuthenticationId.LowPart&&
    a->AuthenticationId.HighPart==b->AuthenticationId.HighPart&&
    a->ModifiedId.LowPart==b->ModifiedId.LowPart&&a->ModifiedId.HighPart==b->ModifiedId.HighPart&&
    a->TokenType==b->TokenType&&a->ImpersonationLevel==b->ImpersonationLevel&&
    a->ExpirationTime.QuadPart==b->ExpirationTime.QuadPart&&
    a->GroupCount==b->GroupCount&&a->PrivilegeCount==b->PrivilegeCount;
}
static int token(struct slcli_writer_mode_capture *c,unsigned i) {
  TOKEN_STATISTICS *r=i?&c->after:&c->before;
  c->token_args[i][0]=(uintptr_t)c->effective_token;c->token_args[i][1]=TokenStatistics;
  c->token_args[i][2]=(uintptr_t)r;c->token_args[i][3]=sizeof(*r);
  c->token_args[i][4]=(uintptr_t)&c->token_bytes[i];
  if(!before(c,1))return 0;
  c->token_ok[i]=GetTokenInformation(c->effective_token,TokenStatistics,r,sizeof(*r),&c->token_bytes[i]);
  c->call_returned[c->call_count-1]=1;
  if(!c->token_ok[i])c->error=GetLastError();
  if(!after(c)||!c->token_ok[i])return 0;
  return c->token_bytes[i]==sizeof(*r)&&r->TokenType==TokenImpersonation&&
    r->ImpersonationLevel>=SecurityImpersonation;
}
static int bounded_sid(struct slcli_writer_mode_capture *c,PSID original,size_t *bytes) {
  uintptr_t x=(uintptr_t)c->security,y=(uintptr_t)original;size_t n;
  if(!original||c->security_bytes>sizeof(c->security)||y<x||
    y-x>c->security_bytes||c->security_bytes-(y-x)<8||y%_Alignof(SID))return 0;
  n=8+4*(size_t)((SID *)original)->SubAuthorityCount;
  if(n>c->security_bytes-(y-x)||!observe_sid(c,2,original,n))return 0;
  *bytes=n;return 1;
}
static int token_user(struct slcli_writer_mode_capture *c,unsigned i,
  const unsigned char *owner,size_t owner_bytes) {
  TOKEN_USER *u=(void *)c->user_raw[i];uintptr_t base=(uintptr_t)u,sid;size_t bytes;
  c->user_args[i][0]=(uintptr_t)c->effective_token;c->user_args[i][1]=TokenUser;
  c->user_args[i][2]=base;c->user_args[i][3]=sizeof(c->user_raw[i]);
  c->user_args[i][4]=(uintptr_t)&c->user_bytes[i];
  if(!before(c,2))return 0;
  c->user_ok[i]=GetTokenInformation(c->effective_token,TokenUser,u,sizeof(c->user_raw[i]),&c->user_bytes[i]);
  c->call_returned[c->call_count-1]=1;
  if(!c->user_ok[i])c->error=GetLastError();
  if(!after(c)||!c->user_ok[i])return 0;
  if(c->user_bytes[i]<sizeof(*u)||c->user_bytes[i]>sizeof(c->user_raw[i]))return 0;
  sid=(uintptr_t)u->User.Sid;
  if(sid<base||sid-base>c->user_bytes[i]||c->user_bytes[i]-(sid-base)<8||sid%_Alignof(SID))return 0;
  bytes=8+4*(size_t)((SID *)sid)->SubAuthorityCount;
  return bytes==owner_bytes&&bytes<=c->user_bytes[i]-(sid-base)&&
    /* The raw SID return/length are retained before diagnostic equality. */
    observe_sid(c,i,(PSID)sid,bytes)&&equal((void *)sid,owner,bytes);
}
static int observe(struct slcli_writer_mode_capture *c,HANDLE original,
  HANDLE effective,const TOKEN_STATISTICS *admitted,const unsigned char *owner,size_t owner_bytes,
  slcli_writer_mode_sink sink,void *archive,int directory) {
  size_t sid_bytes;uintptr_t base,acl;ACCESS_ALLOWED_ACE *ace;unsigned mode=0;int ok=0;
  static const unsigned char owner_rights[12]={1,1,0,0,0,0,0,3,4,0,0,0};
  if(!c||c->used||!original||original==INVALID_HANDLE_VALUE||!effective||
    effective==INVALID_HANDLE_VALUE||!admitted||!owner||!owner_bytes||owner_bytes>184||
    !sink||!archive||!separate(c,sizeof(*c),admitted,sizeof(*admitted))||
    !separate(c,sizeof(*c),owner,owner_bytes))return 0;
  if(c->evidence_file&&(!slcli_writer_mode_bound(c,c->evidence_file)||
    original!=*c->birth_cell||effective!=c->bound_effective||admitted!=c->bound_token||
    owner!=c->bound_owner||owner_bytes!=c->bound_owner_bytes||sink!=c->bound_sink||
    archive!=c->bound_archive))return 0;
  c->used=1;c->failed=1;c->original=original;c->effective_token=effective;
  if(!token(c,0)||!same_token(&c->before,admitted)||!token_user(c,0,owner,owner_bytes)||
    !before(c,5))goto done;
  c->file_observe_ok=directory?slcli_windows_directory_observe(original,&c->file):
    slcli_windows_file_observe(original,&c->file);
  c->call_returned[c->call_count-1]=1;
  if(!after(c)||!c->file_observe_ok)goto done;
  c->security_args[0]=(uintptr_t)original;
  c->security_args[1]=OWNER_SECURITY_INFORMATION|GROUP_SECURITY_INFORMATION|DACL_SECURITY_INFORMATION;
  c->security_args[2]=(uintptr_t)c->security;c->security_args[3]=sizeof(c->security);
  c->security_args[4]=(uintptr_t)&c->security_bytes;
  if(!before(c,6))goto done;
  c->security_ok=GetKernelObjectSecurity(original,(SECURITY_INFORMATION)c->security_args[1],
    (PSECURITY_DESCRIPTOR)c->security,sizeof(c->security),&c->security_bytes);
  c->call_returned[c->call_count-1]=1;
  if(!c->security_ok)c->error=GetLastError();
  if(!after(c)||!c->security_ok||c->security_bytes>sizeof(c->security)||!before(c,7))goto done;
  c->security_valid=IsValidSecurityDescriptor(c->security);
  c->call_returned[c->call_count-1]=1;
  if(!after(c)||!c->security_valid||!before(c,8))goto done;
  c->control_ok=GetSecurityDescriptorControl(c->security,&c->control,&c->revision);
  c->call_returned[c->call_count-1]=1;
  if(!c->control_ok)c->error=GetLastError();
  if(!after(c)||!c->control_ok||!(c->control&SE_DACL_PROTECTED)||!before(c,9))goto done;
  c->owner_ok=GetSecurityDescriptorOwner(c->security,&c->owner,&c->owner_defaulted);
  c->call_returned[c->call_count-1]=1;
  if(!c->owner_ok)c->error=GetLastError();
  if(!after(c)||!c->owner_ok||!bounded_sid(c,c->owner,&sid_bytes)||sid_bytes!=owner_bytes||
    !equal(c->owner,owner,owner_bytes)||c->file.owner_sid_bytes!=owner_bytes||
    !equal(c->file.owner_sid,owner,owner_bytes))goto done;
  if(!before(c,10))goto done;
  c->dacl_ok=GetSecurityDescriptorDacl(c->security,&c->dacl_present,&c->dacl,&c->dacl_defaulted);
  c->call_returned[c->call_count-1]=1;
  if(!c->dacl_ok)c->error=GetLastError();
  if(!after(c))goto done;
  base=(uintptr_t)c->security;acl=(uintptr_t)c->dacl;
  if(!c->dacl_ok||!c->dacl_present||c->dacl_defaulted||!c->dacl||acl<base||
    acl-base>c->security_bytes||c->security_bytes-(acl-base)<sizeof(ACL)||
    acl%_Alignof(ACL)||c->dacl->AclSize!=sizeof(ACL)+20||
    c->dacl->AclSize>c->security_bytes-(acl-base)||c->dacl->AceCount!=1||!before(c,11))goto done;
  c->acl_valid=IsValidAcl(c->dacl);
  c->call_returned[c->call_count-1]=1;
  if(!after(c)||!c->acl_valid)goto done;
  ace=(void *)((unsigned char *)c->dacl+sizeof(ACL));
  if(ace->Header.AceType!=ACCESS_ALLOWED_ACE_TYPE||ace->Header.AceFlags||
    ace->Header.AceSize!=20||!equal((unsigned char *)&ace->SidStart,owner_rights,12))goto done;
  if(directory) { if(ace->Mask!=0x001f01ff)goto done;mode=0700; }
  else if(ace->Mask==0x001f01df)mode=0644;else if(ace->Mask==0x001f01ff)mode=0755;else goto done;
  if(c->file.attributes.FileAttributes&FILE_ATTRIBUTE_READONLY)goto done;
  c->mapping.GenericRead=FILE_GENERIC_READ;c->mapping.GenericWrite=FILE_GENERIC_WRITE;
  c->mapping.GenericExecute=FILE_GENERIC_EXECUTE;c->mapping.GenericAll=0x001f01ff;
  c->privilege_bytes=sizeof(c->privileges);
  c->access_args[0]=(uintptr_t)c->security;c->access_args[1]=(uintptr_t)effective;
  c->access_args[2]=FILE_EXECUTE;c->access_args[3]=(uintptr_t)&c->mapping;
  c->access_args[4]=(uintptr_t)c->privileges;c->access_args[5]=(uintptr_t)&c->privilege_bytes;
  c->access_args[6]=(uintptr_t)&c->granted;c->access_args[7]=(uintptr_t)&c->access_status;
  if(!before(c,12))goto done;
  c->access_ok=AccessCheck(c->security,effective,FILE_EXECUTE,&c->mapping,
    (PPRIVILEGE_SET)c->privileges,&c->privilege_bytes,&c->granted,&c->access_status);
  c->call_returned[c->call_count-1]=1;
  if(!c->access_ok)c->error=GetLastError();
  if(!after(c)||!c->access_ok)goto done;
  if(c->privilege_bytes>sizeof(c->privileges)||((PRIVILEGE_SET *)c->privileges)->PrivilegeCount||
    (mode==0644&&(c->access_status||c->granted))||
    ((mode==0755||mode==0700)&&(!c->access_status||c->granted!=FILE_EXECUTE))||
    !token(c,1)||!same_token(&c->before,&c->after)||!token_user(c,1,owner,owner_bytes))goto done;
  c->mode=mode;ok=1;
done:
  /* Preserve failed/partial native calls too. No resized buffer/retry/close. */
  if(!sink(archive,c))return unresolved(c);
  if(!ok)return unresolved(c);c->complete=1;c->failed=0;return 1;
}
int slcli_writer_observe_windows_mode(struct slcli_writer_mode_capture *c,HANDLE original,
  HANDLE effective,const TOKEN_STATISTICS *admitted,const unsigned char *owner,size_t owner_bytes,
  slcli_writer_mode_sink sink,void *archive) {
  return observe(c,original,effective,admitted,owner,owner_bytes,sink,archive,0);
}
int slcli_writer_observe_windows_directory(struct slcli_writer_mode_capture *c,HANDLE original,
  HANDLE effective,const TOKEN_STATISTICS *admitted,const unsigned char *owner,size_t owner_bytes,
  slcli_writer_mode_sink sink,void *archive) {
  return observe(c,original,effective,admitted,owner,owner_bytes,sink,archive,1);
}
#endif
