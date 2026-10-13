#ifndef SLCLI_INITIAL_INSTALLER_LINUX_H
#define SLCLI_INITIAL_INSTALLER_LINUX_H
#include <stddef.h>
#include <stdint.h>
/* Pure observations over a separately acquired, retained initial stack extent.
 * No pointer, aux value, decoded result or success supplies that acquisition. */
struct slcli_initial_stack_linux_original {
  uintptr_t stack_start, extent_start;
  size_t extent_bytes, argc, environment_count, auxiliary_count;
  const uintptr_t *argv_original, *environment_original, *auxiliary_original;
  uintptr_t phdr, phent, phnum, entry, base, page_bytes, execfn, random;
};
/* Refusal leaves output unchanged. Source extent and output must be disjoint.
 * All pointed argv/env/execfn strings and random bytes must lie in SAME extent.
 * Mandatory aux keys occur exactly once, including AT_BASE with value zero. */
int slcli_initial_stack_linux_decode_original(uintptr_t stack_start,
  const void *acquired_stack_begin,size_t acquired_stack_bytes,
  struct slcli_initial_stack_linux_original *output);
#endif