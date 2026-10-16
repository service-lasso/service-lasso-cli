#include "bootstrap_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);
}
static int zero(const void *p,size_t n){const unsigned char *s=p;size_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;}
static int principal_away(const struct slcli_bq_expected_principal *p,const void *out,size_t n) {
 return p&&p->platform==SLCLI_BQ_EXPECTED_WINDOWS&&p->windows_sid&&p->windows_sid_bytes&&
 p->windows_sid_bytes<=SECURITY_MAX_SID_SIZE&&apart(out,n,p,sizeof(*p))&&
 apart(out,n,p->windows_sid,p->windows_sid_bytes);
}
static int source_away(const struct slcli_build_initial_windows_source *s,const void *out,size_t n) {
 const struct slcli_build_windows_member *m=s->compiler_original;size_t i;
 if(!apart(out,n,s,sizeof(*s))||!apart(out,n,s->origin_original,sizeof(*s->origin_original))||
 !apart(out,n,s->prior_original,sizeof(*s->prior_original))||
 !apart(out,n,s->regions_original,s->region_count*sizeof(*s->regions_original))||
 !apart(out,n,s->profile_original,sizeof(*s->profile_original))||
 !principal_away(s->effective_original,out,n)||!principal_away(s->primary_original,out,n)||
 !apart(out,n,m,sizeof(*m))||!apart(out,n,m->leaf.original,m->leaf.units*sizeof(WCHAR))||
 !apart(out,n,m->ancestors_original,m->ancestor_count*sizeof(*m->ancestors_original)))return 0;
 for(i=0;i<m->ancestor_count;i++)if(!apart(out,n,m->ancestors_original[i].original,
 m->ancestors_original[i].units*sizeof(WCHAR)))return 0;return 1;
}
static int inside_prefix(const struct slcli_build_initial_windows_source *s,const void *out,size_t n) {
 uintptr_t p=(uintptr_t)out;size_t i;
 if(!out||!n||p>UINTPTR_MAX-n)return 0;
 for(i=0;i<s->region_count;i++) {
 const struct slcli_build_initial_windows_region *r=&s->regions_original[i];
 DWORD protection=r->protection&0xff;
 if(r->state==MEM_COMMIT&&!(r->protection&PAGE_GUARD)&&
 (protection==PAGE_READWRITE||protection==PAGE_WRITECOPY||
 protection==PAGE_EXECUTE_READWRITE||protection==PAGE_EXECUTE_WRITECOPY)&&
 p>=r->base&&p+n<=r->base+r->bytes)return 1;
 }return 0;
}
static int begin(struct slcli_build_bootstrap_windows *c,struct slcli_build_windows_call *r) {
 return slcli_native_clock_read(&r->before)&&r->before.tick>=c->prior_tick&&
 r->before.tick<c->source_original->expires;
}
static int end(struct slcli_build_bootstrap_windows *c,struct slcli_build_windows_call *r) {
 if(!slcli_native_clock_read(&r->after)||r->after.tick<r->before.tick||
 r->after.tick>=c->source_original->expires)return 0;c->prior_tick=r->after.tick;return 1;
}
static int checked_add(size_t *total,size_t value) {
 if(!value||value>SIZE_MAX-*total)return 0;*total+=value;return 1;
}
static int deny(struct slcli_build_bootstrap_windows *c) { c->failed=1;c->memory_original.failed=1;return 0; }
int slcli_build_bootstrap_windows_acquire_compiler_original(
 struct slcli_build_bootstrap_windows *c,const struct slcli_build_initial_windows_source *s,
 struct slcli_build_initial_windows_region_capture *rows) {
 size_t i,j,row_bytes,total=0;uint64_t prefix=0;struct slcli_memory_owner *o;
 const struct slcli_build_windows_member *m;const struct slcli_native_clock *prior;
 if(!c||!s||!rows||!s->regions_original||!s->region_count||s->region_count>=256||
 s->region_count>SIZE_MAX/sizeof(*rows)||!s->profile_original||
 !s->compiler_original||!s->effective_original||!s->primary_original||
 !slcli_native_clock_windows_matches_original(s->origin_original)||
 !slcli_native_clock_windows_matches_original(s->prior_original)||
 s->prior_original->tick<s->origin_original->tick||s->expires<=s->prior_original->tick||
 !s->owner_limit||!s->arena_bytes||s->arena_bytes>SIZE_MAX||
 !s->profile_original->combined_memory_bytes||
 s->profile_original->simultaneous.user_virtual_bytes<s->owner_limit||
 zero(s->birth,32)||zero(s->image,32)||zero(s->owner,32)||zero(s->source,32))return 0;
 m=s->compiler_original;row_bytes=s->region_count*sizeof(*rows);
 if(!m->bytes||m->bytes>SIZE_MAX||!m->leaf.original||!m->leaf.units||m->leaf.units>SIZE_MAX/sizeof(WCHAR)||
 !m->ancestors_original||!m->ancestor_count||m->ancestor_count>SIZE_MAX/sizeof(struct slcli_build_windows_open)||
 m->ancestor_count>SIZE_MAX/sizeof(*m->ancestors_original)||!m->read_capacity||
 m->read_capacity>SIZE_MAX/sizeof(struct slcli_windows_file_capture))return 0;
 for(i=0;i<m->ancestor_count;i++)if(!m->ancestors_original[i].original||!m->ancestors_original[i].units||
 m->ancestors_original[i].units>SIZE_MAX/sizeof(WCHAR))return 0;
 for(i=0;i<s->region_count;i++) {
 const struct slcli_build_initial_windows_region *r=&s->regions_original[i];
 if(!r->base||!r->bytes||r->bytes>UINTPTR_MAX-r->base||r->category>SLCLI_IO||
 (r->state!=MEM_COMMIT&&r->state!=MEM_RESERVE)||r->bytes>UINT64_MAX-prefix)return 0;
 prefix+=r->bytes;
 for(j=0;j<i;j++)if(!apart((void *)r->base,(size_t)r->bytes,(void *)s->regions_original[j].base,
 (size_t)s->regions_original[j].bytes))return 0;
 }
 if(prefix>s->owner_limit||s->arena_bytes>s->owner_limit-prefix||
 !source_away(s,c,sizeof(*c))||!source_away(s,rows,row_bytes)||!apart(c,sizeof(*c),rows,row_bytes)||
 !inside_prefix(s,c,sizeof(*c))||!inside_prefix(s,rows,row_bytes)||
 !zero(c,sizeof(*c))||!zero(rows,row_bytes))return 0;
 if(!checked_add(&total,slcli_arena_allocation_bytes(sizeof(struct slcli_build_principal_windows)))||
 !checked_add(&total,slcli_arena_allocation_bytes(sizeof(struct slcli_build_principal_windows)))||
 !checked_add(&total,slcli_arena_allocation_bytes(sizeof(struct slcli_build_held_file_windows)))||
 !checked_add(&total,slcli_arena_allocation_bytes(m->ancestor_count*sizeof(struct slcli_build_windows_open)))||
 !checked_add(&total,slcli_arena_allocation_bytes(m->read_capacity*sizeof(struct slcli_windows_file_capture)))||
 !checked_add(&total,slcli_arena_allocation_bytes((size_t)m->bytes))||total>s->arena_bytes)return 0;
 c->source_original=s;c->regions_original=rows;c->started=1;c->failed=1;c->prior_tick=s->prior_original->tick;
 if(!begin(c,&c->pages))return deny(c);c->pages.args[0]=(uintptr_t)&c->pages_original;
 c->pages.called=1;GetSystemInfo(&c->pages_original);c->pages.returned=1;
 if(!end(c,&c->pages)||!c->pages_original.dwPageSize||!c->pages_original.dwAllocationGranularity||
 c->pages_original.dwAllocationGranularity%c->pages_original.dwPageSize)return deny(c);
 o=&c->memory_original;o->limit=s->owner_limit;o->next_sequence=1;
 o->native_page_size=c->pages_original.dwPageSize;
 o->native_page_geometry[0]=c->pages_original.dwPageSize;o->native_page_geometry[1]=c->pages_original.dwAllocationGranularity;
 for(i=0;i<32;i++){o->original_birth[i]=s->birth[i];o->original_image[i]=s->image[i];
 o->original_owner[i]=s->owner[i];o->original_source[i]=s->source[i];}
 for(i=0;i<s->region_count;i++) {
 const struct slcli_build_initial_windows_region *r=&s->regions_original[i];
 struct slcli_build_initial_windows_region_capture *v=&rows[i];MEMORY_BASIC_INFORMATION *g=&v->geometry_original;
 if(r->base%o->native_page_size||r->bytes%o->native_page_size||!begin(c,&v->query))return deny(c);
 v->query.args[0]=r->base;v->query.args[1]=(uintptr_t)g;v->query.args[2]=sizeof(*g);
 v->query.called=1;v->query.raw=VirtualQuery((void *)r->base,g,sizeof(*g));v->query.returned=1;
 if(!v->query.raw){v->query.error=GetLastError();v->query.error_observed=1;}
 if(!end(c,&v->query)||v->query.raw!=sizeof(*g)||g->BaseAddress!=(void *)r->base||
 g->RegionSize!=r->bytes||g->State!=r->state||g->Type!=r->type||g->Protect!=r->protection)return deny(c);
 /* Adopt observed initial backing into its SAME ledger; this is not a new
  * OS allocation, source authority, or a second independent owner allowance. */
 if(!slcli_memory_prepare(o,r->category,r->bytes,&v->request_original)||
 !slcli_memory_partial_acquired(o,&v->request_original,r->base)||
 !slcli_memory_acquired(o,&v->request_original,r->base,r->bytes,r->state==MEM_COMMIT?r->bytes:0,1))return deny(c);
 }
 if(!slcli_native_clock_read(&c->arena_before)||c->arena_before.tick<c->prior_tick||c->arena_before.tick>=s->expires)return deny(c);
 if(!slcli_platform_acquire(o,SLCLI_RUNTIME,s->arena_bytes,&c->arena_acquisition_original)||
 c->arena_acquisition_original.tick<c->prior_tick||c->arena_acquisition_original.tick>=s->expires||
 !slcli_native_clock_read(&c->arena_after)||c->arena_after.tick<c->arena_acquisition_original.tick||
 c->arena_after.tick<c->arena_before.tick||c->arena_after.tick>=s->expires||
 !slcli_arena_init(&c->arena_original,o,c->arena_acquisition_original.request.index))return deny(c);
 c->prior_tick=c->arena_after.tick;
 c->effective_original=slcli_arena_calloc(&c->arena_original,1,sizeof(*c->effective_original));
 c->primary_original=slcli_arena_calloc(&c->arena_original,1,sizeof(*c->primary_original));
 c->compiler_original=slcli_arena_calloc(&c->arena_original,1,sizeof(*c->compiler_original));
 c->compiler_ancestors_original=slcli_arena_calloc(&c->arena_original,m->ancestor_count,sizeof(*c->compiler_ancestors_original));
 c->compiler_reads_original=slcli_arena_calloc(&c->arena_original,m->read_capacity,sizeof(*c->compiler_reads_original));
 c->compiler_body_original=slcli_arena_calloc(&c->arena_original,(size_t)m->bytes,1);
 if(!c->effective_original||!c->primary_original||!c->compiler_original||!c->compiler_ancestors_original||
 !c->compiler_reads_original||!c->compiler_body_original||!slcli_native_clock_read(&c->finish)||
 c->finish.tick<c->prior_tick||c->finish.tick>=s->expires)return deny(c);
 prior=&c->finish;
 if(!slcli_build_principal_windows_capture_original(c->effective_original,s->effective_original,s->origin_original,prior,s->expires)||
 !slcli_build_process_primary_windows_capture_original(c->primary_original,c->effective_original,s->primary_original)||
 !slcli_build_principal_windows_issuing_relation_original(c->effective_original,c->primary_original)||
 !slcli_build_held_file_windows_capture_original(c->compiler_original,c->effective_original,m,
 c->compiler_ancestors_original,c->compiler_reads_original,c->compiler_body_original))return deny(c);
 c->prior_tick=c->compiler_original->finish.tick;c->completed=1;c->failed=0;return 1;
}
#endif
