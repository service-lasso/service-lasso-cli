#include "credential_create_linux_vectors.h"
#if defined(__linux__)
int slcli_management_credential_linux_native_positive(struct slcli_management_credential_linux_creation *c) {
  size_t i;uint64_t tick,status;int fd;
  if(!c||c->started||!slcli_management_credential_linux_create(c)||
     !c->complete||c->unresolved||!c->uid_called||!c->uid_observed||
     !c->open_called||!c->open_observed||!c->file_created||!c->descriptor_acquired||
     c->created_original<0||c->open_status!=(uint64_t)c->created_original||
     c->open_args[0]!=(uint64_t)c->parent_original||
     c->open_args[1]!=(uintptr_t)c->name||c->open_args[3]!=0600)return 0;
  for(i=0;i<10;i++)if(!c->clocks[i].complete)return 0;
  tick=c->previous_tick;status=c->open_status;fd=c->created_original;
  if(slcli_management_credential_linux_create(c))return 0;
  return c->previous_tick==tick&&c->open_status==status&&c->created_original==fd&&
    c->complete&&!c->unresolved&&c->file_created&&c->descriptor_acquired;
}
#endif