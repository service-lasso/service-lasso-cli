#include "ha2_large_members.h"
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int text(const unsigned char *raw,size_t bytes,struct slcli_json_span s,const char *v) {
  size_t at=s.start,i=0;
  if(s.end>bytes||s.start>=s.end||raw[at++]!='"')return 0;
  while(v[i])if(at>=s.end||raw[at++]!=(unsigned char)v[i++])return 0;
  return at+1==s.end&&raw[at]=='"';
}
static int array(const unsigned char *raw,size_t bytes,struct slcli_json_span s) {
  return s.start<s.end&&s.end<=bytes&&raw[s.start]=='['&&raw[s.end-1]==']';
}
static int empty(const unsigned char *raw,size_t bytes,struct slcli_json_span s) {
  return array(raw,bytes,s)&&s.end-s.start==2;
}
int slcli_ha2_closure_root_parse(const unsigned char *raw,size_t bytes,
  const struct slcli_json_ha2_plan *plan,struct slcli_ha2_closure_root *out) {
  static const char *const keys[]={"schema","version","sourceUnit","entrySet","compilerApi","regions","unresolved"};
  struct slcli_json_span root,f[7];struct slcli_ha2_closure_root r={0};uint64_t version;
  if(!out||!plan||!away(out,sizeof(*out),raw,bytes)||!away(out,sizeof(*out),plan,sizeof(*plan))||
     !slcli_json_ha2_validate(raw,bytes,524288,plan,&root)||
     !slcli_json_ha2_object(raw,bytes,root,keys,7,plan,f)||
     !slcli_json_canonical_ascii_object(raw,bytes,root,keys,f,7,1)||
     !text(raw,bytes,f[0],"service-lasso.ha2-source-closure.v1")||
     !slcli_json_unsigned(raw,bytes,f[1],1,0,&version)||version!=1||
     !slcli_ha2_ref_parse(raw,bytes,f[2],&r.source_unit)||r.source_unit.slot!=12||
     !slcli_ha2_ref_parse(raw,bytes,f[4],&r.compiler_api)||
     !array(raw,bytes,f[3])||!array(raw,bytes,f[5])||
     empty(raw,bytes,f[3])||empty(raw,bytes,f[5])||!empty(raw,bytes,f[6]))return 0;
  r.raw=raw;r.bytes=bytes;r.entry_set=f[3];r.regions=f[5];*out=r;return 1;
}
int slcli_ha2_envelope_root_parse(const unsigned char *raw,size_t bytes,
  const struct slcli_json_ha2_plan *plan,struct slcli_ha2_envelope_root *out) {
  static const char *const keys[]={"schema","version","phases","domainTotals","unknowns"};
  struct slcli_json_span root,f[5];struct slcli_ha2_envelope_root r={0};uint64_t version;
  if(!out||!plan||!away(out,sizeof(*out),raw,bytes)||!away(out,sizeof(*out),plan,sizeof(*plan))||
     !slcli_json_ha2_validate(raw,bytes,1048576,plan,&root)||
     !slcli_json_ha2_object(raw,bytes,root,keys,5,plan,f)||
     !slcli_json_canonical_ascii_object(raw,bytes,root,keys,f,5,1)||
     !text(raw,bytes,f[0],"service-lasso.ha2-conservative-envelope.v1")||
     !slcli_json_unsigned(raw,bytes,f[1],1,0,&version)||version!=1||
     !array(raw,bytes,f[2])||!array(raw,bytes,f[3])||
     empty(raw,bytes,f[2])||empty(raw,bytes,f[3])||!empty(raw,bytes,f[4]))return 0;
  r.raw=raw;r.bytes=bytes;r.phases=f[2];r.domain_totals=f[3];*out=r;return 1;
}

