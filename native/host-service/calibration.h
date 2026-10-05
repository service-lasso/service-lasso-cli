#ifndef SLCLI_CALIBRATION_H
#define SLCLI_CALIBRATION_H
#include "json.h"
/* Exact original source codecs; decoded numbers/digests are claims, not an
 * owner constructor, independently admitted ROOT or measurement authority. */
struct slcli_qualification {
  unsigned char profile[32],source[32],image[32],cases[32],capacity[32],entry[32];
  uint64_t maximum,page;
  uint32_t domain,objects;
};
struct slcli_evidence_reference {
  unsigned char name[240],digest[32],source[32];
  uint64_t bytes;
  uint32_t name_bytes;
};
struct slcli_calibration {
  unsigned char profile[32],source[32],raw_digest[32];
  uint64_t maximum,metadata,runtime,stack,io,page,peak;
  uint32_t domain,objects,evidence_count;
  const unsigned char *raw;
  size_t raw_bytes;
  struct slcli_evidence_reference evidence[128];
};
int slcli_qualification_parse(const unsigned char *,size_t,const unsigned char[32],
                               struct slcli_qualification *);
int slcli_calibration_parse(const unsigned char *,size_t,const unsigned char[32],
                             struct slcli_calibration *);
#endif
