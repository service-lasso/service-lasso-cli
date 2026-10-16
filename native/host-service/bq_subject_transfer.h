/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_BQ_SUBJECT_TRANSFER_H
#define SLCLI_BQ_SUBJECT_TRANSFER_H
#include <stddef.h>
#include <stdint.h>
#if defined(__cplusplus)
extern "C" {
#endif

/* Private original71/72 routing ONLY. No field is an admission result, native
 * reference, principal, endpoint, credential, or remotely dereferenceable address.
 * An OFFER accompanies exactly one native rights reference. ACK/CLOSED carry
 * none; their receipt alone never establishes native closure or disposition. */
enum { SLCLI_BQ_SUBJECT_RECORD_BYTES=64, SLCLI_BQ_SUBJECT_RECORD_VERSION=1 };
enum slcli_bq_subject_message {
 SLCLI_BQ_SUBJECT_OFFER=1, SLCLI_BQ_SUBJECT_ARCHIVE_ACK=2,
 SLCLI_BQ_SUBJECT_CLOSED=3
};
enum slcli_bq_subject_cut {
 SLCLI_BQ_SUBJECT_R0=0, SLCLI_BQ_SUBJECT_R1=1, SLCLI_BQ_SUBJECT_R2=2,
 SLCLI_BQ_SUBJECT_R3A=3, SLCLI_BQ_SUBJECT_R3B=4, SLCLI_BQ_SUBJECT_R4=5,
 /* Separately finite EF52 prerequisite tools, never product actors42..49. */
 SLCLI_BQ_SUBJECT_INITIAL_PROVIDER=6
};
enum slcli_bq_subject_role {
 SLCLI_BQ_SUBJECT_SOURCE=1, SLCLI_BQ_SUBJECT_TOOL=2,
 SLCLI_BQ_SUBJECT_HEADER=3, SLCLI_BQ_SUBJECT_IMAGE=4,
 SLCLI_BQ_SUBJECT_REFERENCE=5
};
struct slcli_bq_subject_route {
 uint32_t message,cut;
 uint64_t sequence,member;
 uint32_t phase,command,requirement,role;
 uint64_t member_bytes;
};
/* Zero-based phase/command/requirement select the already-owned destination
 * roster. member is the one-based source-owned subject ordinal. File subjects
 * must separately join authenticated complete installed membership; a process
 * REFERENCE instead joins its actual original native process occurrence.
 * sequence is nonzero and source-owned one-use; no wrap/replay or retry implied.
 * Decode publishes only on full syntax success. The native destination must
 * independently join its expected route and actual birth/rights/source owner.
 * INITIAL_PROVIDER sequence1 is the process REFERENCE exception: command and
 * requirement are UINT32_MAX, phase is the actual destination phase ordinal. */
int slcli_bq_subject_route_encode(unsigned char *,size_t,
 const struct slcli_bq_subject_route *);
int slcli_bq_subject_route_decode(struct slcli_bq_subject_route *,
 const unsigned char *,size_t);
int slcli_bq_subject_route_equal(const struct slcli_bq_subject_route *,
 const struct slcli_bq_subject_route *);
/* Borrowed structural roster only, never an authenticated subject factory.
 * requirements_per_command preserves every actual reader role's occurrence;
 * subjects follows that same flattened order, but member is NEVER its index.
 * File family1..4 comes from the upstream original, not a reader-role mapping.
 * Family5 is reserved here for the independently acquired process reference.
 * Actual native callers must preown/retain these complete spans before use. */
struct slcli_bq_provider_subject_selector {
 uint64_t member,member_bytes;
 uint32_t family;
};
struct slcli_bq_provider_roster {
 uint32_t phase;
 uint64_t process_member;
 const uint32_t *requirements_per_command;
 size_t command_count;
 const struct slcli_bq_provider_subject_selector *subjects;
 size_t subject_count;
};
/* Both outputs publish only after whole roster validation; index0 is process.
 * No allocation, native call, admission, replay state or ACK closure occurs. */
/* Sole portable installed-policy codec. Geometry is independently selected
 * correspondence only, never a native extent, reserve or source grant. */
/* Sole pure resource-cost arithmetic; no instance/qualification is supplied. */
struct slcli_bq_resource_cost;
struct slcli_bq_owner_resource_profile;
int slcli_bq_owner_resource_profile_add(struct slcli_bq_owner_resource_profile *,
 const struct slcli_bq_owner_resource_profile *,const struct slcli_bq_resource_cost *);
int slcli_bq_owner_resource_profile_shape(const struct slcli_bq_owner_resource_profile *);
struct slcli_bq_retention_policy_values;
int slcli_bq_retention_policy_values_shape(const struct slcli_bq_retention_policy_values *,uint64_t,uint64_t);
int slcli_bq_retention_policy_encode(unsigned char *,size_t,const struct slcli_bq_retention_policy_values *,uint64_t,uint64_t);
int slcli_bq_retention_policy_decode(struct slcli_bq_retention_policy_values *,const unsigned char *,size_t,uint64_t,uint64_t);
int slcli_bq_provider_roster_count(size_t *,const struct slcli_bq_provider_roster *);
int slcli_bq_provider_roster_offer(struct slcli_bq_subject_route *,
 const struct slcli_bq_provider_roster *,size_t);
#if defined(__cplusplus)
}
#endif
#endif
