#ifndef SLCLI_SHARED_WORKSPACE_H
#define SLCLI_SHARED_WORKSPACE_H
#include <stdint.h>
/* Exactly one original service workspace slice inside the selected finite
 * shared region. The serialized owning loop supplies its charged lifetime.
 * This lock state controls real buffer reuse; it authenticates no caller or
 * source. Failed/unresolved native IO keeps the original lease permanently. */
struct slcli_shared_workspace {
  unsigned char input[1024],readback[1024];
  const void *busy_original;
};
static inline int slcli_workspace_take(struct slcli_shared_workspace *w,const void *owner) {
  if(!w||!owner||w->busy_original)return 0;w->busy_original=owner;return 1;
}
static inline int slcli_workspace_release(struct slcli_shared_workspace *w,const void *owner) {
  if(!w||w->busy_original!=owner)return 0;w->busy_original=0;return 1;
}
#endif
