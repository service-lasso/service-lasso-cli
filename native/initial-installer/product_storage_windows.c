#include "product_storage_windows.h"
#if defined(_WIN32)
/* Region adoption follows the existing bootstrap native-custody pattern; this
 * distinct product preparation never invokes that build/compiler factory. */
static int apart(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);
}
static int zero(const void *p,size_t n){const unsigned char *s=p;size_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;}
static int inside_prefix(const struct slcli_product_storage_windows_source *s,const void *out,size_t n) {
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
static int principal_away(const struct slcli_bq_expected_principal *p,const void *out,size_t n) {
 return p&&p->platform==SLCLI_BQ_EXPECTED_WINDOWS&&p->windows_sid&&p->windows_sid_bytes&&
 p->windows_sid_bytes<=SECURITY_MAX_SID_SIZE&&apart(out,n,p,sizeof(*p))&&
 apart(out,n,p->windows_sid,p->windows_sid_bytes);
}
static int source_away(const struct slcli_product_storage_windows_source *s,const void *out,size_t n) {
 size_t i;const struct slcli_bq_provider_expectations *e=s->initial_source_original;
 if(!e||!e->command_count||e->command_count>SIZE_MAX/sizeof(*e->requirements_per_command)||
 !e->subject_count||e->subject_count>SIZE_MAX/sizeof(*e->subjects)||
 !s->source_graph_original||!s->source_graph_count||
 s->source_graph_count>SIZE_MAX/sizeof(*s->source_graph_original)||
 !apart(out,n,e->requirements_per_command,e->command_count*sizeof(*e->requirements_per_command))||
 !apart(out,n,e->subjects,e->subject_count*sizeof(*e->subjects))||
 !principal_away(&e->principal,out,n)||
 (e->producer.entry_symbol_bytes&&!apart(out,n,e->producer.entry_symbol,e->producer.entry_symbol_bytes))||
 !apart(out,n,s->source_graph_original,s->source_graph_count*sizeof(*s->source_graph_original)))return 0;
 for(i=0;i<e->subject_count;i++)if(e->subjects[i].reader_role_bytes&&
 !apart(out,n,e->subjects[i].reader_role,e->subjects[i].reader_role_bytes))return 0;
 for(i=0;i<s->source_graph_count;i++) {
 const struct slcli_product_initial_windows_source_span *v=&s->source_graph_original[i];
 if(!apart(out,n,v->original,v->bytes)||!apart(out,n,v->actor_original,v->actor_bytes)||
 !apart(out,n,v->occurrence_original,v->occurrence_bytes))return 0;
 }
 return apart(out,n,s,sizeof(*s))&&
 apart(out,n,s->initial_origin_original,sizeof(*s->initial_origin_original))&&
 apart(out,n,s->initial_prior_original,sizeof(*s->initial_prior_original))&&
 apart(out,n,s->regions_original,s->region_count*sizeof(*s->regions_original))&&
 apart(out,n,s->profile_original,sizeof(*s->profile_original))&&
 apart(out,n,s->initial_source_original,sizeof(*s->initial_source_original))&&
 principal_away(s->effective_original,out,n)&&principal_away(s->primary_original,out,n);
}
static int begin(struct slcli_product_storage_windows *c,struct slcli_build_windows_call *r) {
 return slcli_native_clock_read(&r->before)&&r->before.tick>=c->prior_tick&&
 r->before.tick<c->source_original->initial_expires;
}
static int end(struct slcli_product_storage_windows *c,struct slcli_build_windows_call *r) {
 if(!slcli_native_clock_read(&r->after)||r->after.tick<r->before.tick||
 r->after.tick>=c->source_original->initial_expires)return 0;c->prior_tick=r->after.tick;return 1;
}
static int add(size_t *n,size_t v){if(!v||v>SIZE_MAX-*n)return 0;*n+=v;return 1;}
static int stop(struct slcli_product_storage_windows *c){c->failed=1;c->memory_original.failed=1;return 0;}
int slcli_product_storage_windows_prepare_original(struct slcli_product_storage_windows *c,
 const struct slcli_product_storage_windows_source *s,struct slcli_build_initial_windows_region_capture *rows) {
 size_t i,j,total=0,row_bytes;uint64_t prefix=0;struct slcli_memory_owner *o;
 if(!c||!s||!rows||!s->regions_original||!s->region_count||s->region_count>=256||
 s->region_count>SIZE_MAX/sizeof(*rows)||!s->profile_original||!s->initial_source_original||
 !s->effective_original||!s->primary_original||
 !slcli_native_clock_windows_matches_original(s->initial_origin_original)||
 !slcli_native_clock_windows_matches_original(s->initial_prior_original)||
 s->initial_prior_original->tick<s->initial_origin_original->tick||
 s->initial_expires<=s->initial_prior_original->tick||
 s->operation_expires<=s->initial_prior_original->tick||s->operation_expires>s->initial_expires||
 !s->owner_limit||!s->arena_bytes||s->arena_bytes>SIZE_MAX||
 !s->profile_original->combined_memory_bytes||
 s->profile_original->simultaneous.user_virtual_bytes<s->owner_limit||
 zero(s->birth,32)||zero(s->image,32)||zero(s->owner,32)||zero(s->source,32))return 0;
 row_bytes=s->region_count*sizeof(*rows);
 for(i=0;i<s->region_count;i++) {
 const struct slcli_build_initial_windows_region *r=&s->regions_original[i];
 if(!r->base||!r->bytes||r->bytes>SIZE_MAX||r->bytes>UINTPTR_MAX-r->base||
 r->category>SLCLI_IO||(r->state!=MEM_COMMIT&&r->state!=MEM_RESERVE)||r->bytes>UINT64_MAX-prefix)return 0;
 prefix+=r->bytes;
 for(j=0;j<i;j++)if(!apart((void *)r->base,(size_t)r->bytes,
 (void *)s->regions_original[j].base,(size_t)s->regions_original[j].bytes))return 0;
 }
 if(prefix>s->owner_limit||s->arena_bytes>s->owner_limit-prefix||
 !source_away(s,c,sizeof(*c))||!source_away(s,rows,row_bytes)||!apart(c,sizeof(*c),rows,row_bytes)||
 !inside_prefix(s,c,sizeof(*c))||!inside_prefix(s,rows,row_bytes)||
 !zero(c,sizeof(*c))||!zero(rows,row_bytes))return 0;
 if(!add(&total,slcli_arena_allocation_bytes(sizeof(*c->caller_original)))||
 !add(&total,slcli_arena_allocation_bytes(sizeof(*c->operation_origin_original)))||
 !add(&total,slcli_arena_allocation_bytes(sizeof(*c->deadline_original)))||
 !add(&total,slcli_arena_allocation_bytes(sizeof(*c->transport_original)))||
 !add(&total,slcli_arena_allocation_bytes(sizeof(*c->effective_original)))||
 !add(&total,slcli_arena_allocation_bytes(sizeof(*c->primary_original)))||total>s->arena_bytes)return 0;
 c->source_original=s;c->regions_original=rows;c->started=1;c->failed=1;
 c->prior_tick=s->initial_prior_original->tick;
 if(!begin(c,&c->pages))return stop(c);c->pages.args[0]=(uintptr_t)&c->pages_original;
 c->pages.called=1;GetSystemInfo(&c->pages_original);c->pages.returned=1;
 if(!end(c,&c->pages)||!c->pages_original.dwPageSize||!c->pages_original.dwAllocationGranularity||
 c->pages_original.dwAllocationGranularity%c->pages_original.dwPageSize)return stop(c);
 o=&c->memory_original;o->limit=s->owner_limit;o->next_sequence=1;
 o->native_page_size=c->pages_original.dwPageSize;
 o->native_page_geometry[0]=c->pages_original.dwPageSize;o->native_page_geometry[1]=c->pages_original.dwAllocationGranularity;
 for(i=0;i<32;i++){o->original_birth[i]=s->birth[i];o->original_image[i]=s->image[i];
 o->original_owner[i]=s->owner[i];o->original_source[i]=s->source[i];}
 for(i=0;i<s->region_count;i++) {
 const struct slcli_build_initial_windows_region *r=&s->regions_original[i];
 struct slcli_build_initial_windows_region_capture *v=&rows[i];MEMORY_BASIC_INFORMATION *g=&v->geometry_original;
 if(r->base%o->native_page_size||r->bytes%o->native_page_size||!begin(c,&v->query))return stop(c);
 v->query.args[0]=r->base;v->query.args[1]=(uintptr_t)g;v->query.args[2]=sizeof(*g);
 v->query.called=1;v->query.raw=VirtualQuery((void *)r->base,g,sizeof(*g));v->query.returned=1;
 if(!v->query.raw){v->query.error=GetLastError();v->query.error_observed=1;}
 if(!end(c,&v->query)||v->query.raw!=sizeof(*g)||g->BaseAddress!=(void *)r->base||
 g->RegionSize!=r->bytes||g->State!=r->state||g->Type!=r->type||g->Protect!=r->protection)return stop(c);
 if(!slcli_memory_prepare(o,r->category,r->bytes,&v->request_original)||
 !slcli_memory_partial_acquired(o,&v->request_original,r->base)||
 !slcli_memory_acquired(o,&v->request_original,r->base,r->bytes,r->state==MEM_COMMIT?r->bytes:0,1))return stop(c);
 }
 if(!slcli_native_clock_read(&c->arena_before)||c->arena_before.tick<c->prior_tick||
 c->arena_before.tick>=s->initial_expires)return stop(c);
 if(!slcli_platform_acquire(o,SLCLI_RUNTIME,s->arena_bytes,&c->arena_acquisition_original)||
 c->arena_acquisition_original.tick<c->arena_before.tick||c->arena_acquisition_original.tick>=s->initial_expires||
 !slcli_native_clock_read(&c->arena_after)||c->arena_after.tick<c->arena_acquisition_original.tick||
 c->arena_after.tick>=s->initial_expires||
 !slcli_arena_init(&c->arena_original,o,c->arena_acquisition_original.request.index))return stop(c);
 c->prior_tick=c->arena_after.tick;
 c->caller_original=slcli_arena_calloc(&c->arena_original,1,sizeof(*c->caller_original));
 c->operation_origin_original=slcli_arena_calloc(&c->arena_original,1,sizeof(*c->operation_origin_original));
 c->deadline_original=slcli_arena_calloc(&c->arena_original,1,sizeof(*c->deadline_original));
 c->transport_original=slcli_arena_calloc(&c->arena_original,1,sizeof(*c->transport_original));
 c->effective_original=slcli_arena_calloc(&c->arena_original,1,sizeof(*c->effective_original));
 c->primary_original=slcli_arena_calloc(&c->arena_original,1,sizeof(*c->primary_original));
 if(!c->caller_original||!c->operation_origin_original||!c->deadline_original||
 !c->transport_original||!c->effective_original||!c->primary_original||
 !slcli_native_clock_read(&c->finish)||c->finish.tick<c->prior_tick||c->finish.tick>=s->initial_expires)return stop(c);
 if(!slcli_build_principal_windows_capture_original(c->effective_original,s->effective_original,
 s->initial_origin_original,&c->finish,s->initial_expires)||
 !slcli_build_process_primary_windows_capture_original(c->primary_original,c->effective_original,s->primary_original)||
 !slcli_build_principal_windows_issuing_relation_original(c->effective_original,c->primary_original))return stop(c);
 c->prior_tick=c->primary_original->finish.tick;
 /* FIRST construction, before a product origin/deadline/caller is published.
  * No initial clock is copied. This final cell is the actual native output. */
 c->origin_attempted=1;
 c->origin_returned=slcli_native_clock_read(c->operation_origin_original);
 if(!c->origin_returned||c->operation_origin_original->tick<c->prior_tick||
 c->operation_origin_original->tick>=s->operation_expires)return stop(c);
 c->caller_original->initial_source_original=s;c->caller_original->memory_original=o;
 c->caller_original->arena_original=&c->arena_original;c->caller_original->deadline_original=c->deadline_original;
 c->deadline_original->origin=c->operation_origin_original;
 c->deadline_original->caller_original=c->caller_original;c->deadline_original->expires=s->operation_expires;
 if(!slcli_source_deadline_original_matches(&c->arena_original,c->deadline_original,c->caller_original)||
 !slcli_arena_live_block(&c->arena_original,c->caller_original,sizeof(*c->caller_original))||
 !slcli_arena_live_block(&c->arena_original,c->transport_original,sizeof(*c->transport_original)))return stop(c);
 c->prior_tick=c->operation_origin_original->tick;c->storage_constructed=1;c->failed=0;return 1;
}
#endif
