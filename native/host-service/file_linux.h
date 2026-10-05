#ifndef SLCLI_FILE_LINUX_H
#define SLCLI_FILE_LINUX_H
#if defined(__linux__)
#include <stddef.h>
#include <stdint.h>
#include <linux/stat.h>
#include <linux/time_types.h>
#include <asm/stat.h>
/* Raw native operations on an already retained original descriptor. These
 * structures authenticate neither its source nor the provider/caller. The
 * owning native loop must persist each actual capture and returned bytes before
 * any storage can be retired; no aggregate Boolean EOF substitute. Each row is
 * fresh zeroed owner storage and one-use: operation is set before the first
 * native effect, and every later call rejects it without changing any bytes.
 * There is no caller reset/erase permission, including after synchronous return. */
struct slcli_linux_file_capture {
  uint64_t args[6], status, clock_args[2], clock_status, tick;
  struct __kernel_timespec clock_raw;
  struct statx identity;
  struct stat original_stat;
  uint64_t stat_args[2], stat_status, flags_args[3], flags_status;
  uint64_t descriptor_args[3], descriptor_status;
  uint32_t stat_observed, flags_observed, descriptor_observed;
  uint32_t operation, observed, clock_observed, returned_bytes_observed;
  uint64_t returned_bytes;
};
int slcli_linux_file_observe(int, struct slcli_linux_file_capture *);
int slcli_linux_file_read(int, unsigned char *, size_t, uint64_t,
                          struct slcli_linux_file_capture *);
int slcli_linux_file_write(int, const unsigned char *, size_t, uint64_t,
                           struct slcli_linux_file_capture *);
int slcli_linux_file_flush(int, struct slcli_linux_file_capture *);
#endif
#endif
