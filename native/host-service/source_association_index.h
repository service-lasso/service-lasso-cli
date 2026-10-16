/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_SOURCE_ASSOCIATION_INDEX_H
#define SLCLI_SOURCE_ASSOCIATION_INDEX_H
#include <stddef.h>
#include <stdint.h>
/* Structural streaming A only. Native owners, original source/capture/provider/
 * publication relations and complete Git/manifest verification are separate.
 * No decoded name is a native acquisition path or an admitted member. */
struct slcli_source_association_record {
 unsigned char association[32],capture[32],digest[32];
 unsigned char git[20],revision[20],root[20];
 unsigned char provider[32],publication[32],provenance[32];
 uint32_t kind,project,mode;
 uint64_t name_offset,name_bytes,body_bytes,index;
};
struct slcli_source_association_reader {
 unsigned char header[128],attempted_record[320];
 struct slcli_source_association_record previous;
 uint64_t count,file_bytes,names_offset,names_bytes,next_name,next,attempted;
 uint64_t names_consumed;
 uint32_t started,failed,table_complete,names_complete;
};
#ifdef __cplusplus
extern "C" {
#endif
int slcli_source_association_header(struct slcli_source_association_reader *,
 const unsigned char[128],uint64_t,uint64_t);
int slcli_source_association_record(struct slcli_source_association_reader *,
 const unsigned char[320]);
/* Actual native reader supplies each settled bounded span in exact order.
 * This accounts extent only; caller retains and independently verifies bytes. */
int slcli_source_association_names(struct slcli_source_association_reader *,
 uint64_t,size_t);
#ifdef __cplusplus
}
#endif
#endif
