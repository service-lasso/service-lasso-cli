#include "child_principal_windows.h"
#if defined(_WIN32)
static int zero(const void *p,size_t n){const unsigned char *s=p;size_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;}
static int clocks(const struct slcli_build_windows_call *r,uint64_t *prior,uint64_t expires) {
 if(r->called!=1||r->returned!=1||!slcli_native_clock_windows_matches_original(&r->before)||
 !slcli_native_clock_windows_matches_original(&r->after)||r->before.tick<*prior||
 r->before.tick>=expires||r->after.tick<r->before.tick||r->after.tick>=expires)return 0;
 *prior=r->after.tick;return 1;
}
static int begin(struct slcli_build_windows_call *r,uint64_t prior,uint64_t expires) {
 return !r->called&&!r->returned&&slcli_native_clock_read(&r->before)&&
 slcli_native_clock_windows_matches_original(&r->before)&&r->before.tick>=prior&&r->before.tick<expires;
}
static int end(struct slcli_build_windows_call *r,uint64_t *prior,uint64_t expires) {
 return slcli_native_clock_read(&r->after)&&clocks(r,prior,expires);
}
static int row(const struct slcli_build_windows_call *r,const uintptr_t *args,size_t n,
 uint64_t *prior,uint64_t expires) {
 size_t i;if(n>12||!r->raw||r->error_observed||r->error||!clocks(r,prior,expires))return 0;
 for(i=0;i<12;i++)if(r->args[i]!=(i<n?args[i]:0))return 0;return 1;
}
static int equal_luid(LUID a,LUID b){return a.LowPart==b.LowPart&&a.HighPart==b.HighPart;}
static int sid_extent(const struct slcli_build_child_principal_windows *c,size_t *bytes) {
 uintptr_t base=(uintptr_t)&c->user_original,p=(uintptr_t)c->user_original.header.User.Sid;
 const unsigned char *s=(const unsigned char *)p;size_t n;
 if(c->user_bytes_original<sizeof(TOKEN_USER)||c->user_bytes_original>sizeof(c->user_original)||
 p<base+sizeof(TOKEN_USER)||p>base+c->user_bytes_original||base+c->user_bytes_original-p<8)return 0;
 n=8+4*(size_t)s[1];if(n>SECURITY_MAX_SID_SIZE||n>base+c->user_bytes_original-p)return 0;
 *bytes=n;return 1;
}
static HANDLE process(const struct slcli_build_windows_command *b,size_t i) {
 return i?b->first_event_original.u.CreateProcessInfo.hProcess:b->process_original.hProcess;
}
static HANDLE thread(const struct slcli_build_windows_command *b,size_t i) {
 return i?b->first_event_original.u.CreateProcessInfo.hThread:b->process_original.hThread;
}
static int contents(const struct slcli_build_child_principal_windows *c,uint64_t *last) {
 const struct slcli_build_windows_command *b;const struct slcli_build_principal_windows *primary;
 const struct slcli_bq_expected_principal *expected;
 uintptr_t a[5];uint64_t prior;size_t i,n;
 if(!c||!slcli_build_windows_command_output_away_original(c->birth_original,c,sizeof(*c))||
 c->started!=1||c->failed||c->completed!=1)return 0;b=c->birth_original;
 if(c->prior_tick!=b->finish.tick||c->expires!=b->expires)return 0;prior=c->prior_tick;
 primary=b->source_original->process_primary_original;expected=primary->expected_original;
 for(i=0;i<2;i++) {
 a[0]=(uintptr_t)process(b,i);
 if(!row(&c->calls[SLCLI_BUILD_CHILD_PROCESS_ID+i],a,1,&prior,c->expires)||
 c->calls[SLCLI_BUILD_CHILD_PROCESS_ID+i].raw!=b->process_original.dwProcessId)return 0;
 }
 for(i=0;i<2;i++) {
 a[0]=(uintptr_t)thread(b,i);
 if(!row(&c->calls[SLCLI_BUILD_CHILD_THREAD_ID+i],a,1,&prior,c->expires)||
 c->calls[SLCLI_BUILD_CHILD_THREAD_ID+i].raw!=b->process_original.dwThreadId)return 0;
 }
 for(i=0;i<2;i++) {
 a[0]=(uintptr_t)thread(b,i);
 if(!row(&c->calls[SLCLI_BUILD_CHILD_THREAD_PROCESS_ID+i],a,1,&prior,c->expires)||
 c->calls[SLCLI_BUILD_CHILD_THREAD_PROCESS_ID+i].raw!=b->process_original.dwProcessId)return 0;
 }
 for(i=0;i<2;i++) {
 a[0]=(uintptr_t)process(b,i);a[1]=(uintptr_t)&c->creation[i];a[2]=(uintptr_t)&c->exit[i];
 a[3]=(uintptr_t)&c->kernel[i];a[4]=(uintptr_t)&c->user_time[i];
 if(!row(&c->calls[SLCLI_BUILD_CHILD_PROCESS_TIMES+i],a,5,&prior,c->expires))return 0;
 }
 if(c->creation[0].dwLowDateTime!=c->creation[1].dwLowDateTime||
 c->creation[0].dwHighDateTime!=c->creation[1].dwHighDateTime)return 0;
 a[0]=(uintptr_t)b->process_original.hProcess;a[1]=TOKEN_QUERY;a[2]=(uintptr_t)&c->token_original;
 if(!row(&c->calls[SLCLI_BUILD_CHILD_TOKEN],a,3,&prior,c->expires)||
 !c->token_original||c->token_original==INVALID_HANDLE_VALUE)return 0;
 a[0]=(uintptr_t)c->token_original;a[1]=TokenUser;a[2]=(uintptr_t)&c->user_original;
 a[3]=sizeof(c->user_original);a[4]=(uintptr_t)&c->user_bytes_original;
 if(!row(&c->calls[SLCLI_BUILD_CHILD_USER],a,5,&prior,c->expires)||!sid_extent(c,&n))return 0;
 a[0]=(uintptr_t)c->token_original;a[1]=TokenStatistics;a[2]=(uintptr_t)&c->statistics_original;
 a[3]=sizeof(c->statistics_original);a[4]=(uintptr_t)&c->statistics_bytes_original;
 if(!row(&c->calls[SLCLI_BUILD_CHILD_STATISTICS],a,5,&prior,c->expires)||
 c->statistics_bytes_original!=sizeof(c->statistics_original))return 0;
 a[0]=(uintptr_t)c->user_original.header.User.Sid;
 if(!row(&c->calls[SLCLI_BUILD_CHILD_SID_VALID],a,1,&prior,c->expires)||
 !row(&c->calls[SLCLI_BUILD_CHILD_SID_LENGTH],a,1,&prior,c->expires)||
 c->calls[SLCLI_BUILD_CHILD_SID_LENGTH].raw!=n||n!=expected->windows_sid_bytes)return 0;
 for(i=0;i<n;i++)if(((const unsigned char *)c->user_original.header.User.Sid)[i]!=expected->windows_sid[i])return 0;
 if(c->statistics_original.TokenType!=TokenPrimary||
 !equal_luid(c->statistics_original.AuthenticationId,primary->statistics_original.AuthenticationId))return 0;
 a[0]=(uintptr_t)c->token_original;a[1]=TokenStatistics;a[2]=(uintptr_t)&c->recheck_original;
 a[3]=sizeof(c->recheck_original);a[4]=(uintptr_t)&c->recheck_bytes_original;
 if(!row(&c->calls[SLCLI_BUILD_CHILD_TOKEN_RECHECK],a,5,&prior,c->expires)||
 c->recheck_bytes_original!=sizeof(c->recheck_original)||
 !equal_luid(c->statistics_original.TokenId,c->recheck_original.TokenId)||
 !equal_luid(c->statistics_original.AuthenticationId,c->recheck_original.AuthenticationId)||
 !equal_luid(c->statistics_original.ModifiedId,c->recheck_original.ModifiedId)||
 c->recheck_original.TokenType!=TokenPrimary||
 c->statistics_original.GroupCount!=c->recheck_original.GroupCount||
 c->statistics_original.PrivilegeCount!=c->recheck_original.PrivilegeCount)return 0;
 *last=prior;return 1;
}
int slcli_build_child_principal_windows_matches_original(const struct slcli_build_child_principal_windows *c) {
 uint64_t prior;return contents(c,&prior)&&slcli_native_clock_windows_matches_original(&c->finish)&&
 c->finish.tick>=prior&&c->finish.tick<c->expires;
}
int slcli_build_child_principal_windows_capture_original(struct slcli_build_child_principal_windows *c,
 const struct slcli_build_windows_command *b) {
 struct slcli_build_windows_call *r;uint64_t prior;size_t i,n;
 if(!slcli_build_windows_command_output_away_original(b,c,sizeof(*c))||!zero(c,sizeof(*c)))return 0;
 c->birth_original=b;c->prior_tick=b->finish.tick;c->expires=b->expires;c->started=1;c->failed=1;prior=c->prior_tick;
#define START(index) r=&c->calls[(index)];if(!begin(r,prior,c->expires))return 0
#define CALL(expression) do {r->called=1;r->raw=(uintptr_t)(expression);r->returned=1; \
 if(!r->raw){r->error=GetLastError();r->error_observed=1;}}while(0)
