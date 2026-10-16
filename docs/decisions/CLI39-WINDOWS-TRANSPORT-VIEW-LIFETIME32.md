# CLI39 Windows transport view and backing lifetime32

Source proposal for the existing private section route; no native execution or new ownership authority. The actual parent control owner pays a separate transport IO backing and its capture/view/handle aliases. The child target frozen owner is untouched. The runtime section remains distinct and writable.

Before child birth the fixed producer creates the unnamed private PAGE_READWRITE section, retains the native original HANDLE before diagnostics, maps its parent writer view once and creates the explicitly readonly inheritable child alias using DuplicateHandle desired FILE_MAP_READ with no SAME_ACCESS. Every actual returned partial is captured. The exact child alias joins the native launcher inherited handle roster before CreateProcess. The genuine private descriptor/parent/source/object/rights premise is independent of those scalar values.

After successful suspended child birth and target freeze, the actual parent constructs and encodes the fixed payload from its retained birth/runtime/deadline/owner originals, copies it into the existing writer view under original D, verifies complete bytes, then independently attempts eligible UnmapViewOfFile and original writable HANDLE CloseHandle exactly once. A failed diagnostic does not erase an actual native return. Unknown unmap or live writer prevents release; failed writable-handle close preserves that obligation even if view unmap succeeded. The readonly child alias remains held through actual child exit and backing disposition. No successful view removal or alias close is physical retirement.

Current owner_memory source can represent LIVE->RELEASE_PENDING->RETIRED, but retirement subtracts all backing charge. memory_unobserved retains charge while failing the owner. Neither is the successful writer-view removal with still-held physical section required here. The smallest fixed source extension is a pure SAME-request view-removal correspondence: after actual independently retained successful UnmapViewOfFile, transition its parent transport extent RELEASE_PENDING->BACKING_ACQUIRED, preserve reserved/category/acquisition/latest release sequence, totals/peaks/count/next sequence, and clear current view base/backed/arena state. The original view/base/raw operation remain in the source-owned native capture. No new request/charge is made, no retirement set, and no charge is decremented.

This is not a fresh mapping opportunity: existing backing_mapped requires latest==acquisition, while this extent retains its actual later release sequence. Existing arena initialization requires LIVE, so it cannot use the unmapped address. The parent transport cannot be frozen as a new prebirth target; its later latest sequence does not meet prebirth_shape. A separate eventual actual whole backing/alias/borrow disposition is required before physical retirement, and that source/native operation remains owed. The fixed producer must never call generic VirtualFree on a section view or treat UnmapViewOfFile as section destruction.

The exact proposed helper consumes the original parent owner, original release request (index+sequence), retained original mapped base/reserved bytes and the related original acquisition/latest sequence from prepare_release. It validates the exact current RELEASE_PENDING tuple and no arena initialization, then changes only view state. It does not itself authenticate a native return; only the source-fixed producer calls it after its actual unmap capture/independent qualification. Unknown/false native return leaves RELEASE_PENDING/retained originals and cannot invoke the transition. The helper must be independently reviewed with actual producer and full selected lifetime contracts before target execution.

Linux sealed transport has no parent mapping and needs no such view transition. Its BACKING_ACQUIRED charge remains retained while FD aliases exist. Installed caller/entry/native prefix/full runtime and whole finite native qualification remain unfinished.
## Exact proposed transition body and fixed consumers

Existing state invariants: prepare_release accepts only writable, nonfailed owner plus LIVE nonzero-base extent; allocates one new sequence, preserves acquisition/reserved/category/base/backed and charge, sets latest to that sequence and state RELEASE_PENDING, and returns related=previous latest. retired requires the exact release tuple but subtracts reserved/category charge and sets retirement; it is inapplicable while the readonly alias still owns the backing. unobserved marks RETAINED and owner.failed; it is for uncertainty/failure, not this successful remaining-backing state.

Proposed pure correspondence body (not yet product code):

```c
int slcli_memory_backing_view_unmapped(struct slcli_memory_owner *o,
  const struct slcli_extent_request *release,uint64_t related,
  uintptr_t original_base,uint64_t original_reserved) {
  struct slcli_extent *e;
  if(!slcli_memory_owner_writable(o)||!release||o->failed||
     release->index>=o->count||release->index>=256)return 0;
  e=&o->extents[release->index];
  if(e->state!=SLCLI_EXTENT_RELEASE_PENDING||!original_base||
     !original_reserved||e->base!=original_base||e->reserved!=original_reserved||
     e->arena_initialized||e->retirement||!related||e->acquisition!=related||
     e->latest!=release->sequence||release->sequence<=related||
     release->sequence>=o->next_sequence)return 0;
  e->base=0;e->backed=0;e->backed_observed=0;
  e->state=SLCLI_EXTENT_BACKING_ACQUIRED;
  return 1;
}
```

Rejecting a mismatched/repeated correspondence leaves the exact original extent unchanged; its native caller retains the error and all original view/handle/release captures and denies publication. The helper supplies no native-return authority. It changes neither charged/category totals/peaks nor acquisition/latest/count/next sequence/retirement. It cannot apply to an arena-containing extent. Existing backing_mapped requires latest==acquisition, which now fails; repeat unmap requires RELEASE_PENDING, which now fails. The actual removed base remains in producer's immutable raw view/release record rather than the current extent address.

Only fixed Windows transport publication consumes it: (1) original section map/query association complete and no outstanding writer/read borrower; (2) source preowns release-request/raw-output/clock/error storage; (3) actual memory_prepare_release with original mapped extent records related/index/newsequence; (4) actual UnmapViewOfFile of that same retained mapping, BOOL recorded immediately before last-error capture; (5) only a known successful returned BOOL and same captured argument/request invokes this helper, before fallible postclock; (6) original writable HANDLE CloseHandle is independently once attempted only when its own borrowers are closed; (7) useful transport publication requires actual view removal, successful helper correspondence, actual writable-handle close and original D. No uncertainty retry, raw capture overwrite, or gate after failed/unknown unmap. Readonly child alias remains original backing ownership; final original backing/alias/borrow disposal is separate and does not follow from this helper.

No current caller invokes this proposed helper. Windows producer and launcher callsites must be implemented coherently after source disposition, with exact original raw captures in the same complete review. Linux transport and frozen target owner consumers do not change. Generic platform_memory_windows VirtualFree release must never consume this section-view path.