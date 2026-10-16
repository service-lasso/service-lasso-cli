/* SPDX-License-Identifier: Apache-2.0 */
#include "bq_subject_transfer.h"
#include "bq_provider_expectations.h"

static const unsigned char magic[8]={'S','L','B','Q','S','U','B','1'};
static int apart(const void *a,size_t an,const void *b,size_t bn){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&an&&bn&&an<=UINTPTR_MAX-x&&bn<=UINTPTR_MAX-y&&
  (x+an<=y||y+bn<=x);
}
static uint32_t read32(const unsigned char *p){
 return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
static uint64_t read64(const unsigned char *p){
 return (uint64_t)read32(p)|((uint64_t)read32(p+4)<<32);
}
static void write32(unsigned char *p,uint32_t v){
 unsigned i;for(i=0;i<4;++i)p[i]=(unsigned char)(v>>(8*i));
}
static void write64(unsigned char *p,uint64_t v){
 unsigned i;for(i=0;i<8;++i)p[i]=(unsigned char)(v>>(8*i));
}
static int shape(const struct slcli_bq_subject_route *r){
 return r&&r->message>=SLCLI_BQ_SUBJECT_OFFER&&r->message<=SLCLI_BQ_SUBJECT_CLOSED&&
  r->cut<=SLCLI_BQ_SUBJECT_INITIAL_PROVIDER&&r->cut!=SLCLI_BQ_SUBJECT_R2&&r->sequence&&r->member&&
  r->role>=SLCLI_BQ_SUBJECT_SOURCE&&r->role<=SLCLI_BQ_SUBJECT_REFERENCE;
}
int slcli_bq_subject_route_encode(unsigned char *p,size_t n,
 const struct slcli_bq_subject_route *r){
 unsigned i;
 if(n!=SLCLI_BQ_SUBJECT_RECORD_BYTES||!shape(r)||!apart(p,n,r,sizeof(*r)))return 0;
 for(i=0;i<8;++i)p[i]=magic[i];
 write32(p+8,SLCLI_BQ_SUBJECT_RECORD_VERSION);write32(p+12,SLCLI_BQ_SUBJECT_RECORD_BYTES);
 write32(p+16,r->message);write32(p+20,r->cut);write64(p+24,r->sequence);
 write64(p+32,r->member);write32(p+40,r->phase);write32(p+44,r->command);
 write32(p+48,r->requirement);write32(p+52,r->role);write64(p+56,r->member_bytes);
 return 1;
}
int slcli_bq_subject_route_decode(struct slcli_bq_subject_route *out,
 const unsigned char *p,size_t n){
 struct slcli_bq_subject_route r;unsigned i;
 if(n!=SLCLI_BQ_SUBJECT_RECORD_BYTES||!apart(out,sizeof(*out),p,n))return 0;
 for(i=0;i<8;++i)if(p[i]!=magic[i])return 0;
 if(read32(p+8)!=SLCLI_BQ_SUBJECT_RECORD_VERSION||read32(p+12)!=n)return 0;
 r.message=read32(p+16);r.cut=read32(p+20);r.sequence=read64(p+24);r.member=read64(p+32);
 r.phase=read32(p+40);r.command=read32(p+44);r.requirement=read32(p+48);
 r.role=read32(p+52);r.member_bytes=read64(p+56);
 if(!shape(&r))return 0;
 *out=r;return 1;
}
int slcli_bq_subject_route_equal(const struct slcli_bq_subject_route *a,
 const struct slcli_bq_subject_route *b){
 return shape(a)&&shape(b)&&a->message==b->message&&a->cut==b->cut&&
  a->sequence==b->sequence&&a->member==b->member&&a->phase==b->phase&&
  a->command==b->command&&a->requirement==b->requirement&&a->role==b->role&&
  a->member_bytes==b->member_bytes;
}
static int roster_shape(const struct slcli_bq_provider_roster *r,const void *out,size_t bytes){
 size_t i,total=0,commands,subjects;
 if(!r||!out||!apart(out,bytes,r,sizeof(*r))||r->phase>2||!r->process_member||
  r->command_count>UINT32_MAX||r->command_count>SIZE_MAX/sizeof(*r->requirements_per_command)||
  r->subject_count>SIZE_MAX/sizeof(*r->subjects)||r->subject_count==SIZE_MAX||
  (uint64_t)r->subject_count==UINT64_MAX)return 0;
 commands=r->command_count*sizeof(*r->requirements_per_command);
 subjects=r->subject_count*sizeof(*r->subjects);
 if((commands&&(!r->requirements_per_command||
   !apart(out,bytes,r->requirements_per_command,commands)))||
  (subjects&&(!r->subjects||!apart(out,bytes,r->subjects,subjects))))return 0;
 for(i=0;i<r->command_count;++i){
  size_t n=r->requirements_per_command[i];
  if(n>SIZE_MAX-total)return 0;
  total+=n;
 }
 if(total!=r->subject_count)return 0;
 for(i=0;i<total;++i)if(!r->subjects[i].member||
  r->subjects[i].family<SLCLI_BQ_SUBJECT_SOURCE||r->subjects[i].family>SLCLI_BQ_SUBJECT_IMAGE)return 0;
 return 1;
}
int slcli_bq_provider_roster_count(size_t *out,const struct slcli_bq_provider_roster *r){
 if(!roster_shape(r,out,sizeof(*out)))return 0;
 *out=r->subject_count+1;return 1;
}
int slcli_bq_provider_roster_offer(struct slcli_bq_subject_route *out,
 const struct slcli_bq_provider_roster *r,size_t index){
 struct slcli_bq_subject_route route;size_t command,remaining;
 if(!roster_shape(r,out,sizeof(*out))||index>r->subject_count)return 0;
 route.message=SLCLI_BQ_SUBJECT_OFFER;route.cut=SLCLI_BQ_SUBJECT_INITIAL_PROVIDER;
 route.sequence=(uint64_t)index+1;route.phase=r->phase;
 if(!index){
  route.member=r->process_member;route.command=UINT32_MAX;route.requirement=UINT32_MAX;
  route.role=SLCLI_BQ_SUBJECT_REFERENCE;route.member_bytes=0;
 }else{
  remaining=index-1;
  for(command=0;command<r->command_count;++command){
   if(remaining<r->requirements_per_command[command])break;
   remaining-=r->requirements_per_command[command];
  }
  if(command==r->command_count)return 0;
  route.command=(uint32_t)command;route.requirement=(uint32_t)remaining;
  route.member=r->subjects[index-1].member;route.member_bytes=r->subjects[index-1].member_bytes;
  route.role=r->subjects[index-1].family;
 }
 *out=route;return 1;
}

#define SLCLI_RETENTION_FIELDS(X) \
 X(maximum_bytes,0) X(maximum_record_bytes,1) X(partial_attempts,2) X(failure_tail_bytes,3) \
 X(whole_lineage_bytes,4) X(whole_lineage_attempts,5) X(whole_lineage_nodes,6) \
 X(records_offset,7) X(records_bytes,8) X(index_offset,9) X(index_bytes,10) \
 X(terminal_offset,11) X(terminal_bytes,12) X(native_cell_bytes,13) \
 X(source_window_bytes,14) X(write_window_bytes,15) X(readback_window_bytes,16) \
 X(pair_window_bytes,17) X(active_index_bytes,18)
static const unsigned char retention_magic[8]={'S','L','B','Q','R','T','P','1'};
static uint32_t retention_read32(const unsigned char *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static void retention_write32(unsigned char *p,uint32_t n){unsigned i;for(i=0;i<4;++i)p[i]=(unsigned char)(n>>(8*i));}
int slcli_bq_retention_policy_values_shape(const struct slcli_bq_retention_policy_values *p,uint64_t native_bytes,uint64_t index_bytes){
 uint64_t records,indexes;
 if(!p||!native_bytes||!index_bytes||!p->maximum_bytes||!p->maximum_record_bytes||!p->partial_attempts||!p->failure_tail_bytes||
  !p->whole_lineage_bytes||!p->whole_lineage_attempts||!p->whole_lineage_nodes||
  p->whole_lineage_bytes>p->maximum_bytes||p->partial_attempts>p->whole_lineage_attempts||
  native_bytes>UINT64_MAX-48-16384||p->maximum_record_bytes<native_bytes+48+16384||
  p->whole_lineage_attempts>UINT64_MAX/p->maximum_record_bytes||p->whole_lineage_nodes>UINT64_MAX/index_bytes)return 0;
 records=p->whole_lineage_attempts*p->maximum_record_bytes;indexes=p->whole_lineage_nodes*index_bytes;
 if(records>p->whole_lineage_bytes||indexes>p->whole_lineage_bytes-records)return 0;
 return !p->records_offset&&p->records_bytes==records&&p->index_offset==records&&p->index_bytes==indexes&&
  p->terminal_offset==records+indexes&&p->terminal_bytes==p->whole_lineage_bytes-records-indexes&&
  p->terminal_bytes>=p->failure_tail_bytes&&p->native_cell_bytes==native_bytes&&p->active_index_bytes==index_bytes&&
  p->source_window_bytes==16384&&p->write_window_bytes==16384&&p->readback_window_bytes==16384&&p->pair_window_bytes==32768;
}
int slcli_bq_retention_policy_encode(unsigned char *out,size_t bytes,const struct slcli_bq_retention_policy_values *p,uint64_t native_bytes,uint64_t index_bytes){
 unsigned char next[SLCLI_BQ_RETENTION_POLICY_BYTES];uint64_t fields[19];size_t i,j;
 if(bytes!=sizeof(next)||!apart(out,bytes,p,sizeof(*p))||!slcli_bq_retention_policy_values_shape(p,native_bytes,index_bytes))return 0;
#define PACK(name,index) fields[index]=p->name;
 SLCLI_RETENTION_FIELDS(PACK)
#undef PACK
 for(i=0;i<8;++i)next[i]=retention_magic[i];retention_write32(next+8,SLCLI_BQ_RETENTION_POLICY_VERSION);retention_write32(next+12,sizeof(next));
 for(i=0;i<19;++i)for(j=0;j<8;++j)next[16+i*8+j]=(unsigned char)(fields[i]>>(8*j));
 for(i=0;i<sizeof(next);++i)out[i]=next[i];return 1;
}
int slcli_bq_retention_policy_decode(struct slcli_bq_retention_policy_values *out,const unsigned char *raw,size_t bytes,uint64_t native_bytes,uint64_t index_bytes){
 struct slcli_bq_retention_policy_values next={0};uint64_t fields[19];size_t i,j;
 if(bytes!=SLCLI_BQ_RETENTION_POLICY_BYTES||!apart(out,sizeof(*out),raw,bytes))return 0;
 for(i=0;i<8;++i)if(raw[i]!=retention_magic[i])return 0;
 if(retention_read32(raw+8)!=SLCLI_BQ_RETENTION_POLICY_VERSION||retention_read32(raw+12)!=bytes)return 0;
 for(i=0;i<19;++i){fields[i]=0;for(j=0;j<8;++j)fields[i]|=(uint64_t)raw[16+i*8+j]<<(8*j);}
#define UNPACK(name,index) next.name=fields[index];
 SLCLI_RETENTION_FIELDS(UNPACK)
#undef UNPACK
 if(!slcli_bq_retention_policy_values_shape(&next,native_bytes,index_bytes))return 0;*out=next;return 1;
}
#undef SLCLI_RETENTION_FIELDS

/* Pure dimension-preserving arithmetic. No native/source/finance admission. */
static int resource_memory(const struct slcli_bq_resource_cost *c,uint64_t *n){
 uint64_t value;
 if(!c||!n||c->user_ram_bytes>UINT64_MAX-c->kernel_ram_bytes)return 0;
 value=c->user_ram_bytes+c->kernel_ram_bytes;
 if(value>UINT64_MAX-c->cache_ram_bytes)return 0;
 *n=value+c->cache_ram_bytes;return 1;
}
int slcli_bq_owner_resource_profile_add(struct slcli_bq_owner_resource_profile *out,
 const struct slcli_bq_owner_resource_profile *before,
 const struct slcli_bq_resource_cost *cost){
 struct slcli_bq_owner_resource_profile next={0};uint64_t old_memory;
 if(!out||!before||!cost||!apart(out,sizeof(*out),before,sizeof(*before))||
 !apart(out,sizeof(*out),cost,sizeof(*cost))||
 !resource_memory(&before->simultaneous,&old_memory)||
 old_memory!=before->combined_memory_bytes)return 0;
#define ADD_RESOURCE(field) \
 if(before->simultaneous.field>UINT64_MAX-cost->field)return 0; \
 next.simultaneous.field=before->simultaneous.field+cost->field
 ADD_RESOURCE(user_ram_bytes);ADD_RESOURCE(user_virtual_bytes);
 ADD_RESOURCE(kernel_ram_bytes);ADD_RESOURCE(cache_ram_bytes);
 ADD_RESOURCE(retained_storage_bytes);ADD_RESOURCE(native_references);
 ADD_RESOURCE(physical_extent_events);ADD_RESOURCE(native_attempts);
 ADD_RESOURCE(pure_attempts);ADD_RESOURCE(cpu_nanoseconds);
#undef ADD_RESOURCE
 if(!resource_memory(&next.simultaneous,&next.combined_memory_bytes))return 0;
 *out=next;return 1;
}
int slcli_bq_owner_resource_profile_shape(const struct slcli_bq_owner_resource_profile *p){
 uint64_t memory;
 return p&&resource_memory(&p->simultaneous,&memory)&&memory&&
 memory==p->combined_memory_bytes&&p->simultaneous.user_virtual_bytes&&
 p->simultaneous.native_attempts&&p->simultaneous.cpu_nanoseconds&&
 p->simultaneous.physical_extent_events<=256;
}
