#if defined(__linux__)
#include "file_linux.h"
#include "native_syscall_linux.h"
#include <linux/fcntl.h>
#include <linux/time.h>

static void zero(void *value, size_t bytes) {
  unsigned char *p = value;
  while (bytes--) *p++ = 0;
}
static int disjoint(const struct slcli_linux_file_capture *c,
                      const void *data, size_t bytes) {
  uintptr_t a=(uintptr_t)c,b=(uintptr_t)data;
  return a<=UINTPTR_MAX-sizeof(*c)&&bytes<=UINTPTR_MAX&&
           b<=UINTPTR_MAX-bytes&&!(a<b+bytes&&b<a+sizeof(*c));
}
static int clock_observe(struct slcli_linux_file_capture *c) {
  long result;
  c->clock_args[0] = CLOCK_MONOTONIC;
  c->clock_args[1] = (uintptr_t)&c->clock_raw;
  result = slcli_kernel_call(__NR_clock_gettime, CLOCK_MONOTONIC,
                                   (long)&c->clock_raw, 0, 0, 0, 0);
  c->clock_status = (uint64_t)result; c->clock_observed = 1;
  if (result || c->clock_raw.tv_sec < 0 || c->clock_raw.tv_nsec < 0 ||
      c->clock_raw.tv_nsec >= 1000000000 ||
      (uint64_t)c->clock_raw.tv_sec >
        (UINT64_MAX - (uint64_t)c->clock_raw.tv_nsec) / 1000000000) return 0;
  c->tick = (uint64_t)c->clock_raw.tv_sec * 1000000000 +
             (uint64_t)c->clock_raw.tv_nsec;
  return c->tick != 0;
}
int slcli_linux_file_observe(int original_fd,
                             struct slcli_linux_file_capture *c) {
  static const char empty[] = "";
  long result;
  if (!c || c->operation || original_fd < 0) return 0;
  zero(c, sizeof(*c));
  c->operation = 1;
  c->stat_args[0] = (uint64_t)original_fd;
  c->stat_args[1] = (uintptr_t)&c->original_stat;
  result = slcli_kernel_call(__NR_fstat, original_fd, (long)&c->original_stat,
                             0, 0, 0, 0);
  c->stat_status = (uint64_t)result; c->stat_observed = 1;
  if (result) { clock_observe(c); return 0; }
  c->flags_args[0] = (uint64_t)original_fd; c->flags_args[1] = F_GETFL;
  result = slcli_kernel_call(__NR_fcntl, original_fd, F_GETFL, 0, 0, 0, 0);
  c->flags_status = (uint64_t)result; c->flags_observed = 1;
  if (result < 0) { clock_observe(c); return 0; }
  c->descriptor_args[0] = (uint64_t)original_fd;
  c->descriptor_args[1] = F_GETFD;
  result = slcli_kernel_call(__NR_fcntl, original_fd, F_GETFD, 0, 0, 0, 0);
  c->descriptor_status = (uint64_t)result; c->descriptor_observed = 1;
  if (result < 0) { clock_observe(c); return 0; }
  c->args[0] = (uint64_t)original_fd;
  c->args[1] = (uintptr_t)empty;
  c->args[2] = AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW;
  c->args[3] = STATX_BASIC_STATS;
  c->args[4] = (uintptr_t)&c->identity;
  result = slcli_kernel_call(__NR_statx, original_fd, (long)empty,
                             (long)c->args[2], STATX_BASIC_STATS,
                             (long)&c->identity, 0);
  c->status = (uint64_t)result; c->observed = 1;
  /* Preserve the real result/structure even when clock or required identity
   * fails. An fd number or successful statx alone cannot install a lease. */
  if (!clock_observe(c)) return 0;
  if (result || (c->identity.stx_mask & STATX_BASIC_STATS) != STATX_BASIC_STATS ||
      (c->identity.stx_mode & S_IFMT) != S_IFREG ||
      (c->original_stat.st_mode & S_IFMT) != S_IFREG ||
      c->original_stat.st_size < 0 ||
      (uint64_t)c->original_stat.st_ino != c->identity.stx_ino ||
      (uint64_t)c->original_stat.st_size != c->identity.stx_size ||
      c->original_stat.st_uid != c->identity.stx_uid ||
      c->original_stat.st_gid != c->identity.stx_gid ||
      c->original_stat.st_mode != c->identity.stx_mode ||
      c->original_stat.st_nlink != c->identity.stx_nlink) return 0;
  return 1;
}
int slcli_linux_file_read(int original_fd, unsigned char *destination,
                          size_t bytes, uint64_t offset,
                          struct slcli_linux_file_capture *c) {
  long result;
  if (!c || c->operation || original_fd < 0 || !destination || !bytes || bytes > INT64_MAX ||
      offset > INT64_MAX || bytes > (uint64_t)INT64_MAX - offset) return 0;
  if (!disjoint(c,destination,bytes)) return 0;
  zero(c, sizeof(*c));
  c->operation = 2;
  c->args[0] = (uint64_t)original_fd; c->args[1] = (uintptr_t)destination;
  c->args[2] = bytes; c->args[3] = offset;
  result = slcli_kernel_call(__NR_pread64, original_fd, (long)destination,
                             (long)bytes, (long)offset, 0, 0);
  c->status = (uint64_t)result; c->observed = 1;
  if (result >= 0 && (uint64_t)result <= bytes) {
    c->returned_bytes = (uint64_t)result; c->returned_bytes_observed = 1;
  }
  /* Actual zero return is preserved as an observed empty native read. The
   * source-owning capture sink must persist its real empty body and EOF event;
   * END or a descriptor's requested length cannot manufacture this result. */
  if (!clock_observe(c)) return 0;
  return result >= 0 && (uint64_t)result <= bytes;
}
int slcli_linux_file_write(int original_fd, const unsigned char *source,
                           size_t bytes, uint64_t offset,
                           struct slcli_linux_file_capture *c) {
  long result;
  if (!c || c->operation || original_fd < 0 || !source || !bytes || bytes > 16384 ||
      offset > INT64_MAX || bytes > (uint64_t)INT64_MAX - offset) return 0;
  if (!disjoint(c,source,bytes)) return 0;
  zero(c, sizeof(*c)); c->operation = 3;
  c->args[0] = (uint64_t)original_fd; c->args[1] = (uintptr_t)source;
  c->args[2] = bytes; c->args[3] = offset;
  result = slcli_kernel_call(__NR_pwrite64, original_fd, (long)source,
                             (long)bytes, (long)offset, 0, 0);
  c->status = (uint64_t)result; c->observed = 1;
  if (result >= 0 && (uint64_t)result <= bytes) {
    c->returned_bytes = (uint64_t)result; c->returned_bytes_observed = 1;
  }
  if (!clock_observe(c)) return 0;
  /* Short writes remain original short writes. Owning code advances only by
   * their actual count after preserving the capture, never by requested size. */
  return result >= 0 && (uint64_t)result <= bytes;
}
int slcli_linux_file_flush(int original_fd, struct slcli_linux_file_capture *c) {
  long result;
  if (!c || c->operation || original_fd < 0) return 0;
  zero(c, sizeof(*c)); c->operation = 4;
  c->args[0] = (uint64_t)original_fd;
  result = slcli_kernel_call(__NR_fsync, original_fd, 0, 0, 0, 0, 0);
  c->status = (uint64_t)result; c->observed = 1;
  if (!clock_observe(c)) return 0;
  return result == 0;
}
#endif
