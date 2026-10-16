#ifndef SLCLI_WRITER_CAPTURE_VIEW_WINDOWS_H
#define SLCLI_WRITER_CAPTURE_VIEW_WINDOWS_H
#include "capture_backing_windows.h"
#if defined(_WIN32)
int slcli_writer_capture_view_windows_batch_matches_original(const struct slcli_writer_capture_backing_windows *);
int slcli_writer_capture_view_windows_prepare_batch_original(struct slcli_writer_capture_backing_windows *);
/* Native aliases borrow the original single full-file section pair. */
int slcli_writer_capture_view_windows_prepare_original(
 struct slcli_writer_capture_extent_windows *,struct slcli_writer_capture_backing_windows *,size_t);
int slcli_writer_capture_view_windows_matches_original(
 const struct slcli_writer_capture_extent_windows *);
#endif
#endif
