#ifndef SLCLI_UNIT19_H
#define SLCLI_UNIT19_H
#include "crypto.h"
/* Representation only. Caller retains actual original header/table/native
 * identity/rights/EOF and full independent member readbacks. These borrowed
 * raw bytes cannot enroll an original, seal a unit or create a certificate. */
enum slcli_unit19_kind { SLCLI_ROLE19=1,SLCLI_CONSUMER0_19,SLCLI_CONSUMER2_19,
  SLCLI_CONSUMER_CERT19,SLCLI_HA2_CERT19,SLCLI_HA2_CONTROL19,SLCLI_HA2_DERIVE19,
  SLCLI_HA2_COMPOUND19 };
struct slcli_unit19_record {
  unsigned char association[32],capture[32],digest[32];
  uint64_t offset,bytes;
  uint32_t kind,source_project;
};
struct slcli_unit19_reader {
  struct slcli_unit19_record records[15];
  uint64_t file_bytes,next_offset;
  uint64_t member_limits[15];
  uint32_t type,count,next,source_project,started,failed,complete;
};
/* Source-owned new unit framing storage only; no native identity or unit birth
 * is created here. Actual original member records are independently supplied. */
struct slcli_unit19_framing {
  unsigned char header[64],table[15][128];
  struct slcli_unit19_reader checked;
  uint32_t started,failed,complete;
};
int slcli_unit19_encode(struct slcli_unit19_framing *,enum slcli_unit19_kind,uint32_t,
  const struct slcli_unit19_record *,size_t,const uint64_t *,size_t);
/* Genuine source-designated limits for case/capacity members are supplied
 * BEFORE whole unit construction. Zero/unknown cannot mean unlimited. */
int slcli_unit19_header(struct slcli_unit19_reader *,const unsigned char[64],
  uint64_t,enum slcli_unit19_kind,uint32_t,const uint64_t *,size_t);
int slcli_unit19_record(struct slcli_unit19_reader *,const unsigned char[128]);
/* Compares a previously source/native acquired member. No digest match is a
 * substitute for that acquisition or for full native raw before/read/EOF/after. */
int slcli_unit19_member_matches(const struct slcli_unit19_reader *,uint32_t,
  const struct slcli_unit19_record *);
/* Exact selected unit26 per-member source geometry; zero means no member. */
uint64_t slcli_unit19_derive_member_limit(uint32_t);
#endif
