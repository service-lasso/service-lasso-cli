#if defined(__linux__)
#include <asm/unistd.h>
#include <linux/mman.h>
#include <linux/time.h>
#include <linux/time_types.h>
#include "platform_memory.h"
#include "native_syscall_linux.h"

/* The original selected SDK supplies syscall numbers. These are actual kernel
 * ABI calls, with original signed returns retained before interpretation. No
 * libc/printf/malloc/process helper runs behind this freestanding adapter. */
static void zero(void *value, size_t bytes) {
  unsigned char *p = (unsigned char *)value;
  while (bytes--) *p++ = 0;
}
static void copy(void *to, const void *from, size_t bytes) {
  unsigned char *d = (unsigned char *)to;
  const unsigned char *s = (const unsigned char *)from;
  while (bytes--) *d++ = *s++;
}
static int tick(struct slcli_memory_capture *c) {
  struct __kernel_timespec t;
  long status;
  zero(&t, sizeof(t));
  status = slcli_kernel_call(__NR_clock_gettime, CLOCK_MONOTONIC,
                       (long)&t, 0, 0, 0, 0);
  c->tick_raw[0] = (uint64_t)t.tv_sec;
  c->tick_raw[1] = (uint64_t)t.tv_nsec;
  c->tick_status[0] = (uint64_t)status;
  c->tick_calls = 1;
  if (status || t.tv_sec < 0 || t.tv_nsec < 0 || t.tv_nsec >= 1000000000 ||
      (uint64_t)t.tv_sec > (UINT64_MAX - (uint64_t)t.tv_nsec) / 1000000000)
    return 0;
  c->tick = (uint64_t)t.tv_sec * 1000000000 + (uint64_t)t.tv_nsec;
  return c->tick != 0;
}
int slcli_platform_acquire(struct slcli_memory_owner *o, uint32_t category,
                           uint64_t bytes, struct slcli_memory_capture *c) {
  uint64_t page, rounded, remainder;
  long result;
  if (!o || !c) return 0;
  zero(c, sizeof(*c));
  /* Original kernel-entry AT_PAGESZ, captured by the owning bootstrap before
   * transfer; never an environment/flag or guessed constant. */
  page = o->native_page_size;
  copy(c->page_geometry, o->native_page_geometry,
        sizeof(o->native_page_geometry));
  c->page_geometry_bytes = sizeof(o->native_page_geometry);
  if (!page || page > UINT32_MAX || !bytes || bytes > INT64_MAX ||
      bytes > UINT64_MAX - page + 1) {
    o->failed = 1;
    return 0;
  }
  remainder = bytes % page;
  rounded = remainder ? bytes + page - remainder : bytes;
  if (rounded > INT64_MAX) { o->failed = 1; return 0; }
  c->page_bytes = (uint32_t)page;
  c->bytes = rounded;
  c->operation = 9; /* exact linux.mmap producer operation */
  c->flags = MAP_PRIVATE | MAP_ANONYMOUS;
  c->protection = PROT_READ | PROT_WRITE;
  c->request_args[0] = 0;
  c->request_args[1] = rounded;
  c->request_args[2] = c->protection;
  c->request_args[3] = c->flags;
  c->request_args[4] = UINT64_MAX; /* Native signed fd=-1 carrier. */
  c->request_args[5] = 0;
  if (!slcli_memory_prepare(o, category, rounded, &c->request)) return 0;
  result = slcli_kernel_call(__NR_mmap, 0, (long)rounded, c->protection,
                        c->flags, -1, 0);
  c->result = (uintptr_t)result;
  c->native_status = (uint64_t)result;
  c->observed = 1;
  if ((result < 0 && result >= -4095) || !result) {
    slcli_memory_unobserved(o, &c->request);
    return 0;
  }
  if (!slcli_memory_partial_acquired(o, &c->request, (uintptr_t)result))
    return 0;
  if (!tick(c)) {
    slcli_memory_unobserved(o, &c->request);
    return 0;
  }
  c->reserved = rounded;
  /* Anonymous mmap return observes reservation, not actual physical backing. */
  c->backed = 0;
  c->backed_observed = 0;
  return slcli_memory_acquired(o, &c->request, (uintptr_t)result, rounded, 0, 0);
}
int slcli_platform_release(struct slcli_memory_owner *o, uint32_t index,
                           struct slcli_memory_capture *c) {
  struct slcli_extent *e;
  long result;
  if (!o || !c || index >= 256) return 0;
  e = &o->extents[index];
  if (o->failed || e->state != SLCLI_EXTENT_LIVE || !e->base ||
      e->reserved > INT64_MAX) return 0;
  zero(c, sizeof(*c));
  c->address = e->base;
  c->bytes = e->reserved;
  c->reserved = e->reserved;
  c->request_args[0] = (uint64_t)e->base;
  c->request_args[1] = e->reserved;
  c->operation = 11; /* exact linux.munmap producer operation */
  if (!slcli_memory_prepare_release(o, index, &c->request,
                                    &c->related_sequence)) return 0;
  result = slcli_kernel_call(__NR_munmap, (long)e->base, (long)e->reserved, 0, 0, 0, 0);
  c->result = (uintptr_t)result;
  c->native_status = (uint64_t)result;
  c->observed = 1;
  if (result || !tick(c)) {
    slcli_memory_unobserved(o, &c->request);
    return 0;
  }
  /* Raw original observer/capture validation retires the charge separately. */
  return 1;
}
#endif
