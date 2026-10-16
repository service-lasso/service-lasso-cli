#ifndef SLCLI_STREAM_COMPLETION_VECTORS_H
#define SLCLI_STREAM_COMPLETION_VECTORS_H
#include "stream_native.h"
#if defined(_WIN32)
/* Genuine native pending reader and independently retained expected producer
 * bytes/scratch supplied by precharged original fixture. SOURCE-UNRUN. */
int slcli_stream_completion_positive_original(struct slcli_stream_capture *,
  const unsigned char *,size_t,unsigned char *,size_t);
/* Postcondition only on an independently retained real native success followed
 * by genuine clock failure. This does not cause/exercise that failure. */
int slcli_stream_completion_clock_failure_retained(const struct slcli_stream_capture *,
  const unsigned char *,size_t);
#endif
#endif