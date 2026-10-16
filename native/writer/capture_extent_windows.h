#ifndef SLCLI_WRITER_CAPTURE_EXTENT_WINDOWS_H
#define SLCLI_WRITER_CAPTURE_EXTENT_WINDOWS_H
#include "readback_windows.h"
#if defined(_WIN32)
/* Preowned BQ constructor captures precede R0. These are not relabeled106
 * ranges or source admission. Two section handles and two views remain real
 * distinct native originals, sharing the one charged file-backed extent. */
struct slcli_writer_capture_extent_windows {
 struct slcli_arena *arena;
 struct slcli_writer_capture_backing_windows *backing_original;
 size_t plan_index;
 struct slcli_writer_windows_readback_original *readback;
 struct slcli_writer_capture_ranges_windows *ranges_original;
 struct slcli_memory_owner *owner;
 struct slcli_extent_request request;
 uint64_t file_bytes,file_offset,bytes,previous_tick;
 SYSTEM_INFO system;
 struct slcli_native_clock system_before,system_after;
 int system_before_result,system_after_result;
 uint32_t system_called,system_returned,system_before_returned,system_after_returned;
 uintptr_t system_output;
 HANDLE sections[2];void *views[2];
 uint64_t section_args[2][6],map_args[2][5],query_args[2][3];
 DWORD section_error[2],map_error[2],query_error[2];
 MEMORY_BASIC_INFORMATION geometry[2];SIZE_T query_result[2];
 struct slcli_native_clock clocks[7][2];
 uint32_t called[7],returned[7],before_returned[7],after_returned[7];
 int before_result[7],after_result[7];
 struct slcli_windows_file_capture file_after;
 int file_after_result;uint64_t zero_scanned[2];
 struct slcli_native_clock scan_after[2];
 int scan_after_result[2];uint32_t scan_after_returned[2];
 uint32_t started,failed,complete,backing_registered,view_registered,live_registered;
};
int slcli_writer_capture_extent_windows_prepare_original(
 struct slcli_writer_capture_extent_windows *,struct slcli_arena *,
 struct slcli_writer_windows_readback_original *,struct slcli_memory_owner *,
 const struct slcli_extent_request *,uint64_t,uint64_t,uint64_t);
int slcli_writer_capture_extent_windows_matches_original(
 const struct slcli_writer_capture_extent_windows *);
#endif
#endif