static int refs(const unsigned char *raw,size_t bytes,struct slcli_json_span s,
  const struct slcli_json_ha2_plan *plan,int require_some) {
  struct slcli_json_ha2_array_cursor c={0};struct slcli_json_span item;
  struct slcli_ha2_ref ref;int done;
  do {
    if(!slcli_json_ha2_array_next(raw,bytes,s,plan,&c,&item,&done))return 0;
    if(!done&&!slcli_ha2_ref_parse(raw,bytes,item,&ref))return 0;
  } while(!done);
  return !require_some||c.count;
}
static int domains(const unsigned char *raw,size_t bytes,struct slcli_json_span s,
  const struct slcli_json_ha2_plan *plan) {
  struct slcli_json_ha2_array_cursor c={0};struct slcli_json_span item;
  uint64_t d;uint32_t seen=0;int done;
  do {
    if(!slcli_json_ha2_array_next(raw,bytes,s,plan,&c,&item,&done))return 0;
    if(!done) {
      if(!slcli_json_unsigned(raw,bytes,item,4,1,&d)||d==3||
         (seen&(1u<<(unsigned)d)))return 0;
      seen|=1u<<(unsigned)d;
    }
  } while(!done);
  return c.count>0;
}
static int edges(const unsigned char *raw,size_t bytes,struct slcli_json_span s,
  const struct slcli_json_ha2_plan *plan) {
  static const char *const keys[]={"from","to","kind","sourceRef"};
  struct slcli_json_ha2_array_cursor c={0};struct slcli_json_span item,f[4];
  struct slcli_ha2_ref ref;int done;
  do {
    if(!slcli_json_ha2_array_next(raw,bytes,s,plan,&c,&item,&done))return 0;
    if(!done&&(!slcli_json_ha2_object(raw,bytes,item,keys,4,plan,f)||
       !slcli_json_canonical_ascii_object(raw,bytes,item,keys,f,4,0)||
       (!text(raw,bytes,f[2],"direct")&&!text(raw,bytes,f[2],"indirect")&&
        !text(raw,bytes,f[2],"native")&&!text(raw,bytes,f[2],"compiler-helper"))||
       !slcli_ha2_ref_parse(raw,bytes,f[3],&ref)))return 0;
  } while(!done);
  return 1;
}
static int allocations(const unsigned char *raw,size_t bytes,struct slcli_json_span s,
  const struct slcli_json_ha2_plan *plan) {
  static const char *const keys[]={"term","geometryRef","ownerDomain","startCut","endCut","failureEndCut","evidenceRefs"};
  struct slcli_json_ha2_array_cursor c={0};struct slcli_json_span item,f[7];
  struct slcli_ha2_ref ref;uint64_t d;int done;
  do {
    if(!slcli_json_ha2_array_next(raw,bytes,s,plan,&c,&item,&done))return 0;
    if(!done&&(!slcli_json_ha2_object(raw,bytes,item,keys,7,plan,f)||
       !slcli_json_canonical_ascii_object(raw,bytes,item,keys,f,7,0)||
       !slcli_ha2_ref_parse(raw,bytes,f[1],&ref)||
       !slcli_json_unsigned(raw,bytes,f[2],4,1,&d)||d==3||
       !refs(raw,bytes,f[6],plan,0)))return 0;
  } while(!done);
  return 1;
}
int slcli_ha2_region_parse(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
  const struct slcli_json_ha2_plan *plan,uint32_t ordinal,struct slcli_ha2_region_view *out) {
  static const char *const keys[]={"id","sourceRefs","callEdges","ownerDomains","valueClass",
    "inputBounds","branchCases","allocationTerms","nativeRights","lifetimeCuts","evidenceRefs"};
  struct slcli_ha2_region_view r;char id[13]="SOURCE000000";uint32_t value;size_t i;
  if(!out||!plan||!ordinal||ordinal>999999||!away(out,sizeof(*out),raw,bytes)||
     !away(out,sizeof(*out),plan,sizeof(*plan)))return 0;
  value=ordinal;for(i=12;i>6;i--){id[i-1]=(char)('0'+value%10);value/=10;}
  if(!slcli_json_ha2_object(raw,bytes,span,keys,SLCLI_H2R_FIELDS,plan,r.fields)||
     !slcli_json_canonical_ascii_object(raw,bytes,span,keys,r.fields,SLCLI_H2R_FIELDS,0)||
     !text(raw,bytes,r.fields[SLCLI_H2R_ID],id)||
     !refs(raw,bytes,r.fields[SLCLI_H2R_SOURCE_REFS],plan,1)||
     !domains(raw,bytes,r.fields[SLCLI_H2R_DOMAINS],plan)||
     !edges(raw,bytes,r.fields[SLCLI_H2R_CALL_EDGES],plan)||
     !allocations(raw,bytes,r.fields[SLCLI_H2R_ALLOCATIONS],plan)||
     (!text(raw,bytes,r.fields[SLCLI_H2R_VALUE_CLASS],"public")&&
      !text(raw,bytes,r.fields[SLCLI_H2R_VALUE_CLASS],"control")&&
      !text(raw,bytes,r.fields[SLCLI_H2R_VALUE_CLASS],"secret"))||
     !refs(raw,bytes,r.fields[SLCLI_H2R_EVIDENCE_REFS],plan,0))return 0;
  /* Exact closed root fields are retained. Full source-owned inputBounds,
   * branch/native/lifetime shapes and independent coverage remain mandatory;
   * an uninspected borrowed span cannot activate derivation. */
  *out=r;return 1;
}

