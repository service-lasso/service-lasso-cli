#ifndef SLCLI_WRITER_DIRECTORY_WINDOWS_H
#define SLCLI_WRITER_DIRECTORY_WINDOWS_H
#include "paths_windows.h"
#if defined(_WIN32)
/* Each one-use row, native output pointer and namespace handle remains in the
 * original finite owner, including all failed and STATUS_PENDING results. */
struct slcli_writer_windows_effect {
  HANDLE original,opened;
  uint64_t args[11];
  IO_STATUS_BLOCK io;
  NTSTATUS status;
  DWORD error;
  BOOL result;
  OBJECT_ATTRIBUTES attributes;
  UNICODE_STRING name;
  struct slcli_windows_file_capture file;
  LARGE_INTEGER counter,frequency;
  BOOL counter_ok,frequency_ok;
  uint64_t tick;
  uint32_t used,operation,retained,pending,complete;
};
typedef int (*slcli_writer_windows_effect_sink)(void *,
  const struct slcli_writer_windows_effect *,const unsigned char *,size_t);
struct slcli_writer_windows_child {
  struct slcli_writer_windows_path *created;
  const struct slcli_windows_file_capture *baseline;
  uint32_t seen;
};
struct slcli_writer_windows_discovery {
  HANDLE original;
  struct slcli_writer_windows_child *children;
  size_t child_count;
  struct slcli_writer_windows_effect *captures;
  size_t capture_count,capture_capacity;
  unsigned char *scratch;
  size_t scratch_bytes;
  slcli_writer_windows_effect_sink sink;
  void *archive_original;
  uint64_t bytes,records;
  uint32_t used,failed,complete,eof;
};
struct slcli_writer_windows_publish {
  struct slcli_writer_windows_path *candidate;
  const FILE_ID_INFO *admitted_parent_identity;
  WCHAR final_leaf[256];
  uint64_t rename_raw[80];
  size_t final_units,rename_bytes;
  struct slcli_writer_windows_effect effects[10];
  size_t count;
  slcli_writer_windows_effect_sink sink;
  void *archive_original;
  HANDLE route_original,reopened;
  uint32_t used,failed,complete;
};
int slcli_writer_windows_same_identity(const FILE_ID_INFO *,const FILE_ID_INFO *);
int slcli_writer_windows_same_file(const struct slcli_windows_file_capture *,
  const struct slcli_windows_file_capture *);
int slcli_writer_discover_windows_original(struct slcli_writer_windows_discovery *);
int slcli_writer_publish_windows_directory(struct slcli_writer_windows_publish *);
#endif
#endif
