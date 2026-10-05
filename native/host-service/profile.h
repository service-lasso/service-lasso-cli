#ifndef SLCLI_PROFILE_H
#define SLCLI_PROFILE_H
#include "json.h"
/* Borrowed original profile spans, never an admission seal or compiled
 * self-referential profile catalog. Owning raw-body lease outlives every span. */
struct slcli_profile {
  const unsigned char *raw;
  size_t raw_bytes;
  unsigned char digest[32];
  struct slcli_json_span root[10],service[4],endpoint[2],owner[1],store[4];
  struct slcli_json_span images[4],control[3],retention[5],launch[11];
  struct slcli_json_span launch_endpoint[2],image_bindings[3][5],library[7],copies[3];
  uint64_t retention_maximum,record_maximum,request_maximum,result_maximum;
  uint64_t capture_maximum,stdout_maximum,stderr_maximum;
};
int slcli_profile_parse(const unsigned char *,size_t,struct slcli_profile *);
#endif
