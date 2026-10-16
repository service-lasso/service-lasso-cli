#include "product_source_windows.h"
#include "../build-owner/bootstrap_windows.h"
#if defined(_WIN32)
/* SYNTHETIC EXCLUSION GEOMETRY ONLY: zero clocks/profiles/regions deliberately
 * cannot satisfy the real native storage/source/principal/finance constructor. */
int slcli_product_source_windows_exclusion_vectors(void) {
 struct slcli_product_storage_windows_source s={0};
 struct slcli_bq_provider_expectations e={0};
 struct slcli_bq_expected_subject subject={0};
 struct slcli_bq_expected_principal effective={0},primary={0};
 struct slcli_native_clock origin={0},prior={0};
 struct slcli_bq_owner_resource_profile profile={0};
 struct slcli_build_initial_windows_region region={0};
 struct slcli_product_initial_windows_source_span graph={0};
 uint32_t requirements[1]={1};unsigned char sid0[8]={0},sid1[8]={0},sid2[8]={0};
 unsigned char role[4]={0},symbol[4]={0},data[4]={0},actor[4]={0},occurrence[4]={0},out[4]={0};
 e.principal.platform=effective.platform=primary.platform=SLCLI_BQ_EXPECTED_WINDOWS;
 e.principal.windows_sid=sid0;e.principal.windows_sid_bytes=8;
 effective.windows_sid=sid1;effective.windows_sid_bytes=8;
 primary.windows_sid=sid2;primary.windows_sid_bytes=8;
 e.command_count=1;e.requirements_per_command=requirements;e.subject_count=1;e.subjects=&subject;
 subject.reader_role=role;subject.reader_role_bytes=4;
 e.producer.entry_symbol=symbol;e.producer.entry_symbol_bytes=4;
 graph.original=data;graph.bytes=4;graph.actor_original=actor;graph.actor_bytes=4;
 graph.occurrence_original=occurrence;graph.occurrence_bytes=4;
 s.initial_origin_original=&origin;s.initial_prior_original=&prior;s.region_count=1;s.regions_original=&region;
 s.profile_original=&profile;s.initial_source_original=&e;s.effective_original=&effective;s.primary_original=&primary;
 s.source_graph_count=1;s.source_graph_original=&graph;
#define CHECK(x) do {if(!(x))return __LINE__;}while(0)
#define DENY(p,n) CHECK(!slcli_product_windows_initial_source_excludes_output(&s,(p),(n)))
 CHECK(slcli_product_windows_initial_source_excludes_output(&s,out,sizeof(out)));
 DENY(&s,sizeof(s));DENY(&origin,sizeof(origin));DENY(&prior,sizeof(prior));
 DENY(&region,sizeof(region));DENY(&profile,sizeof(profile));DENY(&e,sizeof(e));
 DENY(requirements,sizeof(requirements));DENY(&subject,sizeof(subject));DENY(role,sizeof(role));
 DENY(symbol,sizeof(symbol));DENY(sid0,sizeof(sid0));DENY(&effective,sizeof(effective));
 DENY(&primary,sizeof(primary));DENY(sid1,sizeof(sid1));DENY(sid2,sizeof(sid2));
 DENY(&graph,sizeof(graph));DENY(data,sizeof(data));DENY(actor,sizeof(actor));DENY(occurrence,sizeof(occurrence));
 CHECK(!slcli_product_windows_initial_source_excludes_output(0,out,sizeof(out)));
 CHECK(!slcli_product_windows_initial_source_excludes_output(&s,0,sizeof(out)));
 CHECK(!slcli_product_windows_initial_source_excludes_output(&s,out,0));
 s.region_count=SIZE_MAX;DENY(out,sizeof(out));s.region_count=1;
 e.command_count=SIZE_MAX;DENY(out,sizeof(out));e.command_count=1;
 e.subject_count=SIZE_MAX;DENY(out,sizeof(out));e.subject_count=1;
 s.source_graph_count=SIZE_MAX;DENY(out,sizeof(out));s.source_graph_count=1;
 graph.actor_bytes=0;DENY(out,sizeof(out));graph.actor_bytes=4;
 CHECK(slcli_product_windows_initial_source_excludes_output(&s,out,sizeof(out)));
#undef DENY
#undef CHECK
 return 0;
}
#endif
