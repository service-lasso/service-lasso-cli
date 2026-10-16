#include "bq_capture_schedule_windows.h"
#include "capture_ranges_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int bytes_same(const void *a,const void *b,size_t n){
 size_t i;const unsigned char *x=a,*y=b;for(i=0;i<n;i++)if(x[i]!=y[i])return 0;return 1;
}
/* Controlled source fixture ONLY. No native-positive factory is invoked.
 * Alias inputs/deadline are separate preowned fixture copies; never mutate an
 * authentic original's caller/deadline to create this negative case. UNRUN. */
int slcli_bq_capture_factory_windows_caller_alias_vector(
 struct slcli_writer_capture_factory_windows *f,struct slcli_arena *a,
 struct slcli_writer_windows_path *creator,struct slcli_writer_windows_output *output,
 const struct slcli_writer_windows_evidence_originals *inputs,
 const struct slcli_writer_windows_evidence_originals *alias_inputs,
 struct slcli_writer_windows_readback_original *readback,
 struct slcli_writer_capture_backing_windows *backing,struct slcli_writer_capture_ranges_windows *ranges,
 size_t index,size_t count,struct slcli_writer_capture_ranges_windows *output_ranges,
 size_t output_index,size_t output_count,struct slcli_writer_capture_ranges_windows *control_ranges,
 size_t control_index,size_t control_count,struct slcli_bq_capture_schedule_windows *s,
 unsigned char *before,size_t before_bytes){
 const void *blocks[20];size_t sizes[20],i,j,total=0,offset=0;
 struct slcli_writer_windows_evidence_originals expected;
 struct slcli_source_deadline_original expected_deadline;
 unsigned char *e=(unsigned char *)&expected,*d=(unsigned char *)&expected_deadline;
 if(!slcli_writer_capture_factory_windows_bq_preflight_original(f,a,creator,output,inputs,
   readback,backing,ranges,index,count,output_ranges,output_index,output_count,control_ranges,control_index,control_count,s)||
  !slcli_arena_live_block(a,alias_inputs,sizeof(*alias_inputs))||
  !slcli_arena_live_block(a,alias_inputs->deadline,sizeof(*alias_inputs->deadline)))return 0;
 for(i=0;i<sizeof(expected);i++)e[i]=((const unsigned char *)inputs)[i];
 for(i=0;i<sizeof(expected_deadline);i++)d[i]=((const unsigned char *)inputs->deadline)[i];
 expected.caller=s->plan[0].capture;expected.caller_bytes=sizeof(*s->plan[0].capture);
 expected.deadline=alias_inputs->deadline;expected_deadline.caller_original=expected.caller;
 if(!bytes_same(&expected,alias_inputs,sizeof(expected))||
  !bytes_same(&expected_deadline,alias_inputs->deadline,sizeof(expected_deadline)))return 0;
 blocks[0]=f;sizes[0]=sizeof(*f);blocks[1]=creator;sizes[1]=sizeof(*creator);
 blocks[2]=output;sizes[2]=sizeof(*output);blocks[3]=readback;sizes[3]=sizeof(*readback);
 blocks[4]=backing;sizes[4]=sizeof(*backing);blocks[5]=ranges;sizes[5]=sizeof(*ranges);
 blocks[6]=s;sizes[6]=sizeof(*s);blocks[7]=s->targets;sizes[7]=s->target_count*sizeof(*s->targets);
 blocks[8]=s->owner;sizes[8]=sizeof(*s->owner);blocks[9]=s->plan;sizes[9]=s->count*sizeof(*s->plan);
 blocks[10]=inputs;sizes[10]=sizeof(*inputs);blocks[11]=alias_inputs;sizes[11]=sizeof(*alias_inputs);
 blocks[12]=inputs->bq_profile;sizes[12]=sizeof(*inputs->bq_profile);
 blocks[13]=inputs->bq_profile->raw;sizes[13]=inputs->bq_profile->raw_bytes;
 blocks[14]=inputs->caller;sizes[14]=inputs->caller_bytes;
 blocks[15]=inputs->deadline;sizes[15]=sizeof(*inputs->deadline);
 blocks[16]=inputs->deadline->origin;sizes[16]=sizeof(*inputs->deadline->origin);
 blocks[17]=alias_inputs->deadline;sizes[17]=sizeof(*alias_inputs->deadline);
 blocks[18]=output_ranges;sizes[18]=sizeof(*output_ranges);
 blocks[19]=control_ranges;sizes[19]=sizeof(*control_ranges);
 for(i=0;i<20;i++){
  if(!sizes[i]||sizes[i]>SIZE_MAX-total||!slcli_arena_live_range(a,blocks[i],sizes[i]))return 0;
  total+=sizes[i];
 }
 if(s->count>(SIZE_MAX-total)/sizeof(*s->plan[0].capture))return 0;
 total+=s->count*sizeof(*s->plan[0].capture);
 if(before_bytes!=total||!slcli_arena_live_block(a,before,before_bytes))return 0;
 for(i=0;i<20;i++)if(!apart(before,before_bytes,blocks[i],sizes[i]))return 0;
 for(i=0;i<s->count;i++)if(!apart(before,before_bytes,s->plan[i].capture,sizeof(*s->plan[i].capture)))return 0;
 for(i=0;i<20;i++){for(j=0;j<sizes[i];j++)before[offset+j]=((const unsigned char *)blocks[i])[j];offset+=sizes[i];}
 for(i=0;i<s->count;i++){
  for(j=0;j<sizeof(*s->plan[i].capture);j++)before[offset+j]=((const unsigned char *)s->plan[i].capture)[j];
  offset+=sizeof(*s->plan[i].capture);
 }
 if(slcli_writer_capture_factory_windows_bq_preflight_original(f,a,creator,output,alias_inputs,
   readback,backing,ranges,index,count,output_ranges,output_index,output_count,control_ranges,control_index,control_count,s)||
  slcli_writer_capture_factory_windows_create_bq_original(f,a,creator,output,alias_inputs,
   readback,backing,ranges,index,count,output_ranges,output_index,output_count,control_ranges,control_index,control_count,s))return 0;
 offset=0;
 for(i=0;i<20;i++){if(!bytes_same(before+offset,blocks[i],sizes[i]))return 0;offset+=sizes[i];}
 for(i=0;i<s->count;i++){
  if(!bytes_same(before+offset,s->plan[i].capture,sizeof(*s->plan[i].capture)))return 0;
  offset+=sizeof(*s->plan[i].capture);
 }
 return offset==before_bytes;
}
#endif
