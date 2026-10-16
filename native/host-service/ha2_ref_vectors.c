#include "ha2_ref_vectors.h"
static int row(unsigned slot,int accepted) {
  static const char prefix[]="{\"slot\":";
  static const char suffix[]=",\"associationId\":\"1111111111111111111111111111111111111111111111111111111111111111\",\"nativeCaptureAssociationId\":\"2222222222222222222222222222222222222222222222222222222222222222\",\"offset\":\"0\",\"bytes\":\"64\",\"rawSha256\":\"3333333333333333333333333333333333333333333333333333333333333333\",\"sourceAssociationId\":\"4444444444444444444444444444444444444444444444444444444444444444\",\"callerAssociationId\":\"5555555555555555555555555555555555555555555555555555555555555555\",\"birthAssociationId\":\"6666666666666666666666666666666666666666666666666666666666666666\"}";
  unsigned char raw[768],saved[sizeof(struct slcli_ha2_ref)];
  struct slcli_ha2_ref result;struct slcli_json_span root;size_t n=0,i;int ok;
  for(i=0;i<sizeof(result);i++)((unsigned char *)&result)[i]=saved[i]=0xa5;
  for(i=0;i<sizeof(prefix)-1;i++)raw[n++]=(unsigned char)prefix[i];
  if(slot>=100)raw[n++]=(unsigned char)('0'+slot/100);
  if(slot>=10)raw[n++]=(unsigned char)('0'+(slot/10)%10);
  raw[n++]=(unsigned char)('0'+slot%10);
  for(i=0;i<sizeof(suffix)-1;i++)raw[n++]=(unsigned char)suffix[i];
  if(!slcli_json_validate(raw,n,sizeof(raw),&root))return 0;
  ok=slcli_ha2_ref_parse(raw,n,root,&result);
  if(accepted)return ok&&result.slot==slot&&result.offset==0&&result.bytes==64&&
    result.association[0]==0x11&&result.capture[0]==0x22&&result.digest[0]==0x33&&
    result.source[0]==0x44&&result.caller[0]==0x55&&result.birth[0]==0x66;
  if(ok)return 0;
  for(i=0;i<sizeof(result);i++)if(((unsigned char *)&result)[i]!=saved[i])return 0;
  return 1;
}
int slcli_ha2_ref_slot_boundary_vectors(void) {
  return row(1,1)&&row(128,1)&&row(0,0)&&row(129,0);
}