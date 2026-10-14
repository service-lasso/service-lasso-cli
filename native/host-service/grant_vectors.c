#include "grant.h"
#define H64 "1111111111111111111111111111111111111111111111111111111111111111"
#if defined(_WIN32)
#define ROUTE "C:\\\\fixture\\\\store"
#define OWNER "S-1-5-21-1"
#define OWNER_ALIAS "S-1-5-21-\\u0031"
#else
#define ROUTE "/fixture/store"
#define OWNER "1000"
#define OWNER_ALIAS "\\u0031000"
#endif
/* Codec-only fictional associations: these values provide no actual owner,
 * source authority, store, HA2 key lease or permission to open a path. */
int slcli_grant_source_vectors(void) {
  static const unsigned char body[]="{\"schema\":\"service-lasso-native-input-grant.v1\",\"profileDigest\":\"" H64 "\",\"storePath\":\"" ROUTE "\",\"keyPath\":\"" ROUTE "\",\"retentionPath\":\"" ROUTE "\",\"clientUsers\":[\"" OWNER "\"]}";
  static const unsigned char duplicate[]="{\"schema\":\"service-lasso-native-input-grant.v1\",\"profileDigest\":\"" H64 "\",\"storePath\":\"" ROUTE "\",\"keyPath\":\"" ROUTE "\",\"retentionPath\":\"" ROUTE "\",\"clientUsers\":[\"" OWNER "\",\"" OWNER_ALIAS "\"]}";
  struct slcli_grant out;unsigned char profile[32];size_t i;
  for(i=0;i<32;++i) profile[i]=0x11;
  if(!slcli_grant_parse(body,sizeof(body)-1,profile,&out)||out.raw!=body||
     out.raw_bytes!=sizeof(body)-1||out.users!=1) return 0;
  if(slcli_grant_parse(duplicate,sizeof(duplicate)-1,profile,&out)) return 0;
  profile[0]=0x12;
  if(slcli_grant_parse(body,sizeof(body)-1,profile,&out)) return 0;
  for(i=0;i<32;++i) profile[i]=0;
  if(slcli_grant_parse(body,sizeof(body)-1,profile,&out)) return 0;
  if(slcli_grant_owner((const unsigned char *)"01000",5)) return 0;
#if defined(_WIN32)
  if(!slcli_grant_route((const unsigned char *)"C:\\fixture",10)||
     slcli_grant_route((const unsigned char *)"C:fixture",9)||
     !slcli_grant_route((const unsigned char *)"\\\\host\\share\\child",18)||
     slcli_grant_route((const unsigned char *)"\\\\host\\share",12)||
     !slcli_grant_route((const unsigned char *)"\\\\?\\C:\\child",12)||
     slcli_grant_route((const unsigned char *)"C:\\a\\..\\b",9)||
     slcli_grant_owner((const unsigned char *)"S-1-05-1",8)) return 0;
#else
  if(!slcli_grant_route((const unsigned char *)"/fixture/store",14)||
     slcli_grant_route((const unsigned char *)"fixture/store",13)||
     slcli_grant_route((const unsigned char *)"/fixture//store",15)||
     slcli_grant_route((const unsigned char *)"/fixture/../store",17)||
     !slcli_grant_owner((const unsigned char *)"0",1)||
     slcli_grant_owner((const unsigned char *)"4294967296",10)) return 0;
#endif
  return 1;
}
