#include "foundation_create_linux_vectors.h"
#if defined(__linux__)
int slcli_management_foundation_linux_native_positive(struct slcli_management_foundation_linux_creation *c) {
  uint64_t mkdir_status,open_status,tick;int fd;size_t i;
  if(!c||c->started||!slcli_management_foundation_linux_create(c)||
     !c->complete||c->unresolved||!c->directory_created||!c->descriptor_acquired||
     !c->mkdir_called||!c->mkdir_observed||!c->open_called||!c->open_observed||
     c->mkdir_status||c->created_original<0||c->open_status!=(uint64_t)c->created_original)return 0;
  for(i=0;i<12;i++)if(!c->clocks[i].started||!c->clocks[i].observed||
    !c->clocks[i].complete)return 0;
  mkdir_status=c->mkdir_status;open_status=c->open_status;tick=c->previous_tick;fd=c->created_original;
  if(slcli_management_foundation_linux_create(c))return 0;
  return c->complete&&!c->unresolved&&c->directory_created&&c->descriptor_acquired&&
    c->mkdir_status==mkdir_status&&c->open_status==open_status&&c->created_original==fd&&
    c->previous_tick==tick;
}
#endif
