#include "ha2_target_vectors.h"
#include "unit19_range_vectors.h"
#if defined(_WIN32) || defined(__linux__)
int slcli_ha2_target_controls_positive_vector(struct slcli_ha2_target_controls *c,
  const struct slcli_unit19_reader *unit,slcli_input_handle handle,
  const struct slcli_unit19_reader *trial,const struct slcli_unit19_reader *certificates,
  const unsigned char source[32]) {
  static const uint32_t ordinal[]={0,6,7,8,11};size_t i;
  if(!slcli_ha2_target_controls_consume(c,unit,handle,trial,certificates,source))return 0;
  for(i=0;i<5;i++)if(!slcli_unit19_range_completed_vector(c->ranges[i],unit,ordinal[i],handle))return 0;
  return c->controls_decoded&&!c->failed&&c->started&&
    c->profile.raw==c->ranges[1]->copies->body&&
    c->grant.raw==c->ranges[2]->copies->body&&
    c->candidate.raw==c->ranges[4]->copies->body&&
    slcli_equal(c->profile.digest,c->qualification.profile,32)&&
    slcli_equal(c->profile.digest,c->candidate.profile,32)&&
    c->qualification.domain==1&&c->candidate.domain==1&&
    c->candidate.maximum==1048576;
}
#endif
