#include "json.h"
int slcli_json_alias_vectors(void) {
  union { unsigned char raw[128]; uint64_t integer; struct slcli_json_span span; } storage;
  static const unsigned char body[]="{\"a\":1}";
  static const char *const keys[]={"a"};
  struct slcli_json_span root={0,sizeof(body)-1},key={1,4},number={5,6},out;
  size_t i,count;
  for(i=0;i<sizeof(storage.raw);++i) storage.raw[i]=0xa5;
  for(i=0;i<sizeof(body)-1;++i) storage.raw[i]=body[i];
  if(slcli_json_validate(storage.raw,sizeof(body)-1,16384,
       (struct slcli_json_span *)(void *)storage.raw)||
     slcli_json_object(storage.raw,sizeof(body)-1,root,keys,1,
       (struct slcli_json_span *)(void *)storage.raw)||
     slcli_json_unsigned(storage.raw,sizeof(body)-1,number,1,0,
       (uint64_t *)(void *)storage.raw)||
     slcli_json_text(storage.raw,sizeof(body)-1,key,storage.raw+20,8,0,
       (size_t *)(void *)storage.raw)||
     slcli_json_text(storage.raw,sizeof(body)-1,key,storage.raw+2,4,0,&count)||
     slcli_json_validate(storage.raw,sizeof(body)-1,16384,
       (struct slcli_json_span *)(uintptr_t)(UINTPTR_MAX-sizeof(out)+2))) return 0;
  for(i=0;i<sizeof(body)-1;++i) if(storage.raw[i]!=body[i]) return 0;
  storage.raw[0]='[';storage.raw[1]='1';storage.raw[2]=']';root.start=0;root.end=3;
  if(slcli_json_array(storage.raw,3,root,&out,1,(size_t *)(void *)storage.raw)||
     storage.raw[0]!='['||storage.raw[1]!='1'||storage.raw[2]!=']') return 0;
  return 1;
}
#define CHECK(raw,expected) do { if(slcli_json_validate((const unsigned char *)(raw),sizeof(raw)-1,16384,&root)!=(expected)) return 0; } while(0)
/* Source-only semantic regressions. They execute neither original input
 * admission nor native observation; invocation requires the whole source gate. */
int slcli_json_source_vectors(void) {
  struct slcli_json_span root,members[2];
  static const char *const keys[]={"name","count"};
  static const unsigned char body[]="{\"name\":\"\\uD83D\\uDE00\",\"count\":9007199254740991}";
  unsigned char decoded[4]; size_t written; uint64_t count;
  if(!slcli_json_alias_vectors()) return 0;
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
