#ifndef SLCLI_WRITER_BUILD_WINDOWS_H
#define SLCLI_WRITER_BUILD_WINDOWS_H
#include "directory_windows.h"
#if defined(_WIN32)
struct slcli_writer_windows_route {
  struct slcli_slice path,leaf;
  size_t parent_directory;
  struct slcli_writer_windows_path *created;
  struct slcli_writer_windows_output *output;
  const unsigned char *descriptor;
  size_t descriptor_bytes;
  WCHAR native_leaf[256];
};
struct slcli_writer_windows_build {
  struct slcli_writer_attempt *attempt;
  HANDLE admitted_parent,effective_original;
  const FILE_ID_INFO *admitted_parent_identity;
  const TOKEN_STATISTICS *admitted_token;
  const unsigned char *admitted_owner,*candidate_descriptor;
  size_t owner_bytes,candidate_descriptor_bytes;
  struct slcli_slice admitted_destination;
  const WCHAR *candidate_leaf;
  size_t candidate_units;
  struct slcli_writer_windows_path *candidate;
  struct slcli_writer_windows_route *directories,*files;
  size_t directory_count,file_count;
  struct slcli_writer_windows_materialization *materialize;
  struct slcli_writer_windows_publish *publish;
  struct slcli_writer_windows_discovery *discovery;
  size_t discovery_count;
  slcli_writer_windows_path_sink path_sink;
  slcli_writer_mode_sink mode_sink;
  slcli_store_capture_sink file_sink;
  void *archive_original;
  uint32_t used,failed,complete;
};
/* Whole existing original materializer chain. This is not an authority or
 * native-memory constructor: every real source/parent/token/capture owner and
 * complete finite allocation remains an independently admitted original. */
int slcli_writer_build_windows_original(struct slcli_writer_windows_build *);
#endif
#endif
