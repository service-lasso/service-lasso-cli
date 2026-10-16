#ifndef SLCLI_INPUT_CONSUME_VECTORS_H
#define SLCLI_INPUT_CONSUME_VECTORS_H
#include "input_consume.h"
#include "input_deadline.h"
#if defined(_WIN32) || defined(__linux__)
struct slcli_input_vector_original {
  slcli_input_handle original;
  uint64_t bytes;
  unsigned char digest[32];
  size_t capture_rows;
  /* Genuine private fixture source tuple, not authority from a callback or
   * digest. All exact constructor blocks/deadline/capture custody preexist. */
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *predecessor;
  const void *caller_original;
  size_t caller_bytes;
  slcli_store_capture_sink capture_sink;
  void *capture_original;
  size_t capture_original_bytes;
};
struct slcli_input_vector_result {
  struct slcli_input_body *bodies[4];
  struct slcli_profile *profile;
  struct slcli_grant *grant;
  struct slcli_qualification *qualification;
  struct slcli_calibration *calibration;
  uint32_t started,complete;
};
int slcli_input_consume_positive_vector(struct slcli_arena *,
  const struct slcli_input_vector_original[4],struct slcli_input_vector_result *);
int slcli_input_consume_alias_vector(struct slcli_input_body *);
struct slcli_actor19_vector_result {
  struct slcli_input_body *profile_body,*actors_body;
  struct slcli_profile *profile;
  struct slcli_actors19 *actors;
  uint32_t started,complete;
};
/* Source-native fixture supplies two independently admitted held originals
 * and a genuine already fully read role13. No fixture path creates a seal. */
int slcli_input_actors19_positive_vector(struct slcli_arena *,
  const struct slcli_input_vector_original[2],const struct slcli_unit19_reader *,
  struct slcli_actor19_vector_result *);
#endif
#endif
