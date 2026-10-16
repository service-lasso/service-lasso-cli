#ifndef SLCLI_EVIDENCE_DEADLINE_H
#define SLCLI_EVIDENCE_DEADLINE_H
#include "image_deadline.h"
#include "profile.h"
#if defined(_WIN32) || defined(__linux__)
/* Private original evidence IO owner. The row view borrows the SAME already
 * charged captures/workspace and supplies no image hash or native authority.
 * Root flush and writer IO are distinct held targets, never interchangeable. */
struct slcli_evidence_deadline_owner {
  struct slcli_image_reader rows;
  struct slcli_evidence_file *file;
  slcli_input_handle root,active_original;
  uint64_t maximum;
  const struct slcli_profile *bq_profile;
  const unsigned char *profile_raw;
  size_t profile_bytes;
  uint64_t record_maximum;
  uint32_t prepared,inflight,failed;
};
int slcli_evidence_deadline_prepare_unborn(struct slcli_arena *,struct slcli_evidence_file *,
  const struct slcli_source_deadline_original *,const void *,size_t,void *,size_t,
  uint64_t,slcli_input_handle,uint64_t,slcli_store_capture_sink,
  slcli_input_handle *,const void *,size_t);
int slcli_evidence_deadline_prepare_bq_unborn(struct slcli_arena *,struct slcli_evidence_file *,
  const struct slcli_source_deadline_original *,const void *,size_t,void *,size_t,
  uint64_t,slcli_input_handle,const struct slcli_profile *,slcli_store_capture_sink,
  slcli_input_handle *,const void *,size_t);
int slcli_evidence_deadline_born(struct slcli_evidence_file *,slcli_input_handle *,
  const struct slcli_native_clock *);
int slcli_evidence_deadline_ready(const struct slcli_evidence_file *);
int slcli_evidence_deadline_precede(struct slcli_evidence_file *,const struct slcli_native_clock *);
int slcli_evidence_deadline_before(struct slcli_evidence_file *,slcli_input_capture *,slcli_input_handle);
int slcli_evidence_deadline_after(struct slcli_evidence_file *,slcli_input_capture *,int);
#endif
#endif
