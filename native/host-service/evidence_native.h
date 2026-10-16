#ifndef SLCLI_EVIDENCE_NATIVE_H
#define SLCLI_EVIDENCE_NATIVE_H
#include "archive_native.h"
#include "shared_workspace.h"
#if defined(_WIN32) || defined(__linux__)
/* A single source-owned exclusively created original retention file. Original
 * owner/provider/identity/retention reservation are independently admitted
 * inputs, never selected by this byte writer. Control/scratch/captures are in
 * the same native arena; the finite disk reserve is not additional RAM. */
struct slcli_profile;
struct slcli_evidence_file {
  struct slcli_evidence_deadline_owner *deadline_owner;
  struct slcli_shared_workspace *workspace;
  slcli_input_capture *captures;
  slcli_input_handle original,root;
  slcli_store_capture_sink capture_sink;
  void *capture_original;
  size_t capture_count,capture_capacity;
  uint64_t maximum,count;
  struct slcli_sha256 content;
  unsigned char digest[32];
  const slcli_input_capture *readback_original;
  uint32_t started,failed,sealed,close_started;
};
struct slcli_evidence_file *slcli_evidence_allocate(struct slcli_arena *,size_t);
int slcli_evidence_begin(struct slcli_evidence_file *,slcli_input_handle,
                          slcli_input_handle,uint64_t,slcli_store_capture_sink,void *);
/* Private BQ byte IO only; owning BQ/source authority remains independent. */
int slcli_evidence_begin_bq(struct slcli_evidence_file *,slcli_input_handle,
  slcli_input_handle,const struct slcli_profile *,slcli_store_capture_sink,void *);
int slcli_evidence_append(void *,const unsigned char *,size_t);
int slcli_evidence_flush_readback(struct slcli_evidence_file *);
/* Fresh native full read/EOF/identity/flush chain, using distinct retained rows
 * and the SAME original sealed logical content. This does not physically seal
 * a file or grant source/retirement authority. Failure retains workspace. */
int slcli_evidence_recheck_readback(struct slcli_evidence_file *);
#endif
#endif
