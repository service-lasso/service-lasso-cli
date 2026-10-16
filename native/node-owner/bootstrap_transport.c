#include "bootstrap_transport.h"
#include "../host-service/source_deadline.h"
static int apart(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static void put(unsigned char **p,uint64_t v,size_t n) {
  size_t i;for(i=0;i<n;i++){*(*p)++=(unsigned char)v;v>>=8;}
}
static uint64_t get(const unsigned char **p,size_t n) {
  uint64_t v=0;size_t i;for(i=0;i<n;i++)v|=(uint64_t)*(*p)++<<(8*i);return v;
}
int slcli_node_prefix_plan_matches(const struct slcli_node_prefix_plan *p,
  const struct slcli_memory_owner *o,const struct slcli_extent_request *runtime) {
  size_t i,j;uint32_t purposes=0;const struct slcli_extent *runtime_extent;
  if(!p||!o||!runtime||!p->count||p->count>o->count||o->count>256||
     (!slcli_memory_owner_prebirth_shape(o)&&!slcli_memory_owner_frozen(o))||
     runtime->index>=o->count||!runtime->sequence)return 0;
  /* Existing backing_request_matches deliberately requires OWNER_CHILD.
   * This parent prebirth/frozen predicate preserves that child-only contract. */
  runtime_extent=&o->extents[runtime->index];
  if(runtime_extent->state!=SLCLI_EXTENT_BACKING_ACQUIRED||runtime_extent->base||
     !runtime_extent->reserved||runtime_extent->backed||runtime_extent->backed_observed||
     runtime_extent->arena_initialized||runtime_extent->retirement||
     runtime_extent->acquisition!=runtime->sequence||runtime_extent->latest!=runtime->sequence)return 0;
  for(i=0;i<256;i++) {
    const struct slcli_node_prefix_row *r=&p->rows[i];const struct slcli_extent *e;
    if(i>=p->count) {
      if(r->purpose||r->index||r->sequence||r->reserved)return 0;
      continue;
    }
    if(!r->purpose||(r->purpose&~15u)||
       r->index>=o->count||r->index==runtime->index||!r->sequence||!r->reserved)return 0;
    e=&o->extents[r->index];
    if(e->state!=SLCLI_EXTENT_RESERVED||e->base||e->backed||e->backed_observed||
       e->arena_initialized||e->retirement||e->acquisition!=r->sequence||
       e->latest!=r->sequence||e->reserved!=r->reserved)return 0;
    if(((r->purpose&SLCLI_PREFIX_STACK)&&
        ((r->purpose&(SLCLI_PREFIX_IMAGE|SLCLI_PREFIX_TLS_LOADER))||e->category!=SLCLI_STACK))||
       (r->purpose==SLCLI_PREFIX_INGRESS&&e->category!=SLCLI_IO)||
       ((r->purpose&(SLCLI_PREFIX_IMAGE|SLCLI_PREFIX_TLS_LOADER))&&
        e->category!=SLCLI_RUNTIME))return 0;
    for(j=0;j<i;j++)if(p->rows[j].index==r->index)return 0;
    purposes|=r->purpose;
  }
  /* Complete source-qualified coverage is a separate original-plan relation;
   * merely seeing each purpose does not prove all reached mappings covered. */
  return (purposes&(SLCLI_PREFIX_IMAGE|SLCLI_PREFIX_STACK))==
    (SLCLI_PREFIX_IMAGE|SLCLI_PREFIX_STACK);
}
int slcli_node_prefix_prepare_original(struct slcli_node_prefix_original *b,
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,
  size_t caller_bytes,const struct slcli_native_clock *prior,
  const struct slcli_node_prefix_plan *p,const struct slcli_memory_owner *target,
  const struct slcli_extent_request *runtime) {
  size_t i,j;uint64_t previous;
  if(!b||!a||!p||!target||target==a->owner||!runtime||!prior||!caller_bytes||
     !slcli_arena_live_block(a,b,sizeof(*b))||
     !slcli_arena_live_block(a,p,sizeof(*p))||
     !slcli_arena_live_block(a,target,sizeof(*target))||
     !slcli_arena_live_range(a,runtime,sizeof(*runtime))||
     !slcli_arena_live_range(a,prior,sizeof(*prior))||
     !slcli_arena_live_block(a,caller,caller_bytes)||
     !slcli_source_deadline_original_matches(a,d,caller)||
     !slcli_memory_owner_prebirth_shape(target)||!prior->complete||
     prior->tick<d->origin->tick||prior->tick>=d->expires||
     !apart(b,sizeof(*b),p,sizeof(*p))||!apart(b,sizeof(*b),target,sizeof(*target))||
     !apart(b,sizeof(*b),runtime,sizeof(*runtime))||
     !apart(b,sizeof(*b),prior,sizeof(*prior))||!apart(b,sizeof(*b),caller,caller_bytes)||
     !slcli_source_deadline_away(d,b,sizeof(*b)))return 0;
  for(i=0;i<sizeof(*b);i++)if(((const unsigned char *)b)[i])return 0;
  b->started=1;b->plan_original=p;b->target_original=target;b->runtime_original=runtime;
  b->deadline_original=d;b->prior_original=prior;b->caller_original=caller;
  b->runtime_snapshot=*runtime;
  b->caller_bytes=caller_bytes;b->target_charged=target->charged;
  b->target_next_sequence=target->next_sequence;b->target_count=target->count;
  b->expires=d->expires;b->origin_tick=d->origin->tick;
  for(j=0;j<32;j++) {
    b->associations[0][j]=target->original_birth[j];b->associations[1][j]=target->original_image[j];
    b->associations[2][j]=target->original_owner[j];b->associations[3][j]=target->original_source[j];
  }
  if(!slcli_node_prefix_plan_matches(p,target,&b->runtime_snapshot))goto terminal;
  b->snapshot.count=p->count;previous=prior->tick;
  for(i=0;i<b->snapshot.count;i++) {
    if(!slcli_native_clock_read(&b->rows[i])||
       !slcli_source_deadline_original_matches(a,d,caller)||d->expires!=b->expires||
       d->origin->tick!=b->origin_tick||b->rows[i].tick<previous||
       b->rows[i].tick>=b->expires||p->count!=b->snapshot.count)goto terminal;
    b->snapshot.rows[i]=p->rows[i];
    if(b->snapshot.rows[i].index>=b->target_count)goto terminal;
    b->extent_originals[i]=&target->extents[b->snapshot.rows[i].index];
    b->copied++;previous=b->rows[i].tick;
  }
  if(!slcli_node_prefix_plan_matches(&b->snapshot,target,&b->runtime_snapshot)||
     runtime->index!=b->runtime_snapshot.index||runtime->sequence!=b->runtime_snapshot.sequence||
     target->charged!=b->target_charged||target->count!=b->target_count||
     target->next_sequence!=b->target_next_sequence)goto terminal;
  for(j=0;j<32;j++)if(b->associations[0][j]!=target->original_birth[j]||
    b->associations[1][j]!=target->original_image[j]||
    b->associations[2][j]!=target->original_owner[j]||
    b->associations[3][j]!=target->original_source[j])goto terminal;
  if(p->count!=b->snapshot.count)goto terminal;
  for(i=0;i<256;i++)if(p->rows[i].purpose!=b->snapshot.rows[i].purpose||
    p->rows[i].index!=b->snapshot.rows[i].index||
    p->rows[i].sequence!=b->snapshot.rows[i].sequence||
    p->rows[i].reserved!=b->snapshot.rows[i].reserved)goto terminal;
  if(!slcli_native_clock_read(&b->after)||b->after.tick<previous||
     b->after.tick>=b->expires||!slcli_source_deadline_original_matches(a,d,caller)||
     d->expires!=b->expires||d->origin->tick!=b->origin_tick)goto terminal;
  b->returned=1;
terminal:b->terminal=1;return b->returned;
}
int slcli_node_prefix_original_matches(const struct slcli_node_prefix_original *b,
  struct slcli_arena *a,const struct slcli_source_deadline_original *d,const void *caller,
  const struct slcli_memory_owner *target,const struct slcli_extent_request *runtime) {
  size_t i,j;const struct slcli_node_prefix_plan *p;
  if(!b||!a||!target||!runtime||!slcli_arena_live_block(a,b,sizeof(*b))||
     !b->started||!b->terminal||!b->returned||b->target_original!=target||
     b->runtime_original!=runtime||b->deadline_original!=d||b->caller_original!=caller||
     !slcli_source_deadline_original_matches(a,d,caller)||
     !slcli_arena_live_block(a,target,sizeof(*target))||
     !slcli_arena_live_range(a,runtime,sizeof(*runtime))||
     d->expires!=b->expires||d->origin->tick!=b->origin_tick||
     target->charged!=b->target_charged||target->count!=b->target_count||
     target->next_sequence!=b->target_next_sequence||
     runtime->index!=b->runtime_snapshot.index||runtime->sequence!=b->runtime_snapshot.sequence||
     b->copied!=b->snapshot.count||!b->after.complete||b->after.tick>=b->expires||
     !slcli_node_prefix_plan_matches(&b->snapshot,target,&b->runtime_snapshot))return 0;
  p=b->plan_original;
  if(!p||!slcli_arena_live_block(a,p,sizeof(*p))||p->count!=b->snapshot.count)return 0;
  for(i=0;i<256;i++) {
    if(p->rows[i].purpose!=b->snapshot.rows[i].purpose||p->rows[i].index!=b->snapshot.rows[i].index||
       p->rows[i].sequence!=b->snapshot.rows[i].sequence||p->rows[i].reserved!=b->snapshot.rows[i].reserved)return 0;
    if(i<b->snapshot.count&&(!b->rows[i].complete||b->rows[i].tick>=b->expires||
       b->extent_originals[i]!=&target->extents[b->snapshot.rows[i].index]))return 0;
  }
  for(j=0;j<32;j++)if(b->associations[0][j]!=target->original_birth[j]||
    b->associations[1][j]!=target->original_image[j]||
    b->associations[2][j]!=target->original_owner[j]||
    b->associations[3][j]!=target->original_source[j])return 0;
  return 1;
}
static int clock_tuple(const struct slcli_native_clock *c) {
  uint64_t whole,fraction;
#if defined(_WIN32)
  if(c->calls_called!=2||!c->status[0]||!c->status[1]||
     !c->raw[0]||!c->raw[1]||c->raw[0]>INT64_MAX||c->raw[1]>INT64_MAX)return 0;
  whole=c->raw[0]/c->raw[1];fraction=c->raw[0]%c->raw[1];
  if(whole>UINT64_MAX/UINT64_C(1000000000)||fraction>UINT64_MAX/UINT64_C(1000000000))return 0;
  whole*=UINT64_C(1000000000);fraction=fraction*UINT64_C(1000000000)/c->raw[1];
#elif defined(__linux__)
  if(c->calls_called!=1||c->status[0]||c->status[1]||c->raw[0]>INT64_MAX||
     c->raw[1]>=UINT64_C(1000000000)||c->raw[0]>UINT64_MAX/UINT64_C(1000000000))return 0;
  whole=c->raw[0]*UINT64_C(1000000000);fraction=c->raw[1];
#endif
  return fraction<=UINT64_MAX-whole&&c->tick==whole+fraction;
}
static int geometry(const struct slcli_node_bootstrap_transport *t) {
  const struct slcli_extent *e;size_t i;
  if(!slcli_memory_owner_frozen(&t->frozen_owner)||
     t->frozen_owner.limit!=t->owner_limit||t->runtime_request.index>=t->frozen_owner.count||
     !slcli_node_prefix_plan_matches(&t->prefix,&t->frozen_owner,&t->runtime_request))return 0;
  e=&t->frozen_owner.extents[t->runtime_request.index];
  if(e->state!=SLCLI_EXTENT_BACKING_ACQUIRED||e->base||e->backed||
     e->reserved!=t->runtime_bytes||e->acquisition!=t->runtime_request.sequence||
     e->latest!=t->runtime_request.sequence)return 0;
#if defined(_WIN32)
  if(!t->parent_pid||!t->capabilities[3]||t->parent_sid_bytes<8||
     t->parent_sid_bytes>SECURITY_MAX_SID_SIZE||t->parent_sid[0]!=SID_REVISION||
     t->parent_sid[1]>SID_MAX_SUB_AUTHORITIES||
     t->parent_sid_bytes!=8u+4u*t->parent_sid[1])return 0;
  for(i=t->parent_sid_bytes;i<SECURITY_MAX_SID_SIZE;i++)if(t->parent_sid[i])return 0;
  if(!t->mapped_view.base||t->mapped_view.base>UINTPTR_MAX||
     t->mapped_view.bytes!=t->runtime_bytes||t->mapped_view.bytes>UINTPTR_MAX-t->mapped_view.base||
     t->mapped_view.section!=t->capabilities[0]||!t->mapped_view.section||
     !t->mapped_view.view_version||!t->mapped_view.section_version||
     t->mapped_view.query_tick<t->origin.tick||t->mapped_view.query_tick>=t->expires||
     (int32_t)t->mapped_view.status<0||t->mapped_view.protection!=PAGE_READWRITE||
     t->mapped_view.state!=MEM_COMMIT||t->mapped_view.type!=MEM_MAPPED)return 0;
#endif
  for(i=0;i<32;i++)if(t->associations[0][i]!=t->frozen_owner.original_birth[i]||
    t->associations[1][i]!=t->frozen_owner.original_image[i]||
    t->associations[2][i]!=t->frozen_owner.original_owner[i]||
    t->associations[3][i]!=t->frozen_owner.original_source[i])return 0;
  return t->origin.started==1&&t->origin.observed==1&&t->origin.complete==1&&
    t->origin.calls_called==t->origin.calls_returned&&t->origin.calls_called&&
    t->origin.calls_called<=2&&clock_tuple(&t->origin)&&t->origin.tick&&t->expires>t->origin.tick&&
    t->owner_limit&&t->bootstrap_bytes&&t->runtime_bytes&&t->capacity&&t->thread_capture_attempts&&
    t->bootstrap_bytes<=t->owner_limit&&t->runtime_bytes<=t->owner_limit&&
    t->process_original&&t->thread_original&&
    t->runtime_request.index<256&&t->runtime_request.sequence;
}
int slcli_node_bootstrap_transport_encode(unsigned char *out,size_t bytes,
  const struct slcli_node_bootstrap_transport *t) {
  unsigned char *p=out;size_t i,j;static const unsigned char magic[8]={'S','L','N','B','v','6',0,0};
  if(bytes!=SLCLI_BOOTSTRAP_TRANSPORT_BYTES||!apart(out,bytes,t,sizeof(*t))||!geometry(t))return 0;
  for(i=0;i<bytes;i++)if(out[i])return 0;
  for(i=0;i<8;i++)*p++=magic[i];put(&p,6,4);put(&p,SLCLI_BOOTSTRAP_ABI,4);
  for(i=0;i<2;i++)put(&p,t->origin.args[i],8);
  for(i=0;i<2;i++)put(&p,t->origin.raw[i],8);
  for(i=0;i<2;i++)put(&p,t->origin.status[i],8);
  put(&p,t->origin.tick,8);put(&p,t->origin.started,4);put(&p,t->origin.observed,4);
  put(&p,t->origin.complete,4);put(&p,t->origin.calls_called,4);put(&p,t->origin.calls_returned,4);
  put(&p,t->expires,8);put(&p,t->owner_limit,8);put(&p,t->bootstrap_bytes,8);
  put(&p,t->runtime_bytes,8);put(&p,t->capacity,8);
  put(&p,t->thread_capture_attempts,8);
  for(i=0;i<4;i++)put(&p,t->capabilities[i],8);
  put(&p,t->process_original,8);put(&p,t->thread_original,8);
  for(i=0;i<2;i++)put(&p,t->birth_raw[i],8);
  for(i=0;i<4;i++)for(j=0;j<32;j++)*p++=t->associations[i][j];
  for(i=0;i<SLCLI_BOOTSTRAP_EXPECTED_BYTES;i++)*p++=t->expected_native[i];
  put(&p,t->runtime_request.index,4);put(&p,t->runtime_request.sequence,8);
  put(&p,t->prefix.count,4);
  for(i=0;i<256;i++) {
    put(&p,t->prefix.rows[i].purpose,4);put(&p,t->prefix.rows[i].index,4);
    put(&p,t->prefix.rows[i].sequence,8);put(&p,t->prefix.rows[i].reserved,8);
  }
#if defined(_WIN32)
  put(&p,t->parent_pid,4);
  for(i=0;i<2;i++)put(&p,t->parent_creation[i],4);
  put(&p,t->parent_sid_bytes,4);
  for(i=0;i<SECURITY_MAX_SID_SIZE;i++)*p++=t->parent_sid[i];
  for(i=0;i<2;i++)put(&p,t->parent_token_id[i],4);
  for(i=0;i<2;i++)put(&p,t->parent_authentication_id[i],4);
  put(&p,t->mapped_view.base,8);put(&p,t->mapped_view.bytes,8);put(&p,t->mapped_view.section,8);
  put(&p,t->mapped_view.view_version,8);put(&p,t->mapped_view.section_version,8);put(&p,t->mapped_view.query_tick,8);
  put(&p,t->mapped_view.status,4);put(&p,t->mapped_view.protection,4);
  put(&p,t->mapped_view.state,4);put(&p,t->mapped_view.type,4);
#endif
  if(!slcli_owner_handoff_encode(p,SLCLI_OWNER_HANDOFF_BYTES,&t->frozen_owner))return 0;
  p+=SLCLI_OWNER_HANDOFF_BYTES;
  return (size_t)(p-out)==bytes;
}
int slcli_node_bootstrap_transport_decode(struct slcli_node_bootstrap_decode *c,
  const unsigned char *raw,size_t bytes) {
  const unsigned char *p=raw;struct slcli_node_bootstrap_transport *t;size_t i,j;
  static const unsigned char magic[8]={'S','L','N','B','v','6',0,0};
  if(!c||!apart(c,sizeof(*c),raw,bytes))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  c->started=1;c->raw_original=raw;c->raw_bytes_original=bytes;
  if(bytes!=SLCLI_BOOTSTRAP_TRANSPORT_BYTES)goto done;
  for(i=0;i<8;i++)if(*p++!=magic[i])goto done;
  if(get(&p,4)!=6||get(&p,4)!=SLCLI_BOOTSTRAP_ABI)goto done;t=&c->transport;
  for(i=0;i<2;i++)t->origin.args[i]=get(&p,8);
  for(i=0;i<2;i++)t->origin.raw[i]=get(&p,8);
  for(i=0;i<2;i++)t->origin.status[i]=get(&p,8);
  t->origin.tick=get(&p,8);t->origin.started=(uint32_t)get(&p,4);t->origin.observed=(uint32_t)get(&p,4);
  t->origin.complete=(uint32_t)get(&p,4);t->origin.calls_called=(uint32_t)get(&p,4);
  t->origin.calls_returned=(uint32_t)get(&p,4);
  t->expires=get(&p,8);t->owner_limit=get(&p,8);t->bootstrap_bytes=get(&p,8);
  t->runtime_bytes=get(&p,8);t->capacity=get(&p,8);
  t->thread_capture_attempts=get(&p,8);
  for(i=0;i<4;i++)t->capabilities[i]=get(&p,8);
  t->process_original=get(&p,8);t->thread_original=get(&p,8);
  for(i=0;i<2;i++)t->birth_raw[i]=get(&p,8);
  for(i=0;i<4;i++)for(j=0;j<32;j++)t->associations[i][j]=*p++;
  for(i=0;i<SLCLI_BOOTSTRAP_EXPECTED_BYTES;i++)t->expected_native[i]=*p++;
  t->runtime_request.index=(uint32_t)get(&p,4);t->runtime_request.sequence=get(&p,8);
  t->prefix.count=(uint32_t)get(&p,4);
  for(i=0;i<256;i++) {
    t->prefix.rows[i].purpose=(uint32_t)get(&p,4);t->prefix.rows[i].index=(uint32_t)get(&p,4);
    t->prefix.rows[i].sequence=get(&p,8);t->prefix.rows[i].reserved=get(&p,8);
  }
#if defined(_WIN32)
  t->parent_pid=(uint32_t)get(&p,4);
  for(i=0;i<2;i++)t->parent_creation[i]=(uint32_t)get(&p,4);
  t->parent_sid_bytes=(uint32_t)get(&p,4);
  for(i=0;i<SECURITY_MAX_SID_SIZE;i++)t->parent_sid[i]=*p++;
  for(i=0;i<2;i++)t->parent_token_id[i]=(uint32_t)get(&p,4);
  for(i=0;i<2;i++)t->parent_authentication_id[i]=(uint32_t)get(&p,4);
  t->mapped_view.base=get(&p,8);t->mapped_view.bytes=get(&p,8);t->mapped_view.section=get(&p,8);
  t->mapped_view.view_version=get(&p,8);t->mapped_view.section_version=get(&p,8);t->mapped_view.query_tick=get(&p,8);
  t->mapped_view.status=(uint32_t)get(&p,4);t->mapped_view.protection=(uint32_t)get(&p,4);
  t->mapped_view.state=(uint32_t)get(&p,4);t->mapped_view.type=(uint32_t)get(&p,4);
#endif
  if(!slcli_owner_handoff_decode(&c->owner_decode,p,SLCLI_OWNER_HANDOFF_BYTES))goto done;
  for(i=0;i<sizeof(t->frozen_owner);i++)
    ((unsigned char *)&t->frozen_owner)[i]=((const unsigned char *)&c->owner_decode.owner)[i];
  p+=SLCLI_OWNER_HANDOFF_BYTES;
  c->qualified=(size_t)(p-raw)==bytes&&geometry(t);
done:c->returned=1;return c->qualified;
}
