#ifndef SLCLI_NODE_TRANSPORT_WINDOWS_H
#define SLCLI_NODE_TRANSPORT_WINDOWS_H
#include "bootstrap_transport.h"
#include "../host-service/source_deadline.h"
#if defined(_WIN32)
struct slcli_node_transport_windows_call {
  struct slcli_native_clock before,after;
  uint64_t args[7],result;
  DWORD error;
  uint32_t called,returned,error_observed,before_result,after_result;
};
/* Parent-owned paid control capture, distinct from frozen target/runtime.
 * Readonly inheritance is routing; genuine private source/object/rights and
 * complete alias closure remain independently required. No implicit close. */
struct slcli_node_transport_windows {
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin,*predecessor;
  const void *caller,*descriptor_original;
  const struct slcli_node_bootstrap_transport *transport_original;
  size_t caller_bytes,descriptor_bytes,compared;
  uint64_t origin_tick,expires,predecessor_tick,physical_bytes,release_related;
  struct slcli_extent_request request,release_request;
  struct slcli_native_clock constructor[4],publication[3];
  /* 0..4 section birth, 5..6 writer disposal, 7..9 parent object,
   * 10..12 real-token capture, 13 independently eligible token close. */
  struct slcli_node_transport_windows_call calls[14];
  unsigned char *encoded,*readback;
  void *descriptor,*writer_view;
  HANDLE current_process,writable,readonly_raw,readonly;
  HANDLE parent_raw,parent;
  DWORD parent_pid;
  FILETIME parent_creation,parent_exit,parent_kernel,parent_user;
  HANDLE parent_token_raw,parent_token;
  union { void *alignment;unsigned char bytes[TOKEN_USER_MAX_SIZE]; } parent_user_buffer;
  TOKEN_STATISTICS parent_statistics;
  DWORD parent_user_bytes,parent_statistics_bytes;
  unsigned char parent_sid[SECURITY_MAX_SID_SIZE];
  uint32_t parent_sid_bytes,parent_token_closed,parent_token_disposal_attempted;
  uint32_t parent_token_disposal_pending;
  SECURITY_ATTRIBUTES security;
  MEMORY_BASIC_INFORMATION geometry;
  int prepare_result,backing_result,partial_result,acquired_result,encode_result;
  int release_prepare_result,view_transition_result;
  uint32_t started,object_prepared,publication_attempted,terminal,returned;
  uint32_t prepare_returned,backing_returned,partial_returned,acquired_returned;
  uint32_t encode_returned,release_prepare_returned,view_transition_returned;
  uint32_t bytes_equal,writer_unmapped,writable_closed;
  uint32_t writer_disposal_attempted,writer_disposal_terminal,writer_disposal_pending;
};
size_t slcli_node_transport_windows_control_bytes(size_t);
int slcli_node_transport_windows_create(struct slcli_node_transport_windows *,
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,
  size_t,const struct slcli_native_clock *,const void *,size_t);
int slcli_node_transport_windows_publish(struct slcli_node_transport_windows *,
  struct slcli_arena *,const struct slcli_native_clock *,const struct slcli_node_bootstrap_transport *);
/* Writer-only disposition. Readonly aliases and physical backing stay charged. */
int slcli_node_transport_windows_dispose_writer(struct slcli_node_transport_windows *,struct slcli_arena *);
#endif
#endif
