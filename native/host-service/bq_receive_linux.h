#ifndef SLCLI_BQ_RECEIVE_LINUX_H
#define SLCLI_BQ_RECEIVE_LINUX_H
#if defined(__linux__)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/socket.h>
#include "primary_engine.h"
enum { SLCLI_BQ_LINUX_CONTROL_BYTES=CMSG_SPACE(sizeof(struct ucred))+CMSG_SPACE(sizeof(int)),
 SLCLI_BQ_LINUX_RECEIVED_FDS=(SLCLI_BQ_LINUX_CONTROL_BYTES-CMSG_LEN(0))/sizeof(int) };
struct slcli_bq_receive_linux_original {
 struct slcli_binding_storage_original *storage;uint32_t ordinal,first_fd_ordinal;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 const struct slcli_native_clock *prior;uint64_t prior_tick;
 const struct slcli_native_clock *origin;uint64_t origin_tick,expires;
 int endpoint;uint64_t args[3];long status;
 struct iovec vector;struct msghdr message;
 unsigned char payload[16384];
 union {struct cmsghdr alignment;unsigned char bytes[SLCLI_BQ_LINUX_CONTROL_BYTES];} control;
 int received_fds[SLCLI_BQ_LINUX_RECEIVED_FDS];uint32_t fd_count,credential_count,rights_count;
 struct ucred credentials;
 struct slcli_native_clock before,after;
 int before_result,after_result;
 uint32_t started,failed,called,returned,before_returned,after_returned,complete;
 struct slcli_native_clock validation_after;
 int validation_result,validation_clock_result;
 uint32_t validation_returned,validation_clock_returned;
};
/* Requires SAME-book whole typed capture claim before this call. Return value
 * is raw structural receipt only, never peer/source/native enrollment. */
int slcli_bq_receive_linux_preclaimed_original(struct slcli_bq_receive_linux_original *,
 struct slcli_binding_storage_original *,uint32_t,uint32_t,int,
 const struct slcli_source_deadline_original *,const void *,size_t,const struct slcli_native_clock *);
int slcli_bq_receive_linux_matches_original(const struct slcli_bq_receive_linux_original *);
#endif
#endif
