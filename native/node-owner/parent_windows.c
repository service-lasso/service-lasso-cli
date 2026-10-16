#include "parent_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int before(struct slcli_node_parent_windows *c,size_t i) {
  struct slcli_node_parent_windows_call *r=&c->calls[i];
  uint64_t prior=i?c->calls[i-1].after.tick:c->prior_tick;
  r->before_result=slcli_native_clock_read(&r->before)&&
    r->before.tick>=prior&&r->before.tick<c->expires;
  return r->before_result;
}
static int after(struct slcli_node_parent_windows *c,size_t i) {
  struct slcli_node_parent_windows_call *r=&c->calls[i];
  r->after_result=slcli_native_clock_read(&r->after)&&
    r->after.tick>=r->before.tick&&r->after.tick<c->expires;
  return r->after_result;
}
int slcli_node_parent_windows_dispose_token(struct slcli_node_parent_windows *c) {
  struct slcli_node_parent_windows_call *r;BOOL result;size_t i;
  if(!c||!c->started)return 0;
  if(c->token_disposal_attempted)return c->token_closed;
  c->token_disposal_attempted=1;
  for(i=3;i<6;i++)if(c->calls[i].called&&!c->calls[i].returned)goto pending;
  if(!c->calls[3].returned||!c->calls[3].result)return 1;
  if(!c->token||c->token==INVALID_HANDLE_VALUE||c->token!=c->token_raw)goto pending;
  r=&c->calls[6];
  /* Actual known-returned borrowers permit cleanup independently of useful D. */
  r->before_result=slcli_native_clock_read(&r->before);
  r->args[0]=(uintptr_t)c->token;r->called=1;
  result=CloseHandle(c->token);r->result=(uint32_t)result;r->returned=1;
  if(result)c->token_closed=1;else{r->error=GetLastError();r->error_observed=1;}
  r->after_result=slcli_native_clock_read(&r->after);
  if(result)return 1;
pending:c->token_disposal_pending=1;return 0;
}
int slcli_node_parent_windows_observe(struct slcli_node_parent_windows *c,
  const struct slcli_node_bootstrap_decode *d,HANDLE parent,const struct slcli_native_clock *prior) {
  const struct slcli_node_bootstrap_transport *t;
  struct slcli_node_parent_windows_call *r;BOOL ok;DWORD wait;
  uintptr_t begin,end,sid;size_t i,n;
  if(!c||!d||!prior||!d->returned||!d->qualified||!prior->complete||
     !apart(c,sizeof(*c),d,sizeof(*d))||!apart(c,sizeof(*c),prior,sizeof(*prior))||
     !apart(c,sizeof(*c),d->raw_original,d->raw_bytes_original))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  t=&d->transport;
  if(!parent||parent==INVALID_HANDLE_VALUE||t->capabilities[3]!=(uintptr_t)parent||
     !t->parent_pid||t->parent_sid_bytes<8||t->parent_sid_bytes>SECURITY_MAX_SID_SIZE||
     prior->tick<t->origin.tick||prior->tick>=t->expires)return 0;
  c->started=1;c->decode_original=d;c->prior_original=prior;c->parent_original=parent;
  c->expires=t->expires;c->origin_tick=t->origin.tick;c->prior_tick=prior->tick;
  c->expected_pid=t->parent_pid;c->expected_sid_bytes=t->parent_sid_bytes;
  for(i=0;i<2;i++) {
    c->expected_creation[i]=t->parent_creation[i];
    c->expected_token_id[i]=t->parent_token_id[i];
    c->expected_authentication_id[i]=t->parent_authentication_id[i];
  }
  for(i=0;i<SECURITY_MAX_SID_SIZE;i++)c->expected_sid[i]=t->parent_sid[i];
  if(!before(c,0))goto failed;
  r=&c->calls[0];r->args[0]=(uintptr_t)parent;r->called=1;
  c->pid=GetProcessId(parent);r->result=c->pid;r->returned=1;
  if(!c->pid){r->error=GetLastError();r->error_observed=1;}
  if(!after(c,0)||!c->pid||c->pid!=c->expected_pid)goto failed;
  if(!before(c,1))goto failed;
  r=&c->calls[1];r->args[0]=(uintptr_t)parent;r->args[1]=(uintptr_t)&c->creation;
  r->args[2]=(uintptr_t)&c->exit;r->args[3]=(uintptr_t)&c->kernel;r->args[4]=(uintptr_t)&c->user;
  r->called=1;ok=GetProcessTimes(parent,&c->creation,&c->exit,&c->kernel,&c->user);
  r->result=(uint32_t)ok;r->returned=1;
  if(!ok){r->error=GetLastError();r->error_observed=1;}
  if(!after(c,1)||!ok||c->creation.dwLowDateTime!=c->expected_creation[0]||
     c->creation.dwHighDateTime!=c->expected_creation[1])goto failed;
  if(!before(c,2))goto failed;
  r=&c->calls[2];r->args[0]=(uintptr_t)parent;r->args[1]=0;r->called=1;
  wait=WaitForSingleObject(parent,0);r->result=wait;r->returned=1;
  if(wait==WAIT_FAILED){r->error=GetLastError();r->error_observed=1;}
  if(!after(c,2)||wait!=WAIT_TIMEOUT)goto failed;
  if(!before(c,3))goto failed;
  r=&c->calls[3];r->args[0]=(uintptr_t)parent;r->args[1]=TOKEN_QUERY;
  r->args[2]=(uintptr_t)&c->token_raw;r->called=1;
  ok=OpenProcessToken(parent,TOKEN_QUERY,&c->token_raw);r->result=(uint32_t)ok;r->returned=1;
  if(ok)c->token=c->token_raw;else{r->error=GetLastError();r->error_observed=1;}
  if(!after(c,3)||!ok||!c->token||c->token==INVALID_HANDLE_VALUE)goto failed;
  if(!before(c,4))goto failed;
  r=&c->calls[4];r->args[0]=(uintptr_t)c->token;r->args[1]=TokenUser;
  r->args[2]=(uintptr_t)c->user_buffer.bytes;r->args[3]=sizeof(c->user_buffer.bytes);
  r->args[4]=(uintptr_t)&c->user_bytes;r->called=1;
  ok=GetTokenInformation(c->token,TokenUser,c->user_buffer.bytes,sizeof(c->user_buffer.bytes),&c->user_bytes);
  r->result=(uint32_t)ok;r->returned=1;
  if(!ok){r->error=GetLastError();r->error_observed=1;}
  if(!after(c,4)||!ok||c->user_bytes<sizeof(TOKEN_USER)||c->user_bytes>sizeof(c->user_buffer.bytes))goto failed;
  begin=(uintptr_t)c->user_buffer.bytes;if(begin>UINTPTR_MAX-c->user_bytes)goto failed;
  end=begin+c->user_bytes;sid=(uintptr_t)((const TOKEN_USER *)(const void *)c->user_buffer.bytes)->User.Sid;
  if(sid<begin+sizeof(TOKEN_USER)||sid>end||end-sid<8||
     ((const unsigned char *)sid)[0]!=SID_REVISION||((const unsigned char *)sid)[1]>SID_MAX_SUB_AUTHORITIES)goto failed;
  n=8u+4u*((const unsigned char *)sid)[1];
  if(n>end-sid||n!=c->expected_sid_bytes)goto failed;
  for(i=0;i<n;i++){c->sid_compared=i+1;if(((const unsigned char *)sid)[i]!=c->expected_sid[i])goto failed;}
  if(!before(c,5))goto failed;
  r=&c->calls[5];r->args[0]=(uintptr_t)c->token;r->args[1]=TokenStatistics;
  r->args[2]=(uintptr_t)&c->statistics;r->args[3]=sizeof(c->statistics);
  r->args[4]=(uintptr_t)&c->statistics_bytes;r->called=1;
  ok=GetTokenInformation(c->token,TokenStatistics,&c->statistics,sizeof(c->statistics),&c->statistics_bytes);
  r->result=(uint32_t)ok;r->returned=1;
  if(!ok){r->error=GetLastError();r->error_observed=1;}
  if(!after(c,5)||!ok||c->statistics_bytes!=sizeof(c->statistics)||
     c->statistics.TokenId.LowPart!=c->expected_token_id[0]||
     (uint32_t)c->statistics.TokenId.HighPart!=c->expected_token_id[1]||
     c->statistics.AuthenticationId.LowPart!=c->expected_authentication_id[0]||
     (uint32_t)c->statistics.AuthenticationId.HighPart!=c->expected_authentication_id[1])goto failed;
  if(!slcli_node_parent_windows_dispose_token(c)||!c->calls[6].before_result||
     !c->calls[6].after_result||c->calls[6].before.tick<c->calls[5].after.tick||
     c->calls[6].after.tick<c->calls[6].before.tick||c->calls[6].after.tick>=c->expires)goto failed;
  c->observations_match=1;c->terminal=1;c->returned=1;return 1;
failed:
  (void)slcli_node_parent_windows_dispose_token(c);
  c->terminal=1;c->returned=1;return 0;
}
#endif