#define DONE() if(!end(r,&prior,c->expires)||!r->raw)return 0
 for(i=0;i<2;i++) {
 START(SLCLI_BUILD_CHILD_PROCESS_ID+i);r->args[0]=(uintptr_t)process(b,i);CALL(GetProcessId(process(b,i)));DONE();
 if(r->raw!=b->process_original.dwProcessId)return 0;
 }
 for(i=0;i<2;i++) {
 START(SLCLI_BUILD_CHILD_THREAD_ID+i);r->args[0]=(uintptr_t)thread(b,i);CALL(GetThreadId(thread(b,i)));DONE();
 if(r->raw!=b->process_original.dwThreadId)return 0;
 }
 for(i=0;i<2;i++) {
 START(SLCLI_BUILD_CHILD_THREAD_PROCESS_ID+i);r->args[0]=(uintptr_t)thread(b,i);
 CALL(GetProcessIdOfThread(thread(b,i)));DONE();if(r->raw!=b->process_original.dwProcessId)return 0;
 }
 for(i=0;i<2;i++) {
 START(SLCLI_BUILD_CHILD_PROCESS_TIMES+i);r->args[0]=(uintptr_t)process(b,i);
 r->args[1]=(uintptr_t)&c->creation[i];r->args[2]=(uintptr_t)&c->exit[i];
 r->args[3]=(uintptr_t)&c->kernel[i];r->args[4]=(uintptr_t)&c->user_time[i];
 CALL(GetProcessTimes(process(b,i),&c->creation[i],&c->exit[i],&c->kernel[i],&c->user_time[i]));DONE();
 }
 START(SLCLI_BUILD_CHILD_TOKEN);r->args[0]=(uintptr_t)b->process_original.hProcess;r->args[1]=TOKEN_QUERY;
 r->args[2]=(uintptr_t)&c->token_original;
 CALL(OpenProcessToken(b->process_original.hProcess,TOKEN_QUERY,&c->token_original));DONE();
 if(!c->token_original||c->token_original==INVALID_HANDLE_VALUE)return 0;
 START(SLCLI_BUILD_CHILD_USER);r->args[0]=(uintptr_t)c->token_original;r->args[1]=TokenUser;
 r->args[2]=(uintptr_t)&c->user_original;r->args[3]=sizeof(c->user_original);r->args[4]=(uintptr_t)&c->user_bytes_original;
 CALL(GetTokenInformation(c->token_original,TokenUser,&c->user_original,sizeof(c->user_original),&c->user_bytes_original));DONE();
 if(!sid_extent(c,&n))return 0;
 START(SLCLI_BUILD_CHILD_STATISTICS);r->args[0]=(uintptr_t)c->token_original;r->args[1]=TokenStatistics;
 r->args[2]=(uintptr_t)&c->statistics_original;r->args[3]=sizeof(c->statistics_original);
 r->args[4]=(uintptr_t)&c->statistics_bytes_original;
 CALL(GetTokenInformation(c->token_original,TokenStatistics,&c->statistics_original,
 sizeof(c->statistics_original),&c->statistics_bytes_original));DONE();
 START(SLCLI_BUILD_CHILD_SID_VALID);r->args[0]=(uintptr_t)c->user_original.header.User.Sid;
 r->called=1;r->raw=(uintptr_t)IsValidSid(c->user_original.header.User.Sid);r->returned=1;DONE();
 START(SLCLI_BUILD_CHILD_SID_LENGTH);r->args[0]=(uintptr_t)c->user_original.header.User.Sid;
 r->called=1;r->raw=GetLengthSid(c->user_original.header.User.Sid);r->returned=1;DONE();if(r->raw!=n)return 0;
 START(SLCLI_BUILD_CHILD_TOKEN_RECHECK);r->args[0]=(uintptr_t)c->token_original;r->args[1]=TokenStatistics;
 r->args[2]=(uintptr_t)&c->recheck_original;r->args[3]=sizeof(c->recheck_original);
 r->args[4]=(uintptr_t)&c->recheck_bytes_original;
 CALL(GetTokenInformation(c->token_original,TokenStatistics,&c->recheck_original,
 sizeof(c->recheck_original),&c->recheck_bytes_original));DONE();
#undef DONE
#undef CALL
#undef START
 c->completed=1;c->failed=0;if(!contents(c,&prior)){c->failed=1;return 0;}c->failed=1;
 if(!slcli_native_clock_read(&c->finish)||!slcli_native_clock_windows_matches_original(&c->finish)||
 c->finish.tick<prior||c->finish.tick>=c->expires)return 0;c->failed=0;return 1;
}
#endif
