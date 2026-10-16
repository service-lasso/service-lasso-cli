#ifndef SLCLI_SOURCE_UNIT12_MEMBER_WINDOWS_H
#define SLCLI_SOURCE_UNIT12_MEMBER_WINDOWS_H
#include "source_unit12_native.h"
#include "../writer/capture_ranges_windows.h"
#if defined(_WIN32)
struct slcli_source_unit12_member_window_windows {
 struct slcli_arena *arena;
 struct slcli_source_unit12_native *table;
 struct slcli_writer_capture_ranges_windows *foundation;
 size_t member_range,requested,returned_bytes[2];uint64_t source_offset;
 size_t journal_payload_maximum;uint64_t journal_record_maximum;
 struct slcli_source_unit12_record member;
 struct slcli_source_unit12_window geometry;
 struct slcli_windows_file_capture *captures[8];
 struct slcli_native_clock clocks[8][2];
 int before_result[8],native_result[8],after_result[8];
 uint32_t before_returned[8],native_called[8],native_returned[8],after_returned[8];
 unsigned char body[16385],readback[16385],digest[32];
 uint64_t previous_tick;size_t reserved_captures,capture_first_ordinal;
 size_t storage_charge;
 struct slcli_native_clock finish;int finish_result;uint32_t finish_returned;
 uint32_t started,failed,observed,full_member_bytes_compared;
};
/* One retained window only. No disposition/reset/reuse API exists. Original
 * A/Git/actor/source/full-member authority remains separately required. */
int slcli_source_unit12_member_window_windows_read_original(
 struct slcli_source_unit12_member_window_windows *,struct slcli_arena *,
 struct slcli_source_unit12_native *,size_t,struct slcli_writer_capture_ranges_windows *);
/* Returning observation only; the actual consumer brackets this complete
 * scan with the SAME original deadline. No admission or disposition. */
int slcli_source_unit12_member_window_windows_matches_original(
 const struct slcli_source_unit12_member_window_windows *);
#endif
#endif
