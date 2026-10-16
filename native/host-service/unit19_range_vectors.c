#include "unit19_range_vectors.h"
#if defined(_WIN32) || defined(__linux__)
int slcli_unit19_range_completed_vector(const struct slcli_unit19_range *r,
  const struct slcli_unit19_reader *unit,uint32_t ordinal,slcli_input_handle original) {
  const struct slcli_input_body *s;size_t i;uint32_t eof_rows=0;
  if(!r||!unit||!unit->started||!unit->complete||unit->failed||
    unit->next!=unit->count||unit->next_offset!=unit->file_bytes||
    ordinal>=unit->count||unit->count>15||!(s=r->copies)||
    s->capture_count>s->capture_capacity||!s->captures||!s->body||!s->readback||
    s->bytes>=s->capacity||s->bytes>=s->readback_capacity)return 0;
  s=r->copies;
  for(i=0;i<s->capture_count;i++) {
    const slcli_input_capture *c=&s->captures[i];
#if defined(_WIN32)
    if(c->offset==unit->file_bytes&&c->requested==1&&c->bytes_observed&&
       !c->returned_bytes&&c->completed)eof_rows++;
#else
    if(c->args[3]==unit->file_bytes&&c->args[2]==1&&c->returned_bytes_observed&&
       !c->returned_bytes&&c->observed)eof_rows++;
#endif
  }
  return r->complete&&!r->failed&&r->unit==unit&&r->ordinal==ordinal&&
    s->original==original&&s->started&&s->complete&&s->eof_observed&&
    s->recheck_started&&s->recheck_complete&&!s->failed&&s->consumed==2&&eof_rows==2&&
    s->bytes==unit->records[ordinal].bytes&&r->whole_file_bytes==unit->file_bytes&&
    slcli_equal(s->body,s->readback,s->bytes)&&
    slcli_equal(s->digest,unit->records[ordinal].digest,32);
}
int slcli_unit19_range_positive_vector(struct slcli_unit19_range *r,
  const struct slcli_unit19_reader *unit,uint32_t ordinal,slcli_input_handle original) {
  return slcli_unit19_range_read(r,unit,ordinal,original)&&
    slcli_unit19_range_completed_vector(r,unit,ordinal,original);
}
int slcli_unit19_range_deadline_positive_vector(struct slcli_unit19_range *r,
  const struct slcli_unit19_reader *unit,uint32_t ordinal,slcli_input_handle original,
  const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
  const struct slcli_native_clock *prior,slcli_store_capture_sink sink,void *owner,size_t owner_bytes) {
  struct slcli_input_deadline_owner *o;size_t rows;uint64_t tick;
  o=slcli_unit19_range_deadline_bind(r,d,caller,caller_bytes,prior,sink,owner,owner_bytes);
  if(!o||!slcli_unit19_range_positive_vector(r,unit,ordinal,original)||
     !slcli_input_deadline_ready(r->copies)||r->copies->deadline_owner!=o||
     o->deadline!=d||o->caller_original!=caller||o->predecessor!=prior||
     o->next!=r->copies->capture_count)return 0;
  rows=o->next;tick=o->previous_tick;
  if(slcli_unit19_range_read(r,unit,ordinal,original))return 0;
  return slcli_unit19_range_completed_vector(r,unit,ordinal,original)&&
    slcli_input_deadline_ready(r->copies)&&o->next==rows&&o->previous_tick==tick;
}
#endif
