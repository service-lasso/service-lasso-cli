#include "grant.h"
#include "crypto.h"
#include "wire.h"
static int disjoint(const void *left,size_t n,const void *right,size_t m) {
  uintptr_t a=(uintptr_t)left,b=(uintptr_t)right;
  return left&&right&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
static int separator(unsigned char b) {
#if defined(_WIN32)
  return b=='/'||b=='\\';
#else
  return b=='/';
#endif
}
#if defined(_WIN32)
/* Copyright 2010 The Go Authors. All rights reserved. Windows volume logic is
 * adapted under GO-ROUTE-LICENSE.txt. The classifier preserves sourceGrantRoute's
 * filepath.IsAbs/VolumeName semantics, including device and UNC volumes.
 * Original Go filepathlite source/license is held in the author custody audit;
 * this classifier does not select Go runtime or grant pathname authority. */
static unsigned char upper(unsigned char b) {return b>='a'&&b<='z'?b-'a'+'A':b;}
static int prefix(const unsigned char *path,size_t n,const char *value,size_t m) {
  size_t i;if(n<m) return 0;
  for(i=0;i<m;++i) if(separator((unsigned char)value[i])?!separator(path[i]):
       upper(path[i])!=upper((unsigned char)value[i])) return 0;
  return n==m||separator(path[m]);
}
static size_t unc(const unsigned char *path,size_t n,size_t start) {
  size_t i,count=0;for(i=start;i<n;++i) if(separator(path[i])&&++count==2) return i;
  return n;
}
static size_t volume(const unsigned char *path,size_t n) {
  size_t i;
  if(n>=2&&path[1]==':') return 2;
  if(!n||!separator(path[0])) return 0;
  if(prefix(path,n,"\\\\.\\UNC",7)) return unc(path,n,8);
  if(prefix(path,n,"\\\\.",3)||prefix(path,n,"\\\\?",3)||prefix(path,n,"\\??",3)) {
    if(n==3) return 3;
    for(i=4;i<n;++i) if(separator(path[i])) return i;
    return n;
  }
  if(n>=2&&separator(path[1])) return unc(path,n,2);
  return 0;
}
#endif
int slcli_grant_route(const unsigned char *path,size_t n) {
  size_t i,start,base;
  if(!path||!n||n>4096||!slcli_utf8(path,n,1)) return 0;
  for(i=0;i<n;++i) if(!path[i]) return 0;
#if defined(_WIN32)
  base=volume(path,n);
  if(!base||base>=n||!separator(path[base])) return 0;
#elif defined(__linux__)
  if(path[0]!='/') return 0;base=0;
#else
  return 0;
#endif
  if(n==base+1) return 1;
  start=base+1;
  for(i=start;i<=n;++i) {
    if(i<n&&!separator(path[i])) continue;
    if(i==start||(i-start==1&&path[start]=='.')||
       (i-start==2&&path[start]=='.'&&path[start+1]=='.')) return 0;
    start=i+1;
  }
  return 1;
}
int slcli_grant_owner(const unsigned char *owner,size_t n) {
  size_t start,i,components=0;uint64_t maximum,value,digit;
  if(!owner||!n||n>184) return 0;
#if defined(_WIN32)
  if(n<5||owner[0]!='S'||owner[1]!='-'||owner[2]!='1'||owner[3]!='-') return 0;
  start=4;
#elif defined(__linux__)
  start=0;
#else
  return 0;
#endif
  value=0;maximum=UINT32_MAX;
#if defined(_WIN32)
  maximum=(UINT64_C(1)<<48)-1;
#endif
  for(i=start;i<=n;++i) {
#if defined(_WIN32)
    if(i==n||owner[i]=='-') {
      if(i==start||(i-start>1&&owner[start]=='0')||++components>16) return 0;
      start=i+1;value=0;maximum=UINT32_MAX;continue;
    }
#else
    if(i==n) break;
#endif
    if(owner[i]<'0'||owner[i]>'9') return 0;
    digit=owner[i]-'0';if(value>(maximum-digit)/10) return 0;
    value=value*10+digit;
  }
#if defined(__linux__)
  (void)components;return n==1||owner[0]!='0';
#else
  return components>=1;
#endif
}
static int decode(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
                    unsigned char *out,size_t cap,size_t *count,int controls) {
  return slcli_json_text(raw,bytes,span,out,cap,controls,count)&&*count;
}
int slcli_grant_parse(const unsigned char *raw,size_t bytes,
                       const unsigned char profile[32],struct slcli_grant *out) {
  static const char *const keys[]={"schema","profileDigest","storePath","keyPath","retentionPath","clientUsers"};
  static const unsigned char schema[]="service-lasso-native-input-grant.v1";
  struct slcli_json_span root,fields[6]; struct slcli_sha256 hash;
  unsigned char decoded[4096],left[184],right[184];
  size_t count,i,j,n,m;uint32_t high,low;unsigned char any=0;
  if(!disjoint(raw,bytes,out,sizeof(*out))||!disjoint(profile,32,out,sizeof(*out))) return 0;
  for(i=0;i<32;++i) any|=profile[i];
  if(!any) return 0;
  slcli_erase(out,sizeof(*out));
  if(!slcli_json_validate(raw,bytes,16384,&root)||
     !slcli_json_object(raw,bytes,root,keys,6,fields)||
     !decode(raw,bytes,fields[0],decoded,sizeof(decoded),&count,0)||
     count!=sizeof(schema)-1||!slcli_equal(decoded,schema,count)||
     !decode(raw,bytes,fields[1],decoded,64,&count,0)||count!=64) return 0;
  for(i=0;i<32;++i) {
    unsigned char a=decoded[i*2],b=decoded[i*2+1];
    if(a>='0'&&a<='9') high=a-'0';else if(a>='a'&&a<='f') high=a-'a'+10;else return 0;
    if(b>='0'&&b<='9') low=b-'0';else if(b>='a'&&b<='f') low=b-'a'+10;else return 0;
    out->profile[i]=(unsigned char)((high<<4)|low);
  }
  if(!slcli_equal(profile,out->profile,32)) return 0;
  for(i=0;i<3;++i) {
    if(!decode(raw,bytes,fields[i+2],decoded,sizeof(decoded),&count,1)||
       !slcli_grant_route(decoded,count)) return 0;
    out->routes[i]=fields[i+2];
  }
  if(!slcli_json_array(raw,bytes,fields[5],out->client_users,128,&out->users)||!out->users) return 0;
  for(i=0;i<out->users;++i) {
    if(!decode(raw,bytes,out->client_users[i],left,sizeof(left),&n,0)||
       !slcli_grant_owner(left,n)) return 0;
    for(j=0;j<i;++j) {
      if(!decode(raw,bytes,out->client_users[j],right,sizeof(right),&m,0)) return 0;
      if(n==m&&slcli_equal(left,right,n)) return 0;
    }
  }
  slcli_sha256_init(&hash);
  if(!slcli_sha256_update(&hash,raw,bytes)||!slcli_sha256_finish(&hash,out->digest)) return 0;
  out->raw=raw;out->raw_bytes=bytes;
  return 1;
}
