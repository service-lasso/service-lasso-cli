#include "memory_capture_vectors.h"

/* Unexecuted original-row preservation regression. Calling these actual
 * platform entry points with an already used capture must return before ANY
 * native acquisition/free/query/clock effect, regardless of completion state.
 * Whole source/native admission precedes compiling or executing this source. */
static struct slcli_memory_capture original;
static unsigned char saved[sizeof(original)];
static int unchanged(void) {
  const unsigned char *raw = (const unsigned char *)&original;
  size_t i;
  for (i = 0; i < sizeof(original); ++i) if (raw[i] != saved[i]) return 0;
  return 1;
}
int slcli_memory_capture_vectors(struct slcli_memory_owner *owner,
                                 uint32_t original_live_extent) {
  unsigned char *raw = (unsigned char *)&original;
  size_t i;
  uint32_t operation, observed;
  /* The full native qualification fixture supplies its genuine already live
   * original owner/extent. No fabricated address or test-owned numeric handle
   * is passed to a native release, even if the rejection guard regresses. */
  if (!owner || owner->failed || original_live_extent >= owner->count ||
      original_live_extent >= 256 ||
      owner->extents[original_live_extent].state != SLCLI_EXTENT_LIVE ||
      !owner->extents[original_live_extent].base) return 0;
  for (operation = 1; operation <= 13; ++operation) {
    for (observed = 0; observed <= 1; ++observed) {
      for (i = 0; i < sizeof(original); ++i) raw[i] = (unsigned char)(i + 1);
      original.operation = operation;
      original.observed = observed;
      for (i = 0; i < sizeof(original); ++i) saved[i] = raw[i];
      if (slcli_platform_acquire(owner, SLCLI_RUNTIME, 4096, &original) ||
          !unchanged()) return 0;
      if (slcli_platform_release(owner, original_live_extent, &original) ||
          !unchanged()) return 0;
    }
  }
  /* Representation-only markerless rows: each non-marker byte independently
   * denies erasure before ANY native call. These are not fabricated native
   * successful acquisition facts or qualified runtime originals. */
  for(i=0;i<sizeof(original);i++) {
    size_t j;
    if(i>=offsetof(struct slcli_memory_capture,operation)&&
       i<offsetof(struct slcli_memory_capture,operation)+sizeof(original.operation))continue;
    for(j=0;j<sizeof(original);j++)raw[j]=0;
    raw[i]=1;for(j=0;j<sizeof(original);j++)saved[j]=raw[j];
    if(slcli_platform_acquire(owner,SLCLI_RUNTIME,4096,&original)||!unchanged()||
       slcli_platform_release(owner,original_live_extent,&original)||!unchanged())return 0;
  }
  /* Overlapping owner/capture control must be rejected without dereferencing
   * an invented capture row or overwriting original owner metadata. */
  if (slcli_memory_capture_fresh(owner,
      (const struct slcli_memory_capture *)owner)) return 0;
  if (slcli_memory_capture_fresh((const struct slcli_memory_owner *)
      (UINTPTR_MAX - sizeof(*owner) + 1), &original)) return 0;
  if (slcli_memory_capture_fresh(owner, (const struct slcli_memory_capture *)
      (UINTPTR_MAX - sizeof(original) + 1))) return 0;
  return 1;
}

/* Actual positive native caller, UNRUN. Genuine serialized fixture constructor
 * owns the full owner/capture/scratch/source/native SDK/original deadline and
 * finite capacity before this call. No mapping is invented; no successful
 * acquisition is treated as physical backing, retirement or admission. */
int slcli_memory_acquire_positive_original(struct slcli_memory_owner *owner,
  uint32_t category,uint64_t bytes,struct slcli_memory_capture *capture,
  unsigned char *scratch,size_t scratch_bytes) {
  uintptr_t o,c,s;size_t i;uint32_t count;uint64_t charged,next,limit;
  const struct slcli_extent *e;
  if(!owner||!capture||!scratch||scratch_bytes<sizeof(*capture)||category>SLCLI_IO||
    !bytes||owner->count>=256||!slcli_memory_capture_fresh(owner,capture))return 0;
  o=(uintptr_t)owner;c=(uintptr_t)capture;s=(uintptr_t)scratch;
  if(s>UINTPTR_MAX-sizeof(*capture)||
    (s<c+sizeof(*capture)&&c<s+sizeof(*capture))||
    (s<o+sizeof(*owner)&&o<s+sizeof(*capture)))return 0;
  count=owner->count;charged=owner->charged;next=owner->next_sequence;limit=owner->limit;
  if(!slcli_platform_acquire(owner,category,bytes,capture)||owner->failed||
    !capture->operation||!capture->observed||!capture->result||!capture->tick||
    !capture->tick_calls||capture->tick_returns!=capture->tick_calls||
    capture->request.index!=count||capture->request.sequence!=next||
    owner->count!=count+1||owner->next_sequence!=next+1||owner->limit!=limit||
    capture->reserved!=capture->bytes||capture->bytes<bytes||
    capture->bytes>UINT64_MAX-charged||owner->charged!=charged+capture->bytes||
    owner->charged>limit||capture->backed||capture->backed_observed)return 0;
  e=&owner->extents[count];
  if(e->state!=SLCLI_EXTENT_LIVE||e->base!=capture->result||e->category!=category||
    e->reserved!=capture->reserved||e->acquisition!=next||e->backed||e->backed_observed)return 0;
  for(i=0;i<sizeof(*capture);i++)scratch[i]=((const unsigned char *)capture)[i];
  /* Used genuine original row cannot perform a second native acquisition. */
  if(slcli_platform_acquire(owner,category,bytes,capture))return 0;
  for(i=0;i<sizeof(*capture);i++)if(scratch[i]!=((const unsigned char *)capture)[i])return 0;
  return !owner->failed&&owner->count==count+1&&owner->charged==charged+capture->bytes;
}