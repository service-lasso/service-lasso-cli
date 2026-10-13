#if defined(__linux__)
#if !defined(__x86_64__) || __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error Initial custody requires separately selected Linux x86_64 little-endian ABI
#endif
#include "custody_memfd_linux.h"
#include "../host-service/native_syscall_linux.h"
#include <linux/memfd.h>
#include <linux/fcntl.h>
#include <linux/stat.h>
#include <limits.h>
static const char ledger_name[]="slcli-initial-ledger";
static int interval(const void *p,uint64_t n) {
 return p && n && n<=SIZE_MAX && (uintptr_t)p<=UINTPTR_MAX-n;
}
static int away(const void *a,uint64_t n,const void *b,uint64_t m) {
 return interval(a,n)&&interval(b,m)&&
 ((uintptr_t)a+n<=(uintptr_t)b || (uintptr_t)b+m<=(uintptr_t)a);
}
static int zeroed(const void *p,uint64_t n) {
 const unsigned char *b=p;uint64_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;
}
static int source_away(const void *p,uint64_t bytes,
 const struct slcli_initial_launcher_linux *l,const unsigned char *ledger,uint64_t n) {
 return away(p,bytes,l,sizeof(*l)) && away(p,bytes,ledger,n) &&
 away(p,bytes,(const void *)(uintptr_t)l->preclaim.result_original,
 l->preclaim.metadata_bytes) &&
 away(p,bytes,l->preclaim.requests_original,
 l->preclaim.count*sizeof(*l->preclaim.requests_original)) &&
 away(p,bytes,l->identity.origin_original,sizeof(*l->identity.origin_original)) &&
 away(p,bytes,l->identity.prior_original,sizeof(*l->identity.prior_original)) &&
 away(p,bytes,ledger_name,sizeof(ledger_name));
}
static int shape(const struct slcli_initial_custody_memfd_linux *c,
 const struct slcli_initial_launcher_linux *l,const unsigned char *ledger,uint64_t n,
 unsigned char *readback,struct slcli_initial_custody_call_linux *rows,uint64_t capacity) {
 uint64_t bytes;
 if(!c || !l || !n || n>LONG_MAX || n>(UINT64_MAX-8)/2 || capacity!=2*n+8 ||
 capacity>SIZE_MAX/sizeof(*rows) || !interval(ledger,n) ||
 !slcli_initial_launcher_linux_prebirth_matches_original(l))return 0;
 bytes=capacity*sizeof(*rows);
 return away(c,sizeof(*c),readback,n) && away(c,sizeof(*c),rows,bytes) &&
 away(rows,bytes,readback,n) && source_away(c,sizeof(*c),l,ledger,n) &&
 source_away(rows,bytes,l,ledger,n) && source_away(readback,n,l,ledger,n);
}
static int row_matches(const struct slcli_initial_custody_call_linux *r,
 long number,long a,long b,long d,long e,long f,long g,uint64_t prior,uint64_t expires) {
 return r->number==number && r->args[0]==a && r->args[1]==b && r->args[2]==d &&
 r->args[3]==e && r->args[4]==f && r->args[5]==g && r->called==1 && r->returned==1 &&
 slcli_native_clock_linux_matches_original(&r->before) &&
 slcli_native_clock_linux_matches_original(&r->after) &&
 r->before.tick>=prior && r->before.tick<expires &&
 r->after.tick>=r->before.tick && r->after.tick<expires;
}
static int issue(struct slcli_initial_custody_memfd_linux *c,
 long number,long a,long b,long d,long e,long f,long g) {
 struct slcli_initial_custody_call_linux *r;
 uint64_t prior=c->used?c->calls_original[c->used-1].after.tick:c->prior_tick;
 if(c->used>=c->capacity)return 0;
 r=&c->calls_original[c->used++];
 r->number=number;r->args[0]=a;r->args[1]=b;r->args[2]=d;
 r->args[3]=e;r->args[4]=f;r->args[5]=g;
 if(!slcli_native_clock_read(&r->before) ||
 !slcli_native_clock_linux_matches_original(&r->before) ||
 r->before.tick<prior || r->before.tick>=c->expires)return 0;
 r->called=1;r->raw=slcli_kernel_call(r->number,r->args[0],r->args[1],r->args[2],
 r->args[3],r->args[4],r->args[5]);r->returned=1;
 /* Descriptor custody precedes the post-call clock, including late failure. */
 if(number==__NR_memfd_create)c->descriptor_original=r->raw;
 if(!slcli_native_clock_read(&r->after))return 0;
 return row_matches(r,number,a,b,d,e,f,g,prior,c->expires);
}
static long last(const struct slcli_initial_custody_memfd_linux *c) {
 return c->calls_original[c->used-1].raw;
}
static int take(const struct slcli_initial_custody_memfd_linux *c,uint64_t *index,
 uint64_t *prior,long number,long a,long b,long d,long e,long f,long g,long *raw) {
 const struct slcli_initial_custody_call_linux *r;
 if(*index>=c->used)return 0;r=&c->calls_original[(*index)++];
 if(!row_matches(r,number,a,b,d,e,f,g,*prior,c->expires))return 0;
 *prior=r->after.tick;*raw=r->raw;return 1;
}
static int contents_matches(const struct slcli_initial_custody_memfd_linux *c,
 uint64_t *last_tick) {
 uint64_t i=0,offset,prior;long raw,fd;const int seals=F_SEAL_WRITE|F_SEAL_GROW|F_SEAL_SHRINK|F_SEAL_SEAL;
 if(!c || !shape(c,c->launcher_original,
 c->ledger_original,c->ledger_bytes,c->readback_original,c->calls_original,c->capacity) ||
 c->started!=1 || c->failed || c->completed!=1 || !c->used || c->used>c->capacity ||
 c->prior_tick!=c->launcher_original->finish.tick || c->expires!=c->launcher_original->identity.expires)return 0;
 prior=c->prior_tick;
 if(!take(c,&i,&prior,__NR_memfd_create,(long)(uintptr_t)ledger_name,
 MFD_CLOEXEC|MFD_ALLOW_SEALING|MFD_NOEXEC_SEAL,0,0,0,0,&raw) || raw<0 || raw>INT32_MAX ||
 raw!=c->descriptor_original)return 0;fd=raw;
 if(!take(c,&i,&prior,__NR_ftruncate,fd,c->ledger_bytes,0,0,0,0,&raw)||raw)return 0;
 for(offset=0;offset<c->ledger_bytes;offset+=(uint64_t)raw)
 if(!take(c,&i,&prior,__NR_pwrite64,fd,(long)(uintptr_t)(c->ledger_original+offset),
 c->ledger_bytes-offset,offset,0,0,&raw)||raw<=0||(uint64_t)raw>c->ledger_bytes-offset)return 0;
 for(offset=0;offset<c->ledger_bytes;offset+=(uint64_t)raw)
 if(!take(c,&i,&prior,__NR_pread64,fd,(long)(uintptr_t)(c->readback_original+offset),
 c->ledger_bytes-offset,offset,0,0,&raw)||raw<=0||(uint64_t)raw>c->ledger_bytes-offset)return 0;
 if(!take(c,&i,&prior,__NR_pread64,fd,(long)(uintptr_t)&c->eof_original,1,c->ledger_bytes,0,0,&raw)||raw)return 0;
 if(!take(c,&i,&prior,__NR_fcntl,fd,F_ADD_SEALS,seals,0,0,0,&raw)||raw)return 0;
 if(!take(c,&i,&prior,__NR_fcntl,fd,F_GET_SEALS,0,0,0,0,&raw)||raw<0||(raw&(seals|F_SEAL_EXEC))!=(seals|F_SEAL_EXEC))return 0;
 if(!take(c,&i,&prior,__NR_fstat,fd,(long)(uintptr_t)&c->stat_original,0,0,0,0,&raw)||raw ||
 (c->stat_original.st_mode&S_IFMT)!=S_IFREG || (c->stat_original.st_mode&0111) || c->stat_original.st_size!=(long)c->ledger_bytes)return 0;
 if(!take(c,&i,&prior,__NR_fcntl,fd,F_GETFL,0,0,0,0,&raw)||raw<0||(raw&O_ACCMODE)!=O_RDWR)return 0;
 if(!take(c,&i,&prior,__NR_fcntl,fd,F_GETFD,0,0,0,0,&raw)||raw<0||!(raw&FD_CLOEXEC)||i!=c->used)return 0;
 for(offset=0;offset<c->ledger_bytes;offset++)if(c->ledger_original[offset]!=c->readback_original[offset])return 0;
 *last_tick=prior;return 1;
}
int slcli_initial_custody_memfd_linux_matches_original(
 const struct slcli_initial_custody_memfd_linux *c) {
 uint64_t prior;
 return contents_matches(c,&prior) &&
 slcli_native_clock_linux_matches_original(&c->finish) &&
 c->finish.tick>=prior && c->finish.tick<c->expires;
}
int slcli_initial_custody_memfd_linux_capture_original(
 struct slcli_initial_custody_memfd_linux *c,const struct slcli_initial_launcher_linux *l,
 const unsigned char *ledger,uint64_t n,unsigned char *readback,
 struct slcli_initial_custody_call_linux *rows,uint64_t capacity) {
 uint64_t offset,prior;long raw,fd;const int seals=F_SEAL_WRITE|F_SEAL_GROW|F_SEAL_SHRINK|F_SEAL_SEAL;
 if(!shape(c,l,ledger,n,readback,rows,capacity) || !zeroed(c,sizeof(*c)) ||
 !zeroed(rows,capacity*sizeof(*rows)) || !zeroed(readback,n))return 0;
 c->launcher_original=l;c->ledger_original=ledger;c->ledger_bytes=n;c->readback_original=readback;
 c->calls_original=rows;c->capacity=capacity;c->expires=l->identity.expires;c->prior_tick=l->finish.tick;
 c->descriptor_original=-1;c->started=1;c->failed=1;
 if(!issue(c,__NR_memfd_create,(long)(uintptr_t)ledger_name,MFD_CLOEXEC|MFD_ALLOW_SEALING|MFD_NOEXEC_SEAL,0,0,0,0))return 0;
 fd=last(c);if(fd<0||fd>INT32_MAX)return 0;
 if(!issue(c,__NR_ftruncate,fd,n,0,0,0,0)||last(c))return 0;
 for(offset=0;offset<n;offset+=(uint64_t)raw) {
 if(!issue(c,__NR_pwrite64,fd,(long)(uintptr_t)(ledger+offset),n-offset,offset,0,0))return 0;
 raw=last(c);if(raw<=0||(uint64_t)raw>n-offset)return 0;
 }
 for(offset=0;offset<n;offset+=(uint64_t)raw) {
 if(!issue(c,__NR_pread64,fd,(long)(uintptr_t)(readback+offset),n-offset,offset,0,0))return 0;
 raw=last(c);if(raw<=0||(uint64_t)raw>n-offset)return 0;
 }
 if(!issue(c,__NR_pread64,fd,(long)(uintptr_t)&c->eof_original,1,n,0,0)||last(c))return 0;
 for(offset=0;offset<n;offset++)if(ledger[offset]!=readback[offset])return 0;
 if(!issue(c,__NR_fcntl,fd,F_ADD_SEALS,seals,0,0,0)||last(c))return 0;
 if(!issue(c,__NR_fcntl,fd,F_GET_SEALS,0,0,0,0)||last(c)<0||(last(c)&(seals|F_SEAL_EXEC))!=(seals|F_SEAL_EXEC))return 0;
 if(!issue(c,__NR_fstat,fd,(long)(uintptr_t)&c->stat_original,0,0,0,0)||last(c) ||
 (c->stat_original.st_mode&S_IFMT)!=S_IFREG || (c->stat_original.st_mode&0111) || c->stat_original.st_size!=(long)n)return 0;
 if(!issue(c,__NR_fcntl,fd,F_GETFL,0,0,0,0)||last(c)<0||(last(c)&O_ACCMODE)!=O_RDWR)return 0;
 if(!issue(c,__NR_fcntl,fd,F_GETFD,0,0,0,0)||last(c)<0||!(last(c)&FD_CLOEXEC))return 0;
 c->completed=1;c->failed=0;
 if(!contents_matches(c,&prior)){c->failed=1;return 0;}
 c->failed=1;
 if(!slcli_native_clock_read(&c->finish) || !slcli_native_clock_linux_matches_original(&c->finish) ||
 c->finish.tick<prior || c->finish.tick>=c->expires)return 0;
 c->failed=0;return 1;
}
#endif
