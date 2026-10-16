#ifndef SLCLI_WRITER_DISCOVERY_LINUX_H
#define SLCLI_WRITER_DISCOVERY_LINUX_H
#include "paths_linux.h"
#if defined(__linux__)
struct slcli_writer_discovery_child {
  int original;
  const char *leaf;
  size_t bytes;
  struct stat admitted_identity;
  const struct slcli_writer_path_effect *baseline;
  uint32_t seen;
};
typedef int (*slcli_writer_discovery_sink)(void *,const struct slcli_writer_path_effect *,
  const unsigned char *,size_t);
struct slcli_writer_discovery {
  int original;
  struct slcli_writer_discovery_child *children;
  size_t child_count;
  struct slcli_writer_path_effect *captures;
  size_t capture_count,capture_capacity;
  unsigned char *scratch;
  size_t scratch_bytes;
  slcli_writer_discovery_sink sink;
  void *archive_original;
  uint64_t bytes,records;
  uint32_t used,failed,complete,eof;
};
/* One serialization-owned held-directory cursor, genuine held child mappings,
 * separately precharged 1024-byte scratch and retained one-use raw rows. */
int slcli_writer_discover_linux_original(struct slcli_writer_discovery *);
#endif
#endif
