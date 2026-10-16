#ifndef SLCLI_NATIVE_CLOCK_H
#define SLCLI_NATIVE_CLOCK_H
#include <stdint.h>
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
#if defined(__linux__)
#include <linux/time_types.h>
#endif
/* Fresh source-owned one-use raw clock observation. Native time is a deadline
 * fact, never original actor/ROOT authority or resource-closure evidence. */
struct slcli_native_clock {
  uint64_t args[2],raw[2],status[2],tick;
  uint32_t started,observed,complete;
  uint32_t calls_called,calls_returned;
#if defined(_WIN32)
  LARGE_INTEGER native_windows_counter,native_windows_frequency;
#endif
#if defined(__linux__)
  struct __kernel_timespec native_linux_output;
#endif
};
int slcli_native_clock_read(struct slcli_native_clock *);
#if defined(_WIN32)
int slcli_native_clock_windows_matches_original(const struct slcli_native_clock *);
int slcli_native_clock_windows_representation_matches_original(
 const struct slcli_native_clock *,const struct slcli_native_clock *);
#endif
#if defined(__linux__)
int slcli_native_clock_linux_matches_original(const struct slcli_native_clock *);
/* SAME-process representation correspondence only; original remains borrowed.
 * Copied syscall pointers stay unchanged, never become a second occurrence. */
int slcli_native_clock_linux_representation_matches_original(
 const struct slcli_native_clock *,const struct slcli_native_clock *);
#endif
#endif
