#include "input_consume_vectors.h"
#if defined(_WIN32) || defined(__linux__)
static int vector_disjoint(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
/* Prospective source vector for ONE complete genuine P/G/Q/C tuple. The native
 * fixture owner supplies original read-only held objects and independently
 * selected lengths/digests after admission. This does not fabricate a provider,
 * claim every tuple has one C, or qualify any TC/CA from synthetic JSON. All
 * original handles/blocks/capture rows survive the vector, including failures. */
int slcli_input_consume_positive_vector(struct slcli_arena *arena,
    const struct slcli_input_vector_original originals[4],
    struct slcli_input_vector_result *result) {
  size_t i;
  if(!arena||!originals||!result||result->started||
     !slcli_arena_live_block(arena,originals,4*sizeof(*originals))||
     !slcli_arena_live_block(arena,result,sizeof(*result))||
     !vector_disjoint(originals,4*sizeof(*originals),result,sizeof(*result)))return 0;
  for(i=0;i<4;i++)if(!slcli_source_deadline_away(originals[i].deadline,result,sizeof(*result))||
    !slcli_source_deadline_away(originals[i].deadline,originals,4*sizeof(*originals))||
    !vector_disjoint(originals[i].capture_original,originals[i].capture_original_bytes,result,sizeof(*result))||
    !vector_disjoint(originals[i].capture_original,originals[i].capture_original_bytes,originals,4*sizeof(*originals))||
    !vector_disjoint(originals[i].caller_original,originals[i].caller_bytes,result,sizeof(*result))||
    !vector_disjoint(originals[i].caller_original,originals[i].caller_bytes,originals,4*sizeof(*originals)))return 0;
  result->started=1;
  /* Allocate ALL prospective raw/readback/control/parser/capture storage first.
   * There is no second quota or fallback allocator and no partial success. */
  result->profile=slcli_arena_calloc(arena,1,sizeof(*result->profile));
  result->grant=slcli_arena_calloc(arena,1,sizeof(*result->grant));
  result->qualification=slcli_arena_calloc(arena,1,sizeof(*result->qualification));
  result->calibration=slcli_arena_calloc(arena,1,sizeof(*result->calibration));
  if(!result->profile||!result->grant||!result->qualification||!result->calibration) return 0;
  for(i=0;i<4;++i) {
    if(!originals[i].bytes||originals[i].bytes>16384) return 0;
    result->bodies[i]=slcli_input_body_allocate(arena,originals[i].bytes,
                                              originals[i].capture_rows);
    if(!result->bodies[i]) return 0;
  }
  /* Every original body receives its whole precharged deadline/capture owner
   * before the first native read of any tuple member. */
  for(i=0;i<4;++i)
    if(!slcli_input_deadline_bind(arena,result->bodies[i],originals[i].deadline,
      originals[i].caller_original,originals[i].caller_bytes,originals[i].predecessor,
      originals[i].capture_sink,originals[i].capture_original,
      originals[i].capture_original_bytes))return 0;
  for(i=0;i<4;++i)
    if(!slcli_input_body_read(result->bodies[i],originals[i].original,
                              originals[i].bytes,originals[i].digest)) return 0;
  if(!slcli_input_profile_consume(result->bodies[0],result->profile)||
     !slcli_input_grant_consume(result->bodies[1],result->bodies[0],result->grant)||
     !slcli_input_qualification_consume(result->bodies[2],result->bodies[0],
                                       result->qualification)||
     !slcli_input_calibration_consume(result->bodies[3],result->bodies[0],
                                     result->calibration)) return 0;
  for(i=0;i<4;++i) {
    struct slcli_input_body *body=result->bodies[i];
    if(!body->recheck_started||!body->recheck_complete||body->failed||
       !slcli_equal(body->body,body->readback,body->bytes)||
       slcli_input_body_recheck(body)||body->failed) return 0;
  }
  /* Same original profile association and domain must survive both complete
   * typed consumers; Q remains qualification-only even when these match. */
  if(result->qualification->domain!=result->calibration->domain||
     !slcli_equal(result->qualification->source,result->calibration->source,32)||
     !slcli_equal(result->profile->digest,result->grant->profile,32)||
     !slcli_equal(result->profile->digest,result->qualification->profile,32)||
     !slcli_equal(result->profile->digest,result->calibration->profile,32)) return 0;
  result->complete=1; return 1;
}
int slcli_input_consume_alias_vector(struct slcli_input_body *body) {
  size_t captures; uint32_t started,failed,consumed;
  unsigned char original_first;
  if(!body||!body->complete||body->failed||!body->bytes||body->recheck_started||!slcli_input_deadline_ready(body)||
     body->capacity<sizeof(struct slcli_profile)) return 0;
  captures=body->capture_count; started=body->recheck_started;
  failed=body->failed; consumed=body->consumed; original_first=body->body[0];
  /* Actual connected consumer must reject raw/control/readback destinations
   * BEFORE native IO or parser erase. Original body bytes are never modified. */
  if(slcli_input_profile_consume(body,(struct slcli_profile *)(void *)body->body)||
     slcli_input_profile_consume(body,(struct slcli_profile *)(void *)body->readback)||
     slcli_input_profile_consume(body,(struct slcli_profile *)(void *)body)||
     body->capture_count!=captures||body->recheck_started!=started||
     body->failed!=failed||body->consumed!=consumed||body->body[0]!=original_first)
    return 0;
  return 1;
}
int slcli_input_actors19_positive_vector(struct slcli_arena *arena,
  const struct slcli_input_vector_original originals[2],
  const struct slcli_unit19_reader *roles,struct slcli_actor19_vector_result *result) {
  size_t i;struct slcli_input_body *bodies[2];
  if(!arena||!originals||!roles||!result||result->started||
     !vector_disjoint(originals,2*sizeof(*originals),roles,sizeof(*roles))||
     !vector_disjoint(originals,2*sizeof(*originals),result,sizeof(*result))||
     !vector_disjoint(roles,sizeof(*roles),result,sizeof(*result))||
     !slcli_arena_live_range(arena,originals,2*sizeof(*originals))||
     !slcli_arena_live_range(arena,roles,sizeof(*roles))||
     !slcli_arena_live_range(arena,result,sizeof(*result))||
     !roles->complete||roles->failed||roles->type!=SLCLI_ROLE19||
     roles->count!=SLCLI_ACTOR19_COUNT||roles->next!=SLCLI_ACTOR19_COUNT)return 0;
  for(i=0;i<2;i++)if(!slcli_source_deadline_away(originals[i].deadline,result,sizeof(*result))||
    !slcli_source_deadline_away(originals[i].deadline,originals,2*sizeof(*originals))||
    !slcli_source_deadline_away(originals[i].deadline,roles,sizeof(*roles))||
    !vector_disjoint(originals[i].capture_original,originals[i].capture_original_bytes,result,sizeof(*result))||
    !vector_disjoint(originals[i].capture_original,originals[i].capture_original_bytes,originals,2*sizeof(*originals))||
    !vector_disjoint(originals[i].capture_original,originals[i].capture_original_bytes,roles,sizeof(*roles))||
    !vector_disjoint(originals[i].caller_original,originals[i].caller_bytes,result,sizeof(*result))||
    !vector_disjoint(originals[i].caller_original,originals[i].caller_bytes,originals,2*sizeof(*originals))||
    !vector_disjoint(originals[i].caller_original,originals[i].caller_bytes,roles,sizeof(*roles)))return 0;
  /* Storage is acquired before any original read. A failed read retains its
   * native evidence and all prospective allocations in the same charged arena. */
  result->started=1;
  result->profile=slcli_arena_calloc(arena,1,sizeof(*result->profile));
  result->actors=slcli_arena_calloc(arena,1,sizeof(*result->actors));
  if(!result->profile||!result->actors)return 0;
  for(i=0;i<2;i++) {
    if(!originals[i].bytes||originals[i].bytes>16384)return 0;
    bodies[i]=slcli_input_body_allocate(arena,originals[i].bytes,originals[i].capture_rows);
    if(i==0)result->profile_body=bodies[i];else result->actors_body=bodies[i];
    if(!bodies[i])return 0;
  }
  for(i=0;i<2;i++)if(!slcli_input_deadline_bind(arena,bodies[i],originals[i].deadline,
    originals[i].caller_original,originals[i].caller_bytes,originals[i].predecessor,
    originals[i].capture_sink,originals[i].capture_original,
    originals[i].capture_original_bytes))return 0;
  for(i=0;i<2;i++)if(!slcli_input_body_read(bodies[i],originals[i].original,
    originals[i].bytes,originals[i].digest))return 0;
  if(!slcli_input_profile_consume(bodies[0],result->profile)||
     !slcli_input_actors19_consume(bodies[1],bodies[0],result->profile,result->actors)||
     !slcli_actors19_role_unit_matches(result->actors,roles))return 0;
  for(i=0;i<2;i++)if(!bodies[i]->recheck_started||!bodies[i]->recheck_complete||
    bodies[i]->failed||!slcli_equal(bodies[i]->body,bodies[i]->readback,bodies[i]->bytes))return 0;
  result->complete=1;return 1;
}
#endif
