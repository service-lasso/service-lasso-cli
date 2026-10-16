/* SPDX-License-Identifier: Apache-2.0 */
#include "source_unit12.h"
static uint32_t le32(const unsigned char *p){
 return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
static uint64_t le64(const unsigned char *p){return (uint64_t)le32(p)|((uint64_t)le32(p+4)<<32);}
static int separate(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_source_unit12_window_geometry(const struct slcli_source_unit12_record *member,
 uint64_t file_bytes,uint64_t relative,size_t maximum,struct slcli_source_unit12_window *out){
 struct slcli_source_unit12_window next={0};uint64_t maximum64,remaining,wanted;
 if(!maximum||!separate(out,sizeof(*out),member,sizeof(*member)))return 0;
 maximum64=(uint64_t)maximum;if((size_t)maximum64!=maximum)return 0;
 if(member->offset>file_bytes||member->bytes>file_bytes-member->offset||
  relative>member->bytes||(member->bytes&&relative==member->bytes))return 0;
 /* The interval subtraction above also proves this addition cannot overflow. */
 if(relative>UINT64_MAX-member->offset)return 0;
 remaining=member->bytes-relative;wanted=remaining<maximum64?remaining:maximum64;
 if((uint64_t)(size_t)wanted!=wanted)return 0;
 next.member_index=member->index;next.member_offset=member->offset;
 next.relative_offset=relative;next.source_offset=member->offset+relative;
 next.requested=(size_t)wanted;
 *out=next;return 1;
}
static int nonzero(const unsigned char *p){unsigned i,n=0;for(i=0;i<32;i++)n|=p[i];return n!=0;}
static int magic_equal(const unsigned char *a,const unsigned char *b){
 unsigned i,different=0;for(i=0;i<8;++i)different|=(unsigned)(a[i]^b[i]);return different==0;
}
static int compare32(const unsigned char *a,const unsigned char *b){
 unsigned i;for(i=0;i<32;i++){if(a[i]<b[i])return -1;if(a[i]>b[i])return 1;}return 0;
}
int slcli_source_unit12_header(struct slcli_source_unit12_reader *r,
 const unsigned char raw[64],uint64_t observed){
 static const unsigned char magic[8]={'S','L','S','P','K','0','0','1'};
 uint64_t count,table,body;size_t i;
 if(!r||!raw||!separate(r,sizeof(*r),raw,64))return 0;
 for(i=0;i<sizeof(*r);i++)if(((const unsigned char *)r)[i])return 0;
 r->started=1;r->failed=1;for(i=0;i<64;i++)r->header[i]=raw[i];
 count=le64(raw+16);table=le64(raw+32);body=le64(raw+40);
 if(!magic_equal(raw,magic)||le32(raw+8)!=1||le32(raw+12)!=128||
  !count||count>(UINT64_MAX-64)/128||table!=count*128||
  le64(raw+24)!=64||body!=64+table||le64(raw+48)!=observed||le64(raw+56)||
  observed<body||observed>UINT64_C(17179869184))return 0;
 r->count=count;r->file_bytes=observed;r->body_offset=body;r->next_offset=body;
 r->failed=0;return 1;
}
int slcli_source_unit12_record(struct slcli_source_unit12_reader *r,const unsigned char raw[128]){
 struct slcli_source_unit12_record member;uint64_t offset,bytes;uint32_t project,kind;size_t i;
 if(!r||!raw||!separate(r,sizeof(*r),raw,128)||!r->started||r->failed||
  r->table_complete||r->next>=r->count||r->attempted!=r->next)return 0;
 /* Attempt remains consumed and exact offending bytes retained on failure.
  * No later call may reset this stream or pretend the malformed row absent. */
 r->failed=1;r->attempted=r->next+1;
 for(i=0;i<128;i++)r->attempted_record[i]=raw[i];
 offset=le64(raw+96);bytes=le64(raw+104);kind=le32(raw+112);project=le32(raw+116);
 if(!nonzero(raw)||!nonzero(raw+32)||!nonzero(raw+64)||kind>4||le64(raw+120)||
  offset!=r->next_offset||offset>r->file_bytes||bytes>r->file_bytes-offset||
  (r->next&&(project<r->previous.source_project||
   (project==r->previous.source_project&&compare32(raw,r->previous.association)<=0))))return 0;
 for(i=0;i<32;i++){member.association[i]=raw[i];member.capture[i]=raw[i+32];member.digest[i]=raw[i+64];}
 member.offset=offset;member.bytes=bytes;member.kind=kind;member.source_project=project;member.index=r->next;
 r->previous=member;r->next_offset=offset+bytes;r->next++;
 if(r->next==r->count){if(r->next_offset!=r->file_bytes)return 0;r->table_complete=1;}
 r->failed=0;return 1;
}

/* Git edge extraction is deliberately pure. Every raw byte/native original,
 * target-project relation, full graph and retained edge lifetime is caller-owned. */
enum { G_COMMIT_FIRST=1,G_COMMIT_HEADERS=2,G_MESSAGE=3,
 G_TREE_MODE=4,G_TREE_NAME=5,G_TREE_ID=6,G_TAG_OBJECT=7,G_TAG_TYPE=8,
 G_TAG_HEADERS=9,G_OPAQUE=10 };
static int git_hex(unsigned char ch){
 if(ch>='0'&&ch<='9')return ch-'0';
 if(ch>='a'&&ch<='f')return ch-'a'+10;
 if(ch>='A'&&ch<='F')return ch-'A'+10;return -1;
}
static int git_prefix(const struct slcli_source_unit12_git_edge_state *s,const char *p,size_t n){
 size_t i;if(s->prefix_bytes<n)return 0;
 for(i=0;i<n;i++)if(s->prefix[i]!=(unsigned char)p[i])return 0;return 1;
}
static int git_digit(struct slcli_source_unit12_git_edge_state *s,unsigned char ch){
 int n=git_hex(ch);uint32_t i=s->hex_digits;
 if(n<0||i>=40)return 0;
 if(i%2)s->object_id[i/2]|=(unsigned char)n;
 else s->object_id[i/2]=(unsigned char)((unsigned)n<<4);
 s->hex_digits++;return 1;
}
static void git_new_line(struct slcli_source_unit12_git_edge_state *s){
 s->line_offset=s->consumed+1;s->line_bytes=0;s->prefix_bytes=0;s->hex_digits=0;
}
static int git_edge_value(struct slcli_source_unit12_git_edge_state *s,
 struct slcli_source_unit12_git_edge *v,uint32_t relation,uint32_t target,
 uint32_t mode,uint64_t name_offset,uint64_t name_bytes){
 size_t i;if(s->emitted==UINT64_MAX||s->edge_offset>s->consumed)return 0;
 v->source_project=s->source_project;v->relation=relation;v->target_kind=target;v->mode=mode;
 for(i=0;i<20;i++)v->object_id[i]=s->object_id[i];
 v->edge_offset=s->edge_offset;v->edge_bytes=s->consumed+1-s->edge_offset;
 v->name_offset=name_offset;v->name_bytes=name_bytes;return 2;
}
static uint32_t git_tree_kind(uint32_t mode){
 if(mode==0040000)return 2;if(mode==0160000)return 1;
 if(mode==0100644||mode==0100755||mode==0120000)return 3;return 0;
}
static int git_step(struct slcli_source_unit12_git_edge_state *s,unsigned char ch,
 struct slcli_source_unit12_git_edge *v){
 uint64_t pos=s->line_bytes;uint32_t k,mask;size_t n;const char *word;
 static const char *const types[5]={"","commit","tree","blob","tag"};
 static const size_t lengths[5]={0,6,4,4,3};
 if(s->phase==G_OPAQUE||s->phase==G_MESSAGE)return 1;
 if(s->phase==G_TREE_MODE){
  if(ch==' '){
   if(!s->line_bytes||!(s->target_kind=git_tree_kind(s->mode)))return 0;
   s->phase=G_TREE_NAME;s->name_offset=s->consumed+1;s->name_bytes=0;return 1;
  }
  if(ch<'0'||ch>'7'||s->mode>(UINT32_MAX-(uint32_t)(ch-'0'))/8||s->line_bytes==UINT64_MAX)return 0;
  if(!s->line_bytes)s->edge_offset=s->consumed;
  s->mode=s->mode*8+(uint32_t)(ch-'0');s->line_bytes++;return 1;
 }
 if(s->phase==G_TREE_NAME){
  if(!ch){
   if(!s->name_bytes||(s->name_bytes==1&&s->name_prefix[0]=='.')||
    (s->name_bytes==2&&s->name_prefix[0]=='.'&&s->name_prefix[1]=='.'))return 0;
   s->phase=G_TREE_ID;s->oid_bytes=0;return 1;
  }
  if(ch=='/'||s->name_bytes==UINT64_MAX)return 0;
  if(s->name_bytes<2)s->name_prefix[(size_t)s->name_bytes]=ch;s->name_bytes++;return 1;
 }
 if(s->phase==G_TREE_ID){
  if(s->oid_bytes>=20)return 0;s->object_id[s->oid_bytes++]=ch;
  if(s->oid_bytes<20)return 1;
  k=s->target_kind;
  if(git_edge_value(s,v,SLCLI_SOURCE_GIT_TREE_ENTRY,k,s->mode,s->name_offset,s->name_bytes)!=2)return 0;
  s->phase=G_TREE_MODE;s->line_bytes=0;s->mode=0;s->oid_bytes=0;return 2;
 }
 if(s->phase==G_COMMIT_FIRST||s->phase==G_TAG_OBJECT){
  word=s->phase==G_COMMIT_FIRST?"tree ":"object ";n=s->phase==G_COMMIT_FIRST?5:7;
  if(pos<n){if(ch!=(unsigned char)word[(size_t)pos])return 0;}
  else if(pos<n+40){if(!git_digit(s,ch))return 0;}
  else {
   if(pos!=n+40||ch!='\n'||s->hex_digits!=40)return 0;
   if(s->phase==G_TAG_OBJECT){s->phase=G_TAG_TYPE;s->header_class=30;git_new_line(s);return 1;}
   s->edge_offset=s->line_offset;
   if(git_edge_value(s,v,SLCLI_SOURCE_GIT_ROOT_TREE,2,0,0,0)!=2)return 0;
   s->phase=G_COMMIT_HEADERS;git_new_line(s);return 2;
  }
  if(s->line_bytes==UINT64_MAX)return 0;s->line_bytes++;return 1;
 }
 if(s->phase==G_TAG_TYPE){
  if(pos<5){if(ch!=(unsigned char)"type "[(size_t)pos])return 0;}
  else {
   mask=s->header_class;
   for(k=1;k<=4;k++)if(mask&(1u<<k)){
    if(ch=='\n') {if(pos-5!=lengths[k])mask&=~(1u<<k);}
    else if(pos-5>=lengths[k]||ch!=(unsigned char)types[k][(size_t)(pos-5)])mask&=~(1u<<k);
   }
   if(!mask)return 0;s->header_class=mask;
   if(ch=='\n'){
    for(k=1;k<=4;k++)if(mask==(1u<<k))break;if(k>4)return 0;
    if(git_edge_value(s,v,SLCLI_SOURCE_GIT_TAG_TARGET,k,0,0,0)!=2)return 0;
    s->phase=G_TAG_HEADERS;git_new_line(s);return 2;
   }
  }
  if(s->line_bytes==UINT64_MAX)return 0;s->line_bytes++;return 1;
 }
 if(s->phase!=G_COMMIT_HEADERS&&s->phase!=G_TAG_HEADERS)return 0;
 if(!ch)return 0; /* Header NUL is invalid; message bytes remain opaque above. */
 if(ch=='\n'){
  if(!pos){s->phase=G_MESSAGE;git_new_line(s);return 1;}
  if(s->phase==G_COMMIT_HEADERS){
   if(git_prefix(s,"tree ",5))return 0;
   if(git_prefix(s,"parent ",7)){
    if(s->parents_closed||pos!=47||s->hex_digits!=40)return 0;
    s->edge_offset=s->line_offset;
    if(git_edge_value(s,v,SLCLI_SOURCE_GIT_PARENT,1,0,0,0)!=2)return 0;
    git_new_line(s);return 2;
   }
   s->parents_closed=1;
  }else if(git_prefix(s,"object ",7)||git_prefix(s,"type ",5))return 0;
  git_new_line(s);return 1;
 }
 if(s->prefix_bytes<sizeof(s->prefix))s->prefix[s->prefix_bytes++]=ch;
 if(s->phase==G_COMMIT_HEADERS&&pos>=7&&git_prefix(s,"parent ",7)){
  if(s->parents_closed||pos>=47||!git_digit(s,ch))return 0;
 }
 if(pos==UINT64_MAX)return 0;s->line_bytes++;return 1;
}
int slcli_source_unit12_git_edge_begin(struct slcli_source_unit12_git_edge_state *s,
 const struct slcli_source_unit12_record *r){
 size_t i;if(!separate(s,sizeof(*s),r,sizeof(*r)))return 0;
 for(i=0;i<sizeof(*s);i++)if(((const unsigned char *)s)[i])return 0;
 s->started=1;s->failed=1;if(r->kind>4)return 0;
 s->kind=r->kind;s->source_project=r->source_project;s->member_bytes=r->bytes;
 s->phase=r->kind==1?G_COMMIT_FIRST:r->kind==2?G_TREE_MODE:r->kind==4?G_TAG_OBJECT:G_OPAQUE;
 s->failed=0;return 1;
}
int slcli_source_unit12_git_edge_feed(struct slcli_source_unit12_git_edge_state *s,
 uint64_t relative,const unsigned char *raw,size_t bytes,size_t *used,
 struct slcli_source_unit12_git_edge *out){
 struct slcli_source_unit12_git_edge next={0};size_t i;int result;uint64_t n=(uint64_t)bytes;
 if(!s||!s->started||s->failed||s->complete)return 0;
 if(!bytes||(size_t)n!=bytes||!separate(s,sizeof(*s),raw,bytes)||
  !separate(s,sizeof(*s),used,sizeof(*used))||!separate(s,sizeof(*s),out,sizeof(*out))||
  !separate(raw,bytes,used,sizeof(*used))||!separate(raw,bytes,out,sizeof(*out))||
  !separate(used,sizeof(*used),out,sizeof(*out)))return 0;
 s->failed=1;
 if(s->pending||relative!=s->consumed||s->consumed>s->member_bytes||n>s->member_bytes-s->consumed)return 0;
 for(i=0;i<bytes;i++){
  result=git_step(s,raw[i],&next);if(!result)return 0;s->consumed++;
  if(result==2){
   s->emitted++;s->pending=1;s->pending_original=out;s->pending_value=next;
   *out=next;*used=i+1;s->failed=0;return 2;
  }
 }
 *used=bytes;s->failed=0;return 1;
}
static int git_same_edge(const struct slcli_source_unit12_git_edge *a,
 const struct slcli_source_unit12_git_edge *b){
 size_t i;if(a->source_project!=b->source_project||a->relation!=b->relation||
  a->target_kind!=b->target_kind||a->mode!=b->mode||a->edge_offset!=b->edge_offset||
  a->edge_bytes!=b->edge_bytes||a->name_offset!=b->name_offset||a->name_bytes!=b->name_bytes)return 0;
 for(i=0;i<20;i++)if(a->object_id[i]!=b->object_id[i])return 0;return 1;
}
int slcli_source_unit12_git_edge_resume(struct slcli_source_unit12_git_edge_state *s,
 const struct slcli_source_unit12_git_edge *edge){
 if(!s||!s->started||s->failed||s->complete||
  !separate(s,sizeof(*s),edge,sizeof(*edge)))return 0;s->failed=1;
 if(!s->pending||edge!=s->pending_original||
  !git_same_edge(edge,&s->pending_value))return 0;
 s->pending=0;s->failed=0;return 1;
}
int slcli_source_unit12_git_edge_finish(struct slcli_source_unit12_git_edge_state *s){
 if(!s||!s->started||s->failed||s->complete)return 0;s->failed=1;
 if(s->pending||s->consumed!=s->member_bytes||
  (s->phase!=G_OPAQUE&&s->phase!=G_MESSAGE&&
   !((s->phase==G_COMMIT_HEADERS||s->phase==G_TAG_HEADERS)&&!s->line_bytes)&&
   !(s->kind==2&&s->phase==G_TREE_MODE&&!s->line_bytes)))return 0;
 s->complete=1;s->failed=0;return 1;
}

/* Pairwise structure only: native receipts and complete candidate stack remain
 * independently retained by the caller for each actual traversal. */
static int name_same_member(const struct slcli_source_unit12_record *a,
 const struct slcli_source_unit12_record *b){
 unsigned i;if(a->offset!=b->offset||a->bytes!=b->bytes||a->index!=b->index||
  a->kind!=b->kind||a->source_project!=b->source_project)return 0;
 for(i=0;i<32;i++)if(a->association[i]!=b->association[i]||a->capture[i]!=b->capture[i]||
  a->digest[i]!=b->digest[i])return 0;return 1;
}
static int name_edge_valid(const struct slcli_source_unit12_record *m,
 const struct slcli_source_unit12_git_edge *e){
 uint32_t kind;if(e->mode==040000)kind=2;
 else if(e->mode==0100644||e->mode==0100755||e->mode==0120000)kind=3;
 else if(e->mode==0160000)kind=1;else return 0;
 return m->kind==2&&e->relation==SLCLI_SOURCE_GIT_TREE_ENTRY&&
  e->source_project==m->source_project&&e->target_kind==kind&&e->edge_bytes&&
  e->edge_offset<=m->bytes&&e->edge_bytes<=m->bytes-e->edge_offset&&
  e->name_bytes&&e->name_offset>=e->edge_offset&&
  e->name_offset-e->edge_offset<=e->edge_bytes&&
  e->name_bytes<=e->edge_bytes-(e->name_offset-e->edge_offset);
}
static int name_origins(const struct slcli_source_unit12_git_name_order_state *s){
 return separate(s,sizeof(*s),s->member_original,sizeof(*s->member_original))&&
  separate(s,sizeof(*s),s->previous_original,sizeof(*s->previous_original))&&
  separate(s,sizeof(*s),s->current_original,sizeof(*s->current_original))&&
  name_same_member(s->member_original,&s->member_value)&&
  git_same_edge(s->previous_original,&s->previous_value)&&
  git_same_edge(s->current_original,&s->current_value);
}
static int name_begin(struct slcli_source_unit12_git_name_order_state *s,
 const struct slcli_source_unit12_record *m,const struct slcli_source_unit12_git_edge *p,
 const struct slcli_source_unit12_git_edge *c,uint32_t purpose){
 size_t i;if(!separate(s,sizeof(*s),m,sizeof(*m))||!separate(s,sizeof(*s),p,sizeof(*p))||
  !separate(s,sizeof(*s),c,sizeof(*c))||!separate(m,sizeof(*m),p,sizeof(*p))||
  !separate(m,sizeof(*m),c,sizeof(*c))||!separate(p,sizeof(*p),c,sizeof(*c)))return 0;
 for(i=0;i<sizeof(*s);i++)if(((const unsigned char *)s)[i])return 0;
 if(!name_edge_valid(m,p)||!name_edge_valid(m,c)||
  p->edge_offset+p->edge_bytes>c->edge_offset)return 0;
 s->member_original=m;s->previous_original=p;s->current_original=c;
 s->member_value=*m;s->previous_value=*p;s->current_value=*c;
 s->purpose=purpose;s->started=1;s->need_previous=1;s->need_current=1;return 1;
}
int slcli_source_unit12_git_name_order_begin(struct slcli_source_unit12_git_name_order_state *s,
 const struct slcli_source_unit12_record *m,const struct slcli_source_unit12_git_edge *p,
 const struct slcli_source_unit12_git_edge *c){return name_begin(s,m,p,c,1);}
int slcli_source_unit12_git_name_prefix_begin(struct slcli_source_unit12_git_name_order_state *s,
 const struct slcli_source_unit12_record *m,const struct slcli_source_unit12_git_edge *p,
 const struct slcli_source_unit12_git_edge *c){return name_begin(s,m,p,c,2);}
static int name_output_separate(const struct slcli_source_unit12_git_name_order_state *s,
 const void *out,size_t n){
 return separate(out,n,s,sizeof(*s))&&
  separate(out,n,s->member_original,sizeof(*s->member_original))&&
  separate(out,n,s->previous_original,sizeof(*s->previous_original))&&
  separate(out,n,s->current_original,sizeof(*s->current_original));
}
int slcli_source_unit12_git_name_order_windows(const struct slcli_source_unit12_git_name_order_state *s,
 uint64_t file_bytes,size_t maximum,struct slcli_source_unit12_window *p,
 struct slcli_source_unit12_window *c){
 struct slcli_source_unit12_window a={0},b={0};uint64_t remaining,limit=(uint64_t)maximum;
 if(!s||!name_origins(s)||!name_output_separate(s,p,sizeof(*p))||
  !name_output_separate(s,c,sizeof(*c))||!separate(p,sizeof(*p),c,sizeof(*c))||
  !maximum||(size_t)limit!=maximum||!s->started||s->failed||s->ordered||s->finished||
  s->member_value.offset>file_bytes||s->member_value.bytes>file_bytes-s->member_value.offset||
  s->previous_cursor>s->previous_value.name_bytes||s->current_cursor>s->current_value.name_bytes)return 0;
 a.member_index=b.member_index=s->member_value.index;
 a.member_offset=b.member_offset=s->member_value.offset;
 a.relative_offset=s->previous_value.name_offset+s->previous_cursor;
 b.relative_offset=s->current_value.name_offset+s->current_cursor;
 a.source_offset=a.member_offset+a.relative_offset;b.source_offset=b.member_offset+b.relative_offset;
 remaining=s->previous_value.name_bytes-s->previous_cursor;
 a.requested=(size_t)(remaining<limit?remaining:limit);
 remaining=s->current_value.name_bytes-s->current_cursor;
 b.requested=(size_t)(remaining<limit?remaining:limit);
 *p=a;*c=b;return 1;
}
int slcli_source_unit12_git_name_order_feed(struct slcli_source_unit12_git_name_order_state *s,
 uint64_t pr,const unsigned char *p,size_t pn,uint64_t cr,const unsigned char *c,size_t cn,
 size_t *pu,size_t *cu){
 size_t i=0,j=0;uint32_t x,y,pv,cv;uint64_t prem,crem;
 if(!s||!name_origins(s)||!name_output_separate(s,pu,sizeof(*pu))||
  !name_output_separate(s,cu,sizeof(*cu))||!separate(pu,sizeof(*pu),cu,sizeof(*cu)))return 0;
 /* Validate the complete storage seam before an absorbing state write. */
 if((pn&&(!name_output_separate(s,p,pn)||!separate(p,pn,pu,sizeof(*pu))||
   !separate(p,pn,cu,sizeof(*cu))))||
  (cn&&(!name_output_separate(s,c,cn)||!separate(c,cn,pu,sizeof(*pu))||
   !separate(c,cn,cu,sizeof(*cu))))||(pn&&cn&&!separate(p,pn,c,cn)))return 0;
 if(!s->started||s->failed||s->finished||s->ordered)return 0;
 s->failed=1;
 if((s->purpose!=1&&s->purpose!=2)||s->previous_cursor>s->previous_value.name_bytes||
  s->current_cursor>s->current_value.name_bytes)return 0;
 prem=s->previous_value.name_bytes-s->previous_cursor;
 crem=s->current_value.name_bytes-s->current_cursor;
 if(pr!=s->previous_value.name_offset+s->previous_cursor||
  cr!=s->current_value.name_offset+s->current_cursor||
  (uint64_t)pn>prem||(uint64_t)cn>crem||(size_t)(uint64_t)pn!=pn||
  (size_t)(uint64_t)cn!=cn||(prem?(!pn||!p):(pn||p))||
  (crem?(!cn||!c):(cn||c))||(!pn&&!cn))return 0;
 for(;;){
  pv=s->previous_cursor==s->previous_value.name_bytes;
  cv=s->current_cursor==s->current_value.name_bytes;
  if((!pv&&i==pn)||(!cv&&j==cn))break;
  x=pv?((s->purpose==1&&s->previous_value.mode==040000)?47:0):p[i];
  y=cv?((s->purpose==1&&s->current_value.mode==040000)?47:0):c[j];
  if((!pv&&(!x||x==47))||(!cv&&(!y||y==47)))return 0;
  if(pv&&cv){
   if(s->purpose==1)return 0;
   s->previous_prefix=1;s->raw_comparison=0;
  }else if(x!=y){
   if(s->purpose==1&&x>=y)return 0;
   s->raw_comparison=x<y?-1:1;s->previous_prefix=pv?1:0;
  }else{
   /* At least one virtual byte implies a slash/NUL in raw input, already refused. */
   if(pv||cv)return 0;
   i++;j++;s->previous_cursor++;s->current_cursor++;s->compared++;continue;
  }
  s->previous_difference=s->previous_cursor;s->current_difference=s->current_cursor;
  s->previous_byte=x;s->current_byte=y;s->previous_virtual=pv;s->current_virtual=cv;
  if(!pv){i++;s->previous_cursor++;}if(!cv){j++;s->current_cursor++;}
  if(!i&&!j)return 0;
  s->ordered=1;s->need_previous=s->need_current=0;s->failed=0;*pu=i;*cu=j;return 2;
 }
 /* Equal raw names can finish on the final actual pair, without a zero-input call. */
 if(s->previous_cursor==s->previous_value.name_bytes&&
  s->current_cursor==s->current_value.name_bytes){
  if(s->purpose==1)return 0;
  s->previous_prefix=1;s->raw_comparison=0;s->previous_byte=s->current_byte=0;
  s->previous_virtual=s->current_virtual=1;
  s->previous_difference=s->previous_cursor;s->current_difference=s->current_cursor;
  s->ordered=1;s->need_previous=s->need_current=0;s->failed=0;*pu=i;*cu=j;return 2;
 }
 if(!i&&!j)return 0;
 s->need_previous=s->previous_cursor<s->previous_value.name_bytes&&i==pn;
 s->need_current=s->current_cursor<s->current_value.name_bytes&&j==cn;
 s->failed=0;*pu=i;*cu=j;return 1;
}
static int name_finish(struct slcli_source_unit12_git_name_order_state *s,uint32_t purpose){
 if(!s||!name_origins(s))return 0;
 if(!s->started||s->failed||s->finished)return 0;
 if(!s->ordered||s->purpose!=purpose){s->failed=1;return 0;}
 s->finished=1;return 1;
}
int slcli_source_unit12_git_name_prefix_finish(struct slcli_source_unit12_git_name_order_state *s){
 return name_finish(s,2);
}
int slcli_source_unit12_git_name_order_finish(struct slcli_source_unit12_git_name_order_state *s){
 return name_finish(s,1);
}
