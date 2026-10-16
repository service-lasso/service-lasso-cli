#ifndef SLCLI_BQ_INSTALLATION_CAPTURE_WINDOWS_H
#define SLCLI_BQ_INSTALLATION_CAPTURE_WINDOWS_H
#include "bq_source_unit_windows.h"
#include "../writer/bq_record_windows.h"
#if defined(_WIN32)
struct slcli_bq_installation_capture_windows_inputs {
 struct slcli_bq_source_unit_windows *source;
 const struct slcli_bq_source_unit_windows_inputs *source_inputs;
 struct slcli_source_unit12_member_window_windows *member;
 size_t member_index;
 struct slcli_writer_bq_record_windows *record;
 struct slcli_writer_windows_readback_original *journal;
 struct slcli_writer_bq_record_io_windows *io,*foundation_io,*control_io;
 size_t io_count,foundation_count,control_count;
};
struct slcli_bq_installation_capture_windows {
 struct slcli_arena *arena;
 const struct slcli_bq_installation_capture_windows_inputs *inputs;
 const struct slcli_source_deadline_original *deadline;
 const struct slcli_native_clock *origin_original;
 uint64_t origin_tick,expires;
 const void *caller;size_t caller_bytes;
 int source_result,member_result,record_result,finish_result;
 uint32_t started,source_returned,member_returned,record_returned,finish_returned,first_failure,observed;
 struct slcli_native_clock finish;
};
/* Owning native source-phase caller only. Genuine initial installer/Q/P/source
 * admission is independently required; observed never means BQ qualification. */
int slcli_bq_installation_capture_windows_original(struct slcli_bq_installation_capture_windows *,
 struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_bq_installation_capture_windows_inputs *);
#endif
#endif
