#include "bq_observation_windows.h"
#if defined(_WIN64)
/* SOURCE-UNRUN regression over caller's genuinely preowned fixture. This does
 * not allocate capacity448, admit a profile or create native authority. The
 * supplied retained before-buffer is separate charged fixture storage. */
int slcli_bq_observation_bitmap_alias_vector_original(
 struct slcli_binding_storage_original *s,struct slcli_bq_observation_preparation *control,
 struct slcli_bq_file_observation_slot *slots,
 const struct slcli_native_reference *plans,const uint32_t *ids,uint32_t count,
 unsigned char *before,size_t before_bytes) {
 struct slcli_binding_storage_geometry g;struct slcli_arena *a;
 const unsigned char *blocks[8];const void *inputs[4];
 size_t sizes[8],input_bytes[4],total=0,i,j,k;int result;
 if(!s||!(a=s->arena)||!slcli_arena_live_block(a,s,sizeof(*s))||
  s->capacity!=448||s->attempted||s->enrolled||!s->prepared||
  !slcli_binding_storage_geometry_derive(s->capacity,&g)||
  sizeof(struct slcli_bq_observation_preparation)!=56||g.bitmap_bytes!=56||
  !before||!count||count>SIZE_MAX/sizeof(*slots)||count>SIZE_MAX/sizeof(*plans)||
  count>SIZE_MAX/sizeof(*ids))return 0;
 blocks[0]=(const void *)s;sizes[0]=sizeof(*s);
 blocks[1]=(const void *)s->book;sizes[1]=sizeof(*s->book);
 blocks[2]=(const void *)s->references;sizes[2]=g.references_bytes;
 blocks[3]=(const void *)s->ids;sizes[3]=g.ids_bytes;
 blocks[4]=(const void *)s->indices;sizes[4]=g.indices_bytes;
 blocks[5]=s->bitmap;sizes[5]=g.bitmap_bytes;blocks[6]=s->snapshot;sizes[6]=g.snapshot_bytes;
 blocks[7]=(const void *)s->claims;sizes[7]=g.claims_bytes;
 for(i=0;i<8;i++) {
  if(!slcli_arena_live_block(a,blocks[i],sizes[i])||sizes[i]>SIZE_MAX-total)return 0;
  total+=sizes[i];
 }
 if(before_bytes!=total||!slcli_arena_live_block(a,before,total)||
  (uintptr_t)before>UINTPTR_MAX-total)return 0;
 for(i=0;i<8;i++) {
  uintptr_t x=(uintptr_t)blocks[i],y=(uintptr_t)before;
  if(x>UINTPTR_MAX-sizes[i]||(x<y+total&&y<x+sizes[i]))return 0;
 }
 inputs[0]=control;input_bytes[0]=sizeof(*control);
 inputs[1]=slots;input_bytes[1]=(size_t)count*sizeof(*slots);
 inputs[2]=plans;input_bytes[2]=(size_t)count*sizeof(*plans);
 inputs[3]=ids;input_bytes[3]=(size_t)count*sizeof(*ids);
 for(i=0;i<4;i++) {
  uintptr_t x=(uintptr_t)inputs[i],y=(uintptr_t)before;
  if(!slcli_arena_live_block(a,inputs[i],input_bytes[i])||x>UINTPTR_MAX-input_bytes[i]||
   (x<y+total&&y<x+input_bytes[i]))return 0;
 }
 /* Zero alias is essential: otherwise the preexisting fresh-zero guard would
  * reject and this case would not exercise the missing book-disjointness. */
 for(i=0;i<g.bitmap_bytes;i++)if(s->bitmap[i])return 0;
 for(i=0,k=0;i<8;i++)for(j=0;j<sizes[i];j++)before[k++]=blocks[i][j];
 result=slcli_bq_observation_prepare_preowned_original(
  (struct slcli_bq_observation_preparation *)s->bitmap,s,slots,plans,ids,count);
 if(result)return 0;
 for(i=0,k=0;i<8;i++)for(j=0;j<sizes[i];j++)if(before[k++]!=blocks[i][j])return 0;
 /* A distinct real positive control must then consume these SAME otherwise
  * valid plans/targets. This prevents unrelated malformed input rejection
  * from being mistaken for the alias regression passing. */
 return slcli_bq_observation_prepare_preowned_original(control,s,slots,plans,ids,count)&&
  control->prepared&&!control->failed&&s->attempted==count;
}
#endif
