#include "product_source_windows.h"
#include "../build-owner/bootstrap_windows.h"
#if defined(_WIN32)
/* Pure complete known-type exclusions only; no native imports or issuer. */
static int apart(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);
}
static int principal_away(const struct slcli_bq_expected_principal *p,const void *out,size_t n) {
 return p&&p->platform==SLCLI_BQ_EXPECTED_WINDOWS&&p->windows_sid&&p->windows_sid_bytes&&
 p->windows_sid_bytes<=SECURITY_MAX_SID_SIZE&&apart(out,n,p,sizeof(*p))&&
 apart(out,n,p->windows_sid,p->windows_sid_bytes);
}
int slcli_product_windows_initial_source_excludes_output(const struct slcli_product_storage_windows_source *s,const void *out,size_t n) {
 size_t i;const struct slcli_bq_provider_expectations *e;
 if(!s||!out||!n||!s->region_count||
 s->region_count>SIZE_MAX/sizeof(*s->regions_original))return 0;
 e=s->initial_source_original;
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
#endif
