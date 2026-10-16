#ifndef SLCLI_UNIT19_RANGE_VECTORS_H
#define SLCLI_UNIT19_RANGE_VECTORS_H
#include "unit19_range.h"
#if defined(_WIN32) || defined(__linux__)
int slcli_unit19_range_positive_vector(struct slcli_unit19_range *,
  const struct slcli_unit19_reader *,uint32_t,slcli_input_handle);
/* Actual bound native positive driver with repeat-no-effect check. Authentic
 * caller/held immutable unit/deadline/namespace inputs must preexist; UNRUN. */
int slcli_unit19_range_deadline_positive_vector(struct slcli_unit19_range *,
  const struct slcli_unit19_reader *,uint32_t,slcli_input_handle,
  const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,slcli_store_capture_sink,void *,size_t);
/* Read-only postcondition for a driver which has already called the actual
 * native consumer. Calling this predicate alone is not a native exercise. */
int slcli_unit19_range_completed_vector(const struct slcli_unit19_range *,
  const struct slcli_unit19_reader *,uint32_t,slcli_input_handle);
#endif
#endif
