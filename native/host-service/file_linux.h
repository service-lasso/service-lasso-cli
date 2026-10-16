#ifndef SLCLI_FILE_LINUX_H
#define SLCLI_FILE_LINUX_H
#if defined(__linux__)
#include <stddef.h>
#include <stdint.h>
#include <linux/stat.h>
#include <linux/time_types.h>
#include <linux/fsverity.h>
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
  /* Selected SDK supplies every ioctl layout/number. These retained arguments
   * are raw native facts, not provider/source authentication. Digest storage
   * uses SDK prefix size, with sufficient alignment for the native structure;
   * no flexible-array structure is embedded before another C member. */
  struct fsverity_enable_arg verity_enable;
  union {
    uint64_t alignment;
    unsigned char raw[sizeof(struct fsverity_digest)+32];
  } verity_digest;
  uint32_t verity_applied,verity_digest_observed,descriptor_closed;
};
int slcli_linux_file_observe(int, struct slcli_linux_file_capture *);
int slcli_linux_directory_source_observe(int,struct slcli_linux_file_capture *);
int slcli_linux_path_source_observe(int,struct slcli_linux_file_capture *,uint32_t);
/* Exact returning native metadata/rights operands and raw clocks only. This
 * never authenticates the descriptor's provider, namespace or owning actor. */
int slcli_linux_file_observation_matches_original(int,const struct slcli_linux_file_capture *,uint32_t);
/* Exact O_RDWR regular-file raw observation; no writer/source admission. */
int slcli_linux_file_writer_observation_matches_original(int,const struct slcli_linux_file_capture *);
/* Metadata-only O_PATH relation, explicitly not readable source rights. */
int slcli_linux_path_observation_matches_original(int,const struct slcli_linux_file_capture *,uint32_t);
int slcli_linux_file_read(int, unsigned char *, size_t, uint64_t,
                          struct slcli_linux_file_capture *);
/* Exact retained pread/clock/raw output tuple only, including genuine zero EOF.
 * The output range remains borrowed; this does not authenticate its source. */
int slcli_linux_file_read_matches_original(int,const unsigned char *,size_t,uint64_t,
 const struct slcli_linux_file_capture *);
int slcli_linux_file_write(int, const unsigned char *, size_t, uint64_t,
                           struct slcli_linux_file_capture *);
int slcli_linux_file_flush(int, struct slcli_linux_file_capture *);
/* Caller must hold the SAME authenticated O_RDONLY descriptor and original
 * finite deadline, and retain the row even if the post-ioctl clock fails.
 * Enable may block in the kernel: this helper cannot bound kernel latency.
 * Actual SDK/kernel/filesystem/page/block geometry and absence of writable
 * aliases are independent installed-path obligations. No settings fallback. */
int slcli_linux_file_verity_enable(int,uint32_t,struct slcli_linux_file_capture *);
int slcli_linux_file_verity_measure(int,struct slcli_linux_file_capture *);
/* Source-owned disposition only. Success closes the actual descriptor before
 * fallible clock diagnostics. An error is retained unresolved and is never
 * retried against a potentially reused numeric fd; this creates no FREE proof. */
int slcli_linux_file_close(int,struct slcli_linux_file_capture *);
#endif
#endif