static int live_terms(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
  const struct slcli_json_ha2_plan *plan) {
  static const char *const keys[]={"term","ownerDomain","reservedBytes","backedBytes","pageBytes",
    "count","geometryRef","startCut","endCut","oldNewGroup","uncertainDisposition"};
  struct slcli_json_ha2_array_cursor c={0};struct slcli_json_span item,f[11];
  struct slcli_ha2_ref ref;uint64_t d,reserved,backed,page,count;int done;
  do {
    if(!slcli_json_ha2_array_next(raw,bytes,span,plan,&c,&item,&done))return 0;
    if(!done&&(!slcli_json_ha2_object(raw,bytes,item,keys,11,plan,f)||
       !slcli_json_canonical_ascii_object(raw,bytes,item,keys,f,11,0)||
       !slcli_json_unsigned(raw,bytes,f[1],4,1,&d)||d==3||
       !slcli_json_unsigned(raw,bytes,f[2],UINT64_MAX,1,&reserved)||
       !slcli_json_unsigned(raw,bytes,f[3],UINT64_MAX,1,&backed)||
       !slcli_json_unsigned(raw,bytes,f[4],UINT64_MAX,1,&page)||
       !slcli_json_unsigned(raw,bytes,f[5],UINT64_MAX,1,&count)||
       !slcli_ha2_ref_parse(raw,bytes,f[6],&ref)))return 0;
    /* Actual geometry/count/page/cuts/old-new/uncertain source meanings and
     * zero-value original observations require independent owning admission. */
  } while(!done);
  return 1;
}
int slcli_ha2_phase_parse(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
  const struct slcli_json_ha2_plan *plan,uint32_t ordinal,struct slcli_ha2_phase_view *out) {
  static const char *const keys[]={"id","event","liveTerms","originalRefs","sourceRefs",
    "captureRefs","ownerTotals","requiredCases"};
  struct slcli_ha2_phase_view r;char id[4]="E00";
  if(!out||!plan||!ordinal||ordinal>16||!away(out,sizeof(*out),raw,bytes)||
     !away(out,sizeof(*out),plan,sizeof(*plan)))return 0;
  id[1]=(char)('0'+ordinal/10);id[2]=(char)('0'+ordinal%10);
  if(!slcli_json_ha2_object(raw,bytes,span,keys,8,plan,r.fields)||
     !slcli_json_canonical_ascii_object(raw,bytes,span,keys,r.fields,8,0)||
     !text(raw,bytes,r.fields[0],id)||!live_terms(raw,bytes,r.fields[2],plan)||
     !refs(raw,bytes,r.fields[3],plan,0)||!refs(raw,bytes,r.fields[4],plan,1)||
     !refs(raw,bytes,r.fields[5],plan,0))return 0;
  *out=r;return 1;
}
int slcli_ha2_domain_total_parse(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
  const struct slcli_json_ha2_plan *plan,struct slcli_ha2_domain_total_view *out) {
  static const char *const keys[]={"domain","maximumBytes","sourceOwnedPeakBytes",
    "categoryMaxima","evidenceRefs","closureRefs"};
  static const char *const category_keys[]={"runtime","stack","io"};
  struct slcli_ha2_domain_total_view r={0};struct slcli_json_span f[3];size_t i;
  if(!out||!plan||!away(out,sizeof(*out),raw,bytes)||!away(out,sizeof(*out),plan,sizeof(*plan))||
     !slcli_json_ha2_object(raw,bytes,span,keys,6,plan,r.fields)||
     !slcli_json_canonical_ascii_object(raw,bytes,span,keys,r.fields,6,0)||
     !slcli_json_unsigned(raw,bytes,r.fields[0],4,1,&r.domain)||r.domain==3||
     !slcli_json_unsigned(raw,bytes,r.fields[1],UINT64_MAX,0,&r.maximum)||
     !slcli_json_unsigned(raw,bytes,r.fields[2],r.maximum,1,&r.source_peak)||
     !slcli_json_ha2_object(raw,bytes,r.fields[3],category_keys,3,plan,f)||
     !slcli_json_canonical_ascii_object(raw,bytes,r.fields[3],category_keys,f,3,0)||
     !refs(raw,bytes,r.fields[4],plan,0)||!refs(raw,bytes,r.fields[5],plan,1)||
     (r.domain==1&&r.maximum>1048576))return 0;
  for(i=0;i<3;i++)if(!slcli_json_unsigned(raw,bytes,f[i],r.maximum,1,&r.categories[i]))return 0;
  /* Individual checked numbers never prove combined simultaneous owner fit.
   * B0/B2/native source/geometry/overlap remain independent full prerequisites. */
  *out=r;return 1;
}

