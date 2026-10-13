#if defined(__linux__)
#include "launcher_linux.h"
#include "../host-service/native_syscall_linux.h"
static int away(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&
 (x+n<=y||y+m<=x);
}
static int zeroed(const void *p,size_t n) {
 const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;
}
static int plan_matches(const struct slcli_initial_launcher_linux *c) {
 const struct slcli_bq_resource_cost *a=&c->plan.resource_profile.simultaneous;
 const struct slcli_bq_resource_cost *b=&c->preclaim.resource_profile.simultaneous;
 return c->plan.metadata_bytes==c->preclaim.metadata_bytes &&
 c->plan.user_bytes==c->preclaim.user_bytes && c->plan.mapping_bytes==c->preclaim.mapping_bytes &&
 c->plan.resource_profile.combined_memory_bytes==c->preclaim.resource_profile.combined_memory_bytes &&
 a->user_ram_bytes==b->user_ram_bytes && a->user_virtual_bytes==b->user_virtual_bytes &&
 a->kernel_ram_bytes==b->kernel_ram_bytes && a->cache_ram_bytes==b->cache_ram_bytes &&
 a->retained_storage_bytes==b->retained_storage_bytes && a->native_references==b->native_references &&
 a->physical_extent_events==b->physical_extent_events && a->native_attempts==b->native_attempts &&
 a->pure_attempts==b->pure_attempts && a->cpu_nanoseconds==b->cpu_nanoseconds;
}
static long operation(size_t i) {
 switch(i) {
 case 0:return __NR_getpid;case 1:return __NR_getppid;case 2:return __NR_gettid;
 case 3:return __NR_getuid;case 4:return __NR_geteuid;case 5:return __NR_getgid;
 case 6:return __NR_getegid;case 7:return __NR_pidfd_open;
 default:return -1;
 }
}
static int identity_matches(const struct slcli_initial_launcher_identity_linux *c) {
 size_t i,j;uint64_t previous;
 if(!c||!away(c,sizeof(*c),c->origin_original,sizeof(*c->origin_original)) ||
 !away(c,sizeof(*c),c->prior_original,sizeof(*c->prior_original)) ||
 !slcli_native_clock_linux_matches_original(c->origin_original) ||
 !slcli_native_clock_linux_matches_original(c->prior_original) ||
 c->origin_tick!=c->origin_original->tick || c->prior_tick!=c->prior_original->tick ||
 c->origin_tick>c->prior_tick || c->prior_tick>=c->expires ||
 c->started!=1 || c->completed!=SLCLI_INITIAL_IDENTITY_CALLS || c->failed)return 0;
 previous=c->prior_tick;
 for(i=0;i<SLCLI_INITIAL_IDENTITY_CALLS;i++) {
  if(c->number[i]!=operation(i) || c->called[i]!=1||c->returned[i]!=1 ||
   !slcli_native_clock_linux_matches_original(&c->before[i]) ||
   !slcli_native_clock_linux_matches_original(&c->after[i]) ||
   c->before[i].tick<previous || c->after[i].tick<c->before[i].tick ||
   c->after[i].tick>=c->expires)return 0;
  for(j=0;j<6;j++)if(c->args[i][j]!=(i==7&&j==0?c->raw[0]:0))return 0;
  if(i<3 && (c->raw[i]<(i==1?0:1) || c->raw[i]>INT32_MAX))return 0;
  if(i>=3&&i<7 && (c->raw[i]<0 || (uint64_t)c->raw[i]>UINT32_MAX))return 0;
  if(i==7 && (c->raw[i]<0 || c->raw[i]>INT32_MAX))return 0;
  previous=c->after[i].tick;
 }
 return slcli_native_clock_linux_matches_original(&c->finish) &&
 c->finish.tick>=previous && c->finish.tick<c->expires;
}
static int identity_capture(struct slcli_initial_launcher_identity_linux *c,
 const struct slcli_native_clock *origin,const struct slcli_native_clock *prior,
 uint64_t expires) {
 size_t i;uint64_t previous=prior->tick;long result;
 c->origin_original=origin;c->prior_original=prior;c->origin_tick=origin->tick;
 c->prior_tick=prior->tick;c->expires=expires;c->started=1;c->failed=1;
 for(i=0;i<SLCLI_INITIAL_IDENTITY_CALLS;i++) {
  if(!slcli_native_clock_read(&c->before[i]) ||
   !slcli_native_clock_linux_matches_original(&c->before[i]) ||
   c->before[i].tick<previous || c->before[i].tick>=expires)return 0;
  if(i==7)c->args[i][0]=c->raw[0];
  c->number[i]=operation(i);c->called[i]=1;
  result=slcli_kernel_call(c->number[i],c->args[i][0],c->args[i][1],c->args[i][2],
   c->args[i][3],c->args[i][4],c->args[i][5]);
  c->raw[i]=result;c->returned[i]=1;
  if(!slcli_native_clock_read(&c->after[i]) ||
   !slcli_native_clock_linux_matches_original(&c->after[i]) ||
   c->after[i].tick<c->before[i].tick || c->after[i].tick>=expires)return 0;
  if(i<3 && (result<(i==1?0:1) || result>INT32_MAX))return 0;
  if(i>=3&&i<7 && (result<0 || (uint64_t)result>UINT32_MAX))return 0;
  if(i==7 && (result<0 || result>INT32_MAX))return 0;
  c->completed=(uint32_t)i+1;previous=c->after[i].tick;
 }
 if(!slcli_native_clock_read(&c->finish) ||
 !slcli_native_clock_linux_matches_original(&c->finish) ||
 c->finish.tick<previous || c->finish.tick>=expires)return 0;
 c->failed=0;
 if(!identity_matches(c)){c->failed=1;return 0;}return 1;
}
static int launcher_rows_matches(
 const struct slcli_initial_launcher_linux *c) {
 if(!c || c->started!=1 || c->failed || c->identity_returned!=1 ||
 c->identity_result!=1 || c->preclaim_returned!=1 || c->preclaim_result!=1 ||
 !identity_matches(&c->identity) ||
 c->preclaim.launcher_original!=c ||
 c->preclaim.origin_original!=c->identity.origin_original ||
 c->preclaim.prior_original!=&c->identity.finish ||
 c->preclaim.expires!=c->identity.expires)return 0;
 return plan_matches(c) && slcli_initial_preclaim_linux_matches_original(&c->preclaim);
}
int slcli_initial_launcher_linux_prebirth_matches_original(
 const struct slcli_initial_launcher_linux *c) {
 if(!launcher_rows_matches(c))return 0;
 return slcli_native_clock_linux_matches_original(&c->finish) &&
 c->finish.tick>=c->preclaim.finish.tick && c->finish.tick<c->identity.expires;
}
int slcli_initial_launcher_linux_capture_prebirth_original(
 struct slcli_initial_launcher_linux *c,
 const struct slcli_initial_preclaim_request *r,uint64_t n,uint64_t page,uint64_t ceiling,
 const struct slcli_native_clock *origin,const struct slcli_native_clock *prior,
 uint64_t expires) {
 if(!c||!n||n>SIZE_MAX/sizeof(*r)||
 !away(c,sizeof(*c),r,(size_t)n*sizeof(*r)) ||
 !away(c,sizeof(*c),origin,sizeof(*origin)) ||
 !away(c,sizeof(*c),prior,sizeof(*prior)) || !zeroed(c,sizeof(*c)) ||
 !slcli_native_clock_linux_matches_original(origin) ||
 !slcli_native_clock_linux_matches_original(prior) ||
 origin->tick>prior->tick || prior->tick>=expires)return 0;
 if(!slcli_initial_preclaim_linux_plan_original(&c->plan,r,n,page,ceiling))return 0;
 c->started=1;c->failed=1;
 c->identity_result=identity_capture(&c->identity,origin,prior,expires);
 c->identity_returned=1;if(!c->identity_result)return 0;
 c->preclaim_result=slcli_initial_preclaim_linux_capture(&c->preclaim,c,r,n,page,ceiling,
 origin,&c->identity.finish,expires);
 c->preclaim_returned=1;if(!c->preclaim_result)return 0;
 c->failed=0;
 if(!launcher_rows_matches(c)){c->failed=1;return 0;}
 c->failed=1;
 if(!slcli_native_clock_read(&c->finish) ||
 !slcli_native_clock_linux_matches_original(&c->finish) ||
 c->finish.tick<c->preclaim.finish.tick || c->finish.tick>=expires)return 0;
 c->failed=0;
 return 1;
}
#endif
