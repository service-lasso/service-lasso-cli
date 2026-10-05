#if defined(__linux__)
#include <linux/auxvec.h>
#include "owner_memory.h"

/* Called only by the owning kernel-entry route with its ORIGINAL stack pointer
 * and already observed original stack extent. No public API, argv/environment
 * selector or synthetic page-size option is exposed. This reads kernel ABI
 * metadata; it does not authenticate the process/source/provider. */
static int word(const struct slcli_extent *e, uintptr_t *cursor, uintptr_t *out) {
  uint64_t offset;
  if (*cursor < e->base) return 0;
  offset = (uint64_t)(*cursor - e->base);
  if (offset > e->reserved || sizeof(uintptr_t) > e->reserved - offset)
    return 0;
  *out = *(const uintptr_t *)*cursor;
  *cursor += sizeof(uintptr_t);
  return 1;
}
int slcli_linux_bootstrap_pages(struct slcli_memory_owner *o, uint32_t stack,
                                uintptr_t original_sp, uintptr_t pair[2]) {
  const struct slcli_extent *e;
  uintptr_t cursor = original_sp, argc, value, tag, pages = 0;
  uint64_t i, maximum_words;
  if (!o || !pair || stack >= 256 || o->failed || o->native_page_size) return 0;
  e = &o->extents[stack];
  maximum_words = e->reserved / sizeof(uintptr_t);
  if (e->state != SLCLI_EXTENT_LIVE || e->category != SLCLI_STACK ||
      original_sp % _Alignof(uintptr_t) || !word(e, &cursor, &argc) ||
      argc >= maximum_words) goto failed;
  for (i = 0; i < argc; ++i)
    if (!word(e, &cursor, &value) || !value) goto failed;
  if (!word(e, &cursor, &value) || value) goto failed;
  for (i = 0; i < maximum_words; ++i) {
    if (!word(e, &cursor, &value)) goto failed;
    if (!value) break;
  }
  if (i == maximum_words) goto failed;
  for (i = 0; i < maximum_words / 2; ++i) {
    if (!word(e, &cursor, &tag) || !word(e, &cursor, &value)) goto failed;
    if (tag == AT_NULL) {
      if (value || !pages || pages > UINT32_MAX || (pages & (pages - 1)))
        goto failed;
      pair[0] = AT_PAGESZ;
      pair[1] = pages;
      o->native_page_geometry[0] = pair[0];
      o->native_page_geometry[1] = pair[1];
      o->native_page_size = pages;
      return 1;
    }
    if (tag == AT_PAGESZ) {
      if (pages || !value) goto failed;
      pages = value;
    }
  }
failed:
  o->failed = 1;
  return 0;
}
#endif
