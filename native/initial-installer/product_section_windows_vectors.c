#include "product_section_windows.h"
#if defined(_WIN32)
/* SYNTHETIC PURE BYTE CORRESPONDENCE ONLY. No native call/constructor/profile,
 * source/actor/reservation/loaded/family authority or actual positive receipt. */
int slcli_product_section_windows_correspondence_vectors(void) {
 struct slcli_product_section_windows c={0};
 struct slcli_bq_expected_principal effective={0},primary={0};
 _Alignas(SECURITY_DESCRIPTOR_RELATIVE) unsigned char body[80]={0};size_t i;
 SECURITY_DESCRIPTOR_RELATIVE *d=(SECURITY_DESCRIPTOR_RELATIVE *)body;
 ACL *a=(ACL *)(body+32);ACCESS_ALLOWED_ACE *first=(ACCESS_ALLOWED_ACE *)(body+40);
 ACCESS_ALLOWED_ACE *second=(ACCESS_ALLOWED_ACE *)(body+60);
 /* SID12 fixture S-1-5-42; deliberately not a designated actual identity. */
 unsigned char sid[12]={1,1,0,0,0,0,0,5,42,0,0,0};
 effective.windows_sid_bytes=primary.windows_sid_bytes=12;
 c.effective_original.expected_original=&effective;c.primary_original.expected_original=&primary;
 c.effective_original.user_original.header.User.Sid=c.effective_original.user_original.sid;
 c.primary_original.user_original.header.User.Sid=c.primary_original.user_original.sid;
 for(i=0;i<12;i++) {
 c.effective_original.user_original.sid[i]=sid[i];c.primary_original.user_original.sid[i]=sid[i];
 body[20+i]=sid[i];body[48+i]=sid[i];body[68+i]=sid[i];
 }
 d->Revision=1;d->Control=SE_SELF_RELATIVE|SE_DACL_PRESENT|SE_DACL_PROTECTED;d->Owner=20;d->Dacl=32;
 a->AclRevision=ACL_REVISION;a->AclSize=48;a->AceCount=2;
 first->Header.AceSize=20;first->Mask=SECTION_QUERY|SECTION_MAP_READ|SECTION_MAP_WRITE|READ_CONTROL;
 second->Header.AceSize=20;second->Mask=SECTION_MAP_READ|READ_CONTROL;
 c.relative_original=body;c.relative_bytes=c.relative_capacity=80;c.acl_bytes=48;
 c.observed_owner_original=body+20;c.observed_acl_original=a;c.dacl_present=TRUE;
 c.control=d->Control;c.revision=1;
#define CHECK(x) do {if(!(x))return __LINE__;}while(0)
 CHECK(slcli_product_windows_descriptor_bytes_correspond_original(&c));
 second->Mask|=SECTION_MAP_WRITE;CHECK(!slcli_product_windows_descriptor_bytes_correspond_original(&c));
 second->Mask=SECTION_MAP_READ|READ_CONTROL;
 a->AceCount=3;CHECK(!slcli_product_windows_descriptor_bytes_correspond_original(&c));a->AceCount=2;
 d->Owner=81;CHECK(!slcli_product_windows_descriptor_bytes_correspond_original(&c));d->Owner=20;
 d->Group=20;CHECK(!slcli_product_windows_descriptor_bytes_correspond_original(&c));d->Group=0;
 d->Sacl=32;CHECK(!slcli_product_windows_descriptor_bytes_correspond_original(&c));d->Sacl=0;
 c.owner_defaulted=TRUE;CHECK(!slcli_product_windows_descriptor_bytes_correspond_original(&c));c.owner_defaulted=FALSE;
 c.dacl_present=FALSE;CHECK(!slcli_product_windows_descriptor_bytes_correspond_original(&c));c.dacl_present=TRUE;
 c.relative_bytes=79;CHECK(!slcli_product_windows_descriptor_bytes_correspond_original(&c));c.relative_bytes=80;
 first->Header.AceFlags=OBJECT_INHERIT_ACE;CHECK(!slcli_product_windows_descriptor_bytes_correspond_original(&c));first->Header.AceFlags=0;
 body[20]=2;CHECK(!slcli_product_windows_descriptor_bytes_correspond_original(&c));body[20]=1;
 d->Control|=SE_DACL_DEFAULTED;CHECK(!slcli_product_windows_descriptor_bytes_correspond_original(&c));d->Control=c.control;
 second->Header.AceSize=16;CHECK(!slcli_product_windows_descriptor_bytes_correspond_original(&c));second->Header.AceSize=20;
 c.observed_acl_original=(ACL *)(body+36);CHECK(!slcli_product_windows_descriptor_bytes_correspond_original(&c));c.observed_acl_original=a;
 CHECK(slcli_product_windows_descriptor_bytes_correspond_original(&c));
#undef CHECK
 return 0;
}
#endif
