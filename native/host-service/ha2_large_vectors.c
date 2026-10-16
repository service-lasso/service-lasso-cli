#include "ha2_large_vectors.h"
#include "ha2_large_consume.h"
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_ha2_large_positive_vector(const unsigned char *closure,size_t closure_bytes,
  const unsigned char *envelope,size_t envelope_bytes,const struct slcli_json_ha2_plan *plan,
  struct slcli_ha2_large_members_view *out) {
  /* Genuine selected whole bodies beyond the old ordinary262144 limit must
   * traverse actual strict/root/all region/E01..16/domain decoder, UNRUN. */
  if(closure_bytes<=262144&&envelope_bytes<=262144)return 0;
  if(!slcli_ha2_large_members_view_decode(closure,closure_bytes,envelope,envelope_bytes,plan,out))return 0;
  return out->closure.raw==closure&&out->closure.bytes==closure_bytes&&
    out->envelope.raw==envelope&&out->envelope.bytes==envelope_bytes&&out->regions&&out->domains;
}
int slcli_ha2_large_failure_vector(const unsigned char *closure,size_t closure_bytes,
  const unsigned char *envelope,size_t envelope_bytes,const struct slcli_json_ha2_plan *plan,
  struct slcli_ha2_large_members_view *out,const struct slcli_ha2_large_members_view *before) {
  /* Independent preattempt byte capture, never a freshly manufactured view.
   * Native cause/authority remains separate from this real decoder attempt. */
  if(!out||!before||!away(before,sizeof(*before),out,sizeof(*out))||
     !away(before,sizeof(*before),closure,closure_bytes)||
     !away(before,sizeof(*before),envelope,envelope_bytes)||
     !away(before,sizeof(*before),plan,sizeof(*plan))||
     !slcli_equal((const unsigned char *)before,(const unsigned char *)out,sizeof(*out)))return 0;
  if(slcli_ha2_large_members_view_decode(closure,closure_bytes,envelope,envelope_bytes,plan,out))return 0;
  return slcli_equal((const unsigned char *)before,(const unsigned char *)out,sizeof(*out));
}

#if defined(_WIN32) || defined(__linux__)
int slcli_ha2_large_positive_original(struct slcli_ha2_large_consumer *c,slcli_input_handle original) {
  size_t first_rows,second_rows;uint64_t first_tick,second_tick,after;
  if(!slcli_ha2_large_consume(c,original)||!c->structural_view_decoded||c->failed||
     !c->ranges[0]->complete||!c->ranges[1]->complete||
     !slcli_input_deadline_ready(c->ranges[0]->copies)||!slcli_input_deadline_ready(c->ranges[1]->copies)||
     (c->ranges[0]->copies->bytes<=262144&&c->ranges[1]->copies->bytes<=262144)||
     c->ranges[0]->copies->original!=original||c->ranges[1]->copies->original!=original||
     !c->bridge.complete||!c->parse_before.complete||!c->parse_after.complete)return 0;
  first_rows=c->ranges[0]->copies->capture_count;second_rows=c->ranges[1]->copies->capture_count;
  first_tick=c->ranges[0]->copies->deadline_owner->previous_tick;
  second_tick=c->ranges[1]->copies->deadline_owner->previous_tick;after=c->parse_after.tick;
  if(slcli_ha2_large_consume(c,original))return 0;
  return c->structural_view_decoded&&!c->failed&&
    c->ranges[0]->copies->capture_count==first_rows&&c->ranges[1]->copies->capture_count==second_rows&&
    c->ranges[0]->copies->deadline_owner->previous_tick==first_tick&&
    c->ranges[1]->copies->deadline_owner->previous_tick==second_tick&&c->parse_after.tick==after;
}
#endif
