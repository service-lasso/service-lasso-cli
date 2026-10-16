#include "ha2_c22_vectors.h"
#if defined(_WIN32) || defined(__linux__)
static int observed_eofs(const struct slcli_input_body *s,uint64_t bytes) {
  size_t i;uint32_t count=0;
  for(i=0;i<s->capture_count;i++) {
    const slcli_input_capture *c=&s->captures[i];
#if defined(_WIN32)
    if(c->offset==bytes&&c->requested==1&&c->bytes_observed&&
      !c->returned_bytes&&c->completed)count++;
#else
    if(c->args[3]==bytes&&c->args[2]==1&&c->returned_bytes_observed&&
      !c->returned_bytes&&c->observed)count++;
#endif
  }
  return count==2;
}
int slcli_ha2_c22_positive_vector(struct slcli_ha2_c22_consumer *c,
  const struct slcli_unit19_reader *unit,slcli_input_handle original,
  const struct slcli_unit19_range *service_p,const struct slcli_input_body *bq_p,
  const unsigned char service_source[32],const unsigned char bq_source[32]) {
  const struct slcli_unit19_range *r[2];size_t i;
  if(!slcli_ha2_c22_consume(c,unit,original,service_p,bq_p,service_source,bq_source))return 0;
  r[0]=c->service;r[1]=c->bq;
  for(i=0;i<2;i++) {
    const struct slcli_input_body *s=r[i]->copies;
    if(!r[i]->complete||r[i]->failed||r[i]->unit!=unit||r[i]->ordinal!=i||
      s->original!=original||!s->complete||s->failed||!s->recheck_complete||
      s->consumed!=2||s->bytes!=unit->records[i].bytes||
      !slcli_equal(s->body,s->readback,s->bytes)||
      !slcli_equal(s->digest,unit->records[i].digest,32)||
      !observed_eofs(s,unit->file_bytes))return 0;
  }
  return c->complete&&!c->failed&&c->service_c.domain==1&&c->bq_c.domain==0&&
    c->service_c.maximum==1048576&&
    slcli_equal(c->service_c.profile,service_p->copies->digest,32)&&
    slcli_equal(c->bq_c.profile,bq_p->digest,32)&&
    slcli_equal(c->service_c.source,service_source,32)&&
    slcli_equal(c->bq_c.source,bq_source,32);
}
#endif
