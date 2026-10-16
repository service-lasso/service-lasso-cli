#ifndef SLCLI_NATIVE_CLOCK_VECTORS_H
#define SLCLI_NATIVE_CLOCK_VECTORS_H
#include "native_clock.h"
#include <stddef.h>
/* Actual clock API caller and representation-only no-effect preservation.
 * Genuine source/native/caller/charged scratch/deadline fixture required; UNRUN. */
int slcli_native_clock_positive_original(struct slcli_native_clock *,unsigned char *,size_t);
int slcli_native_clock_markerless_vectors(void);
#endif