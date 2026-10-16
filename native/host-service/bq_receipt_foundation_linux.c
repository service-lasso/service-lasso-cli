/* SPDX-License-Identifier: Apache-2.0 */
#include "bq_receipt_foundation_linux.h"
#include "input_deadline.h"
#if defined(__linux__)
static int separate(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}

int slcli_bq_retention_policy_linux_values_shape(const struct slcli_bq_retention_policy_values *p){
 return slcli_bq_retention_policy_values_shape(p,sizeof(struct slcli_linux_file_capture),sizeof(struct slcli_bq_receipt_index_linux_original));
}
int slcli_bq_retention_policy_linux_encode(unsigned char *out,size_t n,const struct slcli_bq_retention_policy_values *p){
 return slcli_bq_retention_policy_encode(out,n,p,sizeof(struct slcli_linux_file_capture),sizeof(struct slcli_bq_receipt_index_linux_original));
}
int slcli_bq_retention_policy_linux_decode(struct slcli_bq_retention_policy_values *out,const unsigned char *raw,size_t n){
 return slcli_bq_retention_policy_decode(out,raw,n,sizeof(struct slcli_linux_file_capture),sizeof(struct slcli_bq_receipt_index_linux_original));
}
int slcli_bq_retention_policy_linux_body_matches_original(const struct slcli_bq_retention_policy_linux_original *p){
 const struct slcli_bq_source_readable_linux *r;const struct slcli_input_body *b;
 const struct slcli_input_deadline_owner *d;struct slcli_arena *a;struct slcli_bq_retention_policy_values v;
 size_t total,offset,capture_bytes,i=0,pass,j,wanted,position;uint64_t previous;
 if(!p||!(r=p->installed_policy_readable_original)||!(b=p->installed_policy_body_original)||
  !p->installed_policy_namespace_original||!separate(p,sizeof(*p),r,sizeof(*r))||!separate(p,sizeof(*p),b,sizeof(*b))||
  !(a=r->arena)||!separate(p,sizeof(*p),a,sizeof(*a))||!a->owner||!separate(p,sizeof(*p),a->owner,sizeof(*a->owner))||
  !slcli_arena_live_block(a,p,sizeof(*p))||!slcli_arena_live_range(a,b,sizeof(*b))||
  !slcli_bq_source_readable_linux_matches_original(r)||r->source!=p->installed_policy_namespace_original||
  b->original!=r->descriptor||b->bytes!=SLCLI_BQ_RETENTION_POLICY_BYTES||b->capacity!=SLCLI_BQ_RETENTION_POLICY_BYTES+1||
  b->readback_capacity!=b->capacity||!b->started||!b->complete||!b->eof_observed||b->failed||
  !b->recheck_started||!b->recheck_complete||b->capture_capacity<8||b->capture_capacity>2*(SLCLI_BQ_RETENTION_POLICY_BYTES+3)||
  b->capture_count>b->capture_capacity||b->capture_capacity>SIZE_MAX/sizeof(*b->captures)||b->capture_count>SIZE_MAX/sizeof(*d->before)||
  !(d=b->deadline_owner)||!slcli_input_deadline_ready(b)||d->arena!=a||d->deadline!=r->deadline||d->caller_original!=r->caller||d->caller_bytes!=r->caller_bytes||d->predecessor!=&r->finish||d->predecessor_tick!=r->finish.tick)return 0;
 offset=sizeof(*b)+2*b->capacity;
 if(offset>SIZE_MAX-(_Alignof(slcli_input_capture)-1))return 0;
 offset=(offset+_Alignof(slcli_input_capture)-1)&~(_Alignof(slcli_input_capture)-1);
 capture_bytes=b->capture_capacity*sizeof(*b->captures);if(capture_bytes>SIZE_MAX-offset)return 0;total=offset+capture_bytes;
 if(!slcli_arena_live_block(a,b,total)||b->body!=(unsigned char *)b+sizeof(*b)||b->readback!=b->body+b->capacity||
  b->captures!=(slcli_input_capture *)((unsigned char *)b+offset)||!separate(p,sizeof(*p),b,total)||
  !separate(r,sizeof(*r),b,total)||!slcli_arena_live_range(a,d->before,b->capture_count*sizeof(*d->before)))return 0;
 if(!slcli_native_clock_linux_matches_original(d->predecessor)||
  !slcli_native_clock_linux_matches_original(d->origin)||d->origin->tick!=d->origin_tick||
  d->predecessor->tick!=d->predecessor_tick||d->predecessor_tick<d->origin_tick||
  d->predecessor_tick>=d->expires)return 0;
 previous=d->predecessor_tick;
 for(j=0;j<b->capture_count;++j){
  if(!slcli_native_clock_linux_matches_original(&d->before[j])||d->before[j].tick<previous||
   !b->captures[j].clock_observed||b->captures[j].clock_status||b->captures[j].tick<d->before[j].tick||b->captures[j].tick>=d->expires)return 0;
  previous=b->captures[j].tick;
 }
 if(d->previous_tick!=previous)return 0;
 for(pass=0;pass<2;++pass){const unsigned char *buffer=pass?b->readback:b->body;
  if(i>=b->capture_count||!slcli_linux_file_observation_matches_original(r->descriptor,&b->captures[i],S_IFREG)||
   !slcli_input_original_same(&r->readable,&b->captures[i],SLCLI_BQ_RETENTION_POLICY_BYTES))return 0;++i;position=0;
  for(;;){const slcli_input_capture *c;
   if(i>=b->capture_count)return 0;c=&b->captures[i++];wanted=position<SLCLI_BQ_RETENTION_POLICY_BYTES?SLCLI_BQ_RETENTION_POLICY_BYTES-position:1;
   if(!slcli_linux_file_read_matches_original(r->descriptor,buffer+position,wanted,position,c)||c->returned_bytes>wanted||
    c->returned_bytes>SLCLI_BQ_RETENTION_POLICY_BYTES-position)return 0;
   if(!c->returned_bytes){if(position!=SLCLI_BQ_RETENTION_POLICY_BYTES)return 0;break;}
   position+=(size_t)c->returned_bytes;
  }
  if(i>=b->capture_count||!slcli_linux_file_observation_matches_original(r->descriptor,&b->captures[i],S_IFREG)||
   !slcli_input_original_same(&r->readable,&b->captures[i],SLCLI_BQ_RETENTION_POLICY_BYTES))return 0;++i;
 }
 if(i!=b->capture_count)return 0;
 for(j=0;j<SLCLI_BQ_RETENTION_POLICY_BYTES;++j)if(b->body[j]!=b->readback[j])return 0;
 if(!slcli_bq_retention_policy_linux_decode(&v,b->body,b->bytes))return 0;
 return p->maximum_bytes==v.maximum_bytes&&p->maximum_record_bytes==v.maximum_record_bytes&&p->partial_attempts==v.partial_attempts&&
  p->failure_tail_bytes==v.failure_tail_bytes&&p->whole_lineage_bytes==v.whole_lineage_bytes&&
  p->whole_lineage_attempts==v.whole_lineage_attempts&&p->whole_lineage_nodes==v.whole_lineage_nodes;
}
static int policy_same(const struct slcli_bq_retention_policy_linux_original *a,
 const struct slcli_bq_retention_policy_linux_original *b){
 return a->installed_policy_body_original==b->installed_policy_body_original&&
  a->installed_policy_namespace_original==b->installed_policy_namespace_original&&
  a->installed_policy_readable_original==b->installed_policy_readable_original&&
  a->maximum_bytes==b->maximum_bytes&&a->maximum_record_bytes==b->maximum_record_bytes&&
  a->partial_attempts==b->partial_attempts&&a->failure_tail_bytes==b->failure_tail_bytes&&
  a->whole_lineage_bytes==b->whole_lineage_bytes&&
  a->whole_lineage_attempts==b->whole_lineage_attempts&&a->whole_lineage_nodes==b->whole_lineage_nodes;
}
int slcli_bq_receipt_foundation_linux_plan_original(
 struct slcli_bq_receipt_foundation_linux_plan *out,
 const struct slcli_bq_receipt_foundation_linux_creation *creation,
 const struct slcli_bq_retention_policy_linux_original *policy){
 struct slcli_bq_receipt_foundation_linux_plan next={0};uint64_t records,indexes,end;
 uint64_t native_bytes=(uint64_t)sizeof(struct slcli_linux_file_capture);
 uint64_t index_bytes=(uint64_t)sizeof(struct slcli_bq_receipt_index_linux_original);
 if(!separate(out,sizeof(*out),creation,sizeof(*creation))||
  !separate(out,sizeof(*out),policy,sizeof(*policy))||
  !separate(creation,sizeof(*creation),policy,sizeof(*policy))||
  !policy->installed_policy_body_original||!policy->installed_policy_namespace_original||
  !policy->maximum_bytes||!policy->maximum_record_bytes||!policy->partial_attempts||
  !policy->failure_tail_bytes||!policy->whole_lineage_attempts||!policy->whole_lineage_nodes||
  !policy->whole_lineage_bytes||policy->whole_lineage_bytes>policy->maximum_bytes||
  policy->partial_attempts>policy->whole_lineage_attempts||
  (size_t)native_bytes!=sizeof(struct slcli_linux_file_capture)||
  (size_t)index_bytes!=sizeof(struct slcli_bq_receipt_index_linux_original)||
  native_bytes>UINT64_MAX-48-16384||policy->maximum_record_bytes<native_bytes+48+16384||
  policy->whole_lineage_attempts>UINT64_MAX/policy->maximum_record_bytes||
  policy->whole_lineage_nodes>UINT64_MAX/index_bytes)return 0;
 records=policy->whole_lineage_attempts*policy->maximum_record_bytes;
 indexes=policy->whole_lineage_nodes*index_bytes;
 if(records>policy->whole_lineage_bytes||indexes>policy->whole_lineage_bytes-records)return 0;
 end=records+indexes;
 if(policy->failure_tail_bytes>policy->whole_lineage_bytes-end)return 0;
 next.creation_original=creation;next.policy_original=policy;next.policy_value=*policy;
 next.sequence=1;next.records_bytes=records;next.index_offset=records;next.index_bytes=indexes;
 next.terminal_offset=end;next.terminal_bytes=policy->whole_lineage_bytes-end;
 next.attempt_capacity=policy->whole_lineage_attempts;next.node_capacity=policy->whole_lineage_nodes;
 next.native_cell_bytes=native_bytes;next.source_window_bytes=16384;
 next.write_window_bytes=16384;next.readback_window_bytes=16384;next.pair_window_bytes=32768;
 next.active_index_bytes=index_bytes;next.partial_tail_bytes=policy->failure_tail_bytes;
 /* This is geometry only. No reserve/claim/native effect/frontier is published. */
 *out=next;return 1;
}
int slcli_bq_receipt_foundation_linux_plan_matches_original(
 const struct slcli_bq_receipt_foundation_linux_plan *p){
 struct slcli_bq_receipt_foundation_linux_plan n;
 if(!p||!separate(p,sizeof(*p),p->creation_original,sizeof(*p->creation_original))||
  !separate(p,sizeof(*p),p->policy_original,sizeof(*p->policy_original))||
  !policy_same(p->policy_original,&p->policy_value)||
  !slcli_bq_receipt_foundation_linux_plan_original(&n,p->creation_original,p->policy_original))return 0;
 return p->sequence==n.sequence&&p->records_offset==n.records_offset&&p->records_bytes==n.records_bytes&&
  p->index_offset==n.index_offset&&p->index_bytes==n.index_bytes&&
  p->terminal_offset==n.terminal_offset&&p->terminal_bytes==n.terminal_bytes&&
  p->attempt_capacity==n.attempt_capacity&&p->node_capacity==n.node_capacity&&
  p->native_cell_bytes==n.native_cell_bytes&&p->source_window_bytes==n.source_window_bytes&&
  p->write_window_bytes==n.write_window_bytes&&p->readback_window_bytes==n.readback_window_bytes&&
  p->pair_window_bytes==n.pair_window_bytes&&p->active_index_bytes==n.active_index_bytes&&
  p->partial_tail_bytes==n.partial_tail_bytes;
}
int slcli_bq_retention_policy_linux_encode_plan_original(unsigned char *out,size_t bytes,
 const struct slcli_bq_receipt_foundation_linux_plan *p){
 struct slcli_bq_retention_policy_values v={0};
 if(!p||bytes!=SLCLI_BQ_RETENTION_POLICY_BYTES||!separate(out,bytes,p,sizeof(*p))||
  !separate(out,bytes,p->creation_original,sizeof(*p->creation_original))||
  !separate(out,bytes,p->policy_original,sizeof(*p->policy_original))||
  !slcli_bq_receipt_foundation_linux_plan_matches_original(p))return 0;
 v.maximum_bytes=p->policy_value.maximum_bytes;v.maximum_record_bytes=p->policy_value.maximum_record_bytes;
 v.partial_attempts=p->policy_value.partial_attempts;v.failure_tail_bytes=p->policy_value.failure_tail_bytes;
 v.whole_lineage_bytes=p->policy_value.whole_lineage_bytes;v.whole_lineage_attempts=p->policy_value.whole_lineage_attempts;
 v.whole_lineage_nodes=p->policy_value.whole_lineage_nodes;v.records_offset=p->records_offset;v.records_bytes=p->records_bytes;
 v.index_offset=p->index_offset;v.index_bytes=p->index_bytes;v.terminal_offset=p->terminal_offset;v.terminal_bytes=p->terminal_bytes;
 v.native_cell_bytes=p->native_cell_bytes;v.source_window_bytes=p->source_window_bytes;v.write_window_bytes=p->write_window_bytes;
 v.readback_window_bytes=p->readback_window_bytes;v.pair_window_bytes=p->pair_window_bytes;v.active_index_bytes=p->active_index_bytes;
 return slcli_bq_retention_policy_linux_encode(out,bytes,&v);
}
#include "native_syscall_linux.h"
#include <linux/fcntl.h>
#include <limits.h>
static int fresh(const void *p,size_t n){size_t i;for(i=0;i<n;i++)if(((const unsigned char *)p)[i])return 0;return 1;}
static int creation_source(const struct slcli_bq_receipt_foundation_linux_creation *c){
 struct slcli_arena *a=c->prebirth_arena_original;
 return a&&separate(c,sizeof(*c),a,sizeof(*a))&&a->owner&&
  separate(c,sizeof(*c),a->owner,sizeof(*a->owner))&&c->deadline_original&&c->policy_original&&c->parent_original&&
  c->producer_process_original&&c->prior_original&&c->origin_original&&
  separate(c,sizeof(*c),c->deadline_original,sizeof(*c->deadline_original))&&
  separate(c,sizeof(*c),c->policy_original,sizeof(*c->policy_original))&&
  separate(c,sizeof(*c),c->parent_original,sizeof(*c->parent_original))&&
  separate(c,sizeof(*c),c->producer_process_original,sizeof(*c->producer_process_original))&&
  separate(c,sizeof(*c),c->prior_original,sizeof(*c->prior_original))&&
  separate(c,sizeof(*c),c->origin_original,sizeof(*c->origin_original))&&
  separate(c,sizeof(*c),c->name_original,c->name_bytes)&&
  policy_same(c->policy_original,&c->policy_value)&&slcli_bq_retention_policy_linux_body_matches_original(c->policy_original)&&
  c->prior_tick>=c->policy_original->installed_policy_body_original->deadline_owner->previous_tick&&c->deadline_original->origin==c->origin_original&&
  c->origin_original->tick==c->origin_tick&&c->deadline_original->expires==c->expires&&
  c->prior_original->tick==c->prior_tick&&slcli_memory_owner_mutable(a->owner)&&
  slcli_arena_live_block(a,c,sizeof(*c))&&
  slcli_arena_live_block(a,c->policy_original,sizeof(*c->policy_original))&&
  slcli_arena_live_block(a,c->name_original,c->name_bytes)&&
  slcli_source_deadline_original_matches(a,c->deadline_original,c->deadline_original->caller_original)&&
  slcli_native_clock_linux_matches_original(c->prior_original)&&slcli_native_clock_linux_matches_original(c->origin_original)&&
  c->prior_original->tick>=c->deadline_original->origin->tick&&
  c->prior_original->tick<c->deadline_original->expires&&
  slcli_bq_source_namespace_linux_matches_original(c->parent_original)&&
  c->parent_original->leaf_kind==S_IFDIR&&
  slcli_bq_process_reference_linux_matches_original(c->producer_process_original);
}
static int clock_original(struct slcli_native_clock *clock,int *result,uint32_t *returned,
 const struct slcli_source_deadline_original *d,uint64_t *previous){
 *result=slcli_native_clock_read(clock);*returned=1;
 if(!*result||!slcli_native_clock_linux_matches_original(clock)||clock->tick<*previous||clock->tick>=d->expires)return 0;
 *previous=clock->tick;return 1;
}
static int creation_observe(struct slcli_bq_receipt_foundation_linux_creation *c,
 size_t ordinal,int descriptor,struct slcli_linux_file_capture *out,int directory){
 size_t k=ordinal*2;int ok;
 if(!creation_source(c)||!clock_original(&c->observation_clocks[k],
  &c->observation_clock_results[k],&c->observation_clock_returned[k],c->deadline_original,&c->previous_tick))return 0;
 ok=directory?slcli_linux_path_source_observe(descriptor,out,S_IFDIR):slcli_linux_file_observe(descriptor,out);
 c->observation_results[ordinal]=ok;c->observation_returned[ordinal]=1;
 if(!ok||!out->clock_observed||out->tick<c->previous_tick||out->tick>=c->deadline_original->expires)return 0;
 c->previous_tick=out->tick;
 return clock_original(&c->observation_clocks[k+1],&c->observation_clock_results[k+1],
  &c->observation_clock_returned[k+1],c->deadline_original,&c->previous_tick)&&creation_source(c);
}
static int native_original(struct slcli_bq_receipt_foundation_linux_creation *c,
 struct slcli_bq_retention_native_linux_original *r){
 if(!creation_source(c)||!clock_original(&r->before,&r->before_result,&r->before_returned,
  c->deadline_original,&c->previous_tick))return 0;
 r->started=1;r->called=1;
 r->raw_return=slcli_kernel_call(r->number,r->args[0],r->args[1],r->args[2],r->args[3],r->args[4],r->args[5]);
 r->returned=1;
 /* Actual fd is saved before a fallible postclock; no cleanup on refusal. */
 if(r==&c->create108&&r->raw_return>=0&&r->raw_return<=INT_MAX)c->writer108=(int)r->raw_return;
 return clock_original(&r->after,&r->after_result,&r->after_returned,
  c->deadline_original,&c->previous_tick)&&creation_source(c);
}
static int same_identity(const struct slcli_linux_file_capture *a,
 const struct slcli_linux_file_capture *b,int size){
 return a->original_stat.st_dev==b->original_stat.st_dev&&a->original_stat.st_ino==b->original_stat.st_ino&&
  a->original_stat.st_mode==b->original_stat.st_mode&&a->original_stat.st_uid==b->original_stat.st_uid&&
  a->original_stat.st_gid==b->original_stat.st_gid&&a->original_stat.st_nlink==b->original_stat.st_nlink&&
  (!size||a->original_stat.st_size==b->original_stat.st_size);
}
int slcli_bq_receipt_foundation_linux_create_original(
 struct slcli_bq_receipt_foundation_linux_creation *c,struct slcli_arena *a,
 const struct slcli_source_deadline_original *d,const struct slcli_native_clock *prior,
 const struct slcli_bq_source_namespace_linux *parent,
 const struct slcli_bq_process_reference_linux_original *process,
 const struct slcli_bq_retention_policy_linux_original *policy,const unsigned char *name,size_t bytes){
 struct slcli_bq_receipt_foundation_linux_plan plan;size_t i;int fd;
 if(!a||!a->owner||!slcli_memory_owner_mutable(a->owner)||
  !slcli_arena_live_block(a,c,sizeof(*c))||!slcli_arena_live_block(a,policy,sizeof(*policy))||
  !slcli_arena_live_block(a,name,bytes)||!parent||!process||!d||!prior||bytes<2||
  !separate(c,sizeof(*c),parent,sizeof(*parent))||!separate(c,sizeof(*c),process,sizeof(*process))||
  !separate(c,sizeof(*c),d,sizeof(*d))||!separate(c,sizeof(*c),prior,sizeof(*prior))||
  !separate(c,sizeof(*c),name,bytes)||!separate(c,sizeof(*c),a,sizeof(*a))||
  !separate(c,sizeof(*c),a->owner,sizeof(*a->owner))||
  !slcli_bq_receipt_foundation_linux_plan_original(&plan,c,policy)||
  !slcli_bq_retention_policy_linux_body_matches_original(policy)||!fresh(c,sizeof(*c))||
  !slcli_source_deadline_original_matches(a,d,d->caller_original)||
  !slcli_native_clock_linux_matches_original(prior)||!slcli_native_clock_linux_matches_original(d->origin)||prior->tick<d->origin->tick||prior->tick>=d->expires||
  prior->tick<policy->installed_policy_body_original->deadline_owner->previous_tick||
  !slcli_bq_source_namespace_linux_matches_original(parent)||parent->leaf_kind!=S_IFDIR||
  !slcli_bq_process_reference_linux_matches_original(process)||name[bytes-1]||
  (bytes==2&&name[0]=='.')||(bytes==3&&name[0]=='.'&&name[1]=='.')||
  policy->whole_lineage_bytes>(uint64_t)LONG_MAX)return 0;
 for(i=0;i+1<bytes;i++)if(!name[i]||name[i]=='/')return 0;
 fd=parent->components[parent->count-1].descriptor;
 c->prebirth_arena_original=a;c->deadline_original=d;c->prior_original=prior;
 c->parent_original=parent;c->producer_process_original=process;c->policy_original=policy;c->policy_value=*policy;
 c->origin_original=d->origin;c->origin_tick=d->origin->tick;c->prior_tick=prior->tick;c->expires=d->expires;
 c->name_original=name;c->name_bytes=bytes;c->writer108=-1;c->previous_tick=prior->tick;
 c->started=1;c->failed=1;
 if(!creation_observe(c,0,fd,&c->parent_before,1))return 0;
 c->create108.number=__NR_openat;c->create108.args[0]=fd;c->create108.args[1]=(long)name;
 c->create108.args[2]=O_RDWR|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC;c->create108.args[3]=0600;
 if(!native_original(c,&c->create108))return 0;c->create_returned=1;
 if(c->writer108<0||!creation_observe(c,1,c->writer108,&c->writer_before,0)||
  (c->writer_before.original_stat.st_mode&07777)!=0600||c->writer_before.original_stat.st_size||
  (c->writer_before.flags_status&O_ACCMODE)!=O_RDWR||!(c->writer_before.descriptor_status&FD_CLOEXEC))return 0;
 c->allocate108.number=__NR_fallocate;c->allocate108.args[0]=c->writer108;
 c->allocate108.args[3]=(long)policy->whole_lineage_bytes;
 if(!native_original(c,&c->allocate108))return 0;c->allocation_returned=1;
 if(c->allocate108.raw_return||!creation_observe(c,2,c->writer108,&c->writer_after,0)||
  !same_identity(&c->writer_before,&c->writer_after,0)||c->writer_after.original_stat.st_size<0||
  (uint64_t)c->writer_after.original_stat.st_size!=policy->whole_lineage_bytes||
  !creation_observe(c,3,fd,&c->parent_after,1)||!same_identity(&c->parent_before,&c->parent_after,0))return 0;
 c->failed=0;
 if(!slcli_bq_receipt_foundation_linux_creation_matches_original(c)){c->failed=1;return 0;}return 1;
}
static int native_matches(const struct slcli_bq_retention_native_linux_original *r,
 const struct slcli_source_deadline_original *d,uint64_t previous){
 return r->started&&r->called&&r->returned&&r->before_returned&&r->after_returned&&
  r->before_result&&r->after_result&&slcli_native_clock_linux_matches_original(&r->before)&&
  slcli_native_clock_linux_matches_original(&r->after)&&r->before.tick>=previous&&
  r->after.tick>=r->before.tick&&r->after.tick<d->expires;
}
int slcli_bq_receipt_foundation_linux_creation_matches_original(
 const struct slcli_bq_receipt_foundation_linux_creation *c){
 size_t i;uint64_t previous;int fd;
 if(!c||!creation_source(c)||!c->started||c->failed||!c->create_returned||!c->allocation_returned||
  c->writer108<0||!c->parent_original->count)return 0;
 fd=c->parent_original->components[c->parent_original->count-1].descriptor;previous=c->prior_original->tick;
 for(i=0;i<4;i++){
  size_t k=i*2;const struct slcli_linux_file_capture *r=i==0?&c->parent_before:
   i==1?&c->writer_before:i==2?&c->writer_after:&c->parent_after;
  if(!c->observation_returned[i]||!c->observation_results[i]||
   !c->observation_clock_returned[k]||!c->observation_clock_returned[k+1]||
   !c->observation_clock_results[k]||!c->observation_clock_results[k+1]||
   !slcli_native_clock_linux_matches_original(&c->observation_clocks[k])||
   !slcli_native_clock_linux_matches_original(&c->observation_clocks[k+1])||
   c->observation_clocks[k].tick<previous||r->tick<c->observation_clocks[k].tick||
   c->observation_clocks[k+1].tick<r->tick||c->observation_clocks[k+1].tick>=c->deadline_original->expires||
   !((i==0||i==3)?slcli_linux_path_observation_matches_original(fd,r,S_IFDIR):slcli_linux_file_writer_observation_matches_original(c->writer108,r)))return 0;
  previous=c->observation_clocks[k+1].tick;
  if(i==0){if(!native_matches(&c->create108,c->deadline_original,previous))return 0;previous=c->create108.after.tick;}
  if(i==1){if(!native_matches(&c->allocate108,c->deadline_original,previous))return 0;previous=c->allocate108.after.tick;}
 }
 return c->previous_tick==previous&&c->create108.number==__NR_openat&&
  c->create108.args[0]==fd&&c->create108.args[1]==(long)c->name_original&&
  c->create108.args[2]==(O_RDWR|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC)&&c->create108.args[3]==0600&&
  !c->create108.args[4]&&!c->create108.args[5]&&c->create108.raw_return==c->writer108&&
  c->allocate108.number==__NR_fallocate&&c->allocate108.args[0]==c->writer108&&
  !c->allocate108.args[1]&&!c->allocate108.args[2]&&
  c->allocate108.args[3]==(long)c->policy_original->whole_lineage_bytes&&
  !c->allocate108.args[4]&&!c->allocate108.args[5]&&!c->allocate108.raw_return&&
  same_identity(&c->writer_before,&c->writer_after,0)&&same_identity(&c->parent_before,&c->parent_after,0)&&
  (c->writer_before.original_stat.st_mode&07777)==0600&&
  c->writer_before.original_stat.st_size==0&&c->writer_after.original_stat.st_size>=0&&
  (uint64_t)c->writer_after.original_stat.st_size==c->policy_original->whole_lineage_bytes;
}
static int bind_source(const struct slcli_bq_receipt_readback_linux_bind *b){
 return b&&b->arena_original&&b->creation_original&&b->readback_acquisition_original&&
  b->deadline_original&&b->prior_original&&b->origin_original&&
  separate(b,sizeof(*b),b->arena_original,sizeof(*b->arena_original))&&b->arena_original->owner&&
  separate(b,sizeof(*b),b->arena_original->owner,sizeof(*b->arena_original->owner))&&
  separate(b,sizeof(*b),b->creation_original,sizeof(*b->creation_original))&&
  separate(b,sizeof(*b),b->readback_acquisition_original,sizeof(*b->readback_acquisition_original))&&
  separate(b,sizeof(*b),b->deadline_original,sizeof(*b->deadline_original))&&
  separate(b,sizeof(*b),b->prior_original,sizeof(*b->prior_original))&&
  separate(b,sizeof(*b),b->origin_original,sizeof(*b->origin_original))&&
  slcli_arena_live_block(b->arena_original,b,sizeof(*b))&&
  b->creation_original->prebirth_arena_original==b->arena_original&&
  b->deadline_original->origin==b->origin_original&&b->origin_original->tick==b->origin_tick&&
  b->deadline_original->expires==b->expires&&b->prior_original->tick==b->prior_tick&&
  slcli_bq_receipt_foundation_linux_creation_matches_original(b->creation_original)&&
  slcli_bq_source_readable_linux_matches_original(b->readback_acquisition_original)&&
  b->writer_descriptor_original==b->creation_original->writer108&&
  b->readback_descriptor_original==b->readback_acquisition_original->descriptor&&
  b->writer_descriptor_original>=0&&b->readback_descriptor_original>=0&&
  b->deadline_original==b->creation_original->deadline_original&&
  b->readback_acquisition_original->deadline==b->deadline_original&&
  slcli_native_clock_linux_matches_original(b->prior_original)&&slcli_native_clock_linux_matches_original(b->origin_original)&&
  b->prior_original->tick>=b->creation_original->previous_tick&&
  b->prior_original->tick>=b->readback_acquisition_original->finish.tick&&
  b->prior_original->tick<b->deadline_original->expires;
}
int slcli_bq_receipt_readback_linux_bind_original(struct slcli_bq_receipt_readback_linux_bind *b,
 const struct slcli_bq_receipt_foundation_linux_creation *c,
 const struct slcli_bq_source_readable_linux *r,const struct slcli_source_deadline_original *d,
 const struct slcli_native_clock *prior){
 uint64_t previous;size_t i;struct slcli_linux_file_capture *out;int fd;
 if(!c||!r||!d||!prior||!slcli_bq_receipt_foundation_linux_creation_matches_original(c)||
  !slcli_bq_source_readable_linux_matches_original(r)||
  !slcli_arena_live_block(c->prebirth_arena_original,b,sizeof(*b))||
  !separate(b,sizeof(*b),c,sizeof(*c))||!separate(b,sizeof(*b),r,sizeof(*r))||
  !separate(b,sizeof(*b),d,sizeof(*d))||!separate(b,sizeof(*b),prior,sizeof(*prior))||
  !separate(b,sizeof(*b),c->prebirth_arena_original,sizeof(*c->prebirth_arena_original))||
  !separate(b,sizeof(*b),c->prebirth_arena_original->owner,sizeof(*c->prebirth_arena_original->owner))||
  !fresh(b,sizeof(*b)))return 0;
 b->arena_original=c->prebirth_arena_original;b->origin_original=c->origin_original;
 b->origin_tick=c->origin_tick;b->prior_tick=prior->tick;b->expires=c->expires;
 b->creation_original=c;b->readback_acquisition_original=r;b->deadline_original=d;b->prior_original=prior;
 b->writer_descriptor_original=c->writer108;b->readback_descriptor_original=r->descriptor;
 b->started=1;b->failed=1;if(!bind_source(b))return 0;previous=prior->tick;
 if(!clock_original(&b->clocks[0],&b->clock_results[0],&b->clock_returned[0],d,&previous))return 0;
 for(i=0;i<4;i++){
  out=i==0?&b->writer_before:i==1?&b->readback_before:i==2?&b->writer_after:&b->readback_after;
  fd=(i==0||i==2)?b->writer_descriptor_original:b->readback_descriptor_original;
  if(!bind_source(b))return 0;
  b->observe_results[i]=slcli_linux_file_observe(fd,out);b->observe_returned[i]=1;
  if(!b->observe_results[i]||!out->clock_observed||out->tick<previous||out->tick>=d->expires)return 0;
  previous=out->tick;
  if(!clock_original(&b->clocks[i+1],&b->clock_results[i+1],&b->clock_returned[i+1],d,&previous)||
   !bind_source(b))return 0;
 }
 if(!same_identity(&c->writer_after,&b->writer_before,1)||
  !same_identity(&b->writer_before,&b->readback_before,1)||
  !same_identity(&b->writer_before,&b->writer_after,1)||
  !same_identity(&b->readback_before,&b->readback_after,1))return 0;
 b->returned=1;b->failed=0;
 if(!slcli_bq_receipt_readback_linux_bind_matches_original(b)){b->failed=1;return 0;}return 1;
}
int slcli_bq_receipt_readback_linux_bind_matches_original(const struct slcli_bq_receipt_readback_linux_bind *b){
 size_t i;uint64_t previous;
 if(!bind_source(b)||!b->started||b->failed||!b->returned)return 0;previous=b->prior_original->tick;
 for(i=0;i<5;i++){
  const struct slcli_native_clock *clock=&b->clocks[i];
  if(!b->clock_returned[i]||!b->clock_results[i]||!slcli_native_clock_linux_matches_original(clock)||
   clock->tick<previous||clock->tick>=b->deadline_original->expires)return 0;
  previous=clock->tick;
  if(i<4){const struct slcli_linux_file_capture *r=i==0?&b->writer_before:i==1?&b->readback_before:
    i==2?&b->writer_after:&b->readback_after;
   if(!b->observe_returned[i]||!b->observe_results[i]||r->tick<previous||
    !((i==0||i==2)?slcli_linux_file_writer_observation_matches_original(b->writer_descriptor_original,r):
     slcli_linux_file_observation_matches_original(b->readback_descriptor_original,r,S_IFREG)))return 0;
   previous=r->tick;
  }
 }
 return same_identity(&b->creation_original->writer_after,&b->writer_before,1)&&
  same_identity(&b->writer_before,&b->readback_before,1)&&same_identity(&b->writer_before,&b->writer_after,1)&&
  same_identity(&b->readback_before,&b->readback_after,1);
}
static int entry_operands_same(const struct slcli_bq_receipt_readback_linux_entry_operands *a,
 const struct slcli_bq_receipt_readback_linux_entry_operands *b){
 return a->arena_original==b->arena_original&&a->deadline_original==b->deadline_original&&
 a->caller_original==b->caller_original&&a->caller_bytes==b->caller_bytes&&
 a->creation_original==b->creation_original&&a->proc_directory_original==b->proc_directory_original&&
 a->process_original==b->process_original&&a->source_path_original==b->source_path_original&&
 a->source_path_bytes==b->source_path_bytes&&a->source_output_original==b->source_output_original&&
 a->names_output_original==b->names_output_original&&a->components_output_original==b->components_output_original&&
 a->component_count==b->component_count&&a->readable_output_original==b->readable_output_original&&
 a->bind_output_original==b->bind_output_original&&a->enrollment_prior_original==b->enrollment_prior_original&&
 a->enrollment_prior_tick==b->enrollment_prior_tick&&a->origin_tick==b->origin_tick&&a->expires==b->expires;
}
static int entry_path_shape(const char *p,size_t n,size_t count){
 size_t i,start=1,actual=1;
 if(!p||n<3||n>SIZE_MAX-2||p[0]!='/'||p[n-1])return 0;
 for(i=1;i<n;i++){
  if(!p[i]&&i!=n-1)return 0;
  if(p[i]=='/'||i==n-1){size_t bytes=i-start;
   if(!bytes||(bytes==1&&p[start]=='.')||(bytes==2&&p[start]=='.'&&p[start+1]=='.')||actual==SIZE_MAX)return 0;
   ++actual;start=i+1;
  }
 }
 return actual==count;
}
static int entry_storage(const struct slcli_bq_receipt_readback_linux_entry *e,
 const struct slcli_bq_receipt_readback_linux_entry_operands *o,int unborn){
 const void *out[6],*in[13];size_t os[6],is[13],i,j;
 struct slcli_arena *a=o->arena_original;
 if(!a||!a->owner||!slcli_memory_owner_mutable(a->owner)||!o->deadline_original||!o->caller_original||
  !o->caller_bytes||!o->creation_original||!o->proc_directory_original||!o->process_original||
  !o->enrollment_prior_original||!o->source_path_original||
  !separate(e,sizeof(*e),a,sizeof(*a))||!separate(e,sizeof(*e),a->owner,sizeof(*a->owner))||
  !separate(e,sizeof(*e),o->deadline_original,sizeof(*o->deadline_original))||
  !separate(e,sizeof(*e),o->caller_original,o->caller_bytes)||
  !separate(e,sizeof(*e),o->creation_original,sizeof(*o->creation_original))||
  !separate(e,sizeof(*e),o->proc_directory_original,sizeof(*o->proc_directory_original))||
  !separate(e,sizeof(*e),o->process_original,sizeof(*o->process_original))||
  !separate(e,sizeof(*e),o->enrollment_prior_original,sizeof(*o->enrollment_prior_original))||
  !separate(e,sizeof(*e),o->source_path_original,o->source_path_bytes)||
  o->component_count>SIZE_MAX/sizeof(*o->components_output_original)||
  !slcli_arena_live_range(a,o->source_path_original,o->source_path_bytes)||
  !entry_path_shape(o->source_path_original,o->source_path_bytes,o->component_count)||
  !slcli_source_deadline_original_matches(a,o->deadline_original,o->caller_original)||
  !slcli_bq_source_namespace_linux_matches_original(o->proc_directory_original)||
  !slcli_bq_process_reference_linux_matches_original(o->process_original)||
  !slcli_native_clock_linux_matches_original(o->enrollment_prior_original)||
  o->enrollment_prior_original->tick!=o->enrollment_prior_tick||
  o->deadline_original->origin->tick!=o->origin_tick||o->deadline_original->expires!=o->expires||
  o->enrollment_prior_tick<o->origin_tick||o->enrollment_prior_tick>=o->expires||
  o->proc_directory_original->arena!=a||o->process_original->arena!=a||
  o->proc_directory_original->deadline!=o->deadline_original||o->process_original->deadline!=o->deadline_original||
  o->proc_directory_original->caller!=o->caller_original||o->process_original->caller!=o->caller_original||
  o->proc_directory_original->finish.tick>o->enrollment_prior_tick||o->process_original->finish.tick>o->enrollment_prior_tick||
  !slcli_arena_live_block(a,o->creation_original,sizeof(*o->creation_original)))return 0;
 out[0]=e;os[0]=sizeof(*e);out[1]=o->source_output_original;os[1]=sizeof(*o->source_output_original);
 out[2]=o->names_output_original;os[2]=o->source_path_bytes+2;
 out[3]=o->components_output_original;os[3]=o->component_count*sizeof(*o->components_output_original);
 out[4]=o->readable_output_original;os[4]=sizeof(*o->readable_output_original);
 out[5]=o->bind_output_original;os[5]=sizeof(*o->bind_output_original);
 in[0]=a;is[0]=sizeof(*a);in[1]=a->owner;is[1]=sizeof(*a->owner);
 in[2]=o->deadline_original;is[2]=sizeof(*o->deadline_original);in[3]=o->caller_original;is[3]=o->caller_bytes;
 in[4]=o->creation_original;is[4]=sizeof(*o->creation_original);
 in[5]=o->proc_directory_original;is[5]=sizeof(*o->proc_directory_original);
 in[6]=o->process_original;is[6]=sizeof(*o->process_original);
 in[7]=o->source_path_original;is[7]=o->source_path_bytes;
 in[8]=o->enrollment_prior_original;is[8]=sizeof(*o->enrollment_prior_original);
 in[9]=o->deadline_original->origin;is[9]=sizeof(*o->deadline_original->origin);
 in[10]=o->proc_directory_original->path;is[10]=o->proc_directory_original->path_bytes;
 in[11]=o->proc_directory_original->names;is[11]=o->proc_directory_original->path_bytes+2;
 if(o->proc_directory_original->count>SIZE_MAX/sizeof(*o->proc_directory_original->components))return 0;
 in[12]=o->proc_directory_original->components;is[12]=o->proc_directory_original->count*sizeof(*o->proc_directory_original->components);
 for(i=0;i<6;i++){
  if(!slcli_arena_live_block(a,out[i],os[i])||(unborn&&i&& !fresh(out[i],os[i])))return 0;
  for(j=0;j<i;j++)if(!separate(out[i],os[i],out[j],os[j]))return 0;
  for(j=0;j<13;j++)if(!separate(out[i],os[i],in[j],is[j]))return 0;
 }
 return unborn!=1||fresh(o->creation_original,sizeof(*o->creation_original));
}
int slcli_bq_receipt_readback_linux_entry_enroll_original(
 struct slcli_bq_receipt_readback_linux_entry *e,struct slcli_arena *a,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_bq_receipt_foundation_linux_creation *c,
 const struct slcli_bq_source_namespace_linux *proc,const struct slcli_bq_process_reference_linux_original *process,
 const char *path,size_t path_bytes,struct slcli_bq_source_namespace_linux *source,char *names,
 struct slcli_bq_source_namespace_component_linux *components,size_t count,
 struct slcli_bq_source_readable_linux *readable,struct slcli_bq_receipt_readback_linux_bind *bind,
 const struct slcli_native_clock *prior){
 struct slcli_bq_receipt_readback_linux_entry_operands o={0};
 if(!a||!d||!prior||!slcli_arena_live_block(a,e,sizeof(*e))||!fresh(e,sizeof(*e)))return 0;
 o.arena_original=a;o.deadline_original=d;o.caller_original=caller;o.caller_bytes=caller_bytes;
 o.creation_original=c;o.proc_directory_original=proc;o.process_original=process;
 o.source_path_original=path;o.source_path_bytes=path_bytes;o.source_output_original=source;o.names_output_original=names;
 o.components_output_original=components;o.component_count=count;o.readable_output_original=readable;o.bind_output_original=bind;
 o.enrollment_prior_original=prior;o.enrollment_prior_tick=prior->tick;o.origin_tick=d->origin?d->origin->tick:0;o.expires=d->expires;
 if(!entry_storage(e,&o,1))return 0;
 e->operands=o;e->retained_operands=o;e->enrolled=1;return 1;
}
static int entry_created_path(const struct slcli_bq_receipt_readback_linux_entry_operands *o){
 const struct slcli_bq_receipt_foundation_linux_creation *c=o->creation_original;
 const char *parent=c->parent_original->path;size_t n=c->parent_original->path_bytes,i,offset,total;
 if(n<2||c->name_bytes<2||n>SIZE_MAX-c->name_bytes)return 0;
 offset=n==2?1:n;total=offset+c->name_bytes;
 if(total!=o->source_path_bytes)return 0;
 for(i=0;i<n-1;i++)if(o->source_path_original[i]!=parent[i])return 0;
 if(n!=2&&o->source_path_original[n-1]!='/')return 0;
 for(i=0;i<c->name_bytes;i++)if((unsigned char)o->source_path_original[offset+i]!=c->name_original[i])return 0;
 return 1;
}
static int entry_outputs_away(const struct slcli_bq_receipt_readback_linux_entry *e,const void *p,size_t n){
 const struct slcli_bq_receipt_readback_linux_entry_operands *o=&e->operands;
 return separate(e,sizeof(*e),p,n)&&separate(o->source_output_original,sizeof(*o->source_output_original),p,n)&&
 separate(o->names_output_original,o->source_path_bytes+2,p,n)&&
 separate(o->components_output_original,o->component_count*sizeof(*o->components_output_original),p,n)&&
 separate(o->readable_output_original,sizeof(*o->readable_output_original),p,n)&&
 separate(o->bind_output_original,sizeof(*o->bind_output_original),p,n);
}
static int entry_namespace_away(const struct slcli_bq_receipt_readback_linux_entry *e,
 const struct slcli_bq_source_namespace_linux *n){
 if(!n||!entry_outputs_away(e,n,sizeof(*n))||!slcli_bq_source_namespace_linux_matches_original(n)||
  n->count>SIZE_MAX/sizeof(*n->components)||n->path_bytes>SIZE_MAX-2)return 0;
 return entry_outputs_away(e,n->path,n->path_bytes)&&entry_outputs_away(e,n->names,n->path_bytes+2)&&
 entry_outputs_away(e,n->components,n->count*sizeof(*n->components))&&
 entry_outputs_away(e,n->caller,n->caller_bytes)&&entry_outputs_away(e,n->deadline,sizeof(*n->deadline))&&
 entry_outputs_away(e,n->prior,sizeof(*n->prior))&&entry_outputs_away(e,n->origin,sizeof(*n->origin))&&
 entry_outputs_away(e,n->arena,sizeof(*n->arena))&&entry_outputs_away(e,n->arena->owner,sizeof(*n->arena->owner));
}
static int entry_process_away(const struct slcli_bq_receipt_readback_linux_entry *e,
 const struct slcli_bq_process_reference_linux_original *p){
 if(!p||!entry_outputs_away(e,p,sizeof(*p))||!slcli_bq_process_reference_linux_matches_original(p))return 0;
 return entry_outputs_away(e,p->caller,p->caller_bytes)&&entry_outputs_away(e,p->deadline,sizeof(*p->deadline))&&
 entry_outputs_away(e,p->prior,sizeof(*p->prior))&&entry_outputs_away(e,p->origin,sizeof(*p->origin))&&
 entry_outputs_away(e,p->arena,sizeof(*p->arena))&&entry_outputs_away(e,p->arena->owner,sizeof(*p->arena->owner));
}
static int entry_readable_away(const struct slcli_bq_receipt_readback_linux_entry *e,
 const struct slcli_bq_source_readable_linux *r){
 if(!r||!entry_outputs_away(e,r,sizeof(*r))||!slcli_bq_source_readable_linux_matches_original(r))return 0;
 return entry_namespace_away(e,r->source)&&entry_namespace_away(e,r->proc_directory)&&entry_process_away(e,r->process)&&
 entry_outputs_away(e,r->caller,r->caller_bytes)&&entry_outputs_away(e,r->deadline,sizeof(*r->deadline))&&
 entry_outputs_away(e,r->prior,sizeof(*r->prior))&&entry_outputs_away(e,r->origin,sizeof(*r->origin))&&
 entry_outputs_away(e,r->arena,sizeof(*r->arena))&&entry_outputs_away(e,r->arena->owner,sizeof(*r->arena->owner));
}
static int entry_born_source(const struct slcli_bq_receipt_readback_linux_entry *e){
 const struct slcli_bq_receipt_readback_linux_entry_operands *o=&e->operands;
 const struct slcli_bq_receipt_foundation_linux_creation *c=o->creation_original;
 const struct slcli_bq_source_namespace_linux *parent;const struct slcli_input_body *body;
 if(!e->enrolled||!entry_operands_same(o,&e->retained_operands)||!entry_storage(e,o,0)||
  !slcli_bq_receipt_foundation_linux_creation_matches_original(c)||c->prebirth_arena_original!=o->arena_original||
  c->deadline_original!=o->deadline_original||!entry_created_path(o)||c->previous_tick<o->enrollment_prior_tick)return 0;
 parent=c->parent_original;body=c->policy_original->installed_policy_body_original;
 if(parent->count>SIZE_MAX/sizeof(*parent->components)||body->capture_capacity>SIZE_MAX/sizeof(*body->captures))return 0;
 return entry_outputs_away(e,c->name_original,c->name_bytes)&&entry_outputs_away(e,parent,sizeof(*parent))&&
 entry_namespace_away(e,parent)&&entry_process_away(e,c->producer_process_original)&&
 entry_namespace_away(e,c->policy_original->installed_policy_namespace_original)&&
 entry_readable_away(e,c->policy_original->installed_policy_readable_original)&&
 entry_outputs_away(e,parent->path,parent->path_bytes)&&entry_outputs_away(e,parent->names,parent->path_bytes+2)&&
 entry_outputs_away(e,parent->components,parent->count*sizeof(*parent->components))&&
 entry_outputs_away(e,c->producer_process_original,sizeof(*c->producer_process_original))&&
 entry_outputs_away(e,c->policy_original,sizeof(*c->policy_original))&&entry_outputs_away(e,body,sizeof(*body))&&
 entry_outputs_away(e,body->body,body->capacity)&&entry_outputs_away(e,body->readback,body->readback_capacity)&&
 entry_outputs_away(e,body->captures,body->capture_capacity*sizeof(*body->captures))&&
 entry_outputs_away(e,body->deadline_owner,sizeof(*body->deadline_owner))&&
 entry_outputs_away(e,body->deadline_owner->capture_original,body->deadline_owner->capture_original_bytes)&&
 body->deadline_owner->row_capacity<=SIZE_MAX/sizeof(*body->deadline_owner->before)&&
 entry_outputs_away(e,body->deadline_owner->before,body->deadline_owner->row_capacity*sizeof(*body->deadline_owner->before));
}
int slcli_bq_receipt_readback_linux_entry_acquire_original(struct slcli_bq_receipt_readback_linux_entry *e){
 const struct slcli_bq_receipt_readback_linux_entry_operands *o;
 if(!e||e->started||e->failed||!entry_born_source(e)||!entry_storage(e,&e->operands,2))return 0;
 o=&e->operands;e->started=1;e->failed=1;
 e->acquired_before_result=slcli_native_clock_read(&e->acquired_before);
 if(!e->acquired_before_result||!slcli_native_clock_linux_matches_original(&e->acquired_before)||
  e->acquired_before.tick<o->creation_original->previous_tick||e->acquired_before.tick>=o->expires||!entry_born_source(e))return 0;
 e->source_result=slcli_bq_source_namespace_linux_acquire_original(o->source_output_original,o->arena_original,
  o->deadline_original,o->caller_original,o->caller_bytes,&e->acquired_before,o->source_path_original,o->source_path_bytes,
  o->names_output_original,o->components_output_original,o->component_count);e->source_returned=1;
 if(!e->source_result||!entry_born_source(e))return 0;
 e->readable_result=slcli_bq_source_readable_linux_acquire_original(o->readable_output_original,o->arena_original,
  o->deadline_original,o->caller_original,o->caller_bytes,o->source_output_original,o->proc_directory_original,
  o->process_original,&o->source_output_original->finish);e->readable_returned=1;
 if(!e->readable_result||!entry_born_source(e))return 0;
 e->bind_result=slcli_bq_receipt_readback_linux_bind_original(o->bind_output_original,o->creation_original,
  o->readable_output_original,o->deadline_original,&o->readable_output_original->finish);e->bind_returned=1;
 if(!e->bind_result||!entry_born_source(e))return 0;
 e->failed=0;if(!slcli_bq_receipt_readback_linux_entry_matches_original(e)){e->failed=1;return 0;}return 1;
}
int slcli_bq_receipt_readback_linux_entry_matches_original(const struct slcli_bq_receipt_readback_linux_entry *e){
 const struct slcli_bq_receipt_readback_linux_entry_operands *o;
 if(!e||!e->started||e->failed||!entry_born_source(e)||!e->acquired_before_result||
  !slcli_native_clock_linux_matches_original(&e->acquired_before)||
  !e->source_returned||!e->source_result||!e->readable_returned||!e->readable_result||!e->bind_returned||!e->bind_result)return 0;
 o=&e->operands;
 return e->acquired_before.tick>=o->creation_original->previous_tick&&e->acquired_before.tick<o->expires&&
 slcli_bq_source_namespace_linux_matches_original(o->source_output_original)&&
 o->source_output_original->prior==&e->acquired_before&&
 slcli_bq_source_readable_linux_matches_original(o->readable_output_original)&&
 o->readable_output_original->source==o->source_output_original&&
 o->readable_output_original->prior==&o->source_output_original->finish&&
 slcli_bq_receipt_readback_linux_bind_matches_original(o->bind_output_original)&&
 o->bind_output_original->creation_original==o->creation_original&&
 o->bind_output_original->readback_acquisition_original==o->readable_output_original&&
 o->bind_output_original->prior_original==&o->readable_output_original->finish;
}
#endif
