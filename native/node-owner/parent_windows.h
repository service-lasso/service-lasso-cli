#ifndef SLCLI_NODE_PARENT_WINDOWS_H
#define SLCLI_NODE_PARENT_WINDOWS_H
#include "bootstrap_transport.h"
#if defined(_WIN32)
struct slcli_node_parent_windows_call {
  struct slcli_native_clock before,after;
  uint64_t args[5],result;
  DWORD error;
  uint32_t called,returned,error_observed,before_result,after_result;
};
/* Paid zero image storage owned by the actual earliest entry, not a heap
 * allocation or new owner. Equality is custody only, never source admission. */
struct slcli_node_parent_windows {
  const struct slcli_node_bootstrap_decode *decode_original;
  const struct slcli_native_clock *prior_original;
  HANDLE parent_original,token_raw,token;
  uint64_t expires,origin_tick,prior_tick;
  DWORD expected_pid,expected_creation[2],expected_sid_bytes;
  unsigned char expected_sid[SECURITY_MAX_SID_SIZE];
  DWORD expected_token_id[2],expected_authentication_id[2];
  DWORD pid,user_bytes,statistics_bytes;
  FILETIME creation,exit,kernel,user;
  union { void *alignment;unsigned char bytes[TOKEN_USER_MAX_SIZE]; } user_buffer;
  TOKEN_STATISTICS statistics;
  struct slcli_node_parent_windows_call calls[7];
  size_t sid_compared;
  uint32_t started,terminal,observations_match,token_disposal_attempted;
  uint32_t token_closed,token_disposal_pending,returned;
};
/* parent is the separately routed actual inherited original. Decoded numbers
 * alone must never select authority. The actual entry must independently join
 * image/private source/namespace/rights/lifetime before owner continuation. */
int slcli_node_parent_windows_observe(struct slcli_node_parent_windows *,
  const struct slcli_node_bootstrap_decode *,HANDLE,const struct slcli_native_clock *);
int slcli_node_parent_windows_dispose_token(struct slcli_node_parent_windows *);
#endif
#endif
