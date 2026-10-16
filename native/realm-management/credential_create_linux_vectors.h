#ifndef SLCLI_MANAGEMENT_CREDENTIAL_CREATE_LINUX_VECTORS_H
#define SLCLI_MANAGEMENT_CREDENTIAL_CREATE_LINUX_VECTORS_H
#include "credential_create_linux.h"
#if defined(__linux__)
/* Requires genuine qualified parent/actor/namespace/SDK/capture owner and
 * original operation deadline. Calls real native create; source fixture is
 * UNRUN until complete whole source review and fresh input admission. */
int slcli_management_credential_linux_native_positive(struct slcli_management_credential_linux_creation *);
#endif
#endif