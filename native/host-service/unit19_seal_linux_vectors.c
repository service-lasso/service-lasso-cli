#if defined(__linux__)
#include "unit19_seal_linux_vectors.h"
int slcli_unit19_seal_linux_positive_vector(struct slcli_unit19_seal_linux *s,
  struct slcli_unit19_native *u,int readonly_fd,uint32_t block_size,
  const struct slcli_source_deadline_original *d,const void *caller,
  slcli_store_capture_sink sink,void *capture_original) {
  struct slcli_verity_linux *v;
  if(!slcli_unit19_seal_linux_original(s,u,readonly_fd,block_size,d,caller,
      sink,capture_original))return 0;
  v=s->verity;
  return s->sealed&&!s->failed&&s->creator_closed&&u->creator_closed&&
    u->seal_attempted&&s->creator_close.operation==7&&s->creator_close.observed&&
    !s->creator_close.status&&s->creator_close.descriptor_closed&&
    v->verified&&!v->failed&&v->seal_applied&&v->row_count==6&&
    v->rows[1].operation==5&&v->rows[1].observed&&!v->rows[1].status&&
    v->rows[1].verity_applied&&v->rows[2].operation==6&&
    v->rows[2].verity_digest_observed&&v->rows[4].operation==6&&
    v->rows[4].verity_digest_observed&&v->rows[5].operation==1&&
    v->rows[5].tick<d->expires;
}
int slcli_unit19_seal_linux_failed_enable_vector(const struct slcli_unit19_seal_linux *s) {
  const struct slcli_verity_linux *v;
  const struct slcli_linux_file_capture *c;
  if(!s||!s->started||!s->failed||s->sealed||!s->creator_closed||
     !s->prepared||!s->prepared->creator_closed||!s->prepared->seal_attempted||
     !(v=s->verity)||!v->started||!v->failed||v->verified||v->row_count!=2)return 0;
  c=&v->rows[1];
  return c->operation==5&&c->observed&&c->status!=0&&!c->verity_applied&&
    !v->seal_applied&&s->creator_close.descriptor_closed;
}
int slcli_unit19_postseal_linux_positive_vector(struct slcli_unit19_postseal_linux *s,
  const struct slcli_unit19_seal_linux *seal,const struct slcli_unit19_framing *f,
  const unsigned char *const *members,const struct slcli_source_deadline_original *d,
  const void *caller,slcli_store_capture_sink sink,void *capture_original) {
  struct slcli_store_stream *r;
  if(!slcli_unit19_postseal_linux_original(s,seal,f,members,d,caller,sink,capture_original))return 0;
  r=s->read;
  return s->complete&&!s->failed&&r->held_verified&&!r->failed&&
    r->mode==1&&r->original==seal->readonly_original&&r->capture_count>2&&
    r->offset==seal->prepared->parsed.file_bytes&&r->capture_count+1==s->clock_count&&
    r->captures[r->capture_count-2].operation==2&&
    r->captures[r->capture_count-2].returned_bytes_observed&&
    r->captures[r->capture_count-2].returned_bytes==0&&
    slcli_equal(r->digest,seal->prepared->write->digest,32);
}
int slcli_unit19_postseal_linux_no_renewal_vector(struct slcli_unit19_postseal_linux *s,
  const struct slcli_unit19_postseal_linux *positive,
  const struct slcli_unit19_seal_linux *seal,const struct slcli_unit19_framing *f,
  const unsigned char *const *members,const struct slcli_source_deadline_original *later,
  const void *caller,slcli_store_capture_sink sink,void *capture_original) {
  struct slcli_store_stream *r;
  if(!positive||!positive->complete||positive->failed||!positive->read||
     !positive->read->held_verified||positive->read->failed||
     positive->seal_original!=seal||positive->framing_original!=f||
     positive->members_original!=members||positive->caller_original!=caller||
     positive->sink!=sink||positive->capture_original!=capture_original||
     !s||!s->arena||s->arena!=positive->arena||s==positive||
     s->started||s->failed||s->complete||s->clock_count||
     !(r=s->read)||r->attempted||r->capture_count||
     r->capture_capacity!=positive->read->capture_capacity||
     s->clock_capacity!=positive->clock_capacity||!seal||!seal->sealed||
     !seal->verity||!seal->verity->verified||seal->verity->failed||
     !slcli_source_deadline_original_matches(s->arena,later,caller)||
     later==seal->verity->deadline||later->caller_original!=seal->verity->caller_original||
     later->origin!=seal->verity->original_origin||later->expires<=seal->verity->expires)return 0;
  if(slcli_unit19_postseal_linux_original(s,seal,f,members,later,caller,sink,capture_original))return 0;
  return !s->started&&!s->failed&&!s->complete&&!s->clock_count&&
    !r->attempted&&!r->capture_count&&seal->sealed&&seal->verity->verified;
}
#endif
