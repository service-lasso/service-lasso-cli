#ifndef SLCLI_UNIT19_NATIVE_VECTORS_H
#define SLCLI_UNIT19_NATIVE_VECTORS_H
#include "unit19_native.h"
#if defined(_WIN32) || defined(__linux__)
/* Actual original unit-first sources/native output file and capture sink must
 * already be independently admitted for preparation. All originals stay held.
 * This caller produces no passing certificate, principal, seal or source grant. */
int slcli_unit19_native_positive_vector(struct slcli_unit19_native *,
  struct slcli_unit19_framing *,enum slcli_unit19_kind,uint32_t,
  const struct slcli_unit19_record *,size_t,const unsigned char *const *,
  const uint64_t *,size_t,slcli_input_handle,slcli_store_capture_sink,void *,uint64_t);
/* Fresh original deadline owner, then real full encode/write/flush/held-read
 * source driver and repeat-no-effect check. Inputs remain authentic external
 * originals; no native qualification or seal follows before execution. */
int slcli_unit19_native_deadline_positive_vector(struct slcli_unit19_native *,
  struct slcli_unit19_framing *,enum slcli_unit19_kind,uint32_t,
  const struct slcli_unit19_record *,size_t,const unsigned char *const *,
  const uint64_t *,size_t,slcli_input_handle,slcli_store_capture_sink,void *,uint64_t,
  const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,size_t);
/* Exact selected unit26 upper geometry requires the genuine independently
 * admitted twelve full bodies, including1MiB envelope; no padding/fake body. */
int slcli_unit26_native_full_geometry_vector(struct slcli_unit19_native *,
  struct slcli_unit19_framing *,uint32_t,const struct slcli_unit19_record[12],
  const unsigned char *const [12],slcli_input_handle,slcli_store_capture_sink,void *,uint64_t);
#endif
#endif
