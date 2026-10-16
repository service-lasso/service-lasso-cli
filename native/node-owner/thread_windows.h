#ifndef SLCLI_NODE_THREAD_WINDOWS_H
#define SLCLI_NODE_THREAD_WINDOWS_H
#include <stddef.h>
#if defined(_WIN32)
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif
struct slcli_node_thread_birth;
/* Exact compiled source type size for the preplanned capture schedule. */
size_t slcli_node_thread_birth_bytes_original(void);
/* Private actual CRT source constructor only. Return exposes retained custody
 * even when native birth succeeded but later diagnostics failed. A non-NULL
 * actual HANDLE must keep the original parameter/module/storage alive. These
 * bodies supply no principal, native source authority, stack fit or FREE cut. */
struct slcli_node_thread_birth *slcli_node_thread_create_original(
  LPSECURITY_ATTRIBUTES,SIZE_T,LPTHREAD_START_ROUTINE,void *,size_t,void *,DWORD);
HANDLE slcli_node_thread_handle_original(const struct slcli_node_thread_birth *);
DWORD slcli_node_thread_id_original(const struct slcli_node_thread_birth *);
DWORD slcli_node_thread_error_original(const struct slcli_node_thread_birth *);
DWORD slcli_node_thread_resume_error_original(const struct slcli_node_thread_birth *);
int slcli_node_thread_birth_ready_original(const struct slcli_node_thread_birth *);
int slcli_node_thread_resume_original(struct slcli_node_thread_birth *);
/* SAME source caller brackets its actual CRT creation call. These observations
 * retain born effects even after a diagnostic closes future effects. They
 * neither search ambient handles nor authenticate an actor from a pointer. */
const struct slcli_node_thread_birth *slcli_node_thread_checkpoint_original(void);
struct slcli_node_thread_birth *slcli_node_thread_birth_after_original(
  const struct slcli_node_thread_birth *,const void *,HANDLE);
int slcli_node_thread_resume_suspended_original(struct slcli_node_thread_birth *);
/* Actual libuv source lifetime boundary. native_failure distinguishes captured
 * native errors from original-custody/deadline unavailability. A real close
 * clears the caller's HANDLE before diagnostics; it grants no FREE/archive cut. */
struct slcli_node_thread_completion {
  int complete,native_failure;
  DWORD native_error;
};
struct slcli_node_thread_completion slcli_node_thread_join_original(HANDLE *);
struct slcli_node_thread_completion slcli_node_thread_detach_original(HANDLE *);
/* Once-only observation through the distinct retained observer alias. It
 * neither closes that alias nor settles stack/TLS/module/capture borrowers. */
struct slcli_node_thread_completion slcli_node_thread_observer_exit_original(
 struct slcli_node_thread_birth *);
/* Actual original CRT nonreturning terminal call. SAME caller rows retain
 * module/code/attempt, never a fabricated native return or extent retirement.
 * Genuine independent native exit/EOF/archive remains required. */
void slcli_node_thread_exit_original(HMODULE,DWORD);
#ifdef __cplusplus
}
#endif
#endif
#endif
