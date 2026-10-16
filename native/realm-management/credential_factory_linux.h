#ifndef SLCLI_MANAGEMENT_CREDENTIAL_FACTORY_LINUX_H
#define SLCLI_MANAGEMENT_CREDENTIAL_FACTORY_LINUX_H
#include "credential_create_linux.h"
#include "credential_persist.h"
#if defined(__linux__)
/* PRIVATE BQ original preparation. All factory/creator/RNG/three-stream
 * storage is born before native effects. This joins actual file birth and
 * RNG/persistence under SAME original caller/deadline; it does not supply
 * independent designation, readonly transfer, association or signing grant. */
struct slcli_management_credential_linux_factory {
  struct slcli_arena *arena;
  struct slcli_management_credential_linux_creation *creation;
  struct slcli_management_credential_acquisition *acquisition;
  struct slcli_management_credential_persistence *persistence;
  uint32_t prepared,started,unresolved,birth_bound,complete;
};
struct slcli_management_credential_linux_factory *slcli_management_credential_linux_factory_allocate(
  struct slcli_arena *,struct slcli_management_credential_linux_creation *,
  struct slcli_management_credential_acquisition *);
/* Private composition guard only, never independent authority. */
int slcli_management_credential_linux_factory_original_matches(struct slcli_management_credential_linux_factory *);
int slcli_management_credential_linux_factory_run(struct slcli_management_credential_linux_factory *);
int slcli_management_credential_linux_factory_native_positive(struct slcli_management_credential_linux_factory *);
#endif
#endif