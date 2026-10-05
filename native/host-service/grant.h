#ifndef SLCLI_GRANT_H
#define SLCLI_GRANT_H
#include "json.h"
/* Exact PA1 private grant claims. Paths are routing only, not source/store/key
 * authority. HA2 still requires its independent original provider/key lease. */
struct slcli_grant {
  const unsigned char *raw;
  size_t raw_bytes,users;
  unsigned char profile[32],digest[32];
  struct slcli_json_span routes[3],client_users[128];
};
int slcli_grant_route(const unsigned char *,size_t);
int slcli_grant_owner(const unsigned char *,size_t);
int slcli_grant_parse(const unsigned char *,size_t,const unsigned char[32],
                       struct slcli_grant *);
#endif
