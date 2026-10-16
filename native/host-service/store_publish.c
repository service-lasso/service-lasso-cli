#include "store_publish.h"
#include "store_publish_deadline.h"
#include <stddef.h>
#if defined(_WIN32) || defined(__linux__)
static int disjoint(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int leaf_name(struct slcli_store_publish *p,uint32_t leaf) {
  static const char *names[]={"generation-a","generation-b","selector","observation-","closure-","receipt-"};
  unsigned i=0;const char *name=names[leaf];
  while(name[i]){p->leaf_name[i]=name[i];i++;}
  if(leaf>=SLCLI_OBSERVATION) {
    if(p->index>=SLCLI_BINDINGS)return 0;
    if(p->index>=10)p->leaf_name[i++]='1';p->leaf_name[i++]=(char)('0'+p->index%10);
  }
  p->leaf_name[i]=0;return 1;
}
static int same_file(const slcli_input_capture *a,const slcli_input_capture *b) {
#if defined(_WIN32)
  return a->identity.VolumeSerialNumber==b->identity.VolumeSerialNumber&&
    slcli_equal(a->identity.FileId.Identifier,b->identity.FileId.Identifier,16)&&
    a->owner_sid_bytes==b->owner_sid_bytes&&slcli_equal(a->owner_sid,b->owner_sid,a->owner_sid_bytes)&&
    a->standard.EndOfFile.QuadPart==b->standard.EndOfFile.QuadPart&&
    a->attributes.FileAttributes==b->attributes.FileAttributes&&
    a->basic.LastWriteTime.QuadPart==b->basic.LastWriteTime.QuadPart;
#else
  return a->original_stat.st_dev==b->original_stat.st_dev&&a->original_stat.st_ino==b->original_stat.st_ino&&
    a->original_stat.st_mode==b->original_stat.st_mode&&a->original_stat.st_uid==b->original_stat.st_uid&&
    a->original_stat.st_gid==b->original_stat.st_gid&&a->original_stat.st_size==b->original_stat.st_size&&
    a->original_stat.st_nlink==b->original_stat.st_nlink&&
    a->original_stat.st_mtime==b->original_stat.st_mtime&&
    a->original_stat.st_mtime_nsec==b->original_stat.st_mtime_nsec;
#endif
}
static int observe_original(struct slcli_store_publish *p,slcli_input_handle h,
  slcli_input_capture *c,uint32_t kind) {
  int ok;
  if(kind<1||kind>8||!slcli_store_publish_deadline_before(p,kind))return 0;
  p->effect_called[kind-1u]=1;
#if defined(_WIN32)
  ok=slcli_windows_file_observe(h,c);
#else
  ok=slcli_linux_file_observe(h,c);
#endif
  p->effect_returned[kind-1u]=1;return slcli_store_publish_deadline_after(p,ok);
}
#if defined(__linux__)
#include "native_syscall_linux.h"
#include <linux/fcntl.h>
static int effect(struct slcli_store_publish *p,unsigned i,long syscall,
  long a,long b,long c,long d,long e,long f,long *returned) {
  static const uint32_t kinds[5]={4,5,6,7,2};long result;int ok;
  if(i>=5||!returned||!slcli_store_publish_deadline_before(p,kinds[i]))return 0;
  p->args[i][0]=a;p->args[i][1]=b;p->args[i][2]=c;
  p->args[i][3]=d;p->args[i][4]=e;p->args[i][5]=f;
  p->effect_called[kinds[i]-1u]=1;
  result=slcli_kernel_call(syscall,a,b,c,d,e,f);
  p->effect_returned[kinds[i]-1u]=1;p->status[i]=(uint64_t)result;
  p->observed[i]=1;*returned=result;
  ok=(i==3||i==4)?result>=0&&result<=INT32_MAX:result==0;
  /* Born outputs remain attached BEFORE fallible sink/post-clock diagnostics. */
  if(ok&&i==3)p->reopened=(int)result;
  if(ok&&i==4)p->temporary_route_handle=(int)result;
  return slcli_store_publish_deadline_after(p,ok);
}
#endif
int slcli_store_publish_original(struct slcli_store_publish *p,
                                  const struct slcli_store_stream *s,slcli_input_handle root,
                                  const unsigned char nonce[16],uint32_t leaf) {
  static const char prefix[]=".transaction-";
  static const char hex[]="0123456789abcdef";unsigned i;const char *name;
  if(!p||!s||!nonce||p->started||p->failed||leaf>SLCLI_RECEIPT||
     !s->attempted||s->mode||s->failed||!s->held_verified||
     !disjoint(p,sizeof(*p),s,sizeof(*s))||
     !disjoint(p,sizeof(*p),s->captures,s->capture_capacity*sizeof(*s->captures))||
     !disjoint(p,sizeof(*p),nonce,16))return 0;
#if defined(_WIN32)
  if(!root||root==INVALID_HANDLE_VALUE)return 0;
#else
  if(root<0)return 0;
#endif
  p->started=1;p->failed=1;p->root=root;p->temporary=s->original;p->leaf=leaf;
  for(i=0;i<13;i++)p->temporary_name[i]=prefix[i];
  for(i=0;i<16;i++){p->temporary_name[13+2*i]=hex[nonce[i]>>4];p->temporary_name[14+2*i]=hex[nonce[i]&15];}
  p->temporary_name[45]=0;if(!leaf_name(p,leaf))goto failed;name=p->leaf_name;
#if defined(_WIN32)
  if(!observe_original(p,p->temporary,&p->before,1))goto failed;
  /* Actual native FILE_RENAME_INFORMATION layout/offset includes the retained
   * original containing-directory handle and full UTF16 leaf bytes. */
  {
    struct rename_information { BOOLEAN replace; HANDLE root; ULONG bytes; WCHAR name[1]; };
    struct rename_information *r=(void *)p->rename_raw;
    size_t offset=offsetof(struct rename_information,name),n=0;
    while(name[n])n++;
    if(offset+2*n>sizeof(p->rename_raw))goto failed;
    r->replace=1;r->root=root;r->bytes=(ULONG)(2*n);
    for(i=0;i<n;i++){r->name[i]=(WCHAR)(unsigned char)name[i];p->name_raw[i]=r->name[i];}
    p->rename_bytes=(ULONG)(offset+2*n);p->retained_native_io=1;
    if(!slcli_store_publish_deadline_before(p,2))goto failed;
    p->effect_called[1]=1;
    p->rename_status=NtSetInformationFile(p->temporary,&p->rename_io,p->rename_raw,
      p->rename_bytes,(FILE_INFORMATION_CLASS)10);
    p->effect_returned[1]=1;
    if(!slcli_store_publish_deadline_after(p,p->rename_status==0&&p->rename_io.Status==0))goto failed;
    p->retained_native_io=0;
    if(!slcli_store_publish_deadline_before(p,3))goto failed;
    p->effect_called[2]=1;
    p->root_flush=FlushFileBuffers(root);p->effect_returned[2]=1;
    if(!p->root_flush)p->root_error=GetLastError();
    if(!slcli_store_publish_deadline_after(p,p->root_flush!=0))goto failed;
    p->name.Length=(USHORT)(2*n);p->name.MaximumLength=p->name.Length;p->name.Buffer=p->name_raw;
    p->attributes.Length=sizeof(p->attributes);p->attributes.RootDirectory=root;
    p->attributes.ObjectName=&p->name;p->attributes.Attributes=0x40|0x1000;
    p->retained_native_io=1;
    if(!slcli_store_publish_deadline_before(p,4))goto failed;
    p->effect_called[3]=1;
    p->open_status=NtCreateFile(&p->reopened,0x00120081,&p->attributes,&p->open_io,
      0,0,1,1,0x20|0x200000,0,0);
    p->effect_returned[3]=1;
    if(!slcli_store_publish_deadline_after(p,p->open_status==0&&p->open_io.Status==0&&
       p->reopened&&p->reopened!=INVALID_HANDLE_VALUE))goto failed;
    p->retained_native_io=0;
  }
  if(!observe_original(p,p->reopened,&p->after,5))goto failed;
#else
  if(!observe_original(p,p->temporary,&p->before,1))goto failed;
  /* Bind the actual old directory leaf to the SAME held exclusive temporary
   * before rename, rather than treating nonce spelling as native identity. */
  {
    long fd;
    if(!effect(p,4,__NR_openat,root,(long)p->temporary_name,
      O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC,0,0,0,&fd))goto failed;
    if(fd<0||fd>INT32_MAX)goto failed;p->temporary_route_handle=(int)fd;
    if(!observe_original(p,p->temporary_route_handle,&p->temporary_route,3)||
       !same_file(&p->before,&p->temporary_route))goto failed;
  }
  {long raw;if(!effect(p,0,__NR_fsync,root,0,0,0,0,0,&raw))goto failed;}
  {long raw;if(!effect(p,1,__NR_renameat,root,(long)p->temporary_name,root,(long)name,0,0,&raw))goto failed;}
  {long raw;if(!effect(p,2,__NR_fsync,root,0,0,0,0,0,&raw))goto failed;}
  {
    long fd;
    if(!effect(p,3,__NR_openat,root,(long)name,O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC,0,0,0,&fd))goto failed;
    if(fd<0||fd>INT32_MAX)goto failed;p->reopened=(int)fd;
  }
  if(!observe_original(p,p->reopened,&p->after,8))goto failed;
#endif
  if(!same_file(&p->before,&p->after)||!slcli_store_publish_deadline_ready(p))goto failed;
  p->completed=1;p->failed=0;return 1;
failed:
  slcli_store_publish_deadline_fail(p);return 0;
}
#endif
