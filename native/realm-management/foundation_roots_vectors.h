#ifndef SLCLI_FOUNDATION_ROOTS_VECTORS_H
#define SLCLI_FOUNDATION_ROOTS_VECTORS_H
#include "foundation_roots.h"
#if defined(_WIN32) || defined(__linux__)
/* Actual native observation driver needs authentic previously designated held
 * directory originals. No path, identity, protected ACL or caller is fabricated. */
int slcli_management_foundation_roots_native_positive(struct slcli_management_foundation_roots *);
#endif
#endif
