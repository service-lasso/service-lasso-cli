#ifndef SLCLI_STORE_PUBLISH_H
#define SLCLI_STORE_PUBLISH_H
#include "store_body.h"
#if defined(_WIN32) || defined(__linux__)
enum slcli_store_leaf { SLCLI_GENERATION_A,SLCLI_GENERATION_B,SLCLI_SELECTOR,
  SLCLI_OBSERVATION,SLCLI_CLOSURE,SLCLI_RECEIPT };
/* Raw original atomic namespace effects. The admitted original containing
 * directory, exclusive temporary leaf and source authority are external held
 * parameters. No pathname discovery, keyPath read, permission provisioning,
 * close, retry, scratch deletion or new admission occurs here. */
struct slcli_store_publish;
struct slcli_store_publish_deadline_owner;
/* Original independently admitted raw namespace custody sink, never a grant. */
typedef int (*slcli_namespace_capture_sink)(void *,const struct slcli_store_publish *,uint32_t);
struct slcli_store_publish {
  struct slcli_store_publish_deadline_owner *deadline_owner;
  slcli_input_handle root,temporary,reopened;
  uint32_t started,failed,completed,leaf,index;
  uint32_t effect_called[8],effect_returned[8];
  char temporary_name[46];
  char leaf_name[24];
  slcli_input_capture before,after,temporary_route;
#if defined(_WIN32)
  ULONG rename_bytes;
  unsigned char rename_raw[128];
  IO_STATUS_BLOCK rename_io,open_io;
  NTSTATUS rename_status,open_status;
  OBJECT_ATTRIBUTES attributes;
  UNICODE_STRING name;
  WCHAR name_raw[16];
  BOOL root_flush;
  DWORD root_error;
  uint32_t retained_native_io;
#else
  slcli_input_handle temporary_route_handle;
  uint64_t args[5][6],status[5];
  uint32_t observed[5];
#endif
};
int slcli_store_publish_original(struct slcli_store_publish *,
                                  const struct slcli_store_stream *,slcli_input_handle,
                                  const unsigned char[16],uint32_t);
#endif
#endif
