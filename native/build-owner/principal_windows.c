#include "principal_windows.h"
#if defined(_WIN32)
int slcli_build_principal_windows_issuing_relation_original(
 const struct slcli_build_principal_windows *effective,const struct slcli_build_principal_windows *primary) {
 return effective&&primary&&effective!=primary&&!effective->process_primary_only&&
 primary->process_primary_only==1&&primary->issuing_effective_original==effective;
}
static int apart(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);
}
static int zero(const void *p,size_t n){const unsigned char *s=p;size_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;}
static int clocks(const struct slcli_build_windows_call *r,uint64_t prior,uint64_t expiry) {
 return r->called==1&&r->returned==1&&slcli_native_clock_windows_matches_original(&r->before)&&
 slcli_native_clock_windows_matches_original(&r->after)&&r->before.tick>=prior&&
 r->before.tick<expiry&&r->after.tick>=r->before.tick&&r->after.tick<expiry;
}
static int shape(const struct slcli_build_principal_windows *c,
 const struct slcli_bq_expected_principal *e,const struct slcli_native_clock *origin,
 const struct slcli_native_clock *prior,uint64_t expires) {
 return c&&e&&e->platform==SLCLI_BQ_EXPECTED_WINDOWS&&e->windows_sid_bytes>=8&&
 e->windows_sid_bytes<=SECURITY_MAX_SID_SIZE&&apart(c,sizeof(*c),e,sizeof(*e))&&
 apart(c,sizeof(*c),e->windows_sid,e->windows_sid_bytes)&&
 apart(c,sizeof(*c),origin,sizeof(*origin))&&apart(c,sizeof(*c),prior,sizeof(*prior))&&
 slcli_native_clock_windows_matches_original(origin)&&slcli_native_clock_windows_matches_original(prior)&&
 prior->tick>=origin->tick&&prior->tick<expires;
}
static int begin(struct slcli_build_principal_windows *c,uint32_t index,uint64_t prior) {
 struct slcli_build_windows_call *r=&c->calls[index];
 return zero(&r->before,sizeof(r->before))&&zero(&r->after,sizeof(r->after))&&!r->called&&!r->returned&&
 slcli_native_clock_read(&r->before)&&slcli_native_clock_windows_matches_original(&r->before)&&
 r->before.tick>=prior&&r->before.tick<c->expires;
}
static int end(struct slcli_build_principal_windows *c,uint32_t index,uint64_t prior) {
 struct slcli_build_windows_call *r=&c->calls[index];
 return slcli_native_clock_read(&r->after)&&clocks(r,prior,c->expires);
}
static int equal_luid(LUID a,LUID b){return a.LowPart==b.LowPart&&a.HighPart==b.HighPart;}
static int sid_extent(const struct slcli_build_principal_windows *c,size_t *bytes) {
 uintptr_t base=(uintptr_t)&c->user_original,p=(uintptr_t)c->user_original.header.User.Sid;
 const unsigned char *sid=(const unsigned char *)p;size_t n;
 if(c->user_bytes_original<sizeof(TOKEN_USER)||c->user_bytes_original>sizeof(c->user_original)||
 p<base+sizeof(TOKEN_USER)||p>base+c->user_bytes_original||base+c->user_bytes_original-p<8)return 0;
 n=8+4*(size_t)sid[1];if(n>SECURITY_MAX_SID_SIZE||n>base+c->user_bytes_original-p)return 0;
 *bytes=n;return 1;
}
static int operands(const struct slcli_build_windows_call *r,uintptr_t a,uintptr_t b,
 uintptr_t d,uintptr_t e,uintptr_t f,uint64_t *prior,uint64_t expiry) {
 size_t i;
 for(i=5;i<12;i++)if(r->args[i])return 0;
 if(r->args[0]!=a||r->args[1]!=b||r->args[2]!=d||r->args[3]!=e||r->args[4]!=f||
 r->error_observed>1||(!r->error_observed&&r->error)||
 !clocks(r,*prior,expiry))return 0;*prior=r->after.tick;return 1;
}
static int contents(const struct slcli_build_principal_windows *c,uint64_t *last) {
 const struct slcli_bq_expected_principal *e;const struct slcli_build_windows_call *r;
 uint64_t prior;size_t n,i;HANDLE token;
 if(!c||!shape(c,c->expected_original,c->origin_original,c->prior_original,c->expires)||
 c->origin_tick!=c->origin_original->tick||c->prior_tick!=c->prior_original->tick||
 c->started!=1||c->failed||c->completed!=1||c->process_token_selected>1||c->process_primary_only>1)return 0;
 if(c->process_primary_only) {
 const struct slcli_build_principal_windows *effective=c->issuing_effective_original;
 if(!effective||effective->process_primary_only||
 !slcli_build_principal_windows_output_away_original(effective,c,sizeof(*c))||
 c->origin_original!=effective->origin_original||c->prior_original!=&effective->finish||
 c->expires!=effective->expires||c->process_original!=effective->process_original||
 c->thread_original!=effective->thread_original||
 c->process_creation.dwLowDateTime!=effective->process_creation.dwLowDateTime||
 c->process_creation.dwHighDateTime!=effective->process_creation.dwHighDateTime||
 c->thread_creation.dwLowDateTime!=effective->thread_creation.dwLowDateTime||
 c->thread_creation.dwHighDateTime!=effective->thread_creation.dwHighDateTime||
 c->statistics_original.TokenType!=TokenPrimary)return 0;
 }else if(c->issuing_effective_original)return 0;
 e=c->expected_original;prior=c->prior_tick;
 r=&c->calls[SLCLI_BUILD_SELF_PROCESS];
 if(!operands(r,0,0,0,0,0,&prior,c->expires)||!r->raw||r->raw!=c->process_original||r->error_observed)return 0;
 r=&c->calls[SLCLI_BUILD_SELF_THREAD];
 if(!operands(r,0,0,0,0,0,&prior,c->expires)||!r->raw||r->raw!=c->thread_original||r->error_observed)return 0;
 r=&c->calls[SLCLI_BUILD_PROCESS_TIMES];
 if(!operands(r,(uintptr_t)(HANDLE)(LONG_PTR)-1,(uintptr_t)&c->process_creation,(uintptr_t)&c->process_exit,
 (uintptr_t)&c->process_kernel,(uintptr_t)&c->process_user,&prior,c->expires)||!r->raw||r->error_observed)return 0;
 r=&c->calls[SLCLI_BUILD_THREAD_TIMES];
 if(!operands(r,(uintptr_t)(HANDLE)(LONG_PTR)-2,(uintptr_t)&c->thread_creation,(uintptr_t)&c->thread_exit,
 (uintptr_t)&c->thread_kernel,(uintptr_t)&c->thread_user,&prior,c->expires)||!r->raw||r->error_observed)return 0;
 r=&c->calls[SLCLI_BUILD_THREAD_TOKEN];
 if(c->process_primary_only) {
 if(!zero(r,sizeof(*r))||c->thread_token_original||c->process_token_selected!=1)return 0;
 }else if(!operands(r,(uintptr_t)(HANDLE)(LONG_PTR)-2,TOKEN_QUERY,TRUE,(uintptr_t)&c->thread_token_original,0,&prior,c->expires))return 0;
 if(c->process_token_selected) {
 if(!c->process_primary_only&&(r->raw||r->error_observed!=1||r->error!=ERROR_NO_TOKEN||c->thread_token_original))return 0;
 r=&c->calls[SLCLI_BUILD_PROCESS_TOKEN];
 if(!operands(r,(uintptr_t)(HANDLE)(LONG_PTR)-1,TOKEN_QUERY,(uintptr_t)&c->process_token_original,0,0,
 &prior,c->expires)||!r->raw||r->error_observed||!c->process_token_original)return 0;token=c->process_token_original;
 } else {
 if(!r->raw||r->error_observed||!c->thread_token_original||c->process_token_original||
 !zero(&c->calls[SLCLI_BUILD_PROCESS_TOKEN],sizeof(*r)))return 0;token=c->thread_token_original;
 }
 if(c->effective_token_original!=token)return 0;
 r=&c->calls[SLCLI_BUILD_TOKEN_USER];
 if(!operands(r,(uintptr_t)token,TokenUser,(uintptr_t)&c->user_original,sizeof(c->user_original),
 (uintptr_t)&c->user_bytes_original,&prior,c->expires)||!r->raw||r->error_observed||!sid_extent(c,&n))return 0;
 r=&c->calls[SLCLI_BUILD_TOKEN_STATISTICS];
 if(!operands(r,(uintptr_t)token,TokenStatistics,(uintptr_t)&c->statistics_original,sizeof(c->statistics_original),
 (uintptr_t)&c->statistics_bytes_original,&prior,c->expires)||!r->raw||r->error_observed||
 c->statistics_bytes_original!=sizeof(c->statistics_original))return 0;
 r=&c->calls[SLCLI_BUILD_SID_VALID];
 if(!operands(r,(uintptr_t)c->user_original.header.User.Sid,0,0,0,0,&prior,c->expires)||!r->raw||r->error_observed)return 0;
 r=&c->calls[SLCLI_BUILD_SID_LENGTH];
 if(!operands(r,(uintptr_t)c->user_original.header.User.Sid,0,0,0,0,&prior,c->expires)||
 r->raw!=n||n!=e->windows_sid_bytes||r->error_observed)return 0;
 for(i=0;i<n;i++)if(((const unsigned char *)c->user_original.header.User.Sid)[i]!=e->windows_sid[i])return 0;
 if(c->statistics_original.TokenId.LowPart!=e->windows_token_id[0]||
 (uint32_t)c->statistics_original.TokenId.HighPart!=e->windows_token_id[1]||
 c->statistics_original.AuthenticationId.LowPart!=e->windows_authentication_id[0]||
 (uint32_t)c->statistics_original.AuthenticationId.HighPart!=e->windows_authentication_id[1])return 0;
 r=&c->calls[SLCLI_BUILD_TOKEN_RECHECK];
 if(!operands(r,(uintptr_t)token,TokenStatistics,(uintptr_t)&c->recheck_original,sizeof(c->recheck_original),
 (uintptr_t)&c->recheck_bytes_original,&prior,c->expires)||!r->raw||r->error_observed||
 c->recheck_bytes_original!=sizeof(c->recheck_original)||
 !equal_luid(c->statistics_original.TokenId,c->recheck_original.TokenId)||
 !equal_luid(c->statistics_original.AuthenticationId,c->recheck_original.AuthenticationId)||
 !equal_luid(c->statistics_original.ModifiedId,c->recheck_original.ModifiedId)||
 c->statistics_original.TokenType!=c->recheck_original.TokenType||
 (c->statistics_original.TokenType==TokenImpersonation&&
 c->statistics_original.ImpersonationLevel!=c->recheck_original.ImpersonationLevel)||
 c->statistics_original.GroupCount!=c->recheck_original.GroupCount||
 c->statistics_original.PrivilegeCount!=c->recheck_original.PrivilegeCount)return 0;
 *last=prior;return 1;
}
int slcli_build_principal_windows_matches_original(const struct slcli_build_principal_windows *c) {
 uint64_t prior;return contents(c,&prior)&&slcli_native_clock_windows_matches_original(&c->finish)&&
 c->finish.tick>=prior&&c->finish.tick<c->expires;
}
int slcli_build_principal_windows_output_away_original(
 const struct slcli_build_principal_windows *c,const void *out,size_t bytes) {
 return slcli_build_principal_windows_matches_original(c)&&apart(out,bytes,c,sizeof(*c))&&
 apart(out,bytes,c->expected_original,sizeof(*c->expected_original))&&
 apart(out,bytes,c->expected_original->windows_sid,c->expected_original->windows_sid_bytes)&&
 apart(out,bytes,c->origin_original,sizeof(*c->origin_original))&&
 apart(out,bytes,c->prior_original,sizeof(*c->prior_original))&&
 (!c->process_primary_only||slcli_build_principal_windows_output_away_original(c->issuing_effective_original,out,bytes));
}
static int capture_principal(struct slcli_build_principal_windows *c,
 const struct slcli_bq_expected_principal *e,const struct slcli_native_clock *origin,
 const struct slcli_native_clock *previous,uint64_t expires,
 const struct slcli_build_principal_windows *effective) {
 struct slcli_build_windows_call *r;uint64_t prior;size_t n;
 if(!shape(c,e,origin,previous,expires)||!zero(c,sizeof(*c)))return 0;
 c->expected_original=e;c->origin_original=origin;c->prior_original=previous;
 c->issuing_effective_original=effective;c->process_primary_only=effective!=NULL;
 c->origin_tick=origin->tick;c->prior_tick=previous->tick;c->expires=expires;c->started=1;c->failed=1;prior=previous->tick;
#define START(i) r=&c->calls[(i)];if(!begin(c,(i),prior))return 0
#define DONE(i) if(!end(c,(i),prior))return 0;prior=r->after.tick
#define BOOL_RESULT(expression) r->called=1;r->raw=(uintptr_t)(expression);r->returned=1;\
 if(!r->raw){r->error=GetLastError();r->error_observed=1;}
 START(SLCLI_BUILD_SELF_PROCESS);r->called=1;c->process_original=GetCurrentProcessId();r->raw=c->process_original;r->returned=1;
 DONE(SLCLI_BUILD_SELF_PROCESS);if(!r->raw)return 0;
 START(SLCLI_BUILD_SELF_THREAD);r->called=1;c->thread_original=GetCurrentThreadId();r->raw=c->thread_original;r->returned=1;
 DONE(SLCLI_BUILD_SELF_THREAD);if(!r->raw)return 0;
 START(SLCLI_BUILD_PROCESS_TIMES);r->args[0]=(uintptr_t)(HANDLE)(LONG_PTR)-1;
 r->args[1]=(uintptr_t)&c->process_creation;r->args[2]=(uintptr_t)&c->process_exit;
 r->args[3]=(uintptr_t)&c->process_kernel;r->args[4]=(uintptr_t)&c->process_user;
 BOOL_RESULT(GetProcessTimes((HANDLE)(LONG_PTR)-1,&c->process_creation,&c->process_exit,&c->process_kernel,&c->process_user));
 DONE(SLCLI_BUILD_PROCESS_TIMES);if(!r->raw)return 0;
 START(SLCLI_BUILD_THREAD_TIMES);r->args[0]=(uintptr_t)(HANDLE)(LONG_PTR)-2;
 r->args[1]=(uintptr_t)&c->thread_creation;r->args[2]=(uintptr_t)&c->thread_exit;
 r->args[3]=(uintptr_t)&c->thread_kernel;r->args[4]=(uintptr_t)&c->thread_user;
 BOOL_RESULT(GetThreadTimes((HANDLE)(LONG_PTR)-2,&c->thread_creation,&c->thread_exit,&c->thread_kernel,&c->thread_user));
 DONE(SLCLI_BUILD_THREAD_TIMES);if(!r->raw)return 0;
 if(!effective) {
 START(SLCLI_BUILD_THREAD_TOKEN);r->args[0]=(uintptr_t)(HANDLE)(LONG_PTR)-2;r->args[1]=TOKEN_QUERY;r->args[2]=TRUE;
 r->args[3]=(uintptr_t)&c->thread_token_original;
 BOOL_RESULT(OpenThreadToken((HANDLE)(LONG_PTR)-2,TOKEN_QUERY,TRUE,&c->thread_token_original));
 DONE(SLCLI_BUILD_THREAD_TOKEN);
 }
 if(effective||!r->raw) {
 if(!effective&&(r->error!=ERROR_NO_TOKEN||c->thread_token_original))return 0;c->process_token_selected=1;
 START(SLCLI_BUILD_PROCESS_TOKEN);r->args[0]=(uintptr_t)(HANDLE)(LONG_PTR)-1;r->args[1]=TOKEN_QUERY;
 r->args[2]=(uintptr_t)&c->process_token_original;
 BOOL_RESULT(OpenProcessToken((HANDLE)(LONG_PTR)-1,TOKEN_QUERY,&c->process_token_original));
 DONE(SLCLI_BUILD_PROCESS_TOKEN);if(!r->raw||!c->process_token_original)return 0;
 c->effective_token_original=c->process_token_original;
 }else{if(!c->thread_token_original)return 0;c->effective_token_original=c->thread_token_original;}
 START(SLCLI_BUILD_TOKEN_USER);r->args[0]=(uintptr_t)c->effective_token_original;r->args[1]=TokenUser;
 r->args[2]=(uintptr_t)&c->user_original;r->args[3]=sizeof(c->user_original);r->args[4]=(uintptr_t)&c->user_bytes_original;
 BOOL_RESULT(GetTokenInformation(c->effective_token_original,TokenUser,&c->user_original,sizeof(c->user_original),&c->user_bytes_original));
 DONE(SLCLI_BUILD_TOKEN_USER);if(!r->raw||!sid_extent(c,&n))return 0;
 START(SLCLI_BUILD_TOKEN_STATISTICS);r->args[0]=(uintptr_t)c->effective_token_original;r->args[1]=TokenStatistics;
 r->args[2]=(uintptr_t)&c->statistics_original;r->args[3]=sizeof(c->statistics_original);r->args[4]=(uintptr_t)&c->statistics_bytes_original;
 BOOL_RESULT(GetTokenInformation(c->effective_token_original,TokenStatistics,&c->statistics_original,sizeof(c->statistics_original),&c->statistics_bytes_original));
 DONE(SLCLI_BUILD_TOKEN_STATISTICS);if(!r->raw)return 0;
 START(SLCLI_BUILD_SID_VALID);r->args[0]=(uintptr_t)c->user_original.header.User.Sid;
 r->called=1;r->raw=(uintptr_t)IsValidSid(c->user_original.header.User.Sid);r->returned=1;
 DONE(SLCLI_BUILD_SID_VALID);if(!r->raw)return 0;
 START(SLCLI_BUILD_SID_LENGTH);r->args[0]=(uintptr_t)c->user_original.header.User.Sid;
 r->called=1;r->raw=GetLengthSid(c->user_original.header.User.Sid);r->returned=1;
 DONE(SLCLI_BUILD_SID_LENGTH);if(r->raw!=n)return 0;
 START(SLCLI_BUILD_TOKEN_RECHECK);r->args[0]=(uintptr_t)c->effective_token_original;r->args[1]=TokenStatistics;
 r->args[2]=(uintptr_t)&c->recheck_original;r->args[3]=sizeof(c->recheck_original);r->args[4]=(uintptr_t)&c->recheck_bytes_original;
 BOOL_RESULT(GetTokenInformation(c->effective_token_original,TokenStatistics,&c->recheck_original,sizeof(c->recheck_original),&c->recheck_bytes_original));
 DONE(SLCLI_BUILD_TOKEN_RECHECK);if(!r->raw)return 0;
#undef BOOL_RESULT
#undef DONE
#undef START
 c->completed=1;c->failed=0;if(!contents(c,&prior)){c->failed=1;return 0;}c->failed=1;
 if(!slcli_native_clock_read(&c->finish)||!slcli_native_clock_windows_matches_original(&c->finish)||
 c->finish.tick<prior||c->finish.tick>=expires)return 0;c->failed=0;return 1;
}
int slcli_build_principal_windows_capture_original(struct slcli_build_principal_windows *c,
 const struct slcli_bq_expected_principal *e,const struct slcli_native_clock *origin,
 const struct slcli_native_clock *previous,uint64_t expires) {
 return capture_principal(c,e,origin,previous,expires,NULL);
}
int slcli_build_process_primary_windows_capture_original(struct slcli_build_principal_windows *c,
 const struct slcli_build_principal_windows *effective,const struct slcli_bq_expected_principal *expected) {
 if(!effective||effective->process_primary_only||
 !slcli_build_principal_windows_output_away_original(effective,c,sizeof(*c)))return 0;
 return capture_principal(c,expected,effective->origin_original,&effective->finish,effective->expires,effective);
}
#endif
