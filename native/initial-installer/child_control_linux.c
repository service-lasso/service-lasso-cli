#if defined(__linux__)
#include "child_control_linux.h"
#include "../host-service/native_syscall_linux.h"
#include <linux/mman.h>
#include <limits.h>
static int apart(const void *a,uint64_t n,const void *b,uint64_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&n<=SIZE_MAX&&m<=SIZE_MAX&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&
 (x+n<=y||y+m<=x);
}
static int fresh(const void *p,size_t n) {
 const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;
}
static int strings(const void *out,uint64_t bytes,const char *const *v,
 const struct slcli_initial_child_string_linux *s,uint64_t count) {
 uint64_t i,j;
 if(count>SIZE_MAX/sizeof(*s) || count>=SIZE_MAX/sizeof(*v) ||
 !apart(out,bytes,v,(count+1)*sizeof(*v)) || (count&&!apart(out,bytes,s,count*sizeof(*s))))return 0;
 if(v[count])return 0;
 for(i=0;i<count;i++) {
 if(!s[i].bytes || s[i].bytes>SIZE_MAX || v[i]!=s[i].original ||
 !apart(out,bytes,s[i].original,s[i].bytes) || s[i].original[s[i].bytes-1])return 0;
 for(j=0;j<s[i].bytes-1;j++)if(!s[i].original[j])return 0;
 }
 return 1;
}
static int borrowed_away(const void *out,uint64_t bytes,
 const struct slcli_initial_held_file_linux *e,const struct slcli_initial_child_input_linux *in) {
 const struct slcli_initial_custody_memfd_linux *c=e->custody_original;
 const struct slcli_initial_launcher_linux *l=c->launcher_original;
 return apart(out,bytes,e,sizeof(*e)) && apart(out,bytes,in,sizeof(*in)) &&
 apart(out,bytes,e->path_original,e->path_bytes) && apart(out,bytes,c,sizeof(*c)) &&
 apart(out,bytes,l,sizeof(*l)) &&
 apart(out,bytes,l->preclaim.requests_original,l->preclaim.count*sizeof(*l->preclaim.requests_original)) &&
 apart(out,bytes,(const void *)(uintptr_t)l->preclaim.result_original,l->preclaim.metadata_bytes) &&
 apart(out,bytes,l->identity.origin_original,sizeof(*l->identity.origin_original)) &&
 apart(out,bytes,l->identity.prior_original,sizeof(*l->identity.prior_original)) &&
 apart(out,bytes,c->ledger_original,c->ledger_bytes) && apart(out,bytes,c->readback_original,c->ledger_bytes) &&
 apart(out,bytes,c->calls_original,c->capacity*sizeof(*c->calls_original)) &&
 strings(out,bytes,in->argv_original,in->argv_spans,in->argc) &&
 strings(out,bytes,in->env_original,in->env_spans,in->envc);
}
static int geometry(uint64_t page,uint64_t *bytes) {
 uint64_t n=sizeof(struct slcli_initial_child_control_linux),rem;
 if(!page || (page&(page-1)) || page>LONG_MAX)return 0;
 rem=n%page;if(rem && n>UINT64_MAX-(page-rem))return 0;
 if(rem)n+=page-rem;if(!n || n>LONG_MAX)return 0;*bytes=n;return 1;
}
static int source(const struct slcli_initial_child_control_map_linux *out,
 const struct slcli_initial_held_file_linux *e,const struct slcli_initial_child_input_linux *in,
 uint64_t page,uint64_t *bytes) {
 return out&&e&&in && apart(out,sizeof(*out),e,sizeof(*e)) &&
 apart(out,sizeof(*out),in,sizeof(*in)) && slcli_initial_held_file_linux_matches_original(e) &&
 in->argc && geometry(page,bytes) && borrowed_away(out,sizeof(*out),e,in);
}
static int contents(const struct slcli_initial_child_control_map_linux *c) {
 const struct slcli_initial_custody_call_linux *r;uint64_t bytes;
 if(!c || !source(c,c->executable_original,c->input_original,c->page_bytes,&bytes) ||
 c->started!=1 || c->failed || c->prepared!=1 || c->mapping_bytes!=bytes ||
 c->prior_tick!=c->executable_original->finish.tick || c->expires!=c->executable_original->expires)return 0;
 r=&c->mapping;
 return r->number==__NR_mmap && !r->args[0] && r->args[1]==(long)bytes &&
 r->args[2]==(PROT_READ|PROT_WRITE) && r->args[3]==(MAP_SHARED|MAP_ANONYMOUS) &&
 r->args[4]==-1 && !r->args[5] && r->called==1 && r->returned==1 && r->raw>0 &&
 c->control_original==(struct slcli_initial_child_control_linux *)(uintptr_t)r->raw &&
 apart(c,sizeof(*c),c->control_original,bytes) &&
 borrowed_away(c->control_original,bytes,c->executable_original,c->input_original) &&
 slcli_native_clock_linux_matches_original(&r->before) &&
 slcli_native_clock_linux_matches_original(&r->after) &&
 r->before.tick>=c->prior_tick && r->before.tick<c->expires &&
 r->after.tick>=r->before.tick && r->after.tick<c->expires;
}
int slcli_initial_child_control_map_linux_matches_original(
 const struct slcli_initial_child_control_map_linux *c) {
 return contents(c) && slcli_native_clock_linux_matches_original(&c->finish) &&
 c->finish.tick>=c->mapping.after.tick && c->finish.tick<c->expires;
}
int slcli_initial_child_control_map_linux_output_away_original(
 const struct slcli_initial_child_control_map_linux *c,const void *out,uint64_t bytes) {
 return slcli_initial_child_control_map_linux_matches_original(c) &&
 apart(out,bytes,c,sizeof(*c)) && apart(out,bytes,c->control_original,c->mapping_bytes) &&
 borrowed_away(out,bytes,c->executable_original,c->input_original);
}
int slcli_initial_child_control_map_linux_capture_original(
 struct slcli_initial_child_control_map_linux *c,const struct slcli_initial_held_file_linux *e,
 const struct slcli_initial_child_input_linux *in,uint64_t page) {
 uint64_t bytes;struct slcli_initial_custody_call_linux *r;
 if(!source(c,e,in,page,&bytes) || !fresh(c,sizeof(*c)))return 0;
 c->executable_original=e;c->input_original=in;c->page_bytes=page;c->mapping_bytes=bytes;
 c->prior_tick=e->finish.tick;c->expires=e->expires;c->started=1;c->failed=1;r=&c->mapping;
 r->number=__NR_mmap;r->args[1]=bytes;r->args[2]=PROT_READ|PROT_WRITE;
 r->args[3]=MAP_SHARED|MAP_ANONYMOUS;r->args[4]=-1;
 if(!slcli_native_clock_read(&r->before) || !slcli_native_clock_linux_matches_original(&r->before) ||
 r->before.tick<c->prior_tick || r->before.tick>=c->expires)return 0;
 r->called=1;r->raw=slcli_kernel_call(r->number,r->args[0],r->args[1],r->args[2],r->args[3],r->args[4],r->args[5]);
 r->returned=1;c->control_original=(struct slcli_initial_child_control_linux *)(uintptr_t)r->raw;
 if(!slcli_native_clock_read(&r->after) || !slcli_native_clock_linux_matches_original(&r->after) ||
 r->after.tick<r->before.tick || r->after.tick>=c->expires || r->raw<=0 ||
 !apart(c,sizeof(*c),c->control_original,bytes) || !borrowed_away(c->control_original,bytes,e,in) ||
 !fresh(c->control_original,sizeof(*c->control_original)))return 0;
 c->prepared=1;c->failed=0;
 if(!contents(c)){c->failed=1;return 0;}c->failed=1;
 if(!slcli_native_clock_read(&c->finish) || !slcli_native_clock_linux_matches_original(&c->finish) ||
 c->finish.tick<r->after.tick || c->finish.tick>=c->expires)return 0;
 c->failed=0;return 1;
}
#endif