int slcli_ha2_large_members_view_decode(const unsigned char *closure,size_t closure_bytes,
  const unsigned char *envelope,size_t envelope_bytes,const struct slcli_json_ha2_plan *plan,
  struct slcli_ha2_large_members_view *out) {
  struct slcli_ha2_large_members_view v={0};struct slcli_json_ha2_array_cursor regions={0},phases={0},domains_cursor={0};
  struct slcli_json_span item;struct slcli_ha2_region_view region;struct slcli_ha2_phase_view phase;
  struct slcli_ha2_domain_total_view total;uint32_t domain_seen=0;int done;
  if(!out||!plan||!away(out,sizeof(*out),closure,closure_bytes)||
     !away(out,sizeof(*out),envelope,envelope_bytes)||!away(out,sizeof(*out),plan,sizeof(*plan))||
     !away(closure,closure_bytes,envelope,envelope_bytes)||
     !slcli_ha2_closure_root_parse(closure,closure_bytes,plan,&v.closure)||
     !slcli_ha2_envelope_root_parse(envelope,envelope_bytes,plan,&v.envelope)||
     !refs(closure,closure_bytes,v.closure.entry_set,plan,1))return 0;
  do {
    if(!slcli_json_ha2_array_next(closure,closure_bytes,v.closure.regions,plan,&regions,&item,&done))return 0;
    if(!done&&(regions.count>999999||
       !slcli_ha2_region_parse(closure,closure_bytes,item,plan,(uint32_t)regions.count,&region)))return 0;
  } while(!done);
  do {
    if(!slcli_json_ha2_array_next(envelope,envelope_bytes,v.envelope.phases,plan,&phases,&item,&done))return 0;
    if(!done&&(phases.count>16||
       !slcli_ha2_phase_parse(envelope,envelope_bytes,item,plan,(uint32_t)phases.count,&phase)))return 0;
  } while(!done);
  if(phases.count!=16)return 0;
  do {
    if(!slcli_json_ha2_array_next(envelope,envelope_bytes,v.envelope.domain_totals,plan,&domains_cursor,&item,&done))return 0;
    if(!done) {
      if(!slcli_ha2_domain_total_parse(envelope,envelope_bytes,item,plan,&total)||
         (domain_seen&(1u<<(unsigned)total.domain)))return 0;
      domain_seen|=1u<<(unsigned)total.domain;
    }
  } while(!done);
  v.regions=regions.count;v.domains=domains_cursor.count;
  /* No complete source/certificate/activation flag is published here. */
  *out=v;return 1;
}
