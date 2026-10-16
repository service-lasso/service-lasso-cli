#ifndef SLCLI_JSON_H
#define SLCLI_JSON_H
#include <stddef.h>
#include <stdint.h>
/* Borrowed offsets into SAME original held bytes. Validation supplies no source
 * authority; the native owner retains and reobserves the original body/EOF.
 * Outputs/counts must be disjoint from borrowed raw bytes and from one another;
 * overlap/overflow rejection precedes even failure-output zeroing. */
struct slcli_json_span { size_t start, end; };
int slcli_json_validate(const unsigned char *, size_t, size_t,
                         struct slcli_json_span *);
/* Private HA2 large-member work plan, independently admitted by owning source.
 * These bounds confer no source closure, original custody or certificate. */
struct slcli_json_ha2_plan { uint32_t tokens,members,array_items; };
int slcli_json_ha2_validate(const unsigned char *,size_t,size_t,
  const struct slcli_json_ha2_plan *,struct slcli_json_span *);
/* Streaming borrowed row offsets avoid a whole array of4096 span objects.
 * Full strict raw validation precedes this iterator; no cursor is authority. */
struct slcli_json_ha2_array_cursor {
  const unsigned char *raw_original;size_t raw_bytes,at,count;
  struct slcli_json_span array_original;
  const struct slcli_json_ha2_plan *plan_original;
  struct slcli_json_ha2_plan plan_snapshot;
  uint32_t started,complete,failed;
};
int slcli_json_ha2_array_next(const unsigned char *,size_t,struct slcli_json_span,
  const struct slcli_json_ha2_plan *,struct slcli_json_ha2_array_cursor *,
  struct slcli_json_span *,int *);
int slcli_json_object(const unsigned char *, size_t, struct slcli_json_span,
                       const char *const *, size_t, struct slcli_json_span *);
int slcli_json_ha2_object(const unsigned char *,size_t,struct slcli_json_span,
  const char *const *,size_t,const struct slcli_json_ha2_plan *,struct slcli_json_span *);
int slcli_json_text(const unsigned char *, size_t, struct slcli_json_span,
                     unsigned char *, size_t, int, size_t *);
int slcli_json_unsigned(const unsigned char *, size_t, struct slcli_json_span,
                         uint64_t, int, uint64_t *);
int slcli_json_array(const unsigned char *, size_t, struct slcli_json_span,
                      struct slcli_json_span *, size_t, size_t *);
/* Separate private canonical ASCII-object predicate after full strict JSON
 * validation/closed-key decoding. Literal ordered keys, no escape/whitespace
 * alternatives; whole=1 additionally requires exact root plus one final LF.
 * This does not change the ordinary public/profile/Q12/C13 JSON grammar. */
int slcli_json_canonical_ascii_object(const unsigned char *,size_t,
  struct slcli_json_span,const char *const *,const struct slcli_json_span *,size_t,int);
#endif
