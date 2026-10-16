#if defined(__linux__)
#include "child_maps_linux.h"
#include "../host-service/native_syscall_linux.h"
#include <linux/fcntl.h>
#include <linux/stat.h>
#include <limits.h>
static int apart(const void *a,uint64_t n,const void *b,uint64_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&n<=SIZE_MAX&&m<=SIZE_MAX&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&
 (x+n<=y||y+m<=x);
}
static int zero(const void *p,uint64_t n) {
 const unsigned char *s=p;uint64_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;
}
static int plan(uint64_t bytes,uint64_t rows,uint64_t *row_bytes,uint64_t *calls) {
 if(!bytes||bytes>LONG_MAX||bytes>UINT64_MAX-5||rows!=bytes+5||
 rows>SIZE_MAX/sizeof(struct slcli_initial_custody_call_linux)||rows>(UINT64_MAX-1)/3)return 0;
 *row_bytes=rows*sizeof(struct slcli_initial_custody_call_linux);*calls=3*rows+1;return 1;
}
static int path(char *out,long pid) {
 static const char prefix[]="/proc/",suffix[]="/maps";
 char digits[10];uint64_t count=0,i,n=0;
 if(pid<=0||pid>INT_MAX)return 0;
 do{digits[count++]=(char)('0'+pid%10);pid/=10;}while(pid);
 for(i=0;i<sizeof(prefix)-1;i++)out[n++]=prefix[i];
 while(count)out[n++]=digits[--count];
 for(i=0;i<sizeof(suffix);i++)out[n++]=suffix[i];return 1;
}
static int shape(const struct slcli_initial_child_maps_linux *c,
 const struct slcli_initial_child_birth_linux *b,const struct slcli_initial_child_maps_linux *previous,
 unsigned char *bytes,uint64_t capacity,
 struct slcli_initial_custody_call_linux *rows,uint64_t row_capacity) {
 uint64_t row_bytes,calls,previous_rows;
 if(!c||!plan(capacity,row_capacity,&row_bytes,&calls))return 0;
 if(previous) {
 if(previous->prior_cut_original||previous->birth_original!=b||
 !slcli_initial_child_maps_linux_matches_original(previous))return 0;
 previous_rows=previous->row_capacity*sizeof(*previous->calls_original);
 if(!apart(c,sizeof(*c),previous,sizeof(*previous))||
 !apart(c,sizeof(*c),previous->bytes_original,previous->capacity)||
 !apart(c,sizeof(*c),previous->calls_original,previous_rows)||
 !apart(bytes,capacity,previous,sizeof(*previous))||
 !apart(bytes,capacity,previous->bytes_original,previous->capacity)||
 !apart(bytes,capacity,previous->calls_original,previous_rows)||
 !apart(rows,row_bytes,previous,sizeof(*previous))||
 !apart(rows,row_bytes,previous->bytes_original,previous->capacity)||
 !apart(rows,row_bytes,previous->calls_original,previous_rows))return 0;
 }
 return
 slcli_initial_child_birth_linux_output_away_original(b,c,sizeof(*c))&&
 slcli_initial_child_birth_linux_output_away_original(b,bytes,capacity)&&
 slcli_initial_child_birth_linux_output_away_original(b,rows,row_bytes)&&
 apart(c,sizeof(*c),bytes,capacity)&&apart(c,sizeof(*c),rows,row_bytes)&&apart(bytes,capacity,rows,row_bytes);
}
static int row(const struct slcli_initial_custody_call_linux *r,long number,
 long a,long b,long d,long e,uint64_t prior,uint64_t expiry) {
 return r->number==number&&r->args[0]==a&&r->args[1]==b&&r->args[2]==d&&r->args[3]==e&&
 !r->args[4]&&!r->args[5]&&r->called==1&&r->returned==1&&
 slcli_native_clock_linux_matches_original(&r->before)&&slcli_native_clock_linux_matches_original(&r->after)&&
 r->before.tick>=prior&&r->before.tick<expiry&&r->after.tick>=r->before.tick&&r->after.tick<expiry;
}
static int issue(struct slcli_initial_child_maps_linux *c,long number,long a,long b,long d,long e) {
 struct slcli_initial_custody_call_linux *r;uint64_t prior;
 if(c->used>=c->row_capacity)return 0;
 prior=c->used?c->calls_original[c->used-1].after.tick:c->prior_tick;
 r=&c->calls_original[c->used++];r->number=number;r->args[0]=a;r->args[1]=b;r->args[2]=d;r->args[3]=e;
 if(!slcli_native_clock_read(&r->before)||!slcli_native_clock_linux_matches_original(&r->before)||
 r->before.tick<prior||r->before.tick>=c->expires)return 0;
 r->called=1;r->raw=slcli_kernel_call(number,a,b,d,e,0,0);r->returned=1;
 if(number==__NR_openat)c->descriptor_original=r->raw;
 return slcli_native_clock_read(&r->after)&&row(r,number,a,b,d,e,prior,c->expires);
}
static long last(const struct slcli_initial_child_maps_linux *c){return c->calls_original[c->used-1].raw;}
static int take(const struct slcli_initial_child_maps_linux *c,uint64_t *index,uint64_t *prior,
 long number,long a,long b,long d,long e,long *raw) {
 const struct slcli_initial_custody_call_linux *r;
 if(*index>=c->used)return 0;r=&c->calls_original[(*index)++];
 if(!row(r,number,a,b,d,e,*prior,c->expires))return 0;
 *prior=r->after.tick;*raw=r->raw;return 1;
}
static int contents(const struct slcli_initial_child_maps_linux *c,uint64_t *last_tick) {
 uint64_t row_bytes,calls,i=0,prior,offset=0;long raw,fd;char expected[sizeof(c->path_original)]={0};
 if(!c||!shape(c,c->birth_original,c->prior_cut_original,c->bytes_original,c->capacity,c->calls_original,c->row_capacity)||
 !plan(c->capacity,c->row_capacity,&row_bytes,&calls)||c->native_attempts!=calls||
 c->started!=1||c->failed||c->completed!=1||c->eof_observed!=1||c->used>c->row_capacity||
 c->expires!=c->birth_original->map_original->expires||
 c->prior_tick!=(c->prior_cut_original?c->prior_cut_original->finish.tick:c->birth_original->stages[1].finish.tick)||
 !path(expected,c->birth_original->clone_call.raw))return 0;
 for(i=0;i<sizeof(expected);i++)if(c->path_original[i]!=expected[i])return 0;i=0;prior=c->prior_tick;
 if(!take(c,&i,&prior,__NR_openat,AT_FDCWD,(long)(uintptr_t)c->path_original,
 O_RDONLY|O_CLOEXEC|O_NOFOLLOW,0,&raw)||raw<0||raw>INT_MAX||raw!=c->descriptor_original)return 0;fd=raw;
 if(!take(c,&i,&prior,__NR_fstat,fd,(long)(uintptr_t)&c->stat_original,0,0,&raw)||raw||
 (c->stat_original.st_mode&S_IFMT)!=S_IFREG)return 0;
 if(!take(c,&i,&prior,__NR_fcntl,fd,F_GETFL,0,0,&raw)||raw<0||(raw&O_ACCMODE)!=O_RDONLY)return 0;
 if(!take(c,&i,&prior,__NR_fcntl,fd,F_GETFD,0,0,&raw)||raw!=FD_CLOEXEC)return 0;
 for(;;) {
 if(offset==c->capacity) {
 if(!take(c,&i,&prior,__NR_read,fd,(long)(uintptr_t)&c->overflow_original,1,0,&raw)||raw)return 0;
 break;
 }
 if(!take(c,&i,&prior,__NR_read,fd,(long)(uintptr_t)(c->bytes_original+offset),c->capacity-offset,0,&raw)||
 raw<0||(uint64_t)raw>c->capacity-offset)return 0;
 if(!raw)break;offset+=(uint64_t)raw;
 }
 if(offset!=c->bytes_used||i!=c->used)return 0;*last_tick=prior;return 1;
}
int slcli_initial_child_maps_linux_matches_original(const struct slcli_initial_child_maps_linux *c) {
 uint64_t prior;
 return contents(c,&prior)&&slcli_native_clock_linux_matches_original(&c->finish)&&
 c->finish.tick>=prior&&c->finish.tick<c->expires;
}
int slcli_initial_child_maps_linux_capture_original(struct slcli_initial_child_maps_linux *c,
 const struct slcli_initial_child_birth_linux *b,const struct slcli_initial_child_maps_linux *previous,
 unsigned char *bytes,uint64_t capacity,
 struct slcli_initial_custody_call_linux *rows,uint64_t row_capacity) {
 uint64_t row_bytes,calls,prior;long fd,raw;
 if(!shape(c,b,previous,bytes,capacity,rows,row_capacity)||!plan(capacity,row_capacity,&row_bytes,&calls)||
 !zero(c,sizeof(*c))||!zero(bytes,capacity)||!zero(rows,row_bytes))return 0;
 c->birth_original=b;c->prior_cut_original=previous;c->bytes_original=bytes;c->capacity=capacity;c->calls_original=rows;
 c->row_capacity=row_capacity;c->native_attempts=calls;c->expires=b->map_original->expires;
 c->prior_tick=previous?previous->finish.tick:b->stages[1].finish.tick;
 c->descriptor_original=-1;c->started=1;c->failed=1;
 if(!path(c->path_original,b->clone_call.raw)||!issue(c,__NR_openat,AT_FDCWD,(long)(uintptr_t)c->path_original,
 O_RDONLY|O_CLOEXEC|O_NOFOLLOW,0))return 0;fd=last(c);if(fd<0||fd>INT_MAX)return 0;
 if(!issue(c,__NR_fstat,fd,(long)(uintptr_t)&c->stat_original,0,0)||last(c)||
 (c->stat_original.st_mode&S_IFMT)!=S_IFREG)return 0;
 if(!issue(c,__NR_fcntl,fd,F_GETFL,0,0)||last(c)<0||(last(c)&O_ACCMODE)!=O_RDONLY)return 0;
 if(!issue(c,__NR_fcntl,fd,F_GETFD,0,0)||last(c)!=FD_CLOEXEC)return 0;
 for(;;) {
 if(c->bytes_used==capacity) {
 if(!issue(c,__NR_read,fd,(long)(uintptr_t)&c->overflow_original,1,0)||last(c))return 0;
 c->eof_observed=1;break;
 }
 if(!issue(c,__NR_read,fd,(long)(uintptr_t)(bytes+c->bytes_used),capacity-c->bytes_used,0))return 0;
 raw=last(c);if(raw<0||(uint64_t)raw>capacity-c->bytes_used)return 0;
 if(!raw){c->eof_observed=1;break;}c->bytes_used+=(uint64_t)raw;
 }
 c->completed=1;c->failed=0;
 if(!contents(c,&prior)){c->failed=1;return 0;}c->failed=1;
 if(!slcli_native_clock_read(&c->finish)||!slcli_native_clock_linux_matches_original(&c->finish)||
 c->finish.tick<prior||c->finish.tick>=c->expires)return 0;
 c->failed=0;return 1;
}
#endif
