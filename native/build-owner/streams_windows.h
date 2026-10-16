#ifndef SLCLI_BUILD_STREAMS_WINDOWS_H
#define SLCLI_BUILD_STREAMS_WINDOWS_H
#include "command_windows.h"
#include "../host-service/arena.h"
#if defined(_WIN32)
struct slcli_build_stream_windows_row {
 struct slcli_build_windows_call peek,read;
 DWORD available_original,received_original;
 size_t offset_original,requested_original;
 uint32_t data_observed,eof_observed,idle_observed;
};
struct slcli_build_stream_windows {
 struct slcli_build_stream_windows_row *rows_original;
 unsigned char *body_original;
 size_t row_capacity,byte_capacity,rows_used,bytes_used;
 HANDLE reader_original;
 uint32_t eof_observed;
};
struct slcli_build_streams_windows {
 const struct slcli_build_windows_command *birth_original;
 struct slcli_build_windows_call issuing_thread,parent_child_ends[3];
 struct slcli_build_stream_windows streams_original[2];
 struct slcli_build_windows_call *poll_threads_original;
 struct slcli_native_clock *poll_finishes_original;
 struct slcli_native_clock finish;
 size_t poll_capacity,polls_used,parent_ends_closed;
 uint64_t prior_tick,expires;
 uint32_t prepared,started,failed,streams_complete;
};
/* Prebirth checked backing only; counts are source constraints, never funding. */
int slcli_build_stream_windows_read_extent_original(size_t,size_t,DWORD,size_t *);
int slcli_build_streams_windows_prepare_original(struct slcli_build_streams_windows *,struct slcli_arena *,size_t,size_t);
/* Close only the three parent-owned child-end refs, after stopped image joins. */
int slcli_build_streams_windows_begin_original(struct slcli_build_streams_windows *,
 const struct slcli_build_windows_command *,const struct slcli_native_clock *);
/* One finite real poll per stream; 1 is a recorded poll, never compiler success. */
int slcli_build_streams_windows_poll_original(struct slcli_build_streams_windows *);
#endif
#endif
