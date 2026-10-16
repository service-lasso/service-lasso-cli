#include "bootstrap_qualification_admission.h"
#if defined(_WIN32)
static int zero(const void *p,size_t n){const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;}
static int apart(const void *a,size_t n,const void *b,size_t m){uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);}
static void fail(struct slcli_bq_windows_identity_original *c,uint32_t n){if(!c->first_failure)c->first_failure=n;}
static int same(const struct slcli_bq_windows_identity_original *c){
 return c&&c->arena&&slcli_arena_live_block(c->arena,c,sizeof(*c))&&c->started&&
  slcli_arena_live_block(c->arena,c->caller,c->caller_bytes)&&
  slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)&&
  c->deadline->origin==c->origin_original&&c->origin_original->tick==c->origin_tick&&c->deadline->expires==c->expires;
}
static int clock_at(struct slcli_bq_windows_identity_original *c,unsigned row,unsigned side){
 c->clock_results[row][side]=slcli_native_clock_read(&c->clocks[row][side]);c->clock_returned[row][side]=1;
 if(!c->clock_results[row][side]||!same(c)||c->clocks[row][side].tick<c->previous_tick||
  c->clocks[row][side].tick>=c->expires){fail(c,1+row);return 0;}
 c->previous_tick=c->clocks[row][side].tick;return 1;
}
static void returned(struct slcli_bq_windows_identity_original *c,unsigned row){
 c->returned[row]=1;
 if(!c->status[row]){c->errors[row]=GetLastError();c->error_observed[row]=1;}
}
static void close_once(struct slcli_bq_windows_identity_original *c,unsigned row,HANDLE token){
 if(!token||token==INVALID_HANDLE_VALUE||c->called[row])return;
 /* Cleanup occurs once independently of useful clocks or earlier failure. */
 clock_at(c,row,0);c->args[row][0]=(uintptr_t)token;c->called[row]=1;
 c->status[row]=CloseHandle(token);returned(c,row);
 if(!c->status[row])fail(c,1+row);clock_at(c,row,1);
}
int slcli_bootstrap_qualification_windows_identity_matches_original(
 const struct slcli_bq_windows_identity_original *c){
 const TOKEN_USER *u;uintptr_t base,sid;size_t offset,n;uint64_t prior;unsigned i,j;
 if(!same(c)||c->first_failure||!c->process_pseudo||!c->thread_pseudo||!c->process_id||!c->thread_id||
  !c->selected_token||c->selected_token==INVALID_HANDLE_VALUE||c->called[5]||c->called[6])return 0;
 for(i=0;i<4;i++)if(c->pseudo_called[i]!=1||c->pseudo_returned[i]!=1)return 0;
 if(c->args[0][0]!=(uintptr_t)c->thread_pseudo||c->args[0][1]!=TOKEN_QUERY||c->args[0][2]!=TRUE||
  c->args[0][3]!=(uintptr_t)&c->thread_token||c->called[0]!=1||c->returned[0]!=1)return 0;
 if(c->status[0]){
  if(c->selected_token!=c->thread_token||c->process_token||c->called[1]||c->returned[1]||c->error_observed[0])return 0;
 }else{
  if(c->thread_token||c->error_observed[0]!=1||c->errors[0]!=ERROR_NO_TOKEN||
   c->selected_token!=c->process_token||c->called[1]!=1||c->returned[1]!=1||!c->status[1]||c->error_observed[1]||
   c->args[1][0]!=(uintptr_t)c->process_pseudo||c->args[1][1]!=TOKEN_QUERY||
   c->args[1][2]!=(uintptr_t)&c->process_token)return 0;
 }
 for(i=2;i<=4;i++)if(c->called[i]!=1||c->returned[i]!=1||!c->status[i]||c->error_observed[i])return 0;
 if(c->args[2][0]!=(uintptr_t)c->selected_token||c->args[2][1]!=TokenUser||
  c->args[2][2]!=(uintptr_t)c->user.bytes||c->args[2][3]!=sizeof(c->user.bytes)||
  c->args[2][4]!=(uintptr_t)&c->user_bytes||c->user_bytes<sizeof(TOKEN_USER)||c->user_bytes>sizeof(c->user.bytes))return 0;
 u=(const TOKEN_USER *)c->user.bytes;base=(uintptr_t)c->user.bytes;sid=(uintptr_t)u->User.Sid;
 if(sid<base||sid-base>c->user_bytes||c->user_bytes-(sid-base)<8)return 0;
 offset=(size_t)(sid-base);n=8+4*(size_t)c->user.bytes[offset+1];
 if(c->user.bytes[offset]!=SID_REVISION||c->user.bytes[offset+1]>SID_MAX_SUB_AUTHORITIES||
  n>c->user_bytes-offset||c->sid_offset!=offset||c->sid_bytes!=n)return 0;
 if(c->args[3][0]!=(uintptr_t)c->selected_token||c->args[3][1]!=TokenStatistics||
  c->args[3][2]!=(uintptr_t)&c->statistics||c->args[3][3]!=sizeof(c->statistics)||
  c->args[3][4]!=(uintptr_t)&c->statistics_bytes||c->statistics_bytes!=sizeof(c->statistics)||
  c->args[4][0]!=(uintptr_t)c->process_pseudo||c->args[4][1]!=(uintptr_t)&c->creation||
  c->args[4][2]!=(uintptr_t)&c->exit||c->args[4][3]!=(uintptr_t)&c->kernel||
  c->args[4][4]!=(uintptr_t)&c->user_time||(!c->creation.dwLowDateTime&&!c->creation.dwHighDateTime))return 0;
 prior=c->origin_tick;
 /* Pseudo observation then actual token/query/time call order, not row order. */
 for(i=0;i<6;i++){
  unsigned row=i?i-1:7;
  if(row==1&&c->status[0])continue;
  for(j=0;j<2;j++){
   if(c->clock_returned[row][j]!=1||!c->clock_results[row][j]||c->clocks[row][j].complete!=1||
    c->clocks[row][j].tick<prior||c->clocks[row][j].tick>=c->expires)return 0;
   prior=c->clocks[row][j].tick;
  }
 }
 if(c->previous_tick!=prior)return 0;
 if(c->observed&&(c->observed!=1||c->finish_returned!=1||!c->finish_result||c->finish.complete!=1||
  c->finish.tick<prior||c->finish.tick>=c->expires))return 0;
 return 1;
}
int slcli_bootstrap_qualification_windows_identity_original(
 struct slcli_bq_windows_identity_original *c,struct slcli_arena *a,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes){
 const TOKEN_USER *u;uintptr_t base,sid;size_t offset,n;
 if(!c||!a||!d||!caller||!caller_bytes||!slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !slcli_arena_live_block(a,caller,caller_bytes)||!apart(c,sizeof(*c),caller,caller_bytes)||
  !slcli_source_deadline_original_matches(a,d,caller)||!slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
 c->started=1;c->arena=a;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->origin_original=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;c->previous_tick=c->origin_tick;
 if(!clock_at(c,7,0))goto cleanup;
 c->pseudo_called[0]=1;c->process_pseudo=GetCurrentProcess();c->pseudo_returned[0]=1;
 c->pseudo_called[1]=1;c->thread_pseudo=GetCurrentThread();c->pseudo_returned[1]=1;
 c->pseudo_called[2]=1;c->process_id=GetCurrentProcessId();c->pseudo_returned[2]=1;
 c->pseudo_called[3]=1;c->thread_id=GetCurrentThreadId();c->pseudo_returned[3]=1;
 if(!clock_at(c,7,1)||!c->process_pseudo||!c->thread_pseudo||!c->process_id||!c->thread_id||!same(c)){fail(c,8);goto cleanup;}
 if(!clock_at(c,0,0))goto cleanup;
 c->args[0][0]=(uintptr_t)c->thread_pseudo;c->args[0][1]=TOKEN_QUERY;c->args[0][2]=TRUE;
 c->args[0][3]=(uintptr_t)&c->thread_token;c->called[0]=1;
 c->status[0]=OpenThreadToken(c->thread_pseudo,TOKEN_QUERY,TRUE,&c->thread_token);returned(c,0);
 if(!clock_at(c,0,1))goto cleanup;
 if(c->status[0])c->selected_token=c->thread_token;
 else{
  if(c->thread_token||!c->error_observed[0]||c->errors[0]!=ERROR_NO_TOKEN){fail(c,1);goto cleanup;}
  if(!clock_at(c,1,0))goto cleanup;
  c->args[1][0]=(uintptr_t)c->process_pseudo;c->args[1][1]=TOKEN_QUERY;c->args[1][2]=(uintptr_t)&c->process_token;
  c->called[1]=1;c->status[1]=OpenProcessToken(c->process_pseudo,TOKEN_QUERY,&c->process_token);returned(c,1);
  if(!clock_at(c,1,1)||!c->status[1]){fail(c,2);goto cleanup;}c->selected_token=c->process_token;
 }
 if(!c->selected_token||c->selected_token==INVALID_HANDLE_VALUE){fail(c,3);goto cleanup;}
 if(!clock_at(c,2,0))goto cleanup;
 c->args[2][0]=(uintptr_t)c->selected_token;c->args[2][1]=TokenUser;c->args[2][2]=(uintptr_t)c->user.bytes;
 c->args[2][3]=sizeof(c->user.bytes);c->args[2][4]=(uintptr_t)&c->user_bytes;c->called[2]=1;
 c->status[2]=GetTokenInformation(c->selected_token,TokenUser,c->user.bytes,sizeof(c->user.bytes),&c->user_bytes);returned(c,2);
 if(!clock_at(c,2,1)||!c->status[2]||c->user_bytes<sizeof(TOKEN_USER)||c->user_bytes>sizeof(c->user.bytes)){fail(c,3);goto cleanup;}
 u=(const TOKEN_USER *)c->user.bytes;base=(uintptr_t)c->user.bytes;sid=(uintptr_t)u->User.Sid;
 if(sid<base||sid-base>c->user_bytes||c->user_bytes-(sid-base)<8){fail(c,3);goto cleanup;}
 offset=(size_t)(sid-base);n=8+4*(size_t)c->user.bytes[offset+1];
 if(c->user.bytes[offset]!=SID_REVISION||c->user.bytes[offset+1]>SID_MAX_SUB_AUTHORITIES||n>c->user_bytes-offset){fail(c,3);goto cleanup;}
 c->sid_offset=offset;c->sid_bytes=n;
 if(!clock_at(c,3,0))goto cleanup;
 c->args[3][0]=(uintptr_t)c->selected_token;c->args[3][1]=TokenStatistics;c->args[3][2]=(uintptr_t)&c->statistics;
 c->args[3][3]=sizeof(c->statistics);c->args[3][4]=(uintptr_t)&c->statistics_bytes;c->called[3]=1;
 c->status[3]=GetTokenInformation(c->selected_token,TokenStatistics,&c->statistics,sizeof(c->statistics),&c->statistics_bytes);returned(c,3);
 if(!clock_at(c,3,1)||!c->status[3]||c->statistics_bytes!=sizeof(c->statistics)){fail(c,4);goto cleanup;}
 if(!clock_at(c,4,0))goto cleanup;
 c->args[4][0]=(uintptr_t)c->process_pseudo;c->args[4][1]=(uintptr_t)&c->creation;c->args[4][2]=(uintptr_t)&c->exit;
 c->args[4][3]=(uintptr_t)&c->kernel;c->args[4][4]=(uintptr_t)&c->user_time;c->called[4]=1;
 c->status[4]=GetProcessTimes(c->process_pseudo,&c->creation,&c->exit,&c->kernel,&c->user_time);returned(c,4);
 if(!clock_at(c,4,1)||!c->status[4]||(!c->creation.dwLowDateTime&&!c->creation.dwHighDateTime)){fail(c,5);goto cleanup;}
 if(!slcli_bootstrap_qualification_windows_identity_matches_original(c)){fail(c,9);goto cleanup;}
 c->finish_result=slcli_native_clock_read(&c->finish);c->finish_returned=1;
 if(!c->finish_result||c->finish.tick<c->previous_tick||c->finish.tick>=c->expires||!same(c)){fail(c,9);goto cleanup;}
 /* This held token is only a native identity observation. Independent owner
  * designation/source/caller/admission must still precede any BQ effect. */
 c->observed=1;return 1;
cleanup:
 close_once(c,5,c->thread_token);close_once(c,6,c->process_token);
 return 0;
}
#endif
