/* SPDX-License-Identifier: Apache-2.0 */
#include "bq_bootstrap_storage_linux.h"
#include "bq_provider_birth_linux.h"
#include "../host-service/input_deadline.h"
#if defined(__linux__)
static int apart(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&n<=UINTPTR_MAX-x&&m<=UINTPTR_MAX-y&&(x+n<=y||y+m<=x);
}
static size_t targets(const struct slcli_bq_bootstrap_storage_linux_plan *p,const void **v,size_t *n){
 size_t k=0,i;
 v[k]=p->process_offer;n[k++]=sizeof(*p->process_offer);v[k]=p->process;n[k++]=sizeof(*p->process);
 v[k]=p->process_send;n[k++]=sizeof(*p->process_send);v[k]=p->process_payload;n[k++]=64;
 v[k]=p->process_route;n[k++]=sizeof(*p->process_route);v[k]=p->creation;n[k++]=sizeof(*p->creation);
 v[k]=p->bind;n[k++]=sizeof(*p->bind);
 v[k]=p->readback_entry;n[k++]=sizeof(*p->readback_entry);
 v[k]=p->readback_source;n[k++]=sizeof(*p->readback_source);
 v[k]=p->readback_names;n[k++]=p->readback_source_path_bytes+2;
 v[k]=p->readback_components;n[k++]=p->readback_component_count*sizeof(*p->readback_components);
 v[k]=p->readback;n[k++]=sizeof(*p->readback);
 for(i=0;i<2;++i){v[k]=p->readiness[i];n[k++]=sizeof(*p->readiness[i]);v[k]=p->send[i];n[k++]=sizeof(*p->send[i]);
  v[k]=p->payload[i];n[k++]=64;v[k]=p->route[i];n[k++]=sizeof(*p->route[i]);}
 for(i=0;i<3;++i){v[k]=p->ack[i];n[k++]=sizeof(*p->ack[i]);}
 v[k]=p->composer;n[k++]=sizeof(*p->composer);return k;
}
static int raw_away(const struct slcli_bq_bootstrap_storage_linux_plan *p,const void *b,size_t bytes){
 const void *v[24];size_t n[24],count,i;
 if(!apart(p,sizeof(*p),b,bytes))return 0;
 count=targets(p,v,n);for(i=0;i<count;i++)if(!apart(v[i],n[i],b,bytes))return 0;return 1;
}
static int namespace_away(const struct slcli_bq_bootstrap_storage_linux_plan *p,const struct slcli_bq_source_namespace_linux *s){
 if(!s||!raw_away(p,s,sizeof(*s))||!slcli_bq_source_namespace_linux_matches_original(s)||
  s->count>SIZE_MAX/sizeof(*s->components)||s->path_bytes>SIZE_MAX-2)return 0;
 return raw_away(p,s->path,s->path_bytes)&&raw_away(p,s->names,s->path_bytes+2)&&
 raw_away(p,s->components,s->count*sizeof(*s->components))&&raw_away(p,s->caller,s->caller_bytes)&&
 raw_away(p,s->deadline,sizeof(*s->deadline))&&raw_away(p,s->prior,sizeof(*s->prior))&&
 raw_away(p,s->origin,sizeof(*s->origin))&&raw_away(p,s->arena,sizeof(*s->arena))&&raw_away(p,s->arena->owner,sizeof(*s->arena->owner));
}
static int process_away(const struct slcli_bq_bootstrap_storage_linux_plan *p,const struct slcli_bq_process_reference_linux_original *s){
 if(!s||!raw_away(p,s,sizeof(*s))||!slcli_bq_process_reference_linux_matches_original(s))return 0;
 return raw_away(p,s->caller,s->caller_bytes)&&raw_away(p,s->deadline,sizeof(*s->deadline))&&
 raw_away(p,s->prior,sizeof(*s->prior))&&raw_away(p,s->origin,sizeof(*s->origin))&&
 raw_away(p,s->arena,sizeof(*s->arena))&&raw_away(p,s->arena->owner,sizeof(*s->arena->owner));
}
static int inputs_away(const struct slcli_bq_bootstrap_storage_linux_plan *p){
 const struct slcli_bq_retention_policy_linux_original *policy=p->policy_original;
 const struct slcli_input_body *b;const struct slcli_input_deadline_owner *d;
 const struct slcli_bq_source_readable_linux *r;size_t i,n,offset,total,components;
 if(!policy||!p->foundation_name_original||p->foundation_name_bytes<2||
  !raw_away(p,policy,sizeof(*policy))||!raw_away(p,p->foundation_name_original,p->foundation_name_bytes)||
  !slcli_arena_live_block(p->arena_original,p->foundation_name_original,p->foundation_name_bytes)||
  !slcli_bq_retention_policy_linux_body_matches_original(policy)||
  !namespace_away(p,p->foundation_parent_original)||!namespace_away(p,p->readback_proc_original)||
  p->foundation_parent_original->leaf_kind!=S_IFDIR||p->readback_proc_original->leaf_kind!=S_IFDIR)return 0;
 for(i=0;i+1<p->foundation_name_bytes;i++)if(!p->foundation_name_original[i]||p->foundation_name_original[i]=='/')return 0;
 if(p->foundation_name_original[p->foundation_name_bytes-1]||
  (p->foundation_name_bytes==2&&p->foundation_name_original[0]=='.')||
  (p->foundation_name_bytes==3&&p->foundation_name_original[0]=='.'&&p->foundation_name_original[1]=='.'))return 0;
 n=p->foundation_parent_original->path_bytes;
 if(n<2||n>SIZE_MAX-p->foundation_name_bytes)return 0;
 offset=n==2?1:n;total=offset+p->foundation_name_bytes;
 if(total!=p->readback_source_path_bytes)return 0;
 for(i=0;i<n-1;i++)if(p->readback_source_path[i]!=p->foundation_parent_original->path[i])return 0;
 if(n!=2&&p->readback_source_path[n-1]!='/')return 0;
 for(i=0;i<p->foundation_name_bytes;i++)if((unsigned char)p->readback_source_path[offset+i]!=p->foundation_name_original[i])return 0;
 if(p->foundation_parent_original->count==SIZE_MAX)return 0;
 components=p->foundation_parent_original->count+1;
 if(p->readback_component_count!=components)return 0;
 b=policy->installed_policy_body_original;d=b->deadline_owner;r=policy->installed_policy_readable_original;
 if(b->capture_capacity>SIZE_MAX/sizeof(*b->captures)||d->row_capacity>SIZE_MAX/sizeof(*d->before)||
  d->arena!=p->arena_original||d->deadline!=p->deadline_original||d->caller_original!=p->caller_original||d->caller_bytes!=p->caller_bytes||
  p->foundation_parent_original->arena!=p->arena_original||p->readback_proc_original->arena!=p->arena_original||
  p->foundation_parent_original->deadline!=p->deadline_original||p->readback_proc_original->deadline!=p->deadline_original||
  p->foundation_parent_original->caller!=p->caller_original||p->readback_proc_original->caller!=p->caller_original)return 0;
 return namespace_away(p,policy->installed_policy_namespace_original)&&raw_away(p,r,sizeof(*r))&&
 namespace_away(p,r->source)&&namespace_away(p,r->proc_directory)&&process_away(p,r->process)&&
 raw_away(p,r->caller,r->caller_bytes)&&raw_away(p,r->deadline,sizeof(*r->deadline))&&
 raw_away(p,r->prior,sizeof(*r->prior))&&raw_away(p,r->origin,sizeof(*r->origin))&&
 raw_away(p,r->arena,sizeof(*r->arena))&&raw_away(p,r->arena->owner,sizeof(*r->arena->owner))&&
 raw_away(p,b,sizeof(*b))&&raw_away(p,b->body,b->capacity)&&raw_away(p,b->readback,b->readback_capacity)&&
 raw_away(p,b->captures,b->capture_capacity*sizeof(*b->captures))&&raw_away(p,d,sizeof(*d))&&
 raw_away(p,d->before,d->row_capacity*sizeof(*d->before))&&raw_away(p,d->capture_original,d->capture_original_bytes);
}
static int expectations_away(const struct slcli_bq_bootstrap_storage_linux_plan *p){
 const struct slcli_bq_provider_expectations *e=p->expectations_original;size_t i;
 if(!e||!raw_away(p,e,sizeof(*e))||e->command_count>SIZE_MAX/sizeof(*e->requirements_per_command)||
  e->subject_count>SIZE_MAX/sizeof(*e->subjects)||
  !raw_away(p,e->requirements_per_command,e->command_count*sizeof(*e->requirements_per_command))||
  !raw_away(p,e->subjects,e->subject_count*sizeof(*e->subjects))||
  !slcli_bq_provider_expectations_bootstrap_shape(e,p->roster_original->phase)||
  e->command_count!=p->roster_original->command_count||e->subject_count!=p->roster_original->subject_count)return 0;
 if(e->principal.windows_sid_bytes&&!raw_away(p,e->principal.windows_sid,e->principal.windows_sid_bytes))return 0;
 if(e->producer.entry_symbol_bytes&&!raw_away(p,e->producer.entry_symbol,e->producer.entry_symbol_bytes))return 0;
 for(i=0;i<e->command_count;i++)if(e->requirements_per_command[i]!=p->roster_original->requirements_per_command[i])return 0;
 for(i=0;i<e->subject_count;i++){
  if(e->subjects[i].reader_role_bytes&&!raw_away(p,e->subjects[i].reader_role,e->subjects[i].reader_role_bytes))return 0;
  if(e->subjects[i].selector.member!=p->roster_original->subjects[i].member||
   e->subjects[i].selector.member_bytes!=p->roster_original->subjects[i].member_bytes||
   e->subjects[i].selector.family!=p->roster_original->subjects[i].family)return 0;
 }
 return e->subjects[0].source_occurrence.native_file_bytes==p->policy_original->whole_lineage_bytes;
}
static int shape(const struct slcli_bq_bootstrap_storage_linux_plan *p,int fresh){
 const void *v[24],*originals[8];size_t n[24],on[8],count,i,j;
 if(!p||!p->arena_original||!p->owner_original||!p->deadline_original||!p->origin_original||!p->caller_original||!p->caller_bytes||!p->roster_original||
  !apart(p,sizeof(*p),p->arena_original,sizeof(*p->arena_original))||
  !apart(p,sizeof(*p),p->owner_original,sizeof(*p->owner_original))||
    !apart(p,sizeof(*p),p->deadline_original,sizeof(*p->deadline_original))||
  !apart(p,sizeof(*p),p->origin_original,sizeof(*p->origin_original))||
  !apart(p,sizeof(*p),p->caller_original,p->caller_bytes)||
  !apart(p,sizeof(*p),p->roster_original,sizeof(*p->roster_original))||
  !p->readback_source_path||p->readback_source_path_bytes<3||p->readback_source_path_bytes>SIZE_MAX-2||
  !p->readback_component_count||p->readback_component_count>SIZE_MAX/sizeof(*p->readback_components)||
  !apart(p,sizeof(*p),p->readback_source_path,p->readback_source_path_bytes)||
  !slcli_arena_live_range(p->arena_original,p->readback_source_path,p->readback_source_path_bytes)||
  p->arena_original->owner!=p->owner_original||p->arena_original->extent!=p->extent_index_original||p->arena_original->acquisition!=p->acquisition_original||
  !slcli_arena_live_block(p->arena_original,p,sizeof(*p))||
  !slcli_arena_live_block(p->arena_original,p->caller_original,p->caller_bytes)||
  !slcli_arena_live_block(p->arena_original,p->roster_original,sizeof(*p->roster_original))||
  !slcli_source_deadline_original_matches(p->arena_original,p->deadline_original,p->caller_original)||
  p->deadline_original->origin!=p->origin_original)return 0;
 originals[0]=p;on[0]=sizeof(*p);originals[1]=p->arena_original;on[1]=sizeof(*p->arena_original);
 originals[2]=p->owner_original;on[2]=sizeof(*p->owner_original);originals[3]=p->caller_original;on[3]=p->caller_bytes;
 originals[4]=p->deadline_original;on[4]=sizeof(*p->deadline_original);originals[5]=p->origin_original;on[5]=sizeof(*p->origin_original);
 originals[6]=p->roster_original;on[6]=sizeof(*p->roster_original);
 originals[7]=p->readback_source_path;on[7]=p->readback_source_path_bytes;
 if(p->roster_original->command_count>SIZE_MAX/sizeof(*p->roster_original->requirements_per_command)||
  p->roster_original->subject_count>SIZE_MAX/sizeof(*p->roster_original->subjects))return 0;
 count=targets(p,v,n);
 for(i=0;i<count;++i){
  if(!slcli_arena_live_block(p->arena_original,v[i],n[i]))return 0;
  for(j=0;j<8;++j)if(!apart(v[i],n[i],originals[j],on[j]))return 0;
  for(j=0;j<i;++j)if(!apart(v[i],n[i],v[j],n[j]))return 0;
  if(p->roster_original->command_count&&!apart(v[i],n[i],p->roster_original->requirements_per_command,
   p->roster_original->command_count*sizeof(*p->roster_original->requirements_per_command)))return 0;
  if(p->roster_original->subject_count&&!apart(v[i],n[i],p->roster_original->subjects,
   p->roster_original->subject_count*sizeof(*p->roster_original->subjects)))return 0;
  if(fresh)for(j=0;j<n[i];++j)if(((const unsigned char *)v[i])[j])return 0;
 }
 return inputs_away(p)&&expectations_away(p);
}
int slcli_bq_bootstrap_storage_linux_same_original(const struct slcli_bq_bootstrap_storage_linux_plan *a,const struct slcli_bq_bootstrap_storage_linux_plan *b){
 size_t i;
 if(!b||!shape(a,0)||!apart(a,sizeof(*a),b,sizeof(*b))||
  a->arena_original!=b->arena_original||a->owner_original!=b->owner_original||a->extent_index_original!=b->extent_index_original||
  a->acquisition_original!=b->acquisition_original||a->caller_original!=b->caller_original||a->caller_bytes!=b->caller_bytes||
  a->deadline_original!=b->deadline_original||a->origin_original!=b->origin_original||a->roster_original!=b->roster_original||
  a->process_offer!=b->process_offer||a->process!=b->process||a->process_send!=b->process_send||
  a->process_payload!=b->process_payload||a->process_route!=b->process_route||a->creation!=b->creation||a->bind!=b->bind||a->composer!=b->composer||
  a->readback_entry!=b->readback_entry||a->readback_source_path!=b->readback_source_path||
  a->readback_source_path_bytes!=b->readback_source_path_bytes||a->readback_source!=b->readback_source||
  a->readback_names!=b->readback_names||a->readback_components!=b->readback_components||
  a->readback_component_count!=b->readback_component_count||a->readback!=b->readback)return 0;
 if(a->foundation_parent_original!=b->foundation_parent_original||a->readback_proc_original!=b->readback_proc_original||
  a->policy_original!=b->policy_original||a->foundation_name_original!=b->foundation_name_original||
  a->foundation_name_bytes!=b->foundation_name_bytes||a->expectations_original!=b->expectations_original)return 0;
 for(i=0;i<2;++i)if(a->readiness[i]!=b->readiness[i]||a->send[i]!=b->send[i]||a->payload[i]!=b->payload[i]||a->route[i]!=b->route[i])return 0;
 for(i=0;i<3;++i)if(a->ack[i]!=b->ack[i])return 0;return 1;
}
int slcli_bq_bootstrap_storage_linux_fresh_original(const struct slcli_bq_bootstrap_storage_linux_plan *p){return shape(p,1);}
int slcli_bq_bootstrap_storage_linux_matches_original(const struct slcli_bq_bootstrap_storage_linux_plan *p){return shape(p,0);}
int slcli_bq_bootstrap_storage_linux_away_original(const struct slcli_bq_bootstrap_storage_linux_plan *p,const void *b,size_t bytes){
 const void *v[24];size_t n[24],count,i;
 if(!shape(p,0)||!apart(p,sizeof(*p),b,bytes))return 0;
 count=targets(p,v,n);for(i=0;i<count;++i)if(!apart(v[i],n[i],b,bytes))return 0;return 1;
}
static int composer_source(const struct slcli_bq_bootstrap_linux_original *c){
 const struct slcli_bq_bootstrap_storage_linux_plan *p;
 if(!c||!c->birth_original||!(p=c->plan_original)||
  !slcli_bq_bootstrap_storage_linux_same_original(p,&c->plan_snapshot)||p->composer!=c||
  !slcli_bq_bootstrap_storage_linux_away_original(p,c->birth_original,sizeof(*c->birth_original))||
  !slcli_bq_provider_linux_release_matches_original(c->birth_original)||
  !slcli_bq_bootstrap_storage_linux_same_original(p,&c->birth_original->bootstrap_storage_original)||
  c->birth_original->first_offer!=p->process_offer||c->birth_original->first_route!=p->process_route||
  !slcli_bq_process_reference_linux_offer_matches_original(p->process_offer)||
  c->expectations_original!=p->expectations_original||c->prior_original!=&c->birth_original->release_after||
  !slcli_native_clock_linux_matches_original(c->prior_original)||c->prior_tick!=c->prior_original->tick)return 0;
 return c->birth_original->arena==p->arena_original&&c->birth_original->deadline==p->deadline_original&&
 c->birth_original->caller==p->caller_original&&c->birth_original->caller_bytes==p->caller_bytes;
}
int slcli_bq_bootstrap_linux_capture_raw_original(struct slcli_bq_provider_linux_birth_original *birth){
 const struct slcli_bq_bootstrap_storage_linux_plan *p;struct slcli_bq_bootstrap_linux_original *c;
 const struct slcli_native_clock *prior;const struct slcli_bq_channel_linux_original *channel;
 const unsigned char *bytes;size_t i,j;int fd;
 if(!birth||!slcli_bq_provider_linux_release_matches_original(birth)||
  !slcli_bq_provider_linux_bootstrap_borrowed_away_original(birth)||!birth->inputs||
  !(p=birth->inputs->bootstrap_storage)||!slcli_bq_bootstrap_storage_linux_matches_original(p)||
  !(c=p->composer)||!slcli_arena_live_block(p->arena_original,c,sizeof(*c)))return 0;
 bytes=(const unsigned char *)c;for(j=0;j<sizeof(*c);j++)if(bytes[j])return 0;
 c->birth_original=birth;c->plan_original=p;c->plan_snapshot=*p;c->expectations_original=p->expectations_original;
 c->prior_original=&birth->release_after;c->prior_tick=birth->release_after.tick;c->started=1;c->failed=1;
 if(!composer_source(c))return 0;channel=birth->inputs->channel;prior=c->prior_original;
 c->enrollment_result=slcli_bq_receipt_readback_linux_entry_enroll_original(p->readback_entry,p->arena_original,
  p->deadline_original,p->caller_original,p->caller_bytes,p->creation,p->readback_proc_original,p->process,
  p->readback_source_path,p->readback_source_path_bytes,p->readback_source,p->readback_names,
  p->readback_components,p->readback_component_count,p->readback,p->bind,prior);c->enrollment_returned=1;
 if(!c->enrollment_result||!composer_source(c))return 0;c->next=1;
 c->creation_result=slcli_bq_receipt_foundation_linux_create_original(p->creation,p->arena_original,p->deadline_original,
  prior,p->foundation_parent_original,p->process,p->policy_original,p->foundation_name_original,p->foundation_name_bytes);
 c->creation_returned=1;if(!c->creation_result||!composer_source(c))return 0;c->next=2;
 c->acquisition_result=slcli_bq_receipt_readback_linux_entry_acquire_original(p->readback_entry);c->acquisition_returned=1;
 if(!c->acquisition_result||!composer_source(c))return 0;c->next=3;prior=&p->bind->clocks[4];
 for(i=0;i<2;i++){
  if(!composer_source(c))return 0;
  c->route_results[i]=slcli_bq_provider_roster_offer(p->route[i],p->roster_original,i+1);c->route_returned[i]=1;
  if(!c->route_results[i])return 0;
  c->encode_results[i]=slcli_bq_subject_route_encode(p->payload[i],SLCLI_BQ_SUBJECT_RECORD_BYTES,p->route[i]);c->encode_returned[i]=1;
  if(!c->encode_results[i]||!composer_source(c))return 0;
  c->readiness_results[i]=slcli_bq_readiness_linux_preowned_original(p->readiness[i],p->arena_original,
   channel->endpoints[0],POLLOUT,p->deadline_original,p->caller_original,p->caller_bytes,prior);c->readiness_returned[i]=1;
  if(!c->readiness_results[i]||!composer_source(c))return 0;
  fd=i?p->readback->descriptor:p->creation->writer108;
  c->send_results[i]=slcli_bq_send_linux_preowned_original(p->send[i],p->arena_original,channel->endpoints[0],fd,
   p->payload[i],SLCLI_BQ_SUBJECT_RECORD_BYTES,p->deadline_original,p->caller_original,p->caller_bytes,
   &p->readiness[i]->validation_after);c->send_returned[i]=1;
  if(!c->send_results[i]||!composer_source(c))return 0;prior=&p->send[i]->validation_after;c->next=(uint32_t)i+4;
 }
 for(i=0;i<3;i++){
  if(!composer_source(c))return 0;
  c->ack_results[i]=slcli_bq_bootstrap_ack_linux_original(p->ack[i],p->arena_original,channel,
   i?p->route[i-1]:p->process_route,p->deadline_original,p->caller_original,p->caller_bytes,prior);c->ack_returned[i]=1;
  if(!c->ack_results[i]||!composer_source(c))return 0;prior=&p->ack[i]->validation_after;c->next=(uint32_t)i+6;
 }
 c->validation_result=slcli_native_clock_read(&c->validation_after);c->validation_returned=1;
 if(!c->validation_result||!slcli_native_clock_linux_matches_original(&c->validation_after)||
  c->validation_after.tick<prior->tick||c->validation_after.tick>=p->deadline_original->expires||!composer_source(c))return 0;
 c->next=9;c->complete=1;c->failed=0;
 if(!slcli_bq_bootstrap_linux_capture_matches_original(c)){c->failed=1;return 0;}return 1;
}
int slcli_bq_bootstrap_linux_capture_matches_original(const struct slcli_bq_bootstrap_linux_original *c){
 const struct slcli_bq_bootstrap_storage_linux_plan *p;const struct slcli_bq_channel_linux_original *channel;
 const struct slcli_native_clock *prior;struct slcli_bq_subject_route expected={0},decoded={0};size_t i;
 if(!c||!c->started||c->failed||!c->complete||c->next!=9||!composer_source(c)||
  !c->enrollment_returned||!c->enrollment_result||!c->creation_returned||!c->creation_result||
  !c->acquisition_returned||!c->acquisition_result)return 0;
 p=c->plan_original;channel=c->birth_original->inputs->channel;
 if(!slcli_bq_receipt_foundation_linux_creation_matches_original(p->creation)||
  p->creation->parent_original!=p->foundation_parent_original||p->creation->policy_original!=p->policy_original||
  p->creation->producer_process_original!=p->process||p->creation->name_original!=p->foundation_name_original||
  p->creation->name_bytes!=p->foundation_name_bytes||p->creation->prior_original!=c->prior_original||
  !slcli_bq_receipt_readback_linux_entry_matches_original(p->readback_entry)||
  p->readback_entry->operands.creation_original!=p->creation||p->readback_entry->operands.source_output_original!=p->readback_source||
  p->readback_entry->operands.readable_output_original!=p->readback||p->readback_entry->operands.bind_output_original!=p->bind||
  p->readback_entry->operands.enrollment_prior_original!=c->prior_original)return 0;
 prior=&p->bind->clocks[4];
 for(i=0;i<2;i++){
  if(!c->route_returned[i]||!c->route_results[i]||!c->encode_returned[i]||!c->encode_results[i]||
   !c->readiness_returned[i]||!c->readiness_results[i]||!c->send_returned[i]||!c->send_results[i]||
   !slcli_bq_provider_roster_offer(&expected,p->roster_original,i+1)||
   !slcli_bq_subject_route_equal(&expected,p->route[i])||
   !slcli_bq_subject_route_decode(&decoded,p->payload[i],SLCLI_BQ_SUBJECT_RECORD_BYTES)||
   !slcli_bq_subject_route_equal(&expected,&decoded)||
   !slcli_bq_readiness_linux_matches_original(p->readiness[i])||p->readiness[i]->prior!=prior||
   p->readiness[i]->endpoint!=channel->endpoints[0]||p->readiness[i]->requested_events!=POLLOUT||
   !slcli_bq_send_linux_matches_original(p->send[i])||p->send[i]->prior!=&p->readiness[i]->validation_after||
   p->send[i]->endpoint!=channel->endpoints[0]||p->send[i]->source_fd!=(i?p->readback->descriptor:p->creation->writer108)||
   p->send[i]->payload!=p->payload[i]||p->send[i]->payload_bytes!=SLCLI_BQ_SUBJECT_RECORD_BYTES)return 0;
  prior=&p->send[i]->validation_after;
 }
 for(i=0;i<3;i++){
  if(!c->ack_returned[i]||!c->ack_results[i]||!slcli_bq_bootstrap_ack_linux_matches_original(p->ack[i])||
   p->ack[i]->prior!=prior||p->ack[i]->channel_original!=channel||p->ack[i]->offer_original!=(i?p->route[i-1]:p->process_route))return 0;
  prior=&p->ack[i]->validation_after;
 }
 return c->validation_returned&&c->validation_result&&slcli_native_clock_linux_matches_original(&c->validation_after)&&
 c->validation_after.tick>=prior->tick&&c->validation_after.tick<p->deadline_original->expires;
}
#endif
