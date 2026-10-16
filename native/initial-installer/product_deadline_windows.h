#ifndef SLCLI_PRODUCT_DEADLINE_WINDOWS_H
#define SLCLI_PRODUCT_DEADLINE_WINDOWS_H
#include "product_section_windows.h"
#if defined(_WIN32)
/* Private deadline routing, not native clock serialization or source authority.
 * Parent originals remain local; exact readonly object/source/birth acquisition
 * must precede child-local binding. No received duration or pointer exists. */
#define SLCLI_PRODUCT_DEADLINE_BYTES 64u
struct slcli_product_windows_deadline_values {
 uint64_t enclosing_expires,active_phase_expires,qpc_frequency;
 uint32_t active_phase,parent_pid,parent_creation[2];
};
struct slcli_product_windows_deadline_bounds {
 uint64_t guard_nanoseconds,enclosing_expires,active_phase_expires;
};
struct slcli_product_windows_deadline_source {
 /* Exact distinct LIVE arena descriptor before publisher entry. Local phase
  * deadline/origin are LIVE ranges. External/free/header/padding descriptors
  * cannot reach the publisher's allocator or native clock writes. */
 const struct slcli_source_deadline_original *active_phase_original;
 uint32_t active_phase;
};
struct slcli_product_windows_deadline_publication {
 struct slcli_product_storage_windows *storage_original;
 const struct slcli_product_windows_deadline_source *source_original;
 const struct slcli_source_deadline_original *phase_original;
 const struct slcli_native_clock *phase_origin_original;
 uint64_t phase_origin_tick;
 struct slcli_product_windows_deadline_values values;
 struct slcli_native_clock entry,finish;
 uint32_t phase,started,failed,publication_completed;
};
/* Pure exact closed representation. It cannot acquire/bind a native owner,
 * authenticate a source, mint phase progress or establish physical fit. */
int slcli_product_windows_deadline_encode(unsigned char *,size_t,
 const struct slcli_product_windows_deadline_values *);
int slcli_product_windows_deadline_decode(struct slcli_product_windows_deadline_values *,
 const unsigned char *,size_t);
/* Pure no-later constraints only; SAME boot/native QPC-domain/source membership
 * is independent. Never adds a duration or constructs a local clock/caller. */
int slcli_product_windows_deadline_conservative_bounds(
 struct slcli_product_windows_deadline_bounds *,const struct slcli_product_windows_deadline_values *);
/* Actual private section publication + writer disposal only. Full genuine
 * source/active-phase/funding admission must precede this fixed caller. Success
 * is NOT child acquisition, family/entry permission or native qualification. */
int slcli_product_windows_deadline_publish_original(struct slcli_product_storage_windows *,
 const struct slcli_product_windows_deadline_source *);
#endif
#endif
