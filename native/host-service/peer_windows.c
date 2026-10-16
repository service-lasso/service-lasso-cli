#include "peer_windows.h"
#include "image_deadline.h"
#if defined(_WIN32)
#include <stddef.h>
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int time_equal(FILETIME a,FILETIME b) {
  return a.dwLowDateTime==b.dwLowDateTime&&a.dwHighDateTime==b.dwHighDateTime;
}
static int token_equal(const TOKEN_STATISTICS *a,const TOKEN_STATISTICS *b) {
  return a->TokenId.LowPart==b->TokenId.LowPart&&a->TokenId.HighPart==b->TokenId.HighPart&&
    a->AuthenticationId.LowPart==b->AuthenticationId.LowPart&&a->AuthenticationId.HighPart==b->AuthenticationId.HighPart&&
    a->ModifiedId.LowPart==b->ModifiedId.LowPart&&a->ModifiedId.HighPart==b->ModifiedId.HighPart&&
    a->ExpirationTime.QuadPart==b->ExpirationTime.QuadPart&&a->TokenType==b->TokenType&&
    a->ImpersonationLevel==b->ImpersonationLevel&&a->GroupCount==b->GroupCount&&a->PrivilegeCount==b->PrivilegeCount;
}
static int process(struct slcli_peer_windows_capture *c,unsigned i,const struct slcli_peer_windows_originals *o) {
  c->pid_args[i][0]=(uintptr_t)c->process;c->pid[i]=GetProcessId(c->process);
  c->peer_args[i][0]=(uintptr_t)c->channel;c->peer_args[i][1]=(uintptr_t)&c->peer_pid[i];
  c->peer_ok[i]=GetNamedPipeClientProcessId(c->channel,&c->peer_pid[i]);
  if(!c->pid[i]||!c->peer_ok[i]){c->error=GetLastError();return 0;}
  c->time_args[i][0]=(uintptr_t)c->process;c->time_args[i][1]=(uintptr_t)&c->creation[i];
  c->time_args[i][2]=(uintptr_t)&c->exit[i];c->time_args[i][3]=(uintptr_t)&c->kernel[i];
  c->time_args[i][4]=(uintptr_t)&c->user_time[i];
  c->times_ok[i]=GetProcessTimes(c->process,&c->creation[i],&c->exit[i],&c->kernel[i],&c->user_time[i]);
  if(!c->times_ok[i]){c->error=GetLastError();return 0;}
  return c->pid[i]==c->peer_pid[i]&&time_equal(c->creation[i],o->creation)&&
    (c->creation[i].dwLowDateTime||c->creation[i].dwHighDateTime)&&
    !c->exit[i].dwLowDateTime&&!c->exit[i].dwHighDateTime;
}
static int token(struct slcli_peer_windows_capture *c,unsigned i,const struct slcli_peer_windows_originals *o) {
  TOKEN_USER *user=(void *)c->user_raw[i];uintptr_t base=(uintptr_t)user,sid;size_t bytes;
  c->token_args[i][0]=(uintptr_t)c->owner_token;c->token_args[i][1]=TokenStatistics;
  c->token_args[i][2]=(uintptr_t)&c->token[i];c->token_args[i][3]=sizeof(c->token[i]);
  c->token_args[i][4]=(uintptr_t)&c->token_bytes[i];
  c->token_ok[i]=GetTokenInformation(c->owner_token,TokenStatistics,&c->token[i],sizeof(c->token[i]),&c->token_bytes[i]);
  if(!c->token_ok[i]){c->error=GetLastError();return 0;}
  if(c->token_bytes[i]!=sizeof(c->token[i])||c->token[i].TokenType!=TokenPrimary||
    !token_equal(&c->token[i],o->token))return 0;
  c->user_args[i][0]=(uintptr_t)c->owner_token;c->user_args[i][1]=TokenUser;
  c->user_args[i][2]=base;c->user_args[i][3]=sizeof(c->user_raw[i]);
  c->user_args[i][4]=(uintptr_t)&c->user_bytes[i];
  c->user_ok[i]=GetTokenInformation(c->owner_token,TokenUser,user,sizeof(c->user_raw[i]),&c->user_bytes[i]);
  if(!c->user_ok[i]){c->error=GetLastError();return 0;}
  if(c->user_bytes[i]<sizeof(*user)||c->user_bytes[i]>sizeof(c->user_raw[i]))return 0;sid=(uintptr_t)user->User.Sid;
  if(sid<base||sid-base>c->user_bytes[i]||c->user_bytes[i]-(sid-base)<8||sid%_Alignof(SID))return 0;
  bytes=8+4*(size_t)((SID *)sid)->SubAuthorityCount;
  return bytes==o->owner_bytes&&bytes<=c->user_bytes[i]-(sid-base)&&
    IsValidSid((PSID)sid)&&GetLengthSid((PSID)sid)==bytes&&slcli_equal((void *)sid,o->owner,bytes);
}
int slcli_peer_windows_deadline_prepare(struct slcli_arena *a,struct slcli_peer_windows_capture *c,
  const struct slcli_peer_windows_originals *o,const struct slcli_source_deadline_original *d,
  const void *caller,size_t caller_bytes,void *owner,size_t owner_bytes,uint64_t predecessor) {
  struct slcli_image_reader *held,*route;
  if(!a||!c||!o||c->used||c->image_route||!(held=c->held_reader)||!(route=c->route_reader)||
    held==route||held->used||route->used||held->deadline_owner||route->deadline_owner||
    !o->image||o->image==INVALID_HANDLE_VALUE||
    !slcli_arena_live_range(a,c,sizeof(*c))||!slcli_arena_live_range(a,o,sizeof(*o))||
    !separate(c,sizeof(*c),o,sizeof(*o))||
    !held->captures||!route->captures||held->capture_capacity>SIZE_MAX/sizeof(*held->captures)||
    route->capture_capacity>SIZE_MAX/sizeof(*route->captures)||
    !separate(held->captures,held->capture_capacity*sizeof(*held->captures),
      route->captures,route->capture_capacity*sizeof(*route->captures)))return 0;
  held->original=o->image;route->original=0;
  return slcli_image_deadline_bind(a,held,d,caller,caller_bytes,owner,owner_bytes,predecessor)&&
    slcli_image_deadline_prepare_unborn(a,route,d,caller,caller_bytes,owner,owner_bytes,
      predecessor,&c->image_route,c,sizeof(*c));
}
int slcli_peer_capture_windows(struct slcli_peer_windows_capture *c,const struct slcli_peer_windows_originals *o,
  slcli_peer_windows_sink sink,void *archive) {
  size_t i;int ok=0;struct slcli_image_reader *held,*route;
  if(!c||c->used||!o||!sink||!archive||!o->process||o->process==INVALID_HANDLE_VALUE||
    !o->channel||o->channel==INVALID_HANDLE_VALUE||!o->image||o->image==INVALID_HANDLE_VALUE||
    !o->token||!o->owner||!o->owner_bytes||o->owner_bytes>184||!(held=c->held_reader)||
    !(route=c->route_reader)||held==route||held->used||route->used||
    !slcli_image_deadline_ready(held)||!slcli_image_deadline_ready(route)||
    held->deadline_owner->caller!=route->deadline_owner->caller||
    held->deadline_owner->caller_bytes!=route->deadline_owner->caller_bytes||
    held->deadline_owner->deadline!=route->deadline_owner->deadline||
    held->original!=o->image||!route->deadline_owner->unborn||
    route->deadline_owner->birth_cell!=&c->image_route||
    route->deadline_owner->birth_producer!=c||route->deadline_owner->birth_producer_bytes!=sizeof(*c)||
    !c->path||c->path->borrower||
    !c->path->raw||c->path->units!=32772||
    !separate(c,sizeof(*c),o,sizeof(*o))||!separate(c,sizeof(*c),o->token,sizeof(*o->token))||
    !separate(c,sizeof(*c),o->owner,o->owner_bytes)||!separate(c,sizeof(*c),held,sizeof(*held))||
    !separate(c,sizeof(*c),route,sizeof(*route))||
    !separate(c,sizeof(*c),c->path->raw,c->path->units*sizeof(WCHAR))||
    !separate(c->path->raw,c->path->units*sizeof(WCHAR),o,sizeof(*o))||
    !separate(c->path->raw,c->path->units*sizeof(WCHAR),o->owner,o->owner_bytes)||
    !separate(c->path->raw,c->path->units*sizeof(WCHAR),o->token,sizeof(*o->token)))return 0;
  /* Original precharged capture starts entirely zero except its three owning
   * input pointers. Never serialize stale padding or overwrite failed native
   * originals merely because the local used marker happened to be zero. */
  for(i=0;i<sizeof(*c);i++) {
    if(i>=offsetof(struct slcli_peer_windows_capture,held_reader)&&
      i<offsetof(struct slcli_peer_windows_capture,before_clock))continue;
    if(((const unsigned char *)c)[i])return 0;
  }
  c->used=1;c->failed=1;c->path->borrower=c;c->process=o->process;c->channel=o->channel;
  if(!slcli_native_clock_read(&c->before_clock)||
    !slcli_image_deadline_precede(held,&c->before_clock)||
    !slcli_image_deadline_precede(route,&c->before_clock)||!process(c,0,o))goto done;
  c->token_open_args[0]=(uintptr_t)c->process;c->token_open_args[1]=TOKEN_QUERY;
  c->token_open_args[2]=(uintptr_t)&c->owner_token;
  c->token_open_ok=OpenProcessToken(c->process,TOKEN_QUERY,&c->owner_token);
  if(!c->token_open_ok){c->error=GetLastError();goto done;}if(!token(c,0,o))goto done;
  if(!slcli_native_clock_read(&c->held_entry_clock)||
    !slcli_image_deadline_precede(held,&c->held_entry_clock)||!slcli_image_read_original(held))goto done;
  /* Query the SAME process's native Unicode route, without converting DOS/
   * UNC syntax, making a pathname authority or borrowing a code-page path. */
  c->image_units=32768;c->path_args[0]=(uintptr_t)c->process;c->path_args[1]=1;
  for(i=0;i<32768;i++)c->path->raw[i]=0;
  c->path_args[2]=(uintptr_t)c->path->raw;c->path_args[3]=(uintptr_t)&c->image_units;
  c->path_ok=QueryFullProcessImageNameW(c->process,1,c->path->raw,&c->image_units);
  if(!c->path_ok){c->error=GetLastError();goto done;}
  if(!c->image_units||c->image_units>=32768||c->path->raw[0]!='\\')goto done;
  /* NtCreateFile UNICODE_STRING Length is USHORT bytes: retain, reject an
   * unrepresentable native route rather than truncate or create a new path. */
  for(i=0;i<c->image_units;i++)if(!c->path->raw[i])goto done;
  c->image_name.Buffer=c->path->raw;c->image_name.Length=(USHORT)(2*c->image_units);
  c->image_name.MaximumLength=c->image_name.Length;
  c->image_attributes.Length=sizeof(c->image_attributes);c->image_attributes.ObjectName=&c->image_name;
  c->image_attributes.Attributes=0x40|0x1000;
  c->image_args[0]=(uintptr_t)&c->image_route;c->image_args[1]=0x00120081;
  c->image_args[2]=(uintptr_t)&c->image_attributes;c->image_args[3]=(uintptr_t)&c->image_io;
  c->image_args[6]=FILE_SHARE_READ;c->image_args[7]=1;c->image_args[8]=0x20|0x40|0x200000;
  if(!slcli_native_clock_read(&c->route_entry_clock)||
    c->route_entry_clock.tick<held->deadline_owner->previous_tick||
    !slcli_image_deadline_precede(route,&c->route_entry_clock))goto done;
  c->pending=1;
  c->image_status=NtCreateFile(&c->image_route,0x00120081,&c->image_attributes,&c->image_io,
    0,0,FILE_SHARE_READ,1,0x20|0x40|0x200000,0,0);
  if(c->image_status!=0||c->image_io.Status!=0||c->image_io.Information!=1)goto done;c->pending=0;
  /* Actual successful output is joined even if its diagnostic clock fails. */
  slcli_native_clock_read(&c->route_birth_clock);
  if(!slcli_image_deadline_born(route,&c->image_route,&c->route_birth_clock)||
    !slcli_image_read_original(route)||
    held->bytes!=route->bytes||!slcli_equal(held->digest,route->digest,32)||
    held->after->identity.VolumeSerialNumber!=route->after->identity.VolumeSerialNumber||
    !slcli_equal(held->after->identity.FileId.Identifier,route->after->identity.FileId.Identifier,16)||
    !process(c,1,o)||c->pid[0]!=c->pid[1]||!token(c,1,o)||
    !token_equal(&c->token[0],&c->token[1])||!slcli_native_clock_read(&c->after_clock)||
    c->after_clock.tick<route->deadline_owner->previous_tick||
    c->after_clock.tick>=route->deadline_owner->expires)goto done;
  ok=1;
done:
  /* Retain complete ORIGINAL queries and exact native path. The path lease
   * remains held even after successful capture transfer: a callback result
   * cannot grant raw-source retirement or original path-buffer reuse. */
  if(!sink(archive,c,c->path->raw,c->path_args[2]?32768:0)||!ok)return 0;
  c->complete=1;c->failed=0;return 1;
}
#endif
