#include "calibration.h"
#include "crypto.h"
/* Negative source vectors require an already-owned, charged, aligned scratch
 * object. They do not put the large evidence table on an assumed free stack
 * or establish any original profile/provider/ROOT/native-fit authority. */
int slcli_calibration_alias_vectors(struct slcli_calibration *scratch) {
  static const unsigned char raw[]="{}";
  unsigned char profile[32]; size_t i;
  struct slcli_qualification *qualification;
  if(!scratch) return 0;
  for(i=0;i<sizeof(*scratch);++i) ((unsigned char *)scratch)[i]=0xa5;
  for(i=0;i<sizeof(profile);++i) profile[i]=1;
  scratch->evidence[0].name[0]='{'; scratch->evidence[0].name[1]='}';
  if(slcli_calibration_parse(scratch->evidence[0].name,2,profile,scratch)||
     scratch->evidence[0].name[0]!='{'||scratch->evidence[0].name[1]!='}'||
     scratch->profile[0]!=0xa5) return 0;
  if(slcli_calibration_parse(raw,sizeof(raw)-1,scratch->profile,scratch)) return 0;
  for(i=0;i<32;++i) if(scratch->profile[i]!=0xa5) return 0;
  qualification=(struct slcli_qualification *)(void *)scratch;
  if(slcli_qualification_parse(raw,sizeof(raw)-1,qualification->profile,qualification)) return 0;
  for(i=0;i<32;++i) if(qualification->profile[i]!=0xa5) return 0;
  qualification->source[0]='{'; qualification->source[1]='}';
  if(slcli_qualification_parse(qualification->source,2,profile,qualification)||
     qualification->source[0]!='{'||qualification->source[1]!='}') return 0;
  if(slcli_calibration_parse(raw,sizeof(raw)-1,profile,
       (struct slcli_calibration *)(uintptr_t)(UINTPTR_MAX-sizeof(*scratch)+2))) return 0;
  return 1;
}
