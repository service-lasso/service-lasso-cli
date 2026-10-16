#include "unit19_native_vectors.h"
#if defined(_WIN32) || defined(__linux__)
int slcli_unit19_native_positive_vector(struct slcli_unit19_native *u,
  struct slcli_unit19_framing *f,enum slcli_unit19_kind type,uint32_t source,
  const struct slcli_unit19_record *records,size_t count,const unsigned char *const *members,
  const uint64_t *limits,size_t limit_count,slcli_input_handle file,
  slcli_store_capture_sink sink,void *sink_original,uint64_t store_bytes) {
  if(!u||!u->arena||!f||!slcli_arena_live_range(u->arena,f,sizeof(*f))||
     !records||!count||count>15||
     !slcli_arena_live_range(u->arena,records,count*sizeof(*records))||
     !slcli_unit19_native_framing_storage(u,f,members,records,count,limits,limit_count)||
     !slcli_unit19_encode(f,type,source,records,count,limits,limit_count)||
     !slcli_unit19_native_prepare(u,type,source,f->header,&f->table[0][0],count*128,
       members,records,limits,limit_count,file,sink,sink_original,store_bytes))return 0;
  return f->complete&&!f->failed&&u->prepared&&!u->failed&&
    u->write->held_verified&&u->held->held_verified&&u->write->capture_count>2&&
    u->held->capture_count>2&&u->write->expected==f->checked.file_bytes&&
    u->write->offset==f->checked.file_bytes&&u->held->offset==f->checked.file_bytes&&
    slcli_equal(u->write->digest,u->held->digest,32);
}
int slcli_unit26_native_full_geometry_vector(struct slcli_unit19_native *u,
  struct slcli_unit19_framing *f,uint32_t source,const struct slcli_unit19_record records[12],
  const unsigned char *const members[12],slcli_input_handle file,
  slcli_store_capture_sink sink,void *sink_original,uint64_t store_bytes) {
  static const uint64_t lengths[12]={16384,131072,65536,524288,1048576,65536,
    16384,16384,16384,16384,16384,16384};unsigned i;
  if(!records)return 0;
  for(i=0;i<12;i++)if(records[i].bytes!=lengths[i])return 0;
  if(!slcli_unit19_native_positive_vector(u,f,SLCLI_HA2_DERIVE19,source,records,12,
    members,0,0,file,sink,sink_original,store_bytes))return 0;
  return u->parsed.file_bytes==1951296&&f->checked.file_bytes==1951296&&
    u->retained_store_bytes==store_bytes+1951296;
}
int slcli_unit19_native_deadline_positive_vector(struct slcli_unit19_native *u,
  struct slcli_unit19_framing *f,enum slcli_unit19_kind type,uint32_t source,
  const struct slcli_unit19_record *records,size_t count,const unsigned char *const *members,
  const uint64_t *limits,size_t limit_count,slcli_input_handle file,
  slcli_store_capture_sink sink,void *sink_original,uint64_t store_bytes,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  const struct slcli_native_clock *prior,size_t sink_bytes) {
  struct slcli_store_deadline_owner *o;size_t write_rows,held_rows;uint64_t tick;
  if(!slcli_unit19_native_deadline_bind(u,d,caller,caller_bytes,prior,sink,sink_original,sink_bytes)||
     !slcli_unit19_native_positive_vector(u,f,type,source,records,count,members,limits,limit_count,
       file,sink,sink_original,store_bytes)||!(o=u->deadline_owner)||
     !slcli_store_deadline_ready(o)||o->deadline!=d||o->caller_original!=caller||
     o->predecessor!=prior||o->count!=2||o->streams[0]!=u->write||o->streams[1]!=u->held||
     o->used[0]!=u->write->capture_count||o->used[1]!=u->held->capture_count)return 0;
  tick=o->previous_tick;write_rows=u->write->capture_count;held_rows=u->held->capture_count;
  if(slcli_unit19_native_prepare(u,type,source,f->header,&f->table[0][0],count*128,
       members,records,limits,limit_count,file,sink,sink_original,store_bytes))return 0;
  return u->prepared&&!u->failed&&slcli_store_deadline_ready(o)&&o->previous_tick==tick&&
    u->write->capture_count==write_rows&&u->held->capture_count==held_rows;
}
#endif
