/* SPDX-License-Identifier: Apache-2.0 */
#include "bq_provider_channel_windows.h"
#if defined(_WIN32)
static int zero(const void *p,size_t n){const unsigned char *b=p;size_t i;for(i=0;i<n;++i)if(b[i])return 0;return 1;}
static int apart(const void *a,size_t n,const void *b,size_t m){uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&n<=UINTPTR_MAX-x&&m<=UINTPTR_MAX-y&&(x+n<=y||y+m<=x);}
static int equal(const void *a,const void *b,size_t n){const unsigned char *x=a,*y=b;size_t i;
 for(i=0;i<n;++i)if(x[i]!=y[i])return 0;return 1;}
static int same(const struct slcli_bq_provider_channel_windows *c){
 const struct slcli_bq_expected_principal *p;
 if(!c||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||!c->started||
  !slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)||
  !slcli_arena_live_block(c->arena,c->caller,c->caller_bytes)||
  !slcli_arena_live_range(c->arena,c->prior,sizeof(*c->prior))||!c->prior->complete||
  c->prior->tick!=c->prior_tick||c->deadline->origin!=c->origin||
  c->origin->tick!=c->origin_tick||c->deadline->expires!=c->expires||
  !slcli_arena_live_block(c->arena,c->name,c->name_bytes)||
  !slcli_arena_live_block(c->arena,c->principal,sizeof(*c->principal))||
  !slcli_bootstrap_qualification_windows_identity_matches_original(c->identity)||
  !c->identity->observed||c->identity->arena!=c->arena||c->identity->deadline!=c->deadline||
  c->identity->caller!=c->caller||c->identity->caller_bytes!=c->caller_bytes||
  c->identity->finish.tick>c->prior_tick)return 0;
 p=c->principal;
 return p->platform==SLCLI_BQ_EXPECTED_WINDOWS&&p->windows_sid_bytes==c->identity->sid_bytes&&
  slcli_arena_live_block(c->arena,p->windows_sid,p->windows_sid_bytes)&&
  equal(p->windows_sid,c->identity->user.bytes+c->identity->sid_offset,p->windows_sid_bytes)&&
  p->windows_token_id[0]==c->identity->statistics.TokenId.LowPart&&
  p->windows_token_id[1]==(uint32_t)c->identity->statistics.TokenId.HighPart&&
  p->windows_authentication_id[0]==c->identity->statistics.AuthenticationId.LowPart&&
  p->windows_authentication_id[1]==(uint32_t)c->identity->statistics.AuthenticationId.HighPart;
}
static int tick(struct slcli_bq_provider_channel_windows *c,unsigned i,int after){
 struct slcli_native_clock *t=after?&c->after[i]:&c->before[i];int result;
 result=slcli_native_clock_read(t);
 if(after){c->after_result[i]=result;c->after_returned[i]=1;}
 else{c->before_result[i]=result;c->before_returned[i]=1;}
 if(!result||!same(c)||!t->complete||t->tick<c->previous_tick||t->tick>=c->expires){
  if(!c->first_failure)c->first_failure=i+1;return 0;}
 c->previous_tick=t->tick;return 1;
}
static void returned(struct slcli_bq_provider_channel_windows *c,unsigned i,int success){
 c->returned[i]=1;if(!success)c->error[i]=GetLastError();
}
static int descriptor(struct slcli_bq_provider_channel_windows *c){
 SECURITY_DESCRIPTOR_RELATIVE *sd=(void *)c->security.bytes;ACL *acl;ACCESS_ALLOWED_ACE *ace;
 size_t i,n=c->principal->windows_sid_bytes,offset=sizeof(*sd)+n;
 if(n<8||n>SECURITY_MAX_SID_SIZE||n%4||offset>sizeof(c->security.bytes)||
  sizeof(ACL)+8+n>sizeof(c->security.bytes)-offset)return 0;
 sd->Revision=SECURITY_DESCRIPTOR_REVISION;
 sd->Control=SE_SELF_RELATIVE|SE_DACL_PRESENT|SE_DACL_PROTECTED;
 sd->Owner=(DWORD)sizeof(*sd);sd->Group=sd->Owner;sd->Dacl=(DWORD)offset;
 for(i=0;i<n;++i)c->security.bytes[sizeof(*sd)+i]=c->principal->windows_sid[i];
 acl=(void *)(c->security.bytes+offset);acl->AclRevision=ACL_REVISION;
 acl->AclSize=(WORD)(sizeof(ACL)+8+n);acl->AceCount=1;
 ace=(void *)((unsigned char *)acl+sizeof(ACL));ace->Header.AceType=ACCESS_ALLOWED_ACE_TYPE;
 ace->Header.AceSize=(WORD)(8+n);ace->Mask=FILE_GENERIC_READ|FILE_GENERIC_WRITE|SYNCHRONIZE;
 for(i=0;i<n;++i)((unsigned char *)&ace->SidStart)[i]=c->principal->windows_sid[i];
 c->security_bytes=offset+acl->AclSize;c->attributes.nLength=sizeof(c->attributes);
 c->attributes.lpSecurityDescriptor=c->security.bytes;c->attributes.bInheritHandle=FALSE;return 1;
}
static int rows(const struct slcli_bq_provider_channel_windows *c){
 const SECURITY_DESCRIPTOR_RELATIVE *sd;const ACL *acl;const ACCESS_ALLOWED_ACE *ace;
 HANDLE handles[4];uintptr_t expected[8];uint64_t previous;size_t i,j,n,offset;int pending;
 if(!same(c)||c->first_failure||!c->server||c->server==INVALID_HANDLE_VALUE||
  !c->client||c->client==INVALID_HANDLE_VALUE||!c->event||!c->child_endpoint||
  c->child_endpoint==INVALID_HANDLE_VALUE||!c->connect_complete||c->connect_pending)return 0;
 n=c->principal->windows_sid_bytes;offset=sizeof(*sd)+n;
 if(n<8||n>SECURITY_MAX_SID_SIZE||n%4||offset>sizeof(c->security.bytes)||
  sizeof(ACL)+8+n>sizeof(c->security.bytes)-offset)return 0;
 sd=(const void *)c->security.bytes;acl=(const void *)(c->security.bytes+offset);
 ace=(const void *)((const unsigned char *)acl+sizeof(*acl));
 if(sd->Revision!=SECURITY_DESCRIPTOR_REVISION||sd->Sbz1||
  sd->Control!=(SE_SELF_RELATIVE|SE_DACL_PRESENT|SE_DACL_PROTECTED)||sd->Sacl||
  sd->Owner!=sizeof(*sd)||sd->Group!=sizeof(*sd)||sd->Dacl!=offset||
  !equal(c->security.bytes+sizeof(*sd),c->principal->windows_sid,n)||
  acl->AclRevision!=ACL_REVISION||acl->Sbz1||acl->Sbz2||acl->AceCount!=1||acl->AclSize!=sizeof(*acl)+8+n||
  ace->Header.AceType!=ACCESS_ALLOWED_ACE_TYPE||ace->Header.AceFlags||ace->Header.AceSize!=8+n||
  ace->Mask!=(FILE_GENERIC_READ|FILE_GENERIC_WRITE|SYNCHRONIZE)||
  !equal(&ace->SidStart,c->principal->windows_sid,n)||c->security_bytes!=offset+acl->AclSize||
  !zero(c->security.bytes+c->security_bytes,sizeof(c->security.bytes)-c->security_bytes)||
  c->attributes.nLength!=sizeof(c->attributes)||c->attributes.lpSecurityDescriptor!=c->security.bytes||
  c->attributes.bInheritHandle||c->connect.hEvent!=c->event||c->connect.Offset||c->connect.OffsetHigh||
  c->mode!=PIPE_READMODE_MESSAGE)return 0;
 pending=!c->status[2]&&c->error[2]==ERROR_IO_PENDING;
 if(!c->status[2]&&!pending&&c->error[2]!=ERROR_PIPE_CONNECTED)return 0;
 handles[0]=c->server;handles[1]=c->client;handles[2]=c->event;handles[3]=c->child_endpoint;
 for(i=0;i<4;++i)for(j=0;j<i;++j)if(handles[i]==handles[j])return 0;
 previous=c->prior_tick;
 for(i=0;i<11;++i){
  if(i==5&&!pending){
   if(c->called[i]||c->returned[i]||c->before_returned[i]||c->after_returned[i]||
    c->status[i]||c->error[i]||!zero(c->args[i],sizeof(c->args[i])))return 0;continue;
  }
  if(c->called[i]!=1||c->returned[i]!=1||c->before_returned[i]!=1||c->after_returned[i]!=1||
   !c->before_result[i]||!c->after_result[i]||!c->before[i].complete||!c->after[i].complete||
   c->before[i].tick<previous||c->after[i].tick<c->before[i].tick||c->after[i].tick>=c->expires)return 0;
  previous=c->after[i].tick;for(j=0;j<8;++j)expected[j]=0;
  if(i==0){expected[1]=TRUE;if(c->status[i]!=(uintptr_t)c->event)return 0;}
  else if(i==1){expected[0]=(uintptr_t)c->name;
   expected[1]=PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE;
   expected[2]=PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_REJECT_REMOTE_CLIENTS;
   expected[3]=1;expected[4]=64;expected[5]=64;expected[7]=(uintptr_t)&c->attributes;
   if(c->status[i]!=(uintptr_t)c->server)return 0;
  }else if(i==2){expected[0]=(uintptr_t)c->server;expected[1]=(uintptr_t)&c->connect;}
  else if(i==3){expected[0]=(uintptr_t)c->name;expected[1]=GENERIC_READ|GENERIC_WRITE;
   expected[4]=OPEN_EXISTING;expected[5]=FILE_FLAG_OVERLAPPED;
   if(c->status[i]!=(uintptr_t)c->client)return 0;
  }else if(i==4){expected[0]=(uintptr_t)c->client;expected[1]=(uintptr_t)&c->mode;if(!c->status[i])return 0;}
  else if(i==5){expected[0]=(uintptr_t)c->server;expected[1]=(uintptr_t)&c->connect;
   expected[2]=(uintptr_t)&c->connect_count;if(!c->status[i])return 0;
  }else if(i==6){expected[0]=(uintptr_t)c->identity->process_pseudo;expected[1]=(uintptr_t)c->client;
   expected[2]=(uintptr_t)c->identity->process_pseudo;expected[3]=(uintptr_t)&c->child_endpoint;
   expected[5]=TRUE;expected[6]=DUPLICATE_SAME_ACCESS;if(!c->status[i])return 0;
  }else{j=i-7;expected[0]=(uintptr_t)handles[j];expected[1]=(uintptr_t)&c->handle_flags[j];
   if(!c->status[i]||c->handle_flags[j]!=(j==3?HANDLE_FLAG_INHERIT:0))return 0;
  }
  if(!equal(expected,c->args[i],sizeof(expected))||(i!=2&&c->error[i])||
   (i==2&&c->status[i]&&c->error[i]))return 0;
 }
 return c->previous_tick==previous;
}
int slcli_bq_provider_channel_windows_matches_original(const struct slcli_bq_provider_channel_windows *c){
 return c&&c->observed==1&&rows(c)&&c->finish_returned==1&&c->finish_result&&c->finish.complete&&
  c->finish.tick>=c->previous_tick&&c->finish.tick<c->expires;
}
int slcli_bq_provider_channel_windows_original(struct slcli_bq_provider_channel_windows *c,
 struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const struct slcli_bq_windows_identity_original *identity,
 const struct slcli_bq_expected_principal *principal,const WCHAR *name,size_t name_bytes){
 static const WCHAR prefix[]=L"\\\\.\\pipe\\slcli.bq.";size_t i,units;HANDLE handles[4];
 const void *borrowed[9];size_t sizes[9];
 if(!c||!a||!d||!caller||!caller_bytes||!prior||!identity||!principal||!name||
  !slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !slcli_arena_live_block(a,caller,caller_bytes)||!slcli_source_deadline_original_matches(a,d,caller)||
  !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||prior->tick<d->origin->tick||prior->tick>=d->expires||
  !slcli_arena_live_block(a,principal,sizeof(*principal))||!principal->windows_sid_bytes||
  !slcli_arena_live_block(a,principal->windows_sid,principal->windows_sid_bytes)||
  !slcli_arena_live_block(a,name,name_bytes)||name_bytes%sizeof(WCHAR)||
  name_bytes<sizeof(prefix)+sizeof(WCHAR)||name_bytes>256*sizeof(WCHAR))return 0;
 units=name_bytes/sizeof(WCHAR);if(name[units-1])return 0;
 for(i=0;i<sizeof(prefix)/sizeof(WCHAR)-1;++i)if(name[i]!=prefix[i])return 0;
 for(;i+1<units;++i)if(!((name[i]>='a'&&name[i]<='z')||(name[i]>='0'&&name[i]<='9')||name[i]=='.'||name[i]=='-'))return 0;
 borrowed[0]=a;sizes[0]=sizeof(*a);borrowed[1]=d;sizes[1]=sizeof(*d);
 borrowed[2]=d->origin;sizes[2]=sizeof(*d->origin);borrowed[3]=caller;sizes[3]=caller_bytes;
 borrowed[4]=prior;sizes[4]=sizeof(*prior);borrowed[5]=identity;sizes[5]=sizeof(*identity);
 borrowed[6]=principal;sizes[6]=sizeof(*principal);borrowed[7]=principal->windows_sid;sizes[7]=principal->windows_sid_bytes;
 borrowed[8]=name;sizes[8]=name_bytes;
 for(i=0;i<9;++i)if(!apart(c,sizeof(*c),borrowed[i],sizes[i]))return 0;
 c->started=1;c->arena=a;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->prior=prior;c->prior_tick=prior->tick;c->origin=d->origin;c->origin_tick=d->origin->tick;
 c->expires=d->expires;c->previous_tick=prior->tick;c->identity=identity;c->principal=principal;
 c->name=name;c->name_bytes=name_bytes;
 if(!same(c)||!descriptor(c))return 0;
 if(!tick(c,0,0))return 0;
 c->args[0][1]=TRUE;c->called[0]=1;c->event=CreateEventW(NULL,TRUE,FALSE,NULL);
 c->status[0]=(uintptr_t)c->event;returned(c,0,c->event!=NULL);
 if(!tick(c,0,1)||!c->event)return 0;c->connect.hEvent=c->event;
 if(!tick(c,1,0))return 0;
 c->args[1][0]=(uintptr_t)name;c->args[1][1]=PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE;
 c->args[1][2]=PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_REJECT_REMOTE_CLIENTS;
 c->args[1][3]=1;c->args[1][4]=64;c->args[1][5]=64;c->args[1][7]=(uintptr_t)&c->attributes;c->called[1]=1;
 c->server=CreateNamedPipeW(name,(DWORD)c->args[1][1],(DWORD)c->args[1][2],1,64,64,0,&c->attributes);
 c->status[1]=(uintptr_t)c->server;returned(c,1,c->server!=INVALID_HANDLE_VALUE);
 if(!tick(c,1,1)||c->server==INVALID_HANDLE_VALUE)return 0;
 if(!tick(c,2,0))return 0;
 c->args[2][0]=(uintptr_t)c->server;c->args[2][1]=(uintptr_t)&c->connect;c->called[2]=1;
 c->status[2]=(uint32_t)ConnectNamedPipe(c->server,&c->connect);returned(c,2,c->status[2]!=0);
 c->connect_pending=!c->status[2]&&c->error[2]==ERROR_IO_PENDING;
 c->connect_complete=c->status[2]||c->error[2]==ERROR_PIPE_CONNECTED;
 if(!tick(c,2,1)||(!c->connect_pending&&!c->connect_complete))return 0;
 if(!tick(c,3,0))return 0;
 c->args[3][0]=(uintptr_t)name;c->args[3][1]=GENERIC_READ|GENERIC_WRITE;c->args[3][4]=OPEN_EXISTING;
 c->args[3][5]=FILE_FLAG_OVERLAPPED;c->called[3]=1;
 c->client=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
 c->status[3]=(uintptr_t)c->client;returned(c,3,c->client!=INVALID_HANDLE_VALUE);
 if(!tick(c,3,1)||c->client==INVALID_HANDLE_VALUE)return 0;
 if(!tick(c,4,0))return 0;c->mode=PIPE_READMODE_MESSAGE;
 c->args[4][0]=(uintptr_t)c->client;c->args[4][1]=(uintptr_t)&c->mode;c->called[4]=1;
 c->status[4]=(uint32_t)SetNamedPipeHandleState(c->client,&c->mode,NULL,NULL);returned(c,4,c->status[4]!=0);
 if(!tick(c,4,1)||!c->status[4])return 0;
 if(c->connect_pending){
  if(!tick(c,5,0))return 0;c->args[5][0]=(uintptr_t)c->server;c->args[5][1]=(uintptr_t)&c->connect;
  c->args[5][2]=(uintptr_t)&c->connect_count;c->called[5]=1;
  c->status[5]=(uint32_t)GetOverlappedResult(c->server,&c->connect,&c->connect_count,FALSE);returned(c,5,c->status[5]!=0);
  if(c->status[5]){c->connect_pending=0;c->connect_complete=1;}
  if(!tick(c,5,1)||!c->status[5])return 0;
 }
 if(!tick(c,6,0))return 0;
 c->args[6][0]=(uintptr_t)identity->process_pseudo;c->args[6][1]=(uintptr_t)c->client;
 c->args[6][2]=(uintptr_t)identity->process_pseudo;c->args[6][3]=(uintptr_t)&c->child_endpoint;
 c->args[6][5]=TRUE;c->args[6][6]=DUPLICATE_SAME_ACCESS;c->called[6]=1;
 c->status[6]=(uint32_t)DuplicateHandle(identity->process_pseudo,c->client,identity->process_pseudo,
  &c->child_endpoint,0,TRUE,DUPLICATE_SAME_ACCESS);returned(c,6,c->status[6]!=0);
 if(!tick(c,6,1)||!c->status[6]||!c->child_endpoint||c->child_endpoint==INVALID_HANDLE_VALUE)return 0;
 handles[0]=c->server;handles[1]=c->client;handles[2]=c->event;handles[3]=c->child_endpoint;
 for(i=0;i<4;++i){unsigned row=(unsigned)i+7;if(!tick(c,row,0))return 0;
  c->args[row][0]=(uintptr_t)handles[i];c->args[row][1]=(uintptr_t)&c->handle_flags[i];c->called[row]=1;
  c->status[row]=(uint32_t)GetHandleInformation(handles[i],&c->handle_flags[i]);returned(c,row,c->status[row]!=0);
  if(!tick(c,row,1)||!c->status[row]||c->handle_flags[i]!=(i==3?HANDLE_FLAG_INHERIT:0))return 0;
 }
 if(!rows(c))return 0;
 c->finish_result=slcli_native_clock_read(&c->finish);c->finish_returned=1;
 if(!c->finish_result||!same(c)||!c->finish.complete||c->finish.tick<c->previous_tick||c->finish.tick>=c->expires)return 0;
 c->observed=1;return 1;
}
#endif
