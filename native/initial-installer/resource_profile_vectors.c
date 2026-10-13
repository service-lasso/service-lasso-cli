#include "resource_profile_vectors.h"
#include "../host-service/bq_provider_expectations.h"
static void fill(void *p,size_t n,unsigned char v){
 unsigned char *b=p;size_t i;for(i=0;i<n;i++)b[i]=v;
}
static void save(unsigned char *out,const void *p,size_t n){
 const unsigned char *b=p;size_t i;for(i=0;i<n;i++)out[i]=b[i];
}
static int unchanged(const unsigned char *saved,const void *p,size_t n){
 const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(saved[i]!=b[i])return 0;return 1;
}
static void component(struct slcli_bq_resource_cost *c,unsigned which,uint64_t value){
 switch(which){
 case 0:c->user_ram_bytes=value;break;case 1:c->user_virtual_bytes=value;break;
 case 2:c->kernel_ram_bytes=value;break;case 3:c->cache_ram_bytes=value;break;
 case 4:c->retained_storage_bytes=value;break;case 5:c->native_references=value;break;
 case 6:c->physical_extent_events=value;break;case 7:c->native_attempts=value;break;
 case 8:c->pure_attempts=value;break;case 9:c->cpu_nanoseconds=value;break;
 }
}
int slcli_initial_resource_profile_vectors(void){
 struct slcli_bq_owner_resource_profile before,out;
 struct slcli_bq_resource_cost cost;
 unsigned char old[sizeof(out)];unsigned i;
 fill(&before,sizeof(before),0);fill(&cost,sizeof(cost),0);
 fill(&out,sizeof(out),0xa5);save(old,&out,sizeof(out));
 if(!slcli_bq_owner_resource_profile_add(&out,&before,&cost)||
 slcli_bq_owner_resource_profile_shape(&out))return 0;
 /* Real parallel dimensions: VM/storage/CPU must not enter RAM sum. */
 cost.user_ram_bytes=7;cost.kernel_ram_bytes=11;cost.cache_ram_bytes=13;
 cost.user_virtual_bytes=1000;cost.retained_storage_bytes=2000;
 cost.native_references=3;cost.physical_extent_events=2;
 cost.native_attempts=30;cost.pure_attempts=40;cost.cpu_nanoseconds=9999;
 if(!slcli_bq_owner_resource_profile_add(&out,&before,&cost)||
 out.combined_memory_bytes!=31||out.simultaneous.user_virtual_bytes!=1000||
 out.simultaneous.retained_storage_bytes!=2000||out.simultaneous.cpu_nanoseconds!=9999||
 !slcli_bq_owner_resource_profile_shape(&out))return 0;
 for(i=0;i<10;i++){
  fill(&before,sizeof(before),0);fill(&cost,sizeof(cost),0);
  component(&before.simultaneous,i,UINT64_MAX);component(&cost,i,1);
  if(i==0||i==2||i==3)before.combined_memory_bytes=UINT64_MAX;
  fill(&out,sizeof(out),0xa5);save(old,&out,sizeof(out));
  if(slcli_bq_owner_resource_profile_add(&out,&before,&cost)||
   !unchanged(old,&out,sizeof(out)))return 0;
 }
 fill(&before,sizeof(before),0);fill(&cost,sizeof(cost),0);
 before.simultaneous.user_ram_bytes=UINT64_MAX;before.combined_memory_bytes=UINT64_MAX;
 cost.kernel_ram_bytes=1;save(old,&out,sizeof(out));
 if(slcli_bq_owner_resource_profile_add(&out,&before,&cost)||
 !unchanged(old,&out,sizeof(out)))return 0;
 before.simultaneous.user_ram_bytes=1;before.combined_memory_bytes=0;
 fill(&cost,sizeof(cost),0);
 if(slcli_bq_owner_resource_profile_add(&out,&before,&cost)||
 !unchanged(old,&out,sizeof(out)))return 0;
 before.combined_memory_bytes=1;save(old,&before,sizeof(before));
 if(slcli_bq_owner_resource_profile_add(&before,&before,&cost)||
 !unchanged(old,&before,sizeof(before)))return 0;
 /* Cost is a real subobject of output storage; exclude it before any write. */
 fill(&out,sizeof(out),0);save(old,&out,sizeof(out));
 if(slcli_bq_owner_resource_profile_add(&out,&before,&out.simultaneous)||
 !unchanged(old,&out,sizeof(out)))return 0;
 out=before;out.simultaneous.user_virtual_bytes=1;out.simultaneous.native_attempts=1;
 out.simultaneous.cpu_nanoseconds=1;out.simultaneous.physical_extent_events=257;
 if(slcli_bq_owner_resource_profile_shape(&out))return 0;
 return 1;
}
