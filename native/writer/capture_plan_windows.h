#ifndef SLCLI_WRITER_CAPTURE_PLAN_WINDOWS_H
#define SLCLI_WRITER_CAPTURE_PLAN_WINDOWS_H
#include "capture_extent_windows.h"
#if defined(_WIN32)
struct slcli_writer_capture_view_plan_windows {
 uint64_t file_offset,bytes;
 struct slcli_writer_capture_extent_windows *capture;
};
/* Allocation-free geometry over actual preowned schedule/control blocks.
 * No section, source authority, range commitment or admission is produced. */
int slcli_writer_capture_plan_windows_geometry_original(struct slcli_arena *,
 const struct slcli_writer_capture_view_plan_windows *,size_t,uint64_t,
 const SYSTEM_INFO *,uint64_t *);
#endif
#endif
