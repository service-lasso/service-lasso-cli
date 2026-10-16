#ifndef SLCLI_WRITER_MATERIALIZE_WINDOWS_H
#define SLCLI_WRITER_MATERIALIZE_WINDOWS_H
#include "attempt.h"
#include "mode_windows.h"
#include "../host-service/evidence_native.h"
#if defined(_WIN32)
struct slcli_writer_windows_output {
  struct slcli_slice admitted_path;
  struct slcli_evidence_file *file;
  struct slcli_writer_mode_capture before,after;
  uint32_t used,complete,failed;
};
struct slcli_writer_windows_materialization {
  struct slcli_writer_attempt *attempt;
  struct slcli_writer_windows_output *outputs;
  size_t count;
  HANDLE effective_original;
  const TOKEN_STATISTICS *admitted_token;
  const unsigned char *admitted_owner;
  size_t owner_bytes;
  slcli_writer_mode_sink mode_sink;
  void *archive_original;
  uint32_t used,complete,failed;
};
int slcli_writer_materialize_windows_originals(struct slcli_writer_windows_materialization *);
#endif
#endif
