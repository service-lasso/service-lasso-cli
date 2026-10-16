#ifndef SLCLI_MANAGEMENT_CREDENTIAL_CREATE_WINDOWS_VECTORS_H
#define SLCLI_MANAGEMENT_CREDENTIAL_CREATE_WINDOWS_VECTORS_H
#include "credential_create_windows.h"
#if defined(_WIN32)
/* Actual native positive body. Genuine enrolled original parent/actor/SD/full
 * permission/SDK/namespace and remaining deadline inputs are required; UNRUN. */
int slcli_management_credential_windows_native_positive(struct slcli_management_credential_windows_creation *);
#endif
#endif