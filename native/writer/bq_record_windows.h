#ifndef SLCLI_WRITER_BQ_RECORD_WINDOWS_H
#define SLCLI_WRITER_BQ_RECORD_WINDOWS_H
#include "../host-service/source_unit12_member_windows.h"
#include "../host-service/bq_source_control_sink_windows.h"
#if defined(_WIN32)
/* Source-derived roster of actual calls in one retained producer window.
 * These descriptors are neither committed records nor RANGE dispositions. */
struct slcli_writer_bq_member_record_windows {
 const struct slcli_windows_file_capture *capture;
 const unsigned char *payload;
 uint64_t sequence,journal_offset,source_read_offset;
 size_t record_bytes;
 uint32_t producer_lane,kind,requested,returned;
};
/* Actual one-use Layer1 IO cells, never an admitted RANGE/disposition. */
struct slcli_writer_bq_record_io_windows {
 struct slcli_windows_file_capture *capture;
 unsigned char *payload_original;
 const unsigned char *payload_readonly;
 size_t payload_requested;
 HANDLE handle_original;
 const unsigned char *write_original;
 unsigned char *comparison_original;
 uint64_t offset_original;
 size_t bytes_original;
 struct slcli_native_clock before,after;
 uint32_t kind,before_returned,called,returned,after_returned;
 int before_result,result,after_result;
};
struct slcli_writer_bq_record_windows {
 struct slcli_arena *arena;
 struct slcli_writer_windows_readback_original *journal;
 struct slcli_source_unit12_member_window_windows *producer;
 struct slcli_writer_capture_ranges_windows *foundation;
 struct slcli_writer_capture_ranges_windows *foundation_output;
 struct slcli_writer_capture_ranges_windows *foundation_control;
 struct slcli_bq_source_control_sink_windows *source_control;
 size_t source_control_count,source_control_used;
 struct slcli_writer_bq_record_io_windows *control_io;
 size_t control_io_capacity,control_io_attempted,control_prefix_bytes,control_read_bytes;
 uint64_t control_previous_tick;
 uint32_t control_started,control_observed;
 struct slcli_native_clock control_finish;
 int control_finish_result;uint32_t control_finish_returned;
 struct slcli_writer_bq_record_io_windows *io;
 struct slcli_writer_bq_record_io_windows *foundation_io;
 struct slcli_writer_bq_record_io_windows unmap_io;
 struct slcli_native_clock unmap_finish;
 int unmap_finish_result;
 uint32_t unmap_started,unmap_observed,unmap_finish_returned;
 struct slcli_writer_bq_member_record_windows member_records[8];
 size_t member_record_count;
 uint64_t planned_journal_bytes;
 size_t foundation_io_capacity,foundation_io_attempted,foundation_prefix_bytes,foundation_read_bytes;
 size_t foundation_output_first_ordinal;
 uint64_t foundation_previous_tick;
 uint32_t foundation_started,foundation_failed,foundation_io_observed;
 struct slcli_native_clock foundation_finish;
 int foundation_finish_result;uint32_t foundation_finish_returned;
 int foundation_validation_result;uint32_t foundation_validation_called,foundation_validation_returned;
 size_t io_capacity,io_attempted,capture_first_ordinal,record_bytes,header_written,record_written;
 size_t header_read,record_read;
 uint64_t previous_tick;
 unsigned char header[16],header_readback[16],write_window[16384],readback_window[16384],eof;
 uint32_t started,failed,append_pending,io_observed;
 uint32_t validation_called,validation_returned;
 int validation_result;
 struct slcli_native_clock validation_before,validation_after;
 int validation_before_result,validation_after_result;
 uint32_t validation_before_returned,validation_after_returned;
};
/* First record only: actual empty105 -> header -> first BEFORE record ->
 * durability -> original readonly98 comparison/EOF. No current-end/sequence
 * receipt input and no continuation until genuine retention disposition exists. */
int slcli_writer_bq_record_windows_first_original(struct slcli_writer_bq_record_windows *,
 struct slcli_arena *,struct slcli_writer_windows_readback_original *,
 struct slcli_source_unit12_member_window_windows *,struct slcli_writer_capture_ranges_windows *,
 struct slcli_writer_capture_ranges_windows *,
 struct slcli_writer_capture_ranges_windows *,
 struct slcli_bq_source_control_sink_windows *,
 struct slcli_writer_bq_record_io_windows *,size_t,
 struct slcli_writer_bq_record_io_windows *,size_t,
 struct slcli_writer_bq_record_io_windows *,size_t);
/* Reconstruct this exact first-record native sequence from original rows.
 * This observes IO only; it cannot commit or dispose any range. */
int slcli_writer_bq_record_windows_matches_original(struct slcli_writer_bq_record_windows *);
/* Returning raw unmap observation only; not committed-range disposition. */
int slcli_writer_bq_record_windows_unmap_matches_original(struct slcli_writer_bq_record_windows *);
#endif
#endif
