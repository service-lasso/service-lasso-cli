#include "installer_linux.h"
#include <string.h>
#include <limits.h>
#if !defined(__linux__) || !defined(__x86_64__) || !defined(__BYTE_ORDER__) || __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error initial_installer_requires_selected_linux_x86_64_little_endian_ABI
#endif
_Static_assert(sizeof(uintptr_t)==8 && sizeof(size_t)==8, "selected initial stack word ABI");
static int span(uintptr_t p,size_t n,uintptr_t lo,uintptr_t hi) {
  return p>=lo&&p<=hi&&n<=hi-p;
}
static int text(uintptr_t p,uintptr_t lo,uintptr_t hi) {
  if(!p||!span(p,1,lo,hi))return 0;
  while(p<hi){if(*(const unsigned char *)p==0)return 1;++p;}return 0;
}
static int word(uintptr_t *cursor,uintptr_t lo,uintptr_t hi,uintptr_t *value) {
  if((*cursor%_Alignof(uintptr_t))||!span(*cursor,sizeof(uintptr_t),lo,hi))return 0;
  memcpy(value,(const void *)*cursor,sizeof(*value));*cursor+=sizeof(uintptr_t);return 1;
}
int slcli_initial_stack_linux_decode_original(uintptr_t sp,const void *begin,size_t bytes,
  struct slcli_initial_stack_linux_original *out) {
  uintptr_t lo=(uintptr_t)begin,hi,op=(uintptr_t)out,p=sp,v,key;size_t i;
  uint32_t seen=0;struct slcli_initial_stack_linux_original t;
  if(!begin||!bytes||!out||bytes>UINTPTR_MAX-lo||sizeof(*out)>UINTPTR_MAX-op)return 0;
  hi=lo+bytes;if(!(op+sizeof(*out)<=lo||hi<=op))return 0;
  memset(&t,0,sizeof(t));t.stack_start=sp;t.extent_start=lo;t.extent_bytes=bytes;
  if(!word(&p,lo,hi,&v)||v>SIZE_MAX)return 0;t.argc=(size_t)v;
  if(t.argc>(hi-p)/sizeof(uintptr_t))return 0;t.argv_original=(const uintptr_t *)p;
  for(i=0;i<t.argc;++i)if(!word(&p,lo,hi,&v)||!text(v,lo,hi))return 0;
  if(!word(&p,lo,hi,&v)||v)return 0;t.environment_original=(const uintptr_t *)p;
  for(;;){if(!word(&p,lo,hi,&v))return 0;if(!v)break;
    if(!text(v,lo,hi)||t.environment_count==SIZE_MAX)return 0;++t.environment_count;}
  t.auxiliary_original=(const uintptr_t *)p;
  for(;;){uint32_t bit=0;uintptr_t *field=0;
    if(!word(&p,lo,hi,&key)||!word(&p,lo,hi,&v))return 0;
    if(key==0){if(v)return 0;break;}
    if(t.auxiliary_count==SIZE_MAX)return 0;++t.auxiliary_count;
    switch(key){
      case 3:bit=1u;field=&t.phdr;break;case 4:bit=2u;field=&t.phent;break;
      case 5:bit=4u;field=&t.phnum;break;case 9:bit=8u;field=&t.entry;break;
      case 7:bit=16u;field=&t.base;break;case 6:bit=32u;field=&t.page_bytes;break;
      case 31:bit=64u;field=&t.execfn;break;case 25:bit=128u;field=&t.random;break;
      default:break;
    }
    if(bit){if(seen&bit)return 0;seen|=bit;*field=v;}
  }
  if(seen!=255u||!t.phdr||!t.phent||!t.phnum||!t.entry||!t.page_bytes||
    (t.page_bytes&(t.page_bytes-1))||!text(t.execfn,lo,hi)||!span(t.random,16,lo,hi))return 0;
  /* ELF pointers are retained without dereference: separate acquired mapping
   * originals must qualify PHDR/ENTRY/BASE before any loaded-image access. */
  *out=t;return 1;
}