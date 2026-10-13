#if defined(__linux__)
#if !defined(__x86_64__) || __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error Initial launcher preclaims require selected Linux x86_64 little-endian ABI
#endif
#include "launcher_linux.h"
#include "../host-service/native_syscall_linux.h"
#include <linux/mman.h>
#include <limits.h>
_Static_assert(sizeof(uintptr_t)==8 && sizeof(size_t)==8 && sizeof(long)==8,
 "initial preclaim syscall ABI");
static int interval(const void *p,uint64_t n) {
 return p && n && n<=SIZE_MAX && (uintptr_t)p<=UINTPTR_MAX-n;
}
static int away(const void *a,uint64_t an,const void *b,uint64_t bn) {
 return interval(a,an)&&interval(b,bn)&&
 ((uintptr_t)a+an<=(uintptr_t)b || (uintptr_t)b+bn<=(uintptr_t)a);
}
static int zeroed(const void *p,size_t n) {
 const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;
}
static int cost_equal(const struct slcli_bq_resource_cost *a,
 const struct slcli_bq_resource_cost *b) {
 return a->user_ram_bytes==b->user_ram_bytes &&
 a->user_virtual_bytes==b->user_virtual_bytes && a->kernel_ram_bytes==b->kernel_ram_bytes &&
 a->cache_ram_bytes==b->cache_ram_bytes && a->retained_storage_bytes==b->retained_storage_bytes &&
 a->native_references==b->native_references && a->physical_extent_events==b->physical_extent_events &&
 a->native_attempts==b->native_attempts && a->pure_attempts==b->pure_attempts &&
 a->cpu_nanoseconds==b->cpu_nanoseconds;
}
static int geometry(const struct slcli_initial_preclaim_request *r,
 uint64_t n,uint64_t page,uint64_t ceiling,uint64_t *meta,uint64_t *user,
 uint64_t *mapping,struct slcli_bq_owner_resource_profile *profile) {
 uint64_t i,u=0,m,rem;
 struct slcli_bq_owner_resource_profile accumulated={0},next={0};
 struct slcli_bq_resource_cost backing={0},empty={0};
 if(!n || n>SIZE_MAX/sizeof(*r) || !interval(r,n*sizeof(*r)) ||
 !page || (page&(page-1)) || page>LONG_MAX || !ceiling ||
 n>UINT64_MAX/sizeof(struct slcli_initial_preclaim_row))return 0;
 m=n*sizeof(struct slcli_initial_preclaim_row);
 for(i=0;i<n;i++) {
  if(r[i].ordinal!=i || !r[i].category ||
   (!r[i].user_bytes&&cost_equal(&r[i].effects,&empty)) ||
   r[i].user_bytes>UINT64_MAX-u ||
   !slcli_bq_owner_resource_profile_add(&next,&accumulated,&r[i].effects))return 0;
  u+=r[i].user_bytes;accumulated=next;
 }
 if(u>UINT64_MAX-m)return 0;
 *meta=m;*user=u;m+=u;rem=m%page;
 if(rem && m>UINT64_MAX-(page-rem))return 0;
 if(rem)m+=page-rem;
 if(!m || m>LONG_MAX)return 0;
 /* Exact anonymous backing charge once, separate from complete source effects.
  * Maximum touched user RAM is conservative; this does not assert residence. */
 backing.user_ram_bytes=m;backing.user_virtual_bytes=m;
 backing.native_references=2;backing.physical_extent_events=1;
 backing.native_attempts=SLCLI_INITIAL_PREBIRTH_NATIVE_ATTEMPTS;
 if(!slcli_bq_owner_resource_profile_add(&next,&accumulated,&backing) ||
 !slcli_bq_owner_resource_profile_shape(&next) ||
 next.combined_memory_bytes>ceiling)return 0;
 *mapping=m;*profile=next;return 1;
}
int slcli_initial_preclaim_linux_plan_original(
 struct slcli_initial_preclaim_plan_linux *out,
 const struct slcli_initial_preclaim_request *r,uint64_t n,uint64_t page,uint64_t ceiling) {
 struct slcli_initial_preclaim_plan_linux t={0};
 if(!out || !n || n>SIZE_MAX/sizeof(*r) ||
 !away(out,sizeof(*out),r,n*sizeof(*r)) ||
 !geometry(r,n,page,ceiling,&t.metadata_bytes,&t.user_bytes,&t.mapping_bytes,
 &t.resource_profile))return 0;
 *out=t;return 1;
}
static int sources(const struct slcli_initial_preclaim_linux *c) {
 uint64_t bytes;
 if(!c || !interval(c->launcher_original,sizeof(*c->launcher_original)) ||
 c!=&c->launcher_original->preclaim || !c->count || c->count>SIZE_MAX/sizeof(*c->requests_original))return 0;
 bytes=c->count*sizeof(*c->requests_original);
 if(!away(c,sizeof(*c),c->requests_original,bytes) ||
 !away(c,sizeof(*c),c->origin_original,sizeof(*c->origin_original)) ||
 !away(c,sizeof(*c),c->prior_original,sizeof(*c->prior_original)))return 0;
 if(!slcli_native_clock_linux_matches_original(c->origin_original) ||
 !slcli_native_clock_linux_matches_original(c->prior_original))return 0;
 return c->origin_original->tick==c->origin_tick &&
 c->prior_original->tick==c->prior_tick && c->origin_tick<=c->prior_tick &&
 c->prior_tick<c->expires;
}
static int matches_rows(
 const struct slcli_initial_preclaim_linux *c) {
 uint64_t meta,user,mapping,i,offset;
 struct slcli_bq_owner_resource_profile profile;
 const struct slcli_initial_preclaim_row *rows;
 if(!sources(c) || c->started!=1 || c->attempted!=1 || c->returned!=1 ||
 c->initialized!=1 || c->failed || c->result_original<=0 ||
 !geometry(c->requests_original,c->count,c->page_bytes,c->ceiling_bytes,
 &meta,&user,&mapping,&profile))return 0;
 if(meta!=c->metadata_bytes || user!=c->user_bytes || mapping!=c->mapping_bytes ||
 !cost_equal(&profile.simultaneous,&c->resource_profile.simultaneous) ||
 profile.combined_memory_bytes!=c->resource_profile.combined_memory_bytes ||
 c->number_original!=__NR_mmap || c->args[0] || c->args[1]!=mapping || c->args[2]!=(PROT_READ|PROT_WRITE) ||
 c->args[3]!=(MAP_PRIVATE|MAP_ANONYMOUS) || c->args[4]!=-1 || c->args[5] ||
 !slcli_native_clock_linux_matches_original(&c->before) ||
 !slcli_native_clock_linux_matches_original(&c->after) ||
 c->before.tick<c->prior_tick || c->after.tick<c->before.tick ||
 c->after.tick>=c->expires)return 0;
 rows=(const struct slcli_initial_preclaim_row *)(uintptr_t)c->result_original;
 if(!away(rows,mapping,c->launcher_original,sizeof(*c->launcher_original)) ||
 !away(rows,mapping,c,sizeof(*c)) ||
 !away(rows,mapping,c->requests_original,c->count*sizeof(*c->requests_original)) ||
 !away(rows,mapping,c->origin_original,sizeof(*c->origin_original)) ||
 !away(rows,mapping,c->prior_original,sizeof(*c->prior_original)))return 0;
 offset=meta;
 for(i=0;i<c->count;i++) {
  const struct slcli_initial_preclaim_request *r=&c->requests_original[i];
  if(rows[i].request.ordinal!=r->ordinal || rows[i].request.category!=r->category ||
   rows[i].request.user_bytes!=r->user_bytes ||
   !cost_equal(&rows[i].request.effects,&r->effects) ||
   rows[i].payload_offset!=offset)return 0;
  offset+=r->user_bytes;
 }
 return offset==meta+user;
}
int slcli_initial_preclaim_linux_matches_original(
 const struct slcli_initial_preclaim_linux *c) {
 if(!matches_rows(c))return 0;
 return slcli_native_clock_linux_matches_original(&c->finish) &&
 c->finish.tick>=c->after.tick && c->finish.tick<c->expires;
}
int slcli_initial_preclaim_linux_capture(struct slcli_initial_preclaim_linux *c,
 const struct slcli_initial_launcher_linux *launcher,
 const struct slcli_initial_preclaim_request *r,uint64_t n,uint64_t page,
 uint64_t ceiling,const struct slcli_native_clock *origin,
 const struct slcli_native_clock *prior,uint64_t expires) {
 uint64_t meta,user,mapping,i,offset;
 struct slcli_bq_owner_resource_profile profile;
 struct slcli_initial_preclaim_row *rows;
 long result;
 if(!c || !interval(launcher,sizeof(*launcher)) || c!=&launcher->preclaim ||
 !geometry(r,n,page,ceiling,&meta,&user,&mapping,&profile) ||
 !away(c,sizeof(*c),r,n*sizeof(*r)) ||
 !away(c,sizeof(*c),origin,sizeof(*origin)) ||
 !away(c,sizeof(*c),prior,sizeof(*prior)) || !zeroed(c,sizeof(*c)) ||
 !slcli_native_clock_linux_matches_original(origin) ||
 !slcli_native_clock_linux_matches_original(prior) ||
 origin->tick>prior->tick || prior->tick>=expires)return 0;
 c->launcher_original=launcher;c->requests_original=r;c->count=n;c->page_bytes=page;c->ceiling_bytes=ceiling;
 c->origin_original=origin;c->prior_original=prior;c->expires=expires;
 c->origin_tick=origin->tick;c->prior_tick=prior->tick;
 c->metadata_bytes=meta;c->user_bytes=user;c->mapping_bytes=mapping;
 c->resource_profile=profile;c->started=1;c->failed=1;
 c->args[0]=0;c->args[1]=mapping;c->args[2]=PROT_READ|PROT_WRITE;
 c->args[3]=MAP_PRIVATE|MAP_ANONYMOUS;c->args[4]=-1;c->args[5]=0;
 if(!slcli_native_clock_read(&c->before) ||
 !slcli_native_clock_linux_matches_original(&c->before) ||
 c->before.tick<prior->tick || c->before.tick>=expires)return 0;
 c->number_original=__NR_mmap;c->attempted=1;
 result=slcli_kernel_call(c->number_original,(long)c->args[0],(long)c->args[1],
 c->args[2],c->args[3],c->args[4],c->args[5]);
 c->result_original=result;c->returned=1;
 if(!slcli_native_clock_read(&c->after) ||
 !slcli_native_clock_linux_matches_original(&c->after) ||
 c->after.tick<c->before.tick || c->after.tick>=expires || result<=0)return 0;
 rows=(struct slcli_initial_preclaim_row *)(uintptr_t)result;
 if(!away(rows,mapping,c->launcher_original,sizeof(*c->launcher_original)) ||
 !away(rows,mapping,c,sizeof(*c)) || !away(rows,mapping,r,n*sizeof(*r)) ||
 !away(rows,mapping,origin,sizeof(*origin)) ||
 !away(rows,mapping,prior,sizeof(*prior)))return 0;
 offset=meta;
 for(i=0;i<n;i++) {
  rows[i].request=r[i];rows[i].payload_offset=offset;offset+=r[i].user_bytes;
 }
 c->initialized=1;c->failed=0;
 if(!matches_rows(c)){c->failed=1;return 0;}
 c->failed=1;
 if(!slcli_native_clock_read(&c->finish) ||
 !slcli_native_clock_linux_matches_original(&c->finish) ||
 c->finish.tick<c->after.tick || c->finish.tick>=expires)return 0;
 c->failed=0;
 return 1;
}
int slcli_initial_preclaim_linux_span_original(
 const struct slcli_initial_preclaim_linux *c,uint64_t ordinal,
 uint64_t relative,uint64_t bytes,void **output) {
 const struct slcli_initial_preclaim_row *rows;
 uint64_t offset;
 if(!output || !slcli_initial_preclaim_linux_matches_original(c) ||
 ordinal>=c->count || !bytes ||
 !away(output,sizeof(*output),c->launcher_original,sizeof(*c->launcher_original)) ||
 !away(output,sizeof(*output),c,sizeof(*c)) ||
 !away(output,sizeof(*output),c->requests_original,c->count*sizeof(*c->requests_original)) ||
 !away(output,sizeof(*output),c->origin_original,sizeof(*c->origin_original)) ||
 !away(output,sizeof(*output),c->prior_original,sizeof(*c->prior_original)))return 0;
 rows=(const struct slcli_initial_preclaim_row *)(uintptr_t)c->result_original;
 if(!away(output,sizeof(*output),rows,c->mapping_bytes) ||
 relative>rows[ordinal].request.user_bytes ||
 bytes>rows[ordinal].request.user_bytes-relative ||
 relative>UINT64_MAX-rows[ordinal].payload_offset)return 0;
 offset=rows[ordinal].payload_offset+relative;
 if(offset>c->mapping_bytes || bytes>c->mapping_bytes-offset ||
 (uintptr_t)rows>UINTPTR_MAX-offset)return 0;
 *output=(void *)((uintptr_t)rows+(uintptr_t)offset);
 return 1;
}
#endif
