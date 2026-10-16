#ifndef SLCLI_ACTORS19_H
#define SLCLI_ACTORS19_H
#include "profile.h"
#include "unit19.h"
enum { SLCLI_ACTOR19_COUNT=15,SLCLI_ACTOR19_PUBLIC_FACADE=14 };
struct slcli_actor19 {
  unsigned char commit[40],association[32],image[32],entry[32];
  struct slcli_json_span owner;
};
struct slcli_actors19 {
  const unsigned char *raw;
  size_t bytes;
  unsigned char digest[32];
  struct slcli_actor19 actors[SLCLI_ACTOR19_COUNT];
};
/* Closed T2 representation and independent source-range/profile comparisons.
 * The actual fifteen image/entry/native owner/birth/source associations and
 * complete SourceUnit12/role13 admission remain separate installed gates. */
int slcli_actors19_parse(const unsigned char *,size_t,struct slcli_actors19 *);
int slcli_actors19_role_unit_matches(const struct slcli_actors19 *,
  const struct slcli_unit19_reader *);
int slcli_actors19_facade_profile_matches(const struct slcli_actors19 *,const struct slcli_profile *);
#endif
