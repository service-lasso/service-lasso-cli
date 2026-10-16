#ifndef SLCLI_NODE_CAPTURE_LAYOUT_WINDOWS_H
#define SLCLI_NODE_CAPTURE_LAYOUT_WINDOWS_H
#include "../host-service/source_deadline.h"
#if defined(_WIN32)
#include "bootstrap.h"
#include "loader_windows.h"
/* Closed actual source stages, never a caller-provided allocation recipe.
 * Receive already contains its original decode and parent/token capture. */
enum slcli_node_capture_stage {
 SLCLI_CAPTURE_ENTRY,SLCLI_CAPTURE_RECEIVE,SLCLI_CAPTURE_PREFIX,SLCLI_CAPTURE_REGIONS,
 SLCLI_CAPTURE_LOADER,SLCLI_CAPTURE_MODULES,SLCLI_CAPTURE_NAMES,
 SLCLI_CAPTURE_BOOTSTRAP,SLCLI_CAPTURE_BOOTSTRAP_CALLER,
 SLCLI_CAPTURE_BOOTSTRAP_ORIGIN,SLCLI_CAPTURE_BOOTSTRAP_PREDECESSOR,
 SLCLI_CAPTURE_BOOTSTRAP_DEADLINE,SLCLI_CAPTURE_RUNTIME_REQUEST,
 SLCLI_CAPTURE_CREATION,SLCLI_CAPTURE_PREENTRY,SLCLI_CAPTURE_RUNTIME_RECEIVER,
 SLCLI_CAPTURE_MAIN_CALLER,SLCLI_CAPTURE_MAIN_ORIGIN,SLCLI_CAPTURE_MAIN_DEADLINE,
 SLCLI_CAPTURE_MAIN_ALLOCATOR,SLCLI_CAPTURE_MAIN_EVENTS,
 SLCLI_CAPTURE_THREAD_SCHEDULE,SLCLI_CAPTURE_THREAD_SLOTS,
 SLCLI_CAPTURE_STAGE_COUNT
};
struct slcli_node_capture_block { size_t offset,requested,charge; };
struct slcli_node_capture_layout_windows {
 const struct slcli_source_deadline_original *deadline;
 const struct slcli_native_clock *origin,*predecessor;
 const void *caller;size_t caller_bytes;
 uint64_t origin_tick,expires,predecessor_tick;
 struct slcli_native_clock before,after;
 SYSTEM_INFO geometry;
 uint64_t geometry_args[1];
 uint32_t started,geometry_called,geometry_returned,ready;
 size_t threads,events,regions,modules,name_units,birth_bytes;
 struct slcli_node_capture_block stages[SLCLI_CAPTURE_STAGE_COUNT];
 size_t thread_begin,thread_stride,thread_caller_delta,thread_events_delta;
 size_t typed_bytes,padding_offset,padding_requested,capture_bytes,minimum_runtime_bytes;
};
/* This computes actual typed placement under the original deadline. It does
 * not select numeric profile bounds, initialize headers, grant source authority
 * or claim complete TLS/native-object coverage or fit. The installed containing
 * constructor must join its complete genuine profile before birth/import. */
int slcli_node_capture_layout_windows_prepare_original(
 struct slcli_node_capture_layout_windows *,struct slcli_arena *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,size_t,size_t,size_t,size_t,size_t);
struct slcli_capture_alias_windows;
struct slcli_node_route_windows;
/* Pure closed-type derivation before routed pointer/header access. Geometry is
 * observational until independently joined with actual child GetSystemInfo. */
int slcli_node_capture_layout_windows_route_shape_original(
 const struct slcli_node_route_windows *,struct slcli_node_capture_layout_windows *);
int slcli_node_capture_layout_windows_route_headers_original(
 const struct slcli_node_route_windows *,struct slcli_node_capture_layout_windows *);
/* Header geometry only after capture payloads have real effects. Never scan
 * for zero or overwrite those original bytes during prepared-tail adoption. */
int slcli_node_capture_layout_windows_live_headers_original(
 const struct slcli_node_route_windows *,struct slcli_node_capture_layout_windows *);
/* The single owning parent writer, before birth. Header/payload offsets are
 * regenerated from closed source types; an incoming list is never accepted.
 * Child-stage promotion must independently validate exact source originals. */
int slcli_node_capture_layout_windows_initialize_original(
 const struct slcli_node_capture_layout_windows *,struct slcli_arena *,
 struct slcli_capture_alias_windows *);
/* Read back the entire readonly prefix against regenerated closed headers and
 * untouched zero payload before birth. No child/source authority is inferred. */
int slcli_node_capture_layout_windows_validate_original(
 const struct slcli_node_capture_layout_windows *,struct slcli_arena *,
 struct slcli_capture_alias_windows *);
#endif
#endif
