#if defined(__linux__)
#include "held_file_linux.h"
#include "../host-service/native_syscall_linux.h"
#include <linux/fcntl.h>
#include <linux/stat.h>
#include <limits.h>
static int away(const void *a,uint64_t n,const void *b,uint64_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&n<=SIZE_MAX&&m<=SIZE_MAX&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&
 (x+n<=y||y+m<=x);
}
static int fresh(const void *p,size_t n) {
 const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;
}
static int source(const struct slcli_initial_held_file_linux *out,
 const struct slcli_initial_custody_memfd_linux *c,const char *path,uint64_t n) {
 const struct slcli_initial_launcher_linux *l;uint64_t i;
 if(!out || !c || !path || n<2 || n>SIZE_MAX || (uintptr_t)path>UINTPTR_MAX-n ||
 !away(out,sizeof(*out),c,sizeof(*c)) || !away(out,sizeof(*out),path,n) ||
 !slcli_initial_custody_memfd_linux_matches_original(c))return 0;
 l=c->launcher_original;
 if(!away(out,sizeof(*out),l,sizeof(*l)) ||
 !away(out,sizeof(*out),l->preclaim.requests_original,l->preclaim.count*sizeof(*l->preclaim.requests_original)) ||
 !away(out,sizeof(*out),(const void *)(uintptr_t)l->preclaim.result_original,l->preclaim.metadata_bytes) ||
 !away(out,sizeof(*out),l->identity.origin_original,sizeof(*l->identity.origin_original)) ||
 !away(out,sizeof(*out),l->identity.prior_original,sizeof(*l->identity.prior_original)) ||
 !away(out,sizeof(*out),c->ledger_original,c->ledger_bytes) ||
 !away(out,sizeof(*out),c->readback_original,c->ledger_bytes) ||
 !away(out,sizeof(*out),c->calls_original,c->capacity*sizeof(*c->calls_original)))return 0;
 if(path[0]!='/' || path[n-1])return 0;
 for(i=0;i<n-1;i++)if(!path[i])return 0;
 return 1;
}
static int row(const struct slcli_initial_custody_call_linux *r,long number,
 long a,long b,long d,long e,uint64_t prior,uint64_t expires) {
 return r->number==number && r->args[0]==a && r->args[1]==b && r->args[2]==d &&
 r->args[3]==e && !r->args[4] && !r->args[5] && r->called==1 && r->returned==1 &&
 slcli_native_clock_linux_matches_original(&r->before) &&
 slcli_native_clock_linux_matches_original(&r->after) &&
 r->before.tick>=prior && r->before.tick<expires &&
 r->after.tick>=r->before.tick && r->after.tick<expires;
}
static int issue(struct slcli_initial_held_file_linux *c,size_t i,long number,
 long a,long b,long d,long e) {
 struct slcli_initial_custody_call_linux *r=&c->calls[i];
 uint64_t prior=i?c->calls[i-1].after.tick:c->prior_tick;
 r->number=number;r->args[0]=a;r->args[1]=b;r->args[2]=d;r->args[3]=e;
 if(!slcli_native_clock_read(&r->before) ||
 !slcli_native_clock_linux_matches_original(&r->before) ||
 r->before.tick<prior || r->before.tick>=c->expires)return 0;
 r->called=1;r->raw=slcli_kernel_call(r->number,r->args[0],r->args[1],r->args[2],r->args[3],0,0);
 r->returned=1;
 if(!i)c->opened_descriptor_original=r->raw;
 if(i==2)c->descriptor_original=r->raw;
 if(!slcli_native_clock_read(&r->after))return 0;
 return row(r,number,a,b,d,e,prior,c->expires);
}
static int contents(const struct slcli_initial_held_file_linux *c) {
 long fd,opened;
 if(!c || !source(c,c->custody_original,c->path_original,c->path_bytes) ||
 c->started!=1 || c->failed || c->completed!=1 ||
 c->prior_tick!=c->custody_original->finish.tick || c->expires!=c->custody_original->expires)return 0;
 fd=c->descriptor_original;opened=c->opened_descriptor_original;
 if(opened<0 || opened>INT32_MAX || opened==c->custody_original->descriptor_original ||
 fd<4 || fd>INT32_MAX || fd==opened || fd==c->custody_original->descriptor_original ||
 !row(&c->calls[0],__NR_openat,AT_FDCWD,
 (long)(uintptr_t)c->path_original,O_RDONLY|O_CLOEXEC|O_NOFOLLOW,0,c->prior_tick,c->expires) ||
 c->calls[0].raw!=opened)return 0;
 if(!row(&c->calls[1],__NR_fstat,opened,(long)(uintptr_t)&c->opened_stat_original,0,0,
 c->calls[0].after.tick,c->expires) || c->calls[1].raw ||
 (c->opened_stat_original.st_mode&S_IFMT)!=S_IFREG || c->opened_stat_original.st_size<=0)return 0;
 if(!row(&c->calls[2],__NR_fcntl,opened,F_DUPFD_CLOEXEC,4,0,c->calls[1].after.tick,c->expires) ||
 c->calls[2].raw!=fd)return 0;
 if(!row(&c->calls[3],__NR_fstat,fd,(long)(uintptr_t)&c->stat_original,0,0,
 c->calls[2].after.tick,c->expires) || c->calls[3].raw ||
 c->stat_original.st_dev!=c->opened_stat_original.st_dev ||
 c->stat_original.st_ino!=c->opened_stat_original.st_ino ||
 c->stat_original.st_mode!=c->opened_stat_original.st_mode ||
 c->stat_original.st_size!=c->opened_stat_original.st_size)return 0;
 if(!row(&c->calls[4],__NR_fcntl,fd,F_GETFL,0,0,c->calls[3].after.tick,c->expires) ||
 c->calls[4].raw<0 || (c->calls[4].raw&O_ACCMODE)!=O_RDONLY)return 0;
 return row(&c->calls[5],__NR_fcntl,fd,F_GETFD,0,0,c->calls[4].after.tick,c->expires) &&
 c->calls[5].raw>=0 && (c->calls[5].raw&FD_CLOEXEC);
}
int slcli_initial_held_file_linux_matches_original(const struct slcli_initial_held_file_linux *c) {
 return contents(c) && slcli_native_clock_linux_matches_original(&c->finish) &&
 c->finish.tick>=c->calls[5].after.tick && c->finish.tick<c->expires;
}
int slcli_initial_held_file_linux_capture_original(struct slcli_initial_held_file_linux *c,
 const struct slcli_initial_custody_memfd_linux *prior,const char *path,uint64_t n) {
 long fd,opened;
 if(!source(c,prior,path,n) || !fresh(c,sizeof(*c)))return 0;
 c->custody_original=prior;c->path_original=path;c->path_bytes=n;
 c->prior_tick=prior->finish.tick;c->expires=prior->expires;
 c->opened_descriptor_original=-1;c->descriptor_original=-1;c->started=1;c->failed=1;
 if(!issue(c,0,__NR_openat,AT_FDCWD,(long)(uintptr_t)path,O_RDONLY|O_CLOEXEC|O_NOFOLLOW,0))return 0;
 opened=c->opened_descriptor_original;
 if(opened<0||opened>INT32_MAX||opened==prior->descriptor_original)return 0;
 if(!issue(c,1,__NR_fstat,opened,(long)(uintptr_t)&c->opened_stat_original,0,0) || c->calls[1].raw ||
 (c->opened_stat_original.st_mode&S_IFMT)!=S_IFREG || c->opened_stat_original.st_size<=0)return 0;
 if(!issue(c,2,__NR_fcntl,opened,F_DUPFD_CLOEXEC,4,0))return 0;
 fd=c->descriptor_original;
 if(fd<4||fd>INT32_MAX||fd==opened||fd==prior->descriptor_original)return 0;
 if(!issue(c,3,__NR_fstat,fd,(long)(uintptr_t)&c->stat_original,0,0) || c->calls[3].raw ||
 c->stat_original.st_dev!=c->opened_stat_original.st_dev ||
 c->stat_original.st_ino!=c->opened_stat_original.st_ino ||
 c->stat_original.st_mode!=c->opened_stat_original.st_mode ||
 c->stat_original.st_size!=c->opened_stat_original.st_size)return 0;
 if(!issue(c,4,__NR_fcntl,fd,F_GETFL,0,0) || c->calls[4].raw<0 ||
 (c->calls[4].raw&O_ACCMODE)!=O_RDONLY)return 0;
 if(!issue(c,5,__NR_fcntl,fd,F_GETFD,0,0) || c->calls[5].raw<0 || !(c->calls[5].raw&FD_CLOEXEC))return 0;
 c->completed=1;c->failed=0;
 if(!contents(c)){c->failed=1;return 0;}c->failed=1;
 if(!slcli_native_clock_read(&c->finish) || !slcli_native_clock_linux_matches_original(&c->finish) ||
 c->finish.tick<c->calls[5].after.tick || c->finish.tick>=c->expires)return 0;
 c->failed=0;return 1;
}
#endif
