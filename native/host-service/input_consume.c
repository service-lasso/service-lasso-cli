#include "input_consume.h"
#include "input_deadline.h"
#if defined(_WIN32) || defined(__linux__)
static int disjoint(const void *left,size_t n,const void *right,size_t m) {
  uintptr_t a=(uintptr_t)left,b=(uintptr_t)right;
  return left&&right&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
static int outside(const struct slcli_input_body *body,const void *out,size_t n) {
  size_t rows;
  if(!body||!body->body||!body->readback||!body->captures||
     body->capture_capacity>SIZE_MAX/sizeof(*body->captures)) return 0;
  rows=body->capture_capacity*sizeof(*body->captures);
  return disjoint(out,n,body,sizeof(*body))&&
    disjoint(out,n,body->body,body->capacity)&&
    disjoint(out,n,body->readback,body->readback_capacity)&&
    disjoint(out,n,body->captures,rows);
}
static int begin(struct slcli_input_body *body,void *out,size_t bytes,
                    const struct slcli_input_body *profile) {
  if(!body||body->bytes>16384||!body->deadline_owner||!slcli_input_deadline_ready(body)||!outside(body,out,bytes)||
     (profile&&(!profile->complete||profile->failed||!profile->deadline_owner||!slcli_input_deadline_ready(profile)||
       !profile->recheck_complete||profile->consumed!=1||profile==body||
       !outside(profile,out,bytes)||
       !outside(profile,body,sizeof(*body))||
       !outside(profile,body->body,body->capacity)||
       !outside(profile,body->readback,body->readback_capacity)||
       !outside(profile,body->captures,
         body->capture_capacity*sizeof(*body->captures))))) return 0;
  if(body->consumed||!slcli_input_body_recheck(body)||!slcli_input_deadline_ready(body)) return 0;
  body->consumed=2; body->failed=1;
  /* Capture and full second-copy ownership have already been obtained before
   * the complete typed grammar runs. A parser never opens another object or
   * retires either original read or native capture on success/failure. */
  return 1;
}
int slcli_input_profile_consume(struct slcli_input_body *body,
                                 struct slcli_profile *out) {
  if(!begin(body,out,sizeof(*out),0)||
     !slcli_profile_parse(body->body,body->bytes,out)||
     !slcli_equal(out->digest,body->digest,32)) return 0;
  body->consumed=1; body->failed=0; return 1;
}
int slcli_input_grant_consume(struct slcli_input_body *body,
                              const struct slcli_input_body *profile,
                              struct slcli_grant *out) {
  if(!profile||!begin(body,out,sizeof(*out),profile)||
     !slcli_grant_parse(body->body,body->bytes,profile->digest,out)||
     !slcli_equal(out->digest,body->digest,32)) return 0;
  body->failed=0; return 1;
}
int slcli_input_qualification_consume(struct slcli_input_body *body,
                                      const struct slcli_input_body *profile,
                                      struct slcli_qualification *out) {
  if(!profile||!begin(body,out,sizeof(*out),profile)||
     !slcli_qualification_parse(body->body,body->bytes,profile->digest,out)) return 0;
  body->failed=0; return 1;
}
int slcli_input_calibration_consume(struct slcli_input_body *body,
                                    const struct slcli_input_body *profile,
                                    struct slcli_calibration *out) {
  if(!profile||!begin(body,out,sizeof(*out),profile)||
     !slcli_calibration_parse(body->body,body->bytes,profile->digest,out)||
     !slcli_equal(out->raw_digest,body->digest,32)) return 0;
  body->failed=0; return 1;
}
int slcli_input_ha2_service_qualification_consume(struct slcli_input_body *body,
  const struct slcli_input_body *profile,struct slcli_qualification *out) {
  if(!profile||!begin(body,out,sizeof(*out),profile)||
     !slcli_ha2_service_qualification_parse(body->body,body->bytes,profile->digest,out))return 0;
  body->failed=0;return 1;
}
int slcli_input_actors19_consume(struct slcli_input_body *body,
  const struct slcli_input_body *profile,const struct slcli_profile *parsed,
  struct slcli_actors19 *out) {
  if(!profile||!parsed||!out||parsed->raw!=profile->body||
     parsed->raw_bytes!=profile->bytes||
     !slcli_equal(parsed->digest,profile->digest,32)||
     !disjoint(parsed,sizeof(*parsed),out,sizeof(*out))||
     !outside(body,parsed,sizeof(*parsed))||
     !begin(body,out,sizeof(*out),profile)||
     !slcli_actors19_parse(body->body,body->bytes,out)||
     !slcli_equal(out->digest,body->digest,32)||
     !slcli_actors19_facade_profile_matches(out,parsed))return 0;
  body->failed=0;return 1;
}
#endif
