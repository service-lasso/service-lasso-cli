#ifndef SLCLI_JSON_H
#define SLCLI_JSON_H
#include <stddef.h>
#include <stdint.h>
/* Borrowed offsets into SAME original held bytes. Validation supplies no source
 * authority; the native owner retains and reobserves the original body/EOF. */
struct slcli_json_span { size_t start, end; };
int slcli_json_validate(const unsigned char *, size_t, size_t,
                         struct slcli_json_span *);
int slcli_json_object(const unsigned char *, size_t, struct slcli_json_span,
                       const char *const *, size_t, struct slcli_json_span *);
int slcli_json_text(const unsigned char *, size_t, struct slcli_json_span,
                     unsigned char *, size_t, int, size_t *);
int slcli_json_unsigned(const unsigned char *, size_t, struct slcli_json_span,
                         uint64_t, int, uint64_t *);
#endif
