#include "primitive.h"
#include "crypto.h"
/* Unexecuted source vectors for the original private grammar, not synthetic
 * native acceptance or a provider/profile admission fixture. */
static int contains(const unsigned char *raw, size_t bytes, const char *value) {
  size_t i,j;
  for(i=0;i<bytes;++i) {
    for(j=0;value[j]&&i+j<bytes&&raw[i+j]==(unsigned char)value[j];++j) {}
    if(!value[j]) return 1;
  }
  return 0;
}
int slcli_primitive_source_vectors(void) {
  struct slcli_primitive p;
  unsigned char raw[2048], actual_empty=0;
  size_t i,n;
  slcli_erase(&p,sizeof(p));
  for(i=0;i<32;++i) { p.identity[i]=1; p.related[i]=2; p.source[i]=3; }
  p.tick=1; p.kind=SLCLI_EOF;
#if defined(__linux__)
  p.operation=SLCLI_LINUX_READ;
#elif defined(_WIN32)
  p.operation=SLCLI_WIN_READ;
#else
  return 0;
#endif
  p.data=&actual_empty;
  n=slcli_primitive_into(raw,sizeof(raw),&p);
  if(!n||raw[n-1]!='\n'||
     !contains(raw,n,"\"bytesSha256\":\"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855\"")||
     !contains(raw,n,"\"byteCount\":0,\"mode\":null")) return 0;
  p.kind=SLCLI_READ; p.outcome=1; p.data=0;
  n=slcli_primitive_into(raw,sizeof(raw),&p);
  if(!n||!contains(raw,n,"\"outcome\":\"failure\",\"bytesSha256\":null,\"byteCount\":null")) return 0;
  p.outcome=0;
  if(slcli_primitive_into(raw,sizeof(raw),&p)) return 0;
  p.outcome=2; p.kind=SLCLI_EOF;
  if(slcli_primitive_into(raw,sizeof(raw),&p)) return 0;
  p.kind=SLCLI_READ; p.data=raw; p.bytes=1;
  if(slcli_primitive_into(raw,sizeof(raw),&p)) return 0;
  p.data=&actual_empty; p.bytes=0; p.mode=1;
  if(slcli_primitive_into(raw,sizeof(raw),&p)) return 0;
  p.mode=0; p.kind=SLCLI_WRITE;
  /* A read operation cannot be renamed into a write observation. */
  if(slcli_primitive_into(raw,sizeof(raw),&p)) return 0;
  return 1;
}
