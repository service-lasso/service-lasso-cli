#ifndef SLCLI_WIRE_H
#define SLCLI_WIRE_H
#include <stddef.h>
#include <stdint.h>
struct slcli_cursor { const unsigned char *raw; size_t bytes, offset; uint32_t failed; };
struct slcli_slice { const unsigned char *raw; uint32_t bytes; };
int slcli_take(struct slcli_cursor *, size_t, const unsigned char **);
int slcli_u8(struct slcli_cursor *, uint8_t *);
int slcli_u32(struct slcli_cursor *, uint32_t *);
int slcli_u64(struct slcli_cursor *, uint64_t *);
int slcli_string(struct slcli_cursor *, uint32_t, int, struct slcli_slice *);
int slcli_cursor_end(struct slcli_cursor *);
int slcli_utf8(const unsigned char *, size_t, int);
void slcli_put_u32(unsigned char[4], uint32_t);
void slcli_put_u64(unsigned char[8], uint64_t);
#endif
