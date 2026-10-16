#include "credential_create_windows_vectors.h"
#if defined(_WIN32)
int slcli_management_credential_windows_native_positive(struct slcli_management_credential_windows_creation *c) {
  size_t i;uint64_t tick;NTSTATUS status;HANDLE handle;ULONG_PTR information;
  if(!c||c->started||!slcli_management_credential_windows_create(c)||!c->complete||c->unresolved||
     !c->native_called||!c->native_return_observed||!c->native_output_retained||
     c->native_pending_guard||!c->file_created||!c->descriptor_acquired||
     c->create_status||c->create_io.Status||c->create_io.Information!=FILE_CREATED||
     !c->created_original||c->created_original==INVALID_HANDLE_VALUE||
     !c->security_called||!c->security_observed||!c->security_status||
     c->create_args[0]!=(uintptr_t)&c->created_original||c->create_args[6]!=0||
     c->create_args[7]!=FILE_CREATE||c->created.standard.EndOfFile.QuadPart)return 0;
  for(i=0;i<10;i++)if(!c->clocks[i].complete)return 0;
  tick=c->previous_tick;status=c->create_status;handle=c->created_original;information=c->create_io.Information;
  if(slcli_management_credential_windows_create(c))return 0;
  return c->complete&&!c->unresolved&&c->previous_tick==tick&&c->create_status==status&&
    c->created_original==handle&&c->create_io.Information==information&&
    c->file_created&&c->descriptor_acquired&&!c->native_pending_guard;
}
#endif