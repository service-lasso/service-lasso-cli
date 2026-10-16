#ifndef SLCLI_IMAGE_READER_H
#define SLCLI_IMAGE_READER_H
#include "evidence_native.h"
#include "source_deadline.h"
#if defined(_WIN32) || defined(__linux__)
struct slcli_image_deadline_owner;
struct slcli_image_reader {
  struct slcli_image_deadline_owner *deadline_owner;
  struct slcli_shared_workspace *workspace;
  slcli_input_handle original;
  slcli_input_capture *captures;
  size_t capture_count,capture_capacity;
  slcli_store_capture_sink capture_sink;
  void *capture_original;
  uint64_t offset,bytes;
  unsigned char digest[32];
  const slcli_input_capture *before,*after;
  uint32_t used,failed,complete;
};
/* Independent concrete native raw-image reader. Original source lease/actor/
 * owning region and finite captures must be admitted separately; a path, image
 * hash or actual readonly query does not supply that authority. */
int slcli_image_read_original(struct slcli_image_reader *);
/* Private serialized constructor precharges complete native clock rows before
 * any image IO. This binds custody/lifetime, never original actor authority. */
int slcli_image_deadline_bind(struct slcli_arena *,struct slcli_image_reader *,
  const struct slcli_source_deadline_original *,const void *,size_t,
  void *,size_t,uint64_t);
int slcli_image_deadline_ready(const struct slcli_image_reader *);
#endif
#endif
