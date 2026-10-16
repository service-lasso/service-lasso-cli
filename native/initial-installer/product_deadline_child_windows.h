#ifndef SLCLI_PRODUCT_DEADLINE_CHILD_WINDOWS_H
#define SLCLI_PRODUCT_DEADLINE_CHILD_WINDOWS_H
#include "product_route_windows.h"
#if defined(_WIN32)
enum {
 SLCLI_PRODUCT_CHILD_SELF_PID,SLCLI_PRODUCT_CHILD_SELF_TID,SLCLI_PRODUCT_CHILD_SELF_PROCESS,
 SLCLI_PRODUCT_CHILD_SECTION_FLAGS,SLCLI_PRODUCT_CHILD_PARENT_FLAGS,
 SLCLI_PRODUCT_CHILD_SECTION_ACCESS,SLCLI_PRODUCT_CHILD_PARENT_ACCESS,
 SLCLI_PRODUCT_CHILD_PARENT_PID,SLCLI_PRODUCT_CHILD_PARENT_TIMES,
 SLCLI_PRODUCT_CHILD_PARENT_RELATION,SLCLI_PRODUCT_CHILD_PARENT_LIVE,
 SLCLI_PRODUCT_CHILD_MAP,SLCLI_PRODUCT_CHILD_GEOMETRY,
 SLCLI_PRODUCT_CHILD_PARENT_TOKEN,SLCLI_PRODUCT_CHILD_PARENT_USER,
 SLCLI_PRODUCT_CHILD_PARENT_STATISTICS,SLCLI_PRODUCT_CHILD_PARENT_SID_VALID,
 SLCLI_PRODUCT_CHILD_PARENT_SID_LENGTH,SLCLI_PRODUCT_CHILD_PARENT_STATISTICS_AFTER,
 SLCLI_PRODUCT_CHILD_PARENT_TIMES_AFTER,SLCLI_PRODUCT_CHILD_PARENT_PID_AFTER,
 SLCLI_PRODUCT_CHILD_SELF_PID_AFTER,SLCLI_PRODUCT_CHILD_SELF_TID_AFTER,
 SLCLI_PRODUCT_CHILD_PARENT_LIVE_AFTER,
 SLCLI_PRODUCT_CHILD_DEADLINE_CALLS
};
/* Actual child-local native outputs under independently admitted INITIAL paid
 * preparation. A complete capture is NOT genuine source/native-case/family or
 * original-operation admission. No parent clock is copied or newly published. */
struct slcli_product_windows_deadline_acquisition {
 struct slcli_product_storage_windows *storage_original;
 const struct slcli_product_storage_windows_source *initial_source_original;
 const struct slcli_native_clock *initial_origin_original;
 uint64_t initial_origin_tick,initial_expires,prior_tick,physical_bytes;
 struct slcli_product_windows_entry_route route;
 unsigned char route_before[SLCLI_PRODUCT_ROUTE_BYTES],route_after[SLCLI_PRODUCT_ROUTE_BYTES];
 struct slcli_build_windows_call calls[SLCLI_PRODUCT_CHILD_DEADLINE_CALLS];
 struct slcli_native_clock entry,route_cut,body_cut,finish;
 PUBLIC_OBJECT_BASIC_INFORMATION section_access,parent_access;
 ULONG section_access_bytes,parent_access_bytes,pbi_bytes;
 DWORD section_flags,parent_flags,parent_user_bytes,parent_statistics_bytes,parent_statistics_after_bytes;
 /* Selected x64 native PBI layout candidate48: PID slot4/parent PID slot5.
  * SDK reserved fields alone do not qualify this installed private ABI. */
 uint64_t pbi[6];
 FILETIME parent_creation,parent_exit,parent_kernel,parent_user_time;
 FILETIME parent_creation_after,parent_exit_after,parent_kernel_after,parent_user_time_after;
 MEMORY_BASIC_INFORMATION geometry;
 void *view_original;
 HANDLE parent_token_original;
 HANDLE self_process_original;
 struct {TOKEN_USER header;unsigned char sid[SECURITY_MAX_SID_SIZE];} parent_user;
 TOKEN_STATISTICS parent_statistics,parent_statistics_after;
 unsigned char body_original[SLCLI_BOOTSTRAP_TRANSPORT_BYTES];
 unsigned char readback_original[SLCLI_BOOTSTRAP_TRANSPORT_BYTES];
 unsigned char final_body_original[SLCLI_BOOTSTRAP_TRANSPORT_BYTES];
 struct slcli_product_windows_deadline_values values;
 uint32_t started,failed,native_capture_completed;
};
/* Reads ONLY the fixed source-owned product route cell, then actually acquires
 * readonly mapping/rights/process/token observations in retained paid storage.
 * No caller handle/JSON/callback, new clock, operation publication or close.
 * Complete source/own-actor/profile/ABI/native-case admission precedes effects. */
int slcli_product_windows_deadline_acquire_original(struct slcli_product_storage_windows *);
#endif
#endif
