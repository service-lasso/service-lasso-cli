#ifndef SLCLI_WRITER_BUILD_LINUX_H
#define SLCLI_WRITER_BUILD_LINUX_H
#include "discovery_linux.h"
#if defined(__linux__)
struct slcli_writer_linux_route {
  struct slcli_slice leaf,path;
  size_t parent_directory;
  struct slcli_writer_path_original *created;
  struct slcli_writer_output_original *output;
};
struct slcli_writer_linux_build {
  struct slcli_writer_attempt *attempt;
  int admitted_parent;
  const struct stat *admitted_parent_identity;
  struct slcli_slice admitted_destination;
  uint64_t admitted_uid,admitted_gid;
  const char *candidate_leaf,*final_leaf;
  size_t candidate_bytes,final_bytes;
  struct slcli_writer_path_original *candidate;
  struct slcli_writer_linux_route *directories,*files;
  size_t directory_count,file_count;
  struct slcli_writer_materialization *materialize;
  struct slcli_writer_discovery *discovery;
  size_t discovery_count;
  slcli_writer_path_sink path_sink;
  slcli_store_capture_sink file_sink;
  void *archive_original;
  uint32_t used,failed,complete;
};
/* Entire mapped candidate construction/materialization and discovery-before-
 * publish chain. Caller reserves ALL original objects/captures and both sets
 * of discovery contexts in the same finite owner before this first effect.
 * Original authority, source lifetime, final discovery/receipt/physical close
 * remain separately required; native namespace success cannot accept a run. */
int slcli_writer_build_linux_original(struct slcli_writer_linux_build *);
#endif
#endif
