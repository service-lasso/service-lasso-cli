#include "evidence_deadline.h"
#if defined(_WIN32) || defined(__linux__)
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int source(const struct slcli_evidence_file *e) {
  const struct slcli_evidence_deadline_owner *o=e?e->deadline_owner:0;
  const struct slcli_image_deadline_owner *d=o?o->rows.deadline_owner:0;
  return o&&o->prepared&&o->file==e&&d&&d->arena&&
    slcli_arena_live_block(d->arena,o,sizeof(*o))&&
    slcli_arena_live_range(d->arena,e,sizeof(*e))&&
    e->workspace==o->rows.workspace&&e->captures==o->rows.captures&&
    e->capture_capacity==o->rows.capture_capacity&&e->capture_sink==o->rows.capture_sink&&
    e->capture_original==o->rows.capture_original&&e->original==o->rows.original&&
    e->root==o->root&&e->maximum==o->maximum&&e->count<=o->maximum&&
    (!o->bq_profile||(slcli_arena_live_range(d->arena,o->bq_profile,sizeof(*o->bq_profile))&&
      slcli_arena_live_range(d->arena,o->profile_raw,o->profile_bytes)&&
      o->bq_profile->raw==o->profile_raw&&o->bq_profile->raw_bytes==o->profile_bytes&&
      o->bq_profile->retention_maximum==o->maximum&&
      o->bq_profile->record_maximum==o->record_maximum));
}
int slcli_evidence_deadline_ready(const struct slcli_evidence_file *e) {
  const struct slcli_evidence_deadline_owner *o=e?e->deadline_owner:0;
  return source(e)&&!o->failed&&!o->inflight&&e->capture_count==o->rows.capture_count&&
    slcli_image_deadline_ready(&o->rows);
}
int slcli_evidence_deadline_precede(struct slcli_evidence_file *e,
  const struct slcli_native_clock *clock) {
  struct slcli_evidence_deadline_owner *o=e?e->deadline_owner:0;
  struct slcli_image_deadline_owner *d=o?o->rows.deadline_owner:0;
  if(!slcli_evidence_deadline_ready(e)||!clock||!clock->complete||
    !slcli_arena_live_range(d->arena,clock,sizeof(*clock))||clock->tick<d->previous_tick)return 0;
  d->previous_tick=clock->tick;
  if(clock->tick>=d->expires){o->failed=1;d->failed=1;return 0;}
  return slcli_evidence_deadline_ready(e);
}
static int prepare_unborn(struct slcli_arena *a,struct slcli_evidence_file *e,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  void *capture_owner,size_t capture_bytes,uint64_t predecessor,slcli_input_handle root,
  uint64_t maximum,slcli_store_capture_sink sink,slcli_input_handle *birth,
  const void *producer,size_t producer_bytes,const struct slcli_profile *profile) {
  struct slcli_evidence_deadline_owner *o;const void *p[7];size_t n[7],i,j;
  if(!a||!e||!slcli_arena_live_range(a,e,sizeof(*e))||e->deadline_owner||e->started||
    e->failed||e->sealed||e->close_started||e->count||e->capture_count||e->readback_original||
    !e->workspace||!e->captures||!e->capture_capacity||
    e->capture_capacity>SIZE_MAX/sizeof(*e->captures)||!maximum||
    (!profile&&maximum>8388608)||maximum>INT64_MAX||
    !sink||!capture_bytes||!caller_bytes||!birth||
    !slcli_source_deadline_original_matches(a,d,caller))return 0;
  if(profile&&(!slcli_arena_live_range(a,profile,sizeof(*profile))||
    !profile->raw_bytes||!slcli_arena_live_range(a,profile->raw,profile->raw_bytes)||
    profile->retention_maximum!=maximum||maximum>UINT64_C(9007199254740991)||!profile->record_maximum||
    profile->record_maximum>maximum))return 0;
#if defined(_WIN32)
  if(!root||root==INVALID_HANDLE_VALUE||e->original||*birth)return 0;
#else
  if(root<0||e->original!=-1||*birth!=-1)return 0;
#endif
  p[0]=e;n[0]=sizeof(*e);p[1]=caller;n[1]=caller_bytes;
  p[2]=capture_owner;n[2]=capture_bytes;p[3]=e->workspace;n[3]=sizeof(*e->workspace);
  p[4]=e->captures;n[4]=e->capture_capacity*sizeof(*e->captures);
  p[5]=d;n[5]=sizeof(*d);p[6]=d->origin;n[6]=sizeof(*d->origin);
  for(i=0;i<7;i++) {
    if(!slcli_arena_live_range(a,p[i],n[i])||!away(producer,producer_bytes,p[i],n[i]))return 0;
    for(j=i+1;j<7;j++)if(!away(p[i],n[i],p[j],n[j]))return 0;
  }
  if(profile) {
    if(!away(profile,sizeof(*profile),profile->raw,profile->raw_bytes)||
      !away(producer,producer_bytes,profile,sizeof(*profile))||
      !away(producer,producer_bytes,profile->raw,profile->raw_bytes))return 0;
    for(i=0;i<7;i++)if(!away(p[i],n[i],profile,sizeof(*profile))||
      !away(p[i],n[i],profile->raw,profile->raw_bytes))return 0;
  }
  o=slcli_arena_calloc(a,1,sizeof(*o));if(!o)return 0;
  /* Attach partial preparation before further allocation. Failed construction
   * is retained and cannot silently rebind or discard its charged predecessor. */
  e->deadline_owner=o;o->file=e;o->root=root;o->maximum=maximum;
  o->bq_profile=profile;
  if(profile){o->profile_raw=profile->raw;o->profile_bytes=profile->raw_bytes;
    o->record_maximum=profile->record_maximum;}
  e->root=root;e->maximum=maximum;e->capture_sink=sink;e->capture_original=capture_owner;
  o->rows.original=e->original;o->rows.workspace=e->workspace;o->rows.captures=e->captures;
  o->rows.capture_capacity=e->capture_capacity;o->rows.capture_sink=sink;
  o->rows.capture_original=capture_owner;
  if(!slcli_image_deadline_prepare_unborn(a,&o->rows,d,caller,caller_bytes,capture_owner,
    capture_bytes,predecessor,birth,producer,producer_bytes)){o->failed=1;return 0;}
  o->prepared=1;return slcli_evidence_deadline_ready(e);
}
int slcli_evidence_deadline_prepare_unborn(struct slcli_arena *a,struct slcli_evidence_file *e,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  void *capture_owner,size_t capture_bytes,uint64_t predecessor,slcli_input_handle root,
  uint64_t maximum,slcli_store_capture_sink sink,slcli_input_handle *birth,
  const void *producer,size_t producer_bytes) {
  return prepare_unborn(a,e,d,caller,caller_bytes,capture_owner,capture_bytes,predecessor,
    root,maximum,sink,birth,producer,producer_bytes,0);
}
int slcli_evidence_deadline_prepare_bq_unborn(struct slcli_arena *a,struct slcli_evidence_file *e,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  void *capture_owner,size_t capture_bytes,uint64_t predecessor,slcli_input_handle root,
  const struct slcli_profile *profile,slcli_store_capture_sink sink,slcli_input_handle *birth,
  const void *producer,size_t producer_bytes) {
  if(!a||!profile||!slcli_arena_live_range(a,profile,sizeof(*profile)))return 0;
  return prepare_unborn(a,e,d,caller,caller_bytes,capture_owner,capture_bytes,predecessor,
    root,profile->retention_maximum,sink,birth,producer,producer_bytes,profile);
}
int slcli_evidence_deadline_born(struct slcli_evidence_file *e,slcli_input_handle *birth,
  const struct slcli_native_clock *clock) {
  struct slcli_evidence_deadline_owner *o=e?e->deadline_owner:0;int ok;
  if(!slcli_evidence_deadline_ready(e)||e->started)return 0;
  ok=slcli_image_deadline_born(&o->rows,birth,clock);
  e->original=o->rows.original;
  if(!ok){o->failed=1;return 0;}return slcli_evidence_deadline_ready(e);
}
int slcli_evidence_deadline_before(struct slcli_evidence_file *e,slcli_input_capture *c,
  slcli_input_handle original) {
  struct slcli_evidence_deadline_owner *o=e?e->deadline_owner:0;
  if(!source(e)||o->failed||o->inflight||!c||
    (original!=o->root&&original!=o->rows.original)||
    e->capture_count!=o->rows.capture_count+1)return 0;
  o->rows.capture_count=e->capture_count;o->active_original=original;o->inflight=1;
  if(!slcli_image_deadline_before(&o->rows,c)){o->failed=1;return 0;}return 1;
}
int slcli_evidence_deadline_after(struct slcli_evidence_file *e,slcli_input_capture *c,int native_ok) {
  struct slcli_evidence_deadline_owner *o=e?e->deadline_owner:0;int target;
  if(!source(e)||!o->inflight||!c)return 0;
#if defined(_WIN32)
  target=c->original==o->active_original;
#else
  target=(c->operation==1?c->stat_args[0]:c->args[0])==(uint64_t)o->active_original;
#endif
  if(!slcli_image_deadline_after(&o->rows,c,native_ok&&target)){o->failed=1;return 0;}
  o->inflight=0;return slcli_evidence_deadline_ready(e);
}
#endif
