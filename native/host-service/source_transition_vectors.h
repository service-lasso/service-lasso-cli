#ifndef SLCLI_SOURCE_TRANSITION_VECTORS_H
#define SLCLI_SOURCE_TRANSITION_VECTORS_H
#include "source_transition.h"
#include "service_channel.h"
#if defined(_WIN32) || defined(__linux__)
/* Private qualification callers require independently admitted original native
 * owner, origin, caller objects and charged clock rows. They enroll no actor or
 * thread, construct no credential and perform no fixture admission. */
struct slcli_source_expiry_fixture {
  struct slcli_native_clock *rows;
  size_t capacity,used;
  uint32_t started,expired,exhausted,clock_failed;
};
int slcli_source_nested_positive_vector(struct slcli_service_request *,
  struct slcli_primary_engine *,const struct slcli_primary_commit_originals *);
int slcli_source_native_expiry_vector(struct slcli_primary_engine *,
  const struct slcli_primary_commit_originals *,struct slcli_source_expiry_fixture *);
#endif
#endif
