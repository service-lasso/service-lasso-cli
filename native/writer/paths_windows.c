#include "paths_windows.h"
#include "../host-service/evidence_deadline.h"
#if defined(_WIN32)
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int evidence_plan_same(const struct slcli_writer_windows_evidence_originals *x,
  const struct slcli_writer_windows_evidence_originals *y) {
  return x->bq_profile==y->bq_profile&&x->deadline==y->deadline&&x->caller==y->caller&&x->caller_bytes==y->caller_bytes&&
    x->capture_bytes==y->capture_bytes&&x->parent==y->parent&&x->effective_token==y->effective_token&&
    x->parent_identity==y->parent_identity&&x->token==y->token&&x->leaf==y->leaf&&
    x->leaf_units==y->leaf_units&&x->descriptor==y->descriptor&&x->owner==y->owner&&
    x->descriptor_bytes==y->descriptor_bytes&&x->owner_bytes==y->owner_bytes&&
    x->maximum==y->maximum&&x->predecessor==y->predecessor&&x->path_sink==y->path_sink&&
    x->mode_sink==y->mode_sink&&x->capture_sink==y->capture_sink&&x->capture_owner==y->capture_owner;
}
static int call_before(struct slcli_writer_windows_path *p) {
  size_t i;unsigned row=p->call_count;
  if(row>=9||p->evidence_unresolved)return 0;
  for(i=0;i<sizeof(p->call_clocks[row]);i++)
    if(((const unsigned char *)p->call_clocks[row])[i])return 0;
  if(p->call_called[row]||p->call_returned[row])return 0;
  p->call_count++;
  if(p->evidence_plan) {
    if(!p->evidence_original||!p->evidence_file||
      !evidence_plan_same(p->evidence_original,p->evidence_plan)||
      !slcli_evidence_deadline_ready(p->evidence_file)||
      !slcli_native_clock_read(&p->call_clocks[row][0])||
      p->call_clocks[row][0].tick<p->evidence_previous_tick||
      !slcli_evidence_deadline_precede(p->evidence_file,&p->call_clocks[row][0])) {
      p->evidence_unresolved=1;return 0;
    }
    p->evidence_previous_tick=p->call_clocks[row][0].tick;
  }
  p->call_called[row]=1;return 1;
}
static int call_after(struct slcli_writer_windows_path *p) {
  unsigned row=p->call_count-1;
  /* Every caller stores its raw return/output and error before this function.
   * Rows stay in the original path owner even when timing denies continuation. */
  if(!p->call_called[row]||!p->call_returned[row])return 0;
  if(p->evidence_plan) {
    int clock_ok=slcli_native_clock_read(&p->call_clocks[row][1]);
    if(!clock_ok||p->call_clocks[row][1].tick<p->evidence_previous_tick||
      !evidence_plan_same(p->evidence_original,p->evidence_plan)||
      !slcli_evidence_deadline_precede(p->evidence_file,&p->call_clocks[row][1])) {
      p->evidence_unresolved=1;return 0;
    }
    p->evidence_previous_tick=p->call_clocks[row][1].tick;
  }
  return 1;
}
static int create_evidence(struct slcli_arena *a,
  struct slcli_writer_windows_path *p,struct slcli_writer_windows_output *o,
  const struct slcli_writer_windows_evidence_originals *in,int bq) {
  const void *sources[9];size_t sizes[9],i,j,count=7;struct slcli_evidence_file *f;int clock_ok,joined,created_ok;
  const struct slcli_writer_windows_evidence_originals *original=in;
  struct slcli_writer_windows_evidence_originals *plan;
  if(!a||!p||!o||!in||!slcli_arena_live_range(a,p,sizeof(*p))||
    !slcli_arena_live_range(a,o,sizeof(*o))||!slcli_arena_live_range(a,in,sizeof(*in))||
    !(f=o->file)||!slcli_arena_live_range(a,f,sizeof(*f))||o->used||o->failed||o->complete||
    !f->captures||!f->capture_capacity||f->capture_capacity>SIZE_MAX/sizeof(*f->captures)||
    !f->workspace||!slcli_arena_live_range(a,f->workspace,sizeof(*f->workspace))||
    !slcli_arena_live_range(a,f->captures,f->capture_capacity*sizeof(*f->captures))||
    !in->leaf_units||in->leaf_units>255||!in->parent_identity||!in->token||
    !in->descriptor_bytes||in->descriptor_bytes>sizeof(p->descriptor)||
    !in->owner_bytes||in->owner_bytes>184||!in->path_sink||!in->mode_sink||!in->capture_sink||
    !in->caller_bytes||!in->capture_bytes||!in->maximum||
    !slcli_source_deadline_original_matches(a,in->deadline,in->caller)||
    !slcli_arena_live_range(a,in->caller,in->caller_bytes)||
    !slcli_arena_live_range(a,in->capture_owner,in->capture_bytes)||
    !separate(p,sizeof(*p),o,sizeof(*o))||!separate(p,sizeof(*p),f,sizeof(*f))||
    !separate(o,sizeof(*o),f,sizeof(*f))||
    !separate(o,sizeof(*o),in->capture_owner,in->capture_bytes)||
    !slcli_source_deadline_away(in->deadline,o,sizeof(*o))||
    !separate(o,sizeof(*o),f->workspace,sizeof(*f->workspace))||
    !separate(o,sizeof(*o),f->captures,f->capture_capacity*sizeof(*f->captures)))return 0;
  if(!!in->bq_profile!=!!bq)return 0;
  if(bq){
    if(!slcli_arena_live_range(a,in->bq_profile,sizeof(*in->bq_profile))||
      !in->bq_profile->raw_bytes||
      !slcli_arena_live_range(a,in->bq_profile->raw,in->bq_profile->raw_bytes)||
      in->maximum!=in->bq_profile->retention_maximum)return 0;
    sources[7]=in->bq_profile;sizes[7]=sizeof(*in->bq_profile);
    sources[8]=in->bq_profile->raw;sizes[8]=in->bq_profile->raw_bytes;count=9;
  }
  sources[0]=in;sizes[0]=sizeof(*in);sources[1]=in->parent_identity;sizes[1]=sizeof(*in->parent_identity);
  sources[2]=in->token;sizes[2]=sizeof(*in->token);sources[3]=in->leaf;sizes[3]=in->leaf_units*sizeof(*in->leaf);
  sources[4]=in->descriptor;sizes[4]=in->descriptor_bytes;sources[5]=in->owner;sizes[5]=in->owner_bytes;
  sources[6]=in->caller;sizes[6]=in->caller_bytes;
  for(i=0;i<count;i++) {
    if(!slcli_arena_live_range(a,sources[i],sizes[i])||
      !separate(p,sizeof(*p),sources[i],sizes[i])||!separate(o,sizeof(*o),sources[i],sizes[i])||
      !separate(f,sizeof(*f),sources[i],sizes[i])||
      !separate(f->workspace,sizeof(*f->workspace),sources[i],sizes[i])||
      !separate(f->captures,f->capture_capacity*sizeof(*f->captures),sources[i],sizes[i])||
      !separate(in->capture_owner,in->capture_bytes,sources[i],sizes[i]))return 0;
    for(j=i+1;j<count;j++)if(i!=0||j!=6)
      if(!separate(sources[i],sizes[i],sources[j],sizes[j]))return 0;
  }
  for(i=0;i<sizeof(*p);i++)if(((const unsigned char *)p)[i])return 0;
  for(i=0;i<sizeof(o->before);i++)if(((const unsigned char *)&o->before)[i]||
    ((const unsigned char *)&o->after)[i])return 0;
  plan=slcli_arena_calloc(a,1,sizeof(*plan));if(!plan)return 0;
  if(!slcli_arena_live_block(a,plan,sizeof(*plan)))return 0;*plan=*in;
  p->evidence_original=original;p->evidence_plan=plan;p->evidence_file=f;in=plan;
  if(!evidence_plan_same(original,plan))return 0;
  if(bq) {
    if(!slcli_evidence_deadline_prepare_bq_unborn(a,f,in->deadline,in->caller,in->caller_bytes,
      in->capture_owner,in->capture_bytes,in->predecessor,in->parent,in->bq_profile,
      in->capture_sink,&p->original,p,sizeof(*p)))return 0;
  } else if(!slcli_evidence_deadline_prepare_unborn(a,f,in->deadline,in->caller,in->caller_bytes,
    in->capture_owner,in->capture_bytes,in->predecessor,in->parent,in->maximum,
    in->capture_sink,&p->original,p,sizeof(*p)))return 0;
  if(!slcli_writer_mode_prepare_unborn(a,&p->mode,f,&p->original,in->effective_token,
    in->token,in->owner,in->owner_bytes,in->mode_sink,in->capture_owner)||
    !slcli_writer_mode_prepare_unborn(a,&o->before,f,&p->original,in->effective_token,
      in->token,in->owner,in->owner_bytes,in->mode_sink,in->capture_owner)||
    !slcli_writer_mode_prepare_unborn(a,&o->after,f,&p->original,in->effective_token,
      in->token,in->owner,in->owner_bytes,in->mode_sink,in->capture_owner)) {
    f->deadline_owner->failed=1;return 0;
  }
  if(!slcli_native_clock_read(&p->evidence_entry_clock)||
    !slcli_evidence_deadline_precede(f,&p->evidence_entry_clock))return 0;
  p->evidence_previous_tick=p->evidence_entry_clock.tick;
  created_ok=slcli_writer_create_windows_file(p,in->parent,in->parent_identity,in->leaf,in->leaf_units,
    in->descriptor,in->descriptor_bytes,in->effective_token,in->token,in->owner,in->owner_bytes,
    in->path_sink,in->mode_sink,in->capture_owner);
  if(p->create_status!=0||p->create_io.Status!=0||p->create_io.Information!=2||
    !p->original||p->original==INVALID_HANDLE_VALUE){f->deadline_owner->failed=1;return 0;}
  clock_ok=slcli_native_clock_read(&p->evidence_birth_clock);
  joined=slcli_evidence_deadline_born(f,&p->original,&p->evidence_birth_clock);
  if(!clock_ok||!joined||!created_ok||!evidence_plan_same(original,plan)||!p->mode.file.tick||
    p->evidence_birth_clock.tick<p->mode.file.tick){f->deadline_owner->failed=1;return 0;}
  if(bq) {
    if(!p->complete||p->failed||p->directory||p->retained_native_io)return 0;
    return slcli_evidence_begin_bq(f,p->original,p->parent,in->bq_profile,in->capture_sink,in->capture_owner);
  }
  return slcli_writer_begin_windows_created_file(p,o,in->maximum,in->capture_sink,in->capture_owner);
}
int slcli_writer_create_windows_evidence_original(struct slcli_arena *a,
  struct slcli_writer_windows_path *p,struct slcli_writer_windows_output *o,
  const struct slcli_writer_windows_evidence_originals *in) {
  return create_evidence(a,p,o,in,0);
}
int slcli_writer_create_windows_bq_original(struct slcli_arena *a,
  struct slcli_writer_windows_path *p,struct slcli_writer_windows_output *o,
  const struct slcli_writer_windows_evidence_originals *in) {
  return create_evidence(a,p,o,in,1);
}
static int parent(struct slcli_writer_windows_path *p,unsigned i,const FILE_ID_INFO *expected) {
  FILE_ID_INFO *r=i?&p->parent_after:&p->parent_before;
  p->parent_args[i][0]=(uintptr_t)p->parent;p->parent_args[i][1]=FileIdInfo;
  p->parent_args[i][2]=(uintptr_t)r;p->parent_args[i][3]=sizeof(*r);
  if(!call_before(p))return 0;
  p->parent_id_ok[i]=GetFileInformationByHandleEx(p->parent,FileIdInfo,r,sizeof(*r));
  p->call_returned[p->call_count-1]=1;
  if(!p->parent_id_ok[i])p->error=GetLastError();
  if(!call_after(p)||!p->parent_id_ok[i])return 0;
  return r->VolumeSerialNumber==expected->VolumeSerialNumber&&
    slcli_equal(r->FileId.Identifier,expected->FileId.Identifier,16);
}
static int sid_range(const unsigned char *raw,size_t bytes,DWORD offset,size_t *n) {
  if(offset<sizeof(SECURITY_DESCRIPTOR_RELATIVE)||offset>bytes||bytes-offset<8||offset%4)return 0;
  *n=8+4*(size_t)raw[offset+1];return *n<=bytes-offset;
}
static int descriptor_shape(struct slcli_writer_windows_path *p,
  const unsigned char *owner,size_t owner_bytes) {
  SECURITY_DESCRIPTOR_RELATIVE *sd=(void *)p->descriptor;ACL *acl;
  ACCESS_ALLOWED_ACE *ace;size_t n;unsigned char *raw=(void *)p->descriptor;
  static const unsigned char owner_rights[12]={1,1,0,0,0,0,0,3,4,0,0,0};
  if(p->descriptor_bytes<sizeof(*sd)||sd->Revision!=SECURITY_DESCRIPTOR_REVISION||
    (sd->Control&(SE_SELF_RELATIVE|SE_DACL_PROTECTED|SE_DACL_PRESENT))!=
      (SE_SELF_RELATIVE|SE_DACL_PROTECTED|SE_DACL_PRESENT)||sd->Sacl||
    !sid_range(raw,p->descriptor_bytes,sd->Owner,&n)||n!=owner_bytes||
    !slcli_equal(raw+sd->Owner,owner,n)||
    !sid_range(raw,p->descriptor_bytes,sd->Group,&n)||
    sd->Dacl<sizeof(*sd)||sd->Dacl>p->descriptor_bytes||
    p->descriptor_bytes-sd->Dacl<sizeof(ACL)+20||sd->Dacl%4)return 0;
  acl=(void *)(raw+sd->Dacl);ace=(void *)((unsigned char *)acl+sizeof(ACL));
  return acl->AclSize==sizeof(ACL)+20&&acl->AceCount==1&&
    ace->Header.AceType==ACCESS_ALLOWED_ACE_TYPE&&!ace->Header.AceFlags&&ace->Header.AceSize==20&&
    (ace->Mask==0x001f01df||ace->Mask==0x001f01ff)&&
    slcli_equal((void *)&ace->SidStart,owner_rights,12);
}
static int create(struct slcli_writer_windows_path *p,HANDLE root,
  const FILE_ID_INFO *expected,const WCHAR *leaf,size_t units,const unsigned char *descriptor,
  size_t descriptor_bytes,HANDLE effective,const TOKEN_STATISTICS *token,
  const unsigned char *owner,size_t owner_bytes,slcli_writer_windows_path_sink sink,
  slcli_writer_mode_sink mode_sink,void *archive,int directory) {
  size_t i;int ok=0;
  if(!p||p->used||!root||root==INVALID_HANDLE_VALUE||!expected||!leaf||!units||units>255||
    (p->evidence_plan&&(!p->evidence_file||!p->evidence_original||
      !p->evidence_entry_clock.complete||!slcli_evidence_deadline_ready(p->evidence_file)||
      !evidence_plan_same(p->evidence_original,p->evidence_plan)||directory||
      root!=p->evidence_plan->parent||expected!=p->evidence_plan->parent_identity||
      leaf!=p->evidence_plan->leaf||units!=p->evidence_plan->leaf_units||
      descriptor!=p->evidence_plan->descriptor||descriptor_bytes!=p->evidence_plan->descriptor_bytes||
      effective!=p->evidence_plan->effective_token||token!=p->evidence_plan->token||
      owner!=p->evidence_plan->owner||owner_bytes!=p->evidence_plan->owner_bytes||
      sink!=p->evidence_plan->path_sink||mode_sink!=p->evidence_plan->mode_sink||
      archive!=p->evidence_plan->capture_owner))||
    !descriptor||!descriptor_bytes||descriptor_bytes>sizeof(p->descriptor)||
    !sink||!mode_sink||!archive||!token||!owner||!owner_bytes||owner_bytes>184||
    !separate(p,sizeof(*p),expected,sizeof(*expected))||
    !separate(p,sizeof(*p),leaf,units*sizeof(*leaf))||
    !separate(p,sizeof(*p),descriptor,descriptor_bytes)||
    !separate(p,sizeof(*p),token,sizeof(*token))||!separate(p,sizeof(*p),owner,owner_bytes))return 0;
  if((units==1&&leaf[0]=='.')||(units==2&&leaf[0]=='.'&&leaf[1]=='.'))return 0;
  for(i=0;i<units;i++)if(!leaf[i]||leaf[i]=='/'||leaf[i]=='\\'||leaf[i]==':')return 0;
  for(i=0;i<units;i++) {
    if(leaf[i]>=0xdc00&&leaf[i]<=0xdfff)return 0;
    if(leaf[i]>=0xd800&&leaf[i]<=0xdbff) {
      if(i+1==units||leaf[i+1]<0xdc00||leaf[i+1]>0xdfff)return 0;i++;
    }
  }
  /* Preserve Unicode code units; the ordinary attempt decoder/route adapter
   * supplies the selected strict UTF8→UTF16 conversion, never a code-page cast. */
  for(i=0;i<units;i++)p->leaf[i]=leaf[i];p->leaf[units]=0;
  for(i=0;i<descriptor_bytes;i++)((unsigned char *)p->descriptor)[i]=descriptor[i];
  p->descriptor_bytes=(DWORD)descriptor_bytes;p->used=1;p->failed=1;p->parent=root;
  p->directory=(uint32_t)directory;
  if(!descriptor_shape(p,owner,owner_bytes)||!call_before(p))goto done;
  p->descriptor_valid=IsValidSecurityDescriptor(p->descriptor);
  p->call_returned[p->call_count-1]=1;
  if(!p->descriptor_valid)p->error=GetLastError();
  if(!call_after(p)||!p->descriptor_valid||!call_before(p))goto done;
  p->descriptor_control_ok=GetSecurityDescriptorControl(p->descriptor,
    &p->descriptor_control,&p->descriptor_revision);
  p->call_returned[p->call_count-1]=1;
  if(!p->descriptor_control_ok)p->error=GetLastError();
  if(!call_after(p)||!p->descriptor_control_ok||
    !(p->descriptor_control&SE_SELF_RELATIVE)||!(p->descriptor_control&SE_DACL_PROTECTED)||
    !call_before(p))goto done;
  p->descriptor_length=GetSecurityDescriptorLength(p->descriptor);
  p->call_returned[p->call_count-1]=1;
  if(!call_after(p)||p->descriptor_length!=descriptor_bytes)goto done;
  if(directory) {
    SECURITY_DESCRIPTOR_RELATIVE *sd=(void *)p->descriptor;
    ACL *acl=(void *)((unsigned char *)p->descriptor+sd->Dacl);
    ACCESS_ALLOWED_ACE *ace=(void *)((unsigned char *)acl+sizeof(*acl));
    if(ace->Mask!=0x001f01ff)goto done;
  }
  if(!parent(p,0,expected))goto done;
  p->parent_args[2][0]=(uintptr_t)root;p->parent_args[2][1]=FileStandardInfo;
  p->parent_args[2][2]=(uintptr_t)&p->parent_standard;p->parent_args[2][3]=sizeof(p->parent_standard);
  if(!call_before(p))goto done;
  p->parent_standard_ok=GetFileInformationByHandleEx(root,FileStandardInfo,&p->parent_standard,sizeof(p->parent_standard));
  p->call_returned[p->call_count-1]=1;
  if(!p->parent_standard_ok)p->error=GetLastError();
  if(!call_after(p)||!p->parent_standard_ok)goto done;
  p->parent_args[3][0]=(uintptr_t)root;p->parent_args[3][1]=FileAttributeTagInfo;
  p->parent_args[3][2]=(uintptr_t)&p->parent_attributes;p->parent_args[3][3]=sizeof(p->parent_attributes);
  if(!call_before(p))goto done;
  p->parent_attributes_ok=GetFileInformationByHandleEx(root,FileAttributeTagInfo,&p->parent_attributes,sizeof(p->parent_attributes));
  p->call_returned[p->call_count-1]=1;
  if(!p->parent_attributes_ok)p->error=GetLastError();
  if(!call_after(p)||!p->parent_attributes_ok)goto done;
  if(!p->parent_standard.Directory||p->parent_standard.DeletePending||
    p->parent_attributes.FileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)goto done;
  p->name.Length=(USHORT)(units*sizeof(WCHAR));p->name.MaximumLength=p->name.Length;
  p->name.Buffer=p->leaf;p->attributes.Length=sizeof(p->attributes);
  p->attributes.RootDirectory=root;p->attributes.ObjectName=&p->name;
  p->attributes.Attributes=0x40|0x1000;p->attributes.SecurityDescriptor=p->descriptor;
  p->create_args[0]=(uintptr_t)&p->original;
  p->create_args[1]=FILE_READ_DATA|FILE_WRITE_DATA|FILE_READ_ATTRIBUTES|DELETE|READ_CONTROL|SYNCHRONIZE;
  if(directory)p->create_args[1]|=FILE_ADD_SUBDIRECTORY|FILE_TRAVERSE;
  p->create_args[2]=(uintptr_t)&p->attributes;p->create_args[3]=(uintptr_t)&p->create_io;
  p->create_args[4]=0;p->create_args[5]=FILE_ATTRIBUTE_NORMAL;p->create_args[6]=FILE_SHARE_READ;
  p->create_args[7]=2;p->create_args[8]=0x20|0x200000|(directory?0x1:0x40);
  p->create_args[9]=0;p->create_args[10]=0;p->retained_native_io=1;
  if(!call_before(p))goto done;
  p->create_status=NtCreateFile(&p->original,(ACCESS_MASK)p->create_args[1],&p->attributes,
    &p->create_io,0,FILE_ATTRIBUTE_NORMAL,FILE_SHARE_READ,2,(ULONG)p->create_args[8],0,0);
  p->call_returned[p->call_count-1]=1;
  if(!call_after(p)||p->create_status!=0||p->create_io.Status!=0||p->create_io.Information!=2)goto done;
  p->retained_native_io=0;
  if(!call_before(p))goto done;
  p->mode_observe_ok=directory?slcli_writer_observe_windows_directory(&p->mode,p->original,effective,token,owner,
    owner_bytes,mode_sink,archive):slcli_writer_observe_windows_mode(&p->mode,p->original,effective,token,owner,
    owner_bytes,mode_sink,archive);
  p->call_returned[p->call_count-1]=1;
  if(!call_after(p)||!p->mode_observe_ok||(!directory&&p->mode.file.standard.EndOfFile.QuadPart)||
    !parent(p,1,expected))goto done;
  ok=1;
done:
  if(!sink(archive,p)||!ok)return 0;p->complete=1;p->failed=0;return 1;
}
int slcli_writer_create_windows_file(struct slcli_writer_windows_path *p,HANDLE root,
  const FILE_ID_INFO *expected,const WCHAR *leaf,size_t units,const unsigned char *descriptor,
  size_t descriptor_bytes,HANDLE effective,const TOKEN_STATISTICS *token,
  const unsigned char *owner,size_t owner_bytes,slcli_writer_windows_path_sink sink,
  slcli_writer_mode_sink mode_sink,void *archive) {
  return create(p,root,expected,leaf,units,descriptor,descriptor_bytes,effective,token,
    owner,owner_bytes,sink,mode_sink,archive,0);
}
int slcli_writer_create_windows_directory(struct slcli_writer_windows_path *p,HANDLE root,
  const FILE_ID_INFO *expected,const WCHAR *leaf,size_t units,const unsigned char *descriptor,
  size_t descriptor_bytes,HANDLE effective,const TOKEN_STATISTICS *token,
  const unsigned char *owner,size_t owner_bytes,slcli_writer_windows_path_sink sink,
  slcli_writer_mode_sink mode_sink,void *archive) {
  return create(p,root,expected,leaf,units,descriptor,descriptor_bytes,effective,token,
    owner,owner_bytes,sink,mode_sink,archive,1);
}
int slcli_writer_begin_windows_created_file(struct slcli_writer_windows_path *p,
  struct slcli_writer_windows_output *o,uint64_t maximum,slcli_store_capture_sink sink,void *archive) {
  if(!p||!p->complete||p->failed||p->directory||p->retained_native_io||!o||o->used||!o->file||
    !separate(p,sizeof(*p),o,sizeof(*o)))return 0;
  return slcli_evidence_begin(o->file,p->original,p->parent,maximum,sink,archive);
}
#endif
