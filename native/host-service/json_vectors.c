#include "json.h"
#define CHECK(raw,expected) do { if(slcli_json_validate((const unsigned char *)(raw),sizeof(raw)-1,16384,&root)!=(expected)) return 0; } while(0)
/* Source-only semantic regressions. They execute neither original input
 * admission nor native observation; invocation requires the whole source gate. */
int slcli_json_source_vectors(void) {
  struct slcli_json_span root,members[2];
  static const char *const keys[]={"name","count"};
  static const unsigned char body[]="{\"name\":\"\\uD83D\\uDE00\",\"count\":9007199254740991}";
  unsigned char decoded[4]; size_t written; uint64_t count;
  CHECK("{\"a\":1,\"\\u0061\":2}",0);
  CHECK("{\"a\":1,\"nested\":{\"a\":2}}",1);
  CHECK("{\"a\":1,}",0);
  CHECK("[1,]",0);
  CHECK("\"\\uD800\"",0);
  CHECK("\"\\uDC00\"",0);
  CHECK("\"\\uD83D\\uDE00\"",1);
  CHECK("\"\xc0\x80\"",0);
  CHECK("01",0);
  CHECK("true false",0);
  CHECK("[[[[[[[[[[[[[[[[[0]]]]]]]]]]]]]]]]]",0);
  if(!slcli_json_validate(body,sizeof(body)-1,16384,&root)||
     !slcli_json_object(body,sizeof(body)-1,root,keys,2,members)||
     !slcli_json_text(body,sizeof(body)-1,members[0],decoded,sizeof(decoded),0,&written)||
     written!=4||decoded[0]!=0xf0||decoded[1]!=0x9f||decoded[2]!=0x98||decoded[3]!=0x80||
     !slcli_json_unsigned(body,sizeof(body)-1,members[1],UINT64_C(9007199254740991),0,&count)||
     count!=UINT64_C(9007199254740991)) return 0;
  if(slcli_json_unsigned(body,sizeof(body)-1,members[1],UINT64_C(9007199254740990),0,&count)) return 0;
  return 1;
}
