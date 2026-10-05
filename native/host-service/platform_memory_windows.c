#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "platform_memory.h"

_Static_assert(sizeof(MEMORY_BASIC_INFORMATION) <= 128,
               "Original ABI geometry must fit without truncation");
_Static_assert(sizeof(SYSTEM_INFO) <= 64,
               "Original page geometry must fit without truncation");
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
  LARGE_INTEGER v, frequency;
  BOOL counter_ok, frequency_ok;
  uint64_t seconds, remainder, whole, fraction;
  zero(&v, sizeof(v));
  zero(&frequency, sizeof(frequency));
  counter_ok = QueryPerformanceCounter(&v);
  frequency_ok = QueryPerformanceFrequency(&frequency);
  c->tick_raw[0] = (uint64_t)v.QuadPart;
  c->tick_raw[1] = (uint64_t)frequency.QuadPart;
  c->tick_status[0] = (uint64_t)(uint32_t)counter_ok;
  c->tick_status[1] = (uint64_t)(uint32_t)frequency_ok;
  c->tick_calls = 2;
  if (!counter_ok || !frequency_ok || v.QuadPart <= 0 || frequency.QuadPart <= 0)
    return 0;
  seconds = (uint64_t)(v.QuadPart / frequency.QuadPart);
  remainder = (uint64_t)(v.QuadPart % frequency.QuadPart);
  if (seconds > UINT64_MAX / 1000000000 ||
      remainder > UINT64_MAX / 1000000000) return 0;
  whole = seconds * 1000000000;
  fraction = remainder * 1000000000 / (uint64_t)frequency.QuadPart;
  if (fraction > UINT64_MAX - whole) return 0;
  c->tick = whole + fraction;
  return c->tick != 0;
}
int slcli_platform_acquire(struct slcli_memory_owner *o, uint32_t category,
                           uint64_t bytes, struct slcli_memory_capture *c) {
  SYSTEM_INFO info;
  MEMORY_BASIC_INFORMATION geometry;
  uint64_t rounded, remainder, granularity;
  SIZE_T observed;
  void *base;
  if (!slcli_memory_capture_fresh(o, c)) return 0;
  zero(c, sizeof(*c));
  c->operation = 1; /* Retain this attempt even if page/query/capacity fails. */
  zero(&info, sizeof(info));
  GetSystemInfo(&info);
  copy(c->page_geometry, &info, sizeof(info));
  c->page_geometry_bytes = sizeof(info);
  granularity = info.dwAllocationGranularity;
  if (!info.dwPageSize || !granularity || granularity % info.dwPageSize || !bytes ||
      bytes > SIZE_MAX || bytes > UINT64_MAX - granularity + 1) {
    o->failed = 1;
    return 0;
  }
  /* Reserve the full observed allocation granularity explicitly. Charging
   * committed pages alone would omit the reservation's trailing address range. */
  remainder = bytes % granularity;
  rounded = remainder ? bytes + granularity - remainder : bytes;
  if (rounded > SIZE_MAX) { o->failed = 1; return 0; }
  c->page_bytes = info.dwPageSize;
  c->bytes = rounded;
  c->operation = 1; /* exact win32.virtual_alloc producer operation */
  c->flags = MEM_RESERVE | MEM_COMMIT;
  c->protection = PAGE_READWRITE;
  c->request_args[0] = 0;
  c->request_args[1] = rounded;
  c->request_args[2] = c->flags;
  c->request_args[3] = c->protection;
  if (!slcli_memory_prepare(o, category, rounded, &c->request)) return 0;
  base = VirtualAlloc(NULL, (SIZE_T)rounded, MEM_RESERVE | MEM_COMMIT,
                      PAGE_READWRITE);
  c->result = (uintptr_t)base;
  c->native_status = (uint64_t)(uintptr_t)base;
  if (!base) {
    c->native_error = GetLastError();
    c->native_error_observed = 1;
  }
  c->observed = 1;
  if (base && !slcli_memory_partial_acquired(o, &c->request, (uintptr_t)base))
    return 0;
  if (!tick(c) || !base) {
    slcli_memory_unobserved(o, &c->request);
    return 0;
  }
  zero(&geometry, sizeof(geometry));
  c->query_args[0] = (uint64_t)(uintptr_t)base;
  c->query_args[1] = (uint64_t)(uintptr_t)&geometry;
  c->query_args[2] = sizeof(geometry);
  observed = VirtualQuery(base, &geometry, sizeof(geometry));
  c->query_result = observed;
  copy(c->geometry, &geometry, sizeof(geometry));
  c->geometry_bytes = (uint32_t)observed;
  if (observed != sizeof(geometry) || geometry.AllocationBase != base ||
      geometry.BaseAddress != base || geometry.RegionSize != rounded ||
      geometry.State != MEM_COMMIT || geometry.Type != MEM_PRIVATE ||
      geometry.Protect != PAGE_READWRITE) {
    slcli_memory_unobserved(o, &c->request);
    return 0;
  }
  c->reserved = rounded;
  /* MEM_COMMIT does not observe physical backing/residency. The closed event
   * encodes backedBytes=null until an admitted original observation establishes
   * it. Full reserved charge stays conservative regardless. */
  c->backed = 0;
  c->backed_observed = 0;
  return slcli_memory_acquired(o, &c->request, (uintptr_t)base, rounded, 0, 0);
}
int slcli_platform_release(struct slcli_memory_owner *o, uint32_t index,
                           struct slcli_memory_capture *c) {
  struct slcli_extent *e;
  BOOL result;
  if (!slcli_memory_capture_fresh(o, c) || index >= 256) return 0;
  e = &o->extents[index];
  if (o->failed || e->state != SLCLI_EXTENT_LIVE || !e->base) return 0;
  zero(c, sizeof(*c));
  c->operation = 4; /* exact win32.virtual_free producer operation */
  c->flags = MEM_RELEASE;
  c->address = e->base;
  c->bytes = 0; /* Original VirtualFree dwSize for MEM_RELEASE. */
  c->reserved = e->reserved;
  c->request_args[0] = (uint64_t)e->base;
  c->request_args[1] = 0;
  c->request_args[2] = MEM_RELEASE;
  if (!slcli_memory_prepare_release(o, index, &c->request,
                                    &c->related_sequence)) return 0;
  result = VirtualFree((void *)e->base, 0, MEM_RELEASE);
  c->result = (uintptr_t)(uint32_t)result;
  c->native_status = (uint64_t)(uint32_t)result;
  if (!result) {
    c->native_error = GetLastError();
    c->native_error_observed = 1;
  }
  c->observed = 1;
  /* Retain charge until the independent raw-capture reader validates this
   * actual result/tick and calls slcli_memory_retired. Never free on JS dispose. */
  if (!result || !tick(c)) {
    slcli_memory_unobserved(o, &c->request);
    return 0;
  }
  return 1;
}
#endif
