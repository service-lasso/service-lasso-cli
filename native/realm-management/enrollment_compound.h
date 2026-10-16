#ifndef SLCLI_MANAGEMENT_ENROLLMENT_COMPOUND_H
#define SLCLI_MANAGEMENT_ENROLLMENT_COMPOUND_H
#include "enrollment_builder.h"
#include "../host-service/unit19_native.h"
/* New original compound14/5 preparation. Two retained independently designated
 * records select anchor32 then exact signed enrollment; no old unit is repacked.
 * Native seal, original ROOT/provider/caller association admission and readonly
 * independent compound consumption still follow, never inferred from preparation. */
struct slcli_management_enrollment_compound {
  struct slcli_arena *arena;
  struct slcli_unit19_native *native;
  struct slcli_unit19_framing framing;
  const unsigned char *members[2];
  struct slcli_unit19_record records[2];
  const struct slcli_management_enrollment_builder *enrollment;
  uint32_t started,failed,prepared;
};
struct slcli_management_enrollment_compound *slcli_management_enrollment_compound_allocate(
  struct slcli_arena *,size_t,size_t);
int slcli_management_enrollment_compound_prepare(struct slcli_management_enrollment_compound *,
  const struct slcli_management_enrollment_builder *,const struct slcli_unit19_record[2],
  uint32_t,slcli_input_handle,slcli_store_capture_sink,void *,uint64_t);
int slcli_management_enrollment_compound_positive_original(struct slcli_management_enrollment_compound *,
  const struct slcli_management_enrollment_builder *,const struct slcli_unit19_record[2],
  uint32_t,slcli_input_handle,slcli_store_capture_sink,void *,uint64_t);
#endif
