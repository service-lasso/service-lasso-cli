#ifndef SLCLI_MANAGEMENT_CREDENTIAL_ACQUIRE_VECTORS_H
#define SLCLI_MANAGEMENT_CREDENTIAL_ACQUIRE_VECTORS_H
#include "credential_persist.h"
#if defined(_WIN32) || defined(__linux__)
/* Native positive source driver needs authentic private BQ caller, empty held
 * file, admitted capture owner and all prepared storage. It grants none of
 * these, never supplies fixed secret bytes, and remains UNRUN. */
int slcli_management_credential_native_positive(
  struct slcli_management_credential_acquisition *,
  struct slcli_management_credential_persistence *);
#endif
#endif
