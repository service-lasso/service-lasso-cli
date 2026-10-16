#if defined(__linux__)
#include "file_linux.h"
#include "native_syscall_linux.h"
#include <linux/fcntl.h>
#include <linux/time.h>
static const char observation_empty[] = "";

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
static int observe_kind(int original_fd,
                             struct slcli_linux_file_capture *c,uint32_t kind) {
  const char *empty = observation_empty;
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
      (c->identity.stx_mode & S_IFMT) != kind ||
      (c->original_stat.st_mode & S_IFMT) != kind ||
      c->original_stat.st_size < 0 ||
      (uint64_t)c->original_stat.st_ino != c->identity.stx_ino ||
      (uint64_t)c->original_stat.st_size != c->identity.stx_size ||
      c->original_stat.st_uid != c->identity.stx_uid ||
      c->original_stat.st_gid != c->identity.stx_gid ||
      c->original_stat.st_mode != c->identity.stx_mode ||
      c->original_stat.st_nlink != c->identity.stx_nlink) return 0;
  return 1;
}
int slcli_linux_file_observe(int original_fd,struct slcli_linux_file_capture *c) {
  return observe_kind(original_fd,c,S_IFREG);
}
int slcli_linux_directory_source_observe(int original_fd,struct slcli_linux_file_capture *c) {
  if(!observe_kind(original_fd,c,S_IFDIR))return 0;
  /* This only checks this actual held alias. Original creator/ancestor/DAC,
   * every other alias and independent protected-root designation remain gates. */
  return (c->flags_status&O_ACCMODE)==O_RDONLY&&!(c->flags_status&O_PATH)&&
    (c->descriptor_status&FD_CLOEXEC)!=0;
}
int slcli_linux_path_source_observe(int fd,struct slcli_linux_file_capture *c,uint32_t kind){
 if(kind!=S_IFDIR&&kind!=S_IFREG)return 0;
 return observe_kind(fd,c,kind)&&(c->flags_status&O_PATH)&&(c->descriptor_status&FD_CLOEXEC);
}
static int observation_matches(int fd,const struct slcli_linux_file_capture *c,uint32_t kind,int path,int access){
 uint64_t tick;
 if(fd<0||!c||(kind!=S_IFDIR&&kind!=S_IFREG)||c->operation!=1||
  !c->stat_observed||c->stat_status||c->stat_args[0]!=(uint64_t)fd||
  c->stat_args[1]!=(uintptr_t)&c->original_stat||!c->flags_observed||
  (int64_t)c->flags_status<0||c->flags_args[0]!=(uint64_t)fd||c->flags_args[1]!=F_GETFL||c->flags_args[2]||
  (c->flags_status&O_ACCMODE)!=(uint64_t)access||((c->flags_status&O_PATH)!=0)!=path||!c->descriptor_observed||
  (int64_t)c->descriptor_status<0||(c->descriptor_status&FD_CLOEXEC)==0||
  c->descriptor_args[0]!=(uint64_t)fd||c->descriptor_args[1]!=F_GETFD||c->descriptor_args[2]||
  !c->observed||c->status||c->args[0]!=(uint64_t)fd||c->args[1]!=(uintptr_t)observation_empty||
  c->args[2]!=(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW)||c->args[3]!=STATX_BASIC_STATS||
  c->args[4]!=(uintptr_t)&c->identity||c->args[5]||
  !c->clock_observed||c->clock_status||c->clock_args[0]!=CLOCK_MONOTONIC||
  c->clock_args[1]!=(uintptr_t)&c->clock_raw||c->clock_raw.tv_sec<0||
  c->clock_raw.tv_nsec<0||c->clock_raw.tv_nsec>=1000000000||
  (uint64_t)c->clock_raw.tv_sec>(UINT64_MAX-(uint64_t)c->clock_raw.tv_nsec)/1000000000)return 0;
 tick=(uint64_t)c->clock_raw.tv_sec*1000000000+(uint64_t)c->clock_raw.tv_nsec;
 return tick&&c->tick==tick&&(c->identity.stx_mask&STATX_BASIC_STATS)==STATX_BASIC_STATS&&
  (c->identity.stx_mode&S_IFMT)==kind&&(c->original_stat.st_mode&S_IFMT)==kind&&
  c->original_stat.st_size>=0&&(uint64_t)c->original_stat.st_ino==c->identity.stx_ino&&
  (uint64_t)c->original_stat.st_size==c->identity.stx_size&&c->original_stat.st_uid==c->identity.stx_uid&&
  c->original_stat.st_gid==c->identity.stx_gid&&c->original_stat.st_mode==c->identity.stx_mode&&
  c->original_stat.st_nlink==c->identity.stx_nlink;
}
int slcli_linux_file_observation_matches_original(int fd,const struct slcli_linux_file_capture *c,uint32_t kind){
 return observation_matches(fd,c,kind,0,O_RDONLY);
}
int slcli_linux_path_observation_matches_original(int fd,const struct slcli_linux_file_capture *c,uint32_t kind){
 return observation_matches(fd,c,kind,1,O_RDONLY);
}
int slcli_linux_file_writer_observation_matches_original(int fd,const struct slcli_linux_file_capture *c){
 return observation_matches(fd,c,S_IFREG,0,O_RDWR);
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
int slcli_linux_file_read_matches_original(int fd,const unsigned char *output,size_t requested,
 uint64_t offset,const struct slcli_linux_file_capture *c){
 uint64_t tick;
 if(fd<0||!output||!requested||requested>INT64_MAX||offset>INT64_MAX||
  requested>(uint64_t)INT64_MAX-offset||!c||!disjoint(c,output,requested)||c->operation!=2||!c->observed||
  c->args[0]!=(uint64_t)fd||c->args[1]!=(uintptr_t)output||c->args[2]!=requested||
  c->args[3]!=offset||c->args[4]||c->args[5]||(int64_t)c->status<0||c->status>requested||
  !c->returned_bytes_observed||c->returned_bytes!=c->status||!c->clock_observed||
  c->clock_status||c->clock_args[0]!=CLOCK_MONOTONIC||c->clock_args[1]!=(uintptr_t)&c->clock_raw||
  c->clock_raw.tv_sec<0||c->clock_raw.tv_nsec<0||c->clock_raw.tv_nsec>=1000000000||
  (uint64_t)c->clock_raw.tv_sec>(UINT64_MAX-(uint64_t)c->clock_raw.tv_nsec)/1000000000)return 0;
 tick=(uint64_t)c->clock_raw.tv_sec*1000000000+(uint64_t)c->clock_raw.tv_nsec;
 return tick&&c->tick==tick;
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
int slcli_linux_file_verity_enable(int original_fd,uint32_t block_size,
                                  struct slcli_linux_file_capture *c) {
  long result;
  if(!c||c->operation||original_fd<0||!block_size||
     (block_size&(block_size-1)))return 0;
  zero(c,sizeof(*c));c->operation=5;
  c->verity_enable.version=1;
  c->verity_enable.hash_algorithm=FS_VERITY_HASH_ALG_SHA256;
  c->verity_enable.block_size=block_size;
  /* Salt/signature/reserved fields stay exactly zero. This introduces no
   * kernel keyring, signature policy or management signing seed. */
  c->args[0]=(uint64_t)original_fd;c->args[1]=FS_IOC_ENABLE_VERITY;
  c->args[2]=(uintptr_t)&c->verity_enable;
  result=slcli_kernel_call(__NR_ioctl,original_fd,FS_IOC_ENABLE_VERITY,
                          (long)&c->verity_enable,0,0,0);
  c->status=(uint64_t)result;c->observed=1;
  /* Success changed persistent inode content protection before this fallible
   * clock. Never lose that actual effect, retry this row or fake rollback.
   * EEXIST/unsupported/error remain their actual failed native result. */
  if(!result)c->verity_applied=1;
  if(!clock_observe(c))return 0;
  return result==0;
}
int slcli_linux_file_verity_measure(int original_fd,
                                   struct slcli_linux_file_capture *c) {
  struct fsverity_digest *d;
  long result;
  if(!c||c->operation||original_fd<0)return 0;
  zero(c,sizeof(*c));c->operation=6;
  d=(struct fsverity_digest *)(void *)c->verity_digest.raw;
  d->digest_size=32;
  c->args[0]=(uint64_t)original_fd;c->args[1]=FS_IOC_MEASURE_VERITY;
  c->args[2]=(uintptr_t)d;
  result=slcli_kernel_call(__NR_ioctl,original_fd,FS_IOC_MEASURE_VERITY,
                          (long)d,0,0,0);
  c->status=(uint64_t)result;c->observed=1;
  if(!result&&d->digest_algorithm==FS_VERITY_HASH_ALG_SHA256&&d->digest_size==32)
    c->verity_digest_observed=1;
  /* The measured descriptor/Merkle digest is retained separately from the
   * raw whole-file SHA256 used by SP1. It is not a provider authority claim. */
  if(!clock_observe(c))return 0;
  return result==0&&c->verity_digest_observed;
}
int slcli_linux_file_close(int original_fd,struct slcli_linux_file_capture *c) {
  long result;
  if(!c||c->operation||original_fd<0)return 0;
  zero(c,sizeof(*c));c->operation=7;c->args[0]=(uint64_t)original_fd;
  result=slcli_kernel_call(__NR_close,original_fd,0,0,0,0,0);
  c->status=(uint64_t)result;c->observed=1;
  if(!result)c->descriptor_closed=1;
  if(!clock_observe(c))return 0;
  return result==0;
}
#endif
