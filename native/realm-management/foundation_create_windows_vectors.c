#include "foundation_create_windows_vectors.h"
#if defined(_WIN32)
int slcli_management_foundation_windows_native_positive(struct slcli_management_foundation_windows_creation *c) {
  HANDLE handle;NTSTATUS status;ULONG_PTR information;uint64_t tick;size_t i;
  if(!c||c->started||!slcli_management_foundation_windows_create(c)||!c->complete||c->unresolved||
     !c->native_called||!c->native_return_observed||!c->native_output_retained||c->native_pending_guard||
     !c->directory_created||!c->descriptor_acquired||c->create_status||c->create_io.Status||
     c->create_io.Information!=FILE_CREATED||!c->created_original||c->created_original==INVALID_HANDLE_VALUE||
     !c->security_called||!c->security_observed||!c->security_status)return 0;
  for(i=0;i<10;i++)if(!c->clocks[i].started||!c->clocks[i].observed||!c->clocks[i].complete)return 0;
  handle=c->created_original;status=c->create_status;information=c->create_io.Information;tick=c->previous_tick;
  if(slcli_management_foundation_windows_create(c))return 0;
  return c->complete&&!c->unresolved&&!c->native_pending_guard&&c->directory_created&&c->descriptor_acquired&&
    c->created_original==handle&&c->create_status==status&&c->create_io.Information==information&&c->previous_tick==tick;
}
#endif
