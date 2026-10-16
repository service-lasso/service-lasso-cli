#ifndef SLCLI_WRITER_READBACK_WINDOWS_H
#define SLCLI_WRITER_READBACK_WINDOWS_H
#include "paths_windows.h"
#if defined(_WIN32)
/* Actual reduced-rights native duplicate, not independent BQ/source authority.
 * Creator, output, raw observations and both handle lifetimes remain retained. */
struct slcli_writer_windows_readback_original {
 struct slcli_arena *arena;
 struct slcli_writer_windows_path *creator;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 HANDLE original;
 uint64_t duplicate_args[7],previous_tick;
 BOOL duplicate_status;DWORD duplicate_error;
 struct slcli_windows_file_capture observed;
 struct slcli_native_clock before,after,observe_before,observe_after;
 int before_result,after_result,observe_before_result,observe_result,observe_after_result;
 uint32_t started,failed,duplicate_called,duplicate_returned;
 uint32_t before_returned,after_returned,observe_before_returned,observe_returned,observe_after_returned;
 uint32_t complete;
};
int slcli_writer_duplicate_windows_readback_original(
 struct slcli_writer_windows_readback_original *,struct slcli_arena *,
 struct slcli_writer_windows_path *,const struct slcli_source_deadline_original *,
 const void *,size_t,const struct slcli_native_clock *);
int slcli_writer_windows_readback_matches_original(
 const struct slcli_writer_windows_readback_original *);
#endif
#endif
