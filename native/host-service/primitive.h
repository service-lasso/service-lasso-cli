#ifndef SLCLI_PRIMITIVE_H
#define SLCLI_PRIMITIVE_H
#include <stddef.h>
#include <stdint.h>
/* Source-local selectors encode only the unchanged closed eleven-key grammar.
 * They grant no actor, identity, native observation or admission authority. */
enum slcli_primitive_kind { SLCLI_BIRTH, SLCLI_IDENTITY, SLCLI_READ, SLCLI_WRITE,
  SLCLI_EOF, SLCLI_DISCOVERY, SLCLI_LEDGER, SLCLI_FLUSH, SLCLI_EXIT, SLCLI_CLOSE };
enum slcli_primitive_operation {
  SLCLI_LINUX_EXECVEAT, SLCLI_LINUX_PIDFD_OPEN, SLCLI_WIN_CREATE_PROCESS,
  SLCLI_WIN_PROCESS_BIRTH, SLCLI_LINUX_PEERCRED, SLCLI_LINUX_FSTAT,
  SLCLI_LINUX_RENAME, SLCLI_WIN_TOKEN_USER, SLCLI_WIN_PIPE_PEER,
  SLCLI_WIN_FILE_IDENTITY, SLCLI_WIN_SECURITY, SLCLI_WIN_RENAME,
  SLCLI_LINUX_OPEN_READ, SLCLI_LINUX_READ, SLCLI_WIN_OPEN_READ, SLCLI_WIN_READ,
  SLCLI_LINUX_WRITE, SLCLI_WIN_WRITE, SLCLI_LINUX_FSYNC, SLCLI_WIN_FLUSH,
  SLCLI_LINUX_GETDENTS, SLCLI_WIN_DIRECTORY, SLCLI_LINUX_WAITID,
  SLCLI_WIN_PROCESS_WAIT, SLCLI_LINUX_PIDFD_POLL, SLCLI_LINUX_CLOSE,
  SLCLI_WIN_CLOSE, SLCLI_LEDGER_WRITE, SLCLI_LEDGER_READBACK,
  SLCLI_SELECTOR_COMMIT, SLCLI_SELECTOR_READBACK
};
struct slcli_primitive {
  uint32_t kind, operation, outcome, mode;
  unsigned char identity[32], related[32], source[32];
  uint64_t tick;
  /* NULL means unobserved. A real zero-byte capture has a NONNULL original
   * native pointer and zero bytes; it encodes the actual empty SHA/count. */
  const unsigned char *data;
  size_t bytes;
};
size_t slcli_primitive_into(unsigned char *, size_t, const struct slcli_primitive *);
#endif
