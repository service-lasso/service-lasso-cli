/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_BQ_PROVIDER_ENTRY_H
#define SLCLI_BQ_PROVIDER_ENTRY_H
#include <stddef.h>
#include <stdint.h>
#if defined(__cplusplus)
extern "C" {
#endif
/* INITIAL_PROVIDER prerequisite-tool private entry. Native endpoint label72 is
 * NOT fd72. argv1 is canonical decimal of the actual inherited descriptor;
 * argv2 is the positive complete offer count, and prior private operands begin
 * at argv3. Count is geometry only, never financing or authority. Public commands are
 * unchanged. Syntax neither establishes socket type/peer nor any admission. */
enum { SLCLI_BQ_PROVIDER_ENDPOINT_ARGUMENT=1,
 SLCLI_BQ_PROVIDER_OFFER_COUNT_ARGUMENT=2,
 SLCLI_BQ_PROVIDER_PREVIOUS_ARGUMENT_START=3,
 SLCLI_BQ_PROVIDER_ENDPOINT_TEXT_BYTES=11,
 SLCLI_BQ_PROVIDER_COUNT_TEXT_BYTES=21 };
int slcli_bq_provider_endpoint_encode(char *,size_t,size_t *,int);
int slcli_bq_provider_endpoint_decode(int *,const char *,size_t);
int slcli_bq_provider_count_encode(char *,size_t,size_t *,uint64_t);
int slcli_bq_provider_count_decode(uint64_t *,const char *,size_t);
#if defined(__cplusplus)
}
#endif
#endif
