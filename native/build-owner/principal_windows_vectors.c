/* Metadata-only issuing-plane regression cases. These zeroed field carriers
 * have NO native captures, token identity, genuine original or authority. The
 * pure helper does not claim any: full native/source checks remain mandatory. */
#include "principal_windows.h"
#if defined(_WIN32)
int slcli_build_principal_windows_vectors(void) {
 struct slcli_build_principal_windows effective={0},primary={0},other={0};
 primary.process_primary_only=1;primary.issuing_effective_original=&effective;
 if(!slcli_build_principal_windows_issuing_relation_original(&effective,&primary))return 0;
 /* A file attributed to process-primary recreates the actual prebirth bug. */
 if(slcli_build_principal_windows_issuing_relation_original(&primary,&primary))return 0;
 if(slcli_build_principal_windows_issuing_relation_original(&other,&primary))return 0;
 if(slcli_build_principal_windows_issuing_relation_original(&effective,&other))return 0;
 if(slcli_build_principal_windows_issuing_relation_original(NULL,&primary))return 0;
 if(slcli_build_principal_windows_issuing_relation_original(&effective,NULL))return 0;
 primary.issuing_effective_original=&other;
 if(slcli_build_principal_windows_issuing_relation_original(&effective,&primary))return 0;
 primary.issuing_effective_original=&effective;effective.process_primary_only=1;
 if(slcli_build_principal_windows_issuing_relation_original(&effective,&primary))return 0;
 effective.process_primary_only=0;primary.process_primary_only=2;
 if(slcli_build_principal_windows_issuing_relation_original(&effective,&primary))return 0;
 return 1;
}
#endif
