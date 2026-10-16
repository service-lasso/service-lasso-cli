#include "close_native.h"
#if defined(__linux__)
#include "native_syscall_linux.h"
#endif
#if defined(_WIN32) || defined(__linux__)
int slcli_close_original(struct slcli_close_capture *c,slcli_input_handle h) {
  if(!c||c->started)return 0;
#if defined(_WIN32)
  if(!h||h==INVALID_HANDLE_VALUE)return 0;
#else
  if(h<0)return 0;
#endif
  c->started=1;c->original=h;c->args[0]=(uintptr_t)h;
#if defined(_WIN32)
  c->status=(uint32_t)CloseHandle(h);c->observed=1;
  if(!c->status)c->error=GetLastError();else c->closed=1;
#else
  {
    long status=slcli_kernel_call(__NR_close,h,0,0,0,0,0);
    c->status=(uint64_t)status;c->observed=1;c->closed=status==0;
  }
#endif
  if(!slcli_native_clock_read(&c->clock))return 0;
  return c->closed!=0;
}
#endif
