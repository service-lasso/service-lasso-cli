#ifndef SLCLI_WRITER_CAPTURE_RANGES_WINDOWS_H
#define SLCLI_WRITER_CAPTURE_RANGES_WINDOWS_H
#include "capture_extent_windows.h"
#if defined(_WIN32)
struct slcli_writer_bq_record_windows;
struct slcli_writer_capture_ranges_windows {
 const struct slcli_writer_bq_record_windows *write_close_bq_original;
 struct slcli_arena *arena;
 struct slcli_writer_capture_extent_windows *extent;
 size_t capacity,stride,data_offset,payload_offset,payload_capacity,initialized,attempted;
 uint32_t version;
 struct slcli_native_clock before,after;
 int before_result,after_result;
 uint32_t before_returned,after_returned,started,failed,prepared;
};
/* Fixed source-plan count, no allocation/source admission. Header version1
 * binds actual compiled native ABI size; claims are permanently uncertain
 * until an actual fixed consumer and independent file readback join them. */
int slcli_writer_capture_ranges_windows_prepare_original(
 struct slcli_writer_capture_ranges_windows *,struct slcli_arena *,
 struct slcli_writer_capture_extent_windows *,size_t);
/* Explicit v2: native returned bytes reside in the SAME original106 slot.
 * Geometry is prospective/preowned, never an allocation or admission. */
int slcli_writer_capture_ranges_windows_payload_geometry(size_t,size_t,size_t *,size_t *,size_t *);
int slcli_writer_capture_ranges_windows_prepare_payload_original(
 struct slcli_writer_capture_ranges_windows *,struct slcli_arena *,
 struct slcli_writer_capture_extent_windows *,size_t,size_t);
int slcli_writer_capture_ranges_windows_matches_original(const struct slcli_writer_capture_ranges_windows *);
struct slcli_windows_file_capture *slcli_writer_capture_ranges_windows_claim_read_original(
 struct slcli_writer_capture_ranges_windows *,size_t,unsigned char **,const unsigned char **);
/* Returned native output target is directly in the original capture file.
 * This consumes a fresh attempted frontier BEFORE returning the pointer.
 * No reset, completion/disposition setter or producer-window reuse supplied. */
struct slcli_windows_file_capture *slcli_writer_capture_ranges_windows_claim_original(
 struct slcli_writer_capture_ranges_windows *,uint32_t);
#endif
#endif
