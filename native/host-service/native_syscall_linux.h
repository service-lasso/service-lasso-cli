#ifndef SLCLI_NATIVE_SYSCALL_LINUX_H
#define SLCLI_NATIVE_SYSCALL_LINUX_H
#if defined(__linux__)
#include <asm/unistd.h>
/* Only the selected original SDK defines operation numbers. Original ABI width
 * and adapter source must be independently admitted before native execution. */
static inline long slcli_kernel_call(long n, long a, long b, long c,
                                     long d, long e, long f) {
#if defined(__x86_64__)
  register long r10 __asm__("r10") = d;
  register long r8 __asm__("r8") = e;
  register long r9 __asm__("r9") = f;
  long result;
  __asm__ volatile("syscall" : "=a"(result)
    : "a"(n), "D"(a), "S"(b), "d"(c), "r"(r10), "r"(r8), "r"(r9)
    : "rcx", "r11", "memory");
  return result;
#elif defined(__aarch64__)
  register long x8 __asm__("x8") = n;
  register long x0 __asm__("x0") = a;
  register long x1 __asm__("x1") = b;
  register long x2 __asm__("x2") = c;
  register long x3 __asm__("x3") = d;
  register long x4 __asm__("x4") = e;
  register long x5 __asm__("x5") = f;
  __asm__ volatile("svc 0" : "+r"(x0)
    : "r"(x8), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5) : "memory", "cc");
  return x0;
#else
#error Original native syscall ABI needs separate reviewed source support
#endif
}
#endif
#endif
