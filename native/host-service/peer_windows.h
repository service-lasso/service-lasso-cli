#ifndef SLCLI_PEER_WINDOWS_H
#define SLCLI_PEER_WINDOWS_H
#include "image_reader.h"
#if defined(_WIN32)
struct slcli_peer_path_workspace {
  WCHAR *raw;
  size_t units;
  const void *borrower;
};
struct slcli_peer_windows_capture {
  HANDLE process,channel,owner_token,image_route;
  DWORD pid[2],peer_pid[2],image_units,error,token_bytes[2],user_bytes[2];
  BOOL peer_ok[2],times_ok[2],token_open_ok,token_ok[2],user_ok[2],path_ok;
  FILETIME creation[2],exit[2],kernel[2],user_time[2];
  TOKEN_STATISTICS token[2];
  uint64_t user_raw[2][64];
  uint64_t pid_args[2][1],peer_args[2][2],time_args[2][5],token_open_args[3];
  uint64_t token_args[2][5],user_args[2][5],path_args[4],image_args[11];
  UNICODE_STRING image_name;
  OBJECT_ATTRIBUTES image_attributes;
  IO_STATUS_BLOCK image_io;
  NTSTATUS image_status;
  struct slcli_image_reader *held_reader,*route_reader;
  struct slcli_peer_path_workspace *path;
  struct slcli_native_clock before_clock,after_clock,held_entry_clock,route_entry_clock,route_birth_clock;
  uint32_t used,failed,complete,pending;
};
struct slcli_peer_windows_originals {
  HANDLE process,channel,image;
  FILETIME creation;
  const TOKEN_STATISTICS *token;
  const unsigned char *owner;
  size_t owner_bytes;
};
typedef int (*slcli_peer_windows_sink)(void *,const struct slcli_peer_windows_capture *,const WCHAR *,size_t);
/* Real original process/birth/pipe-peer/token/image association. These raw
 * native observations are NOT an actor enrolment or SOURCE grant. Authentic
 * original owner/profile/realm/issuer T+S inputs and independent native source
 * actor admission remain UNKNOWN external requirements. No PID OpenProcess,
 * borrowed request identity or image-path-to-principal selector exists here. */
int slcli_peer_capture_windows(struct slcli_peer_windows_capture *,
  const struct slcli_peer_windows_originals *,slcli_peer_windows_sink,void *);
/* Private serialized preeffect constructor: precharges both whole image clock
 * tables and unborn route output join. Native/source/principal admission stays
 * external; this preparation returns no actor or foundation authority. */
int slcli_peer_windows_deadline_prepare(struct slcli_arena *,struct slcli_peer_windows_capture *,
  const struct slcli_peer_windows_originals *,const struct slcli_source_deadline_original *,
  const void *,size_t,void *,size_t,uint64_t);
#endif
#endif
