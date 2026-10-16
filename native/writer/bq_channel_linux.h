#ifndef SLCLI_BQ_CHANNEL_LINUX_H
#define SLCLI_BQ_CHANNEL_LINUX_H
#include "../host-service/source_deadline.h"
#if defined(__linux__)
#include <sys/socket.h>
/* Actual BQ-domain preowned native cells, not a second binding table. Native
 * completion grants no actor/source authority or child FD inheritance. */
struct slcli_bq_channel_linux_original {
 struct slcli_arena *arena;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_native_clock *prior;uint64_t prior_tick,previous_tick;
 const struct slcli_native_clock *origin;uint64_t origin_tick,expires;
 int endpoints[2],pass_credentials;
 uint64_t args[3][6];long status[3];
 struct slcli_native_clock before[3],after[3];
 int before_result[3],after_result[3];
 uint32_t called[3],returned[3],before_returned[3],after_returned[3];
 uint32_t started,failed,complete,next;
 struct slcli_native_clock validation_after;
 int validation_result,validation_clock_result;
 uint32_t validation_returned,validation_clock_returned;
};
/* Source-owned constructor must have genuine complete prior reservation and
 * qualification BEFORE invoking this raw native body. It allocates nothing. */
int slcli_bq_channel_linux_create_preowned_original(
 struct slcli_bq_channel_linux_original *,struct slcli_arena *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *);
int slcli_bq_channel_linux_matches_original(const struct slcli_bq_channel_linux_original *);
struct slcli_bq_send_linux_original {
 struct slcli_arena *arena;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_native_clock *prior,*origin;
 uint64_t prior_tick,origin_tick,expires,args[3];
 int endpoint,source_fd;const unsigned char *payload;size_t payload_bytes;
 struct iovec vector;struct msghdr message;
 union {struct cmsghdr alignment;unsigned char bytes[CMSG_SPACE(sizeof(int))];} control;
 struct slcli_native_clock before,after;long status;
 int before_result,after_result;
 uint32_t started,failed,called,returned,before_returned,after_returned,complete;
 struct slcli_native_clock validation_after;
 int validation_result,validation_clock_result;
 uint32_t validation_returned,validation_clock_returned;
};
/* Raw one-use send only: no original sender FD or buffer may be retired by
 * this result. Genuine source/birth/stage and receiver ACK are independent. */
int slcli_bq_send_linux_preowned_original(struct slcli_bq_send_linux_original *,
 struct slcli_arena *,int,int,const unsigned char *,size_t,
 const struct slcli_source_deadline_original *,const void *,size_t,const struct slcli_native_clock *);
int slcli_bq_send_linux_matches_original(const struct slcli_bq_send_linux_original *);
#endif
#endif
