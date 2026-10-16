#ifndef SLCLI_CLOSE_NATIVE_H
#define SLCLI_CLOSE_NATIVE_H
#include "input_body.h"
#include "native_clock.h"
#if defined(_WIN32) || defined(__linux__)
struct slcli_close_capture {
  slcli_input_handle original;
  uint64_t args[1],status,error;
  uint32_t started,observed,closed;
  struct slcli_native_clock clock;
};
/* Actual original source-owned native descriptor/handle only. Reserve the
 * once-only attempt before native close. A failure/unknown observation never
 * permits repeating close against a reused integer or manufacturing closure. */
int slcli_close_original(struct slcli_close_capture *,slcli_input_handle);
#endif
#endif
