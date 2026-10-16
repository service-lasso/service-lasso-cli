#ifndef SLCLI_CHANNEL_WRITE_VECTORS_H
#define SLCLI_CHANNEL_WRITE_VECTORS_H
#include "channel_write_native.h"
#if defined(_WIN32)
int slcli_channel_write_completion_positive_original(struct slcli_channel_write_capture *,
  size_t,unsigned char *,size_t);
/* Independently produced genuine native success/clock-failure postcondition,
 * not a cause driver or qualification. */
int slcli_channel_write_clock_failure_retained(const struct slcli_channel_write_capture *,size_t);
#endif
#if defined(_WIN32) || defined(__linux__)
/* Actual returned-zero evidence only, never UNKNOWN translated into zero. */
int slcli_channel_write_observed_zero_retained(const struct slcli_channel_write_capture *);
#endif
#endif