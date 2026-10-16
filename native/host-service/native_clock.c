#include "native_clock.h"
#include <stddef.h>
#if defined(__linux__)
#include "native_syscall_linux.h"
#include <linux/time_types.h>
#include <linux/time.h>
#endif
int slcli_native_clock_read(struct slcli_native_clock *c) {
  uint64_t whole,fraction;
  if(!c||c->started||c->observed||c->complete||c->calls_called||c->calls_returned||
     c->tick||c->args[0]||c->args[1]||c->raw[0]||c->raw[1]||c->status[0]||c->status[1])return 0;
#if defined(__linux__)
  if(c->native_linux_output.tv_sec||c->native_linux_output.tv_nsec)return 0;
#elif defined(_WIN32)
  if(c->native_windows_counter.QuadPart||c->native_windows_frequency.QuadPart)return 0;
#endif
  c->started=1;
#if defined(_WIN32)
  {
    LARGE_INTEGER *counter=&c->native_windows_counter,*frequency=&c->native_windows_frequency;BOOL a,b;
    c->args[0]=(uintptr_t)counter;c->args[1]=(uintptr_t)frequency;
    c->calls_called=1;a=QueryPerformanceCounter(counter);
    c->status[0]=(uint32_t)a;c->raw[0]=(uint64_t)counter->QuadPart;c->calls_returned=1;
    /* First authentic native prefix is retained before the next effect. */
    c->calls_called=2;b=QueryPerformanceFrequency(frequency);
    c->status[1]=(uint32_t)b;c->raw[1]=(uint64_t)frequency->QuadPart;c->calls_returned=2;c->observed=1;
    if(!a||!b||counter->QuadPart<=0||frequency->QuadPart<=0)return 0;
    whole=(uint64_t)(counter->QuadPart/frequency->QuadPart);
    fraction=(uint64_t)(counter->QuadPart%frequency->QuadPart);
    if(whole>UINT64_MAX/1000000000||fraction>UINT64_MAX/1000000000)return 0;
    whole*=1000000000;fraction=fraction*1000000000/(uint64_t)frequency->QuadPart;
  }
#elif defined(__linux__)
  {
    struct __kernel_timespec *raw=&c->native_linux_output;long result;
    c->args[0]=CLOCK_MONOTONIC;c->args[1]=(uintptr_t)raw;
    c->calls_called=1;
    result=slcli_kernel_call(__NR_clock_gettime,CLOCK_MONOTONIC,(long)raw,0,0,0,0);
    c->status[0]=(uint64_t)result;c->observed=1;
    c->raw[0]=(uint64_t)raw->tv_sec;c->raw[1]=(uint64_t)raw->tv_nsec;c->calls_returned=1;
    if(result||raw->tv_sec<0||raw->tv_nsec<0||raw->tv_nsec>=1000000000||
       (uint64_t)raw->tv_sec>UINT64_MAX/1000000000)return 0;
    whole=(uint64_t)raw->tv_sec*1000000000;fraction=(uint64_t)raw->tv_nsec;
  }
#else
  return 0;
#endif
  if(fraction>UINT64_MAX-whole||!(c->tick=whole+fraction))return 0;
  c->complete=1;return 1;
}
#if defined(_WIN32)
int slcli_native_clock_windows_matches_original(const struct slcli_native_clock *c){
  uint64_t whole,fraction;
  if(!c||c->started!=1||c->observed!=1||c->complete!=1||
     c->calls_called!=2||c->calls_returned!=2||!c->status[0]||!c->status[1]||
     c->status[0]>UINT32_MAX||c->status[1]>UINT32_MAX||
     c->args[0]!=(uintptr_t)&c->native_windows_counter||
     c->args[1]!=(uintptr_t)&c->native_windows_frequency||
     c->native_windows_counter.QuadPart<=0||c->native_windows_frequency.QuadPart<=0||
     c->raw[0]!=(uint64_t)c->native_windows_counter.QuadPart||
     c->raw[1]!=(uint64_t)c->native_windows_frequency.QuadPart)return 0;
  whole=c->raw[0]/c->raw[1];fraction=c->raw[0]%c->raw[1];
  if(whole>UINT64_MAX/1000000000||fraction>UINT64_MAX/1000000000)return 0;
  whole*=1000000000;fraction=fraction*1000000000/c->raw[1];
  return fraction<=UINT64_MAX-whole&&c->tick&&c->tick==whole+fraction;
}
int slcli_native_clock_windows_representation_matches_original(
 const struct slcli_native_clock *copy,const struct slcli_native_clock *original){
  uintptr_t x=(uintptr_t)copy,y=(uintptr_t)original;size_t i;
  if(!copy||!original||x>UINTPTR_MAX-sizeof(*copy)||y>UINTPTR_MAX-sizeof(*original)||
     (x<y+sizeof(*original)&&y<x+sizeof(*copy))||
     !slcli_native_clock_windows_matches_original(original))return 0;
  for(i=0;i<2;i++)if(copy->args[i]!=original->args[i]||copy->raw[i]!=original->raw[i]||
    copy->status[i]!=original->status[i])return 0;
  return copy->tick==original->tick&&copy->started==original->started&&
    copy->observed==original->observed&&copy->complete==original->complete&&
    copy->calls_called==original->calls_called&&copy->calls_returned==original->calls_returned&&
    copy->native_windows_counter.QuadPart==original->native_windows_counter.QuadPart&&
    copy->native_windows_frequency.QuadPart==original->native_windows_frequency.QuadPart;
}
#elif defined(__linux__)
int slcli_native_clock_linux_matches_original(const struct slcli_native_clock *c){
  uint64_t whole,fraction;
  if(!c||c->started!=1||c->observed!=1||c->complete!=1||
     c->calls_called!=1||c->calls_returned!=1||c->status[0]||c->status[1]||
     c->args[0]!=CLOCK_MONOTONIC||c->args[1]!=(uintptr_t)&c->native_linux_output||
     c->native_linux_output.tv_sec<0||c->native_linux_output.tv_nsec<0||
     c->native_linux_output.tv_nsec>=1000000000||
     c->raw[0]!=(uint64_t)c->native_linux_output.tv_sec||
     c->raw[1]!=(uint64_t)c->native_linux_output.tv_nsec||
     c->raw[0]>UINT64_MAX/1000000000)return 0;
  whole=c->raw[0]*1000000000;fraction=c->raw[1];
  return fraction<=UINT64_MAX-whole&&c->tick&&c->tick==whole+fraction;
}
int slcli_native_clock_linux_representation_matches_original(
 const struct slcli_native_clock *copy,const struct slcli_native_clock *original){
  uintptr_t x=(uintptr_t)copy,y=(uintptr_t)original;size_t i;
  if(!copy||!original||x>UINTPTR_MAX-sizeof(*copy)||y>UINTPTR_MAX-sizeof(*original)||
     (x<y+sizeof(*original)&&y<x+sizeof(*copy))||
     !slcli_native_clock_linux_matches_original(original))return 0;
  for(i=0;i<2;i++)if(copy->args[i]!=original->args[i]||copy->raw[i]!=original->raw[i]||
    copy->status[i]!=original->status[i])return 0;
  return copy->tick==original->tick&&copy->started==original->started&&
    copy->observed==original->observed&&copy->complete==original->complete&&
    copy->calls_called==original->calls_called&&copy->calls_returned==original->calls_returned&&
    copy->native_linux_output.tv_sec==original->native_linux_output.tv_sec&&
    copy->native_linux_output.tv_nsec==original->native_linux_output.tv_nsec;
}
#endif
