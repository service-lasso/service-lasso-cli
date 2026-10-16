#if defined(_WIN32)
#include "file_windows.h"
#include <stddef.h>
static void zero(void *value, size_t bytes) {
  unsigned char *p = value; while (bytes--) *p++ = 0;
}
static int disjoint(const struct slcli_windows_file_capture *c,
                      const void *data, size_t bytes) {
  uintptr_t a=(uintptr_t)c,b=(uintptr_t)data;
  return a<=UINTPTR_MAX-sizeof(*c)&&bytes<=UINTPTR_MAX&&
           b<=UINTPTR_MAX-bytes&&!(a<b+bytes&&b<a+sizeof(*c));
}
static int clock_observe(struct slcli_windows_file_capture *c) {
  uint64_t seconds, remainder, whole, fraction;
  c->counter_status = QueryPerformanceCounter(&c->counter_raw);
  c->frequency_status = QueryPerformanceFrequency(&c->frequency_raw);
  if (!c->counter_status || !c->frequency_status || c->counter_raw.QuadPart <= 0 ||
      c->frequency_raw.QuadPart <= 0) return 0;
  seconds = (uint64_t)(c->counter_raw.QuadPart / c->frequency_raw.QuadPart);
  remainder = (uint64_t)(c->counter_raw.QuadPart % c->frequency_raw.QuadPart);
  if (seconds > UINT64_MAX / 1000000000 || remainder > UINT64_MAX / 1000000000)
    return 0;
  whole = seconds * 1000000000;
  fraction = remainder * 1000000000 / (uint64_t)c->frequency_raw.QuadPart;
  if (fraction > UINT64_MAX - whole) return 0;
  c->tick = whole + fraction;
  return c->tick != 0;
}
static int error(struct slcli_windows_file_capture *c) {
  c->error = GetLastError(); c->error_observed = 1;
  clock_observe(c);
  return 0;
}
static int mode_observe(struct slcli_windows_file_capture *c) {
  /* Exact FileModeInformation class16 and synchronous0x10/0x20 semantics port
   * the retained source_lease_read_windows.go native boundary. Preserve BOTH
   * original NT and IO returns; a pending/unobserved query is not completion. */
  c->mode_args[0] = (uintptr_t)c->original;
  c->mode_args[1] = (uintptr_t)&c->mode_io;
  c->mode_args[2] = (uintptr_t)&c->mode;
  c->mode_args[3] = sizeof(c->mode); c->mode_args[4] = 16;
  c->retained_native_io = 1;
  c->mode_status = NtQueryInformationFile(c->original, &c->mode_io, &c->mode,
                                          sizeof(c->mode), (FILE_INFORMATION_CLASS)16);
  /* Pending native output still points into this SAME capture. A second call
   * may not zero/reuse it. Only actual completed native query clears the guard. */
  if (c->mode_status == 0 && c->mode_io.Status == 0 &&
      c->mode_io.Information == sizeof(c->mode)) c->retained_native_io = 0;
  return c->mode_status == 0 && c->mode_io.Status == 0 &&
           c->mode_io.Information == sizeof(c->mode) && (c->mode & 0x30) != 0;
}
static int rights_observe(struct slcli_windows_file_capture *c,int source_read_only) {
  ACCESS_MASK access;
  const ACCESS_MASK forbidden=0x00000002|0x00000004|0x00000010|0x00000100|
    0x00010000|0x00040000|0x00080000|0x10000000|0x40000000;
  c->type_args[0]=(uintptr_t)c->original;
  c->type_error_seed=ERROR_SUCCESS;
  SetLastError(c->type_error_seed);
  c->file_type=GetFileType(c->original);
  if(c->file_type!=FILE_TYPE_DISK) {
    if(c->file_type==FILE_TYPE_UNKNOWN) {
      c->error=GetLastError(); c->error_observed=1;
    }
    return 0;
  }
  c->access_args[0]=(uintptr_t)c->original;
  c->access_args[1]=0;
  c->access_args[2]=(uintptr_t)&c->access_raw;
  c->access_args[3]=sizeof(c->access_raw);
  c->access_args[4]=(uintptr_t)&c->access_bytes;
  c->retained_native_io=1;
  c->access_status=NtQueryObject(c->original,ObjectBasicInformation,&c->access_raw,
                                sizeof(c->access_raw),&c->access_bytes);
  if(c->access_status!=0||c->access_bytes!=sizeof(c->access_raw)) return 0;
  c->retained_native_io=0;
  access=c->access_raw.GrantedAccess;
  /* Exact original source_lease_read_windows.go access policy: native granted
   * bits, never requested rights or a successful read as rights evidence. */
  if(source_read_only&&((access&0x00120081)!=0x00120081||(access&forbidden))) return 0;
  c->handle_args[0]=(uintptr_t)c->original;
  c->handle_args[1]=(uintptr_t)&c->handle_flags;
  c->handle_status=GetHandleInformation(c->original,&c->handle_flags);
  if(!c->handle_status) {c->error=GetLastError();c->error_observed=1;return 0;}
  return !source_read_only||!(c->handle_flags&HANDLE_FLAG_INHERIT);
}
static int file_observe(HANDLE original,
                         struct slcli_windows_file_capture *c,int source_read_only,int directory) {
  uintptr_t start, sid;
  DWORD bytes, i;
  if (!c || c->operation || c->retained_native_io || !original || original == INVALID_HANDLE_VALUE) return 0;
  zero(&c->original,sizeof(*c)-offsetof(struct slcli_windows_file_capture,original)); c->original = original; c->operation = 1;
  c->observed = 1;
  /* Writable custody retains actual native grants and handle flags too;
   * its independent owning policy is applied by the genuine creator/caller. */
  if(!rights_observe(c,source_read_only)) { clock_observe(c); return 0; }
  if (!mode_observe(c)) { clock_observe(c); return 0; }
  c->information_args[0][0] = (uintptr_t)original;
  c->information_args[0][1] = FileIdInfo;
  c->information_args[0][2] = (uintptr_t)&c->identity;
  c->information_args[0][3] = sizeof(c->identity);
  c->identity_status = GetFileInformationByHandleEx(original, FileIdInfo,
                                                   &c->identity, sizeof(c->identity));
  if (!c->identity_status) return error(c);
  c->information_args[1][0] = (uintptr_t)original;
  c->information_args[1][1] = FileStandardInfo;
  c->information_args[1][2] = (uintptr_t)&c->standard;
  c->information_args[1][3] = sizeof(c->standard);
  c->standard_status = GetFileInformationByHandleEx(original, FileStandardInfo,
                                                   &c->standard, sizeof(c->standard));
  if (!c->standard_status) return error(c);
  c->information_args[2][0] = (uintptr_t)original;
  c->information_args[2][1] = FileAttributeTagInfo;
  c->information_args[2][2] = (uintptr_t)&c->attributes;
  c->information_args[2][3] = sizeof(c->attributes);
  c->attributes_status = GetFileInformationByHandleEx(original, FileAttributeTagInfo,
                                                      &c->attributes, sizeof(c->attributes));
  if (!c->attributes_status) return error(c);
  c->information_args[3][0] = (uintptr_t)original;
  c->information_args[3][1] = FileBasicInfo;
  c->information_args[3][2] = (uintptr_t)&c->basic;
  c->information_args[3][3] = sizeof(c->basic);
  c->basic_status = GetFileInformationByHandleEx(original, FileBasicInfo,
                                                &c->basic, sizeof(c->basic));
  if (!c->basic_status) return error(c);
  c->security_args[0] = (uintptr_t)original;
  c->security_args[1] = OWNER_SECURITY_INFORMATION;
  c->security_args[2] = (uintptr_t)c->security;
  c->security_args[3] = sizeof(c->security);
  c->security_args[4] = (uintptr_t)&c->security_bytes;
  c->security_status = GetKernelObjectSecurity(original, OWNER_SECURITY_INFORMATION,
                                               c->security, sizeof(c->security),
                                               &c->security_bytes);
  if (!c->security_status) return error(c);
  if (c->security_bytes > sizeof(c->security) || c->security_bytes < offsetof(SID, SubAuthority)) {
    clock_observe(c); return 0;
  }
  c->owner_status = GetSecurityDescriptorOwner(c->security, &c->owner_pointer,
                                               &c->owner_defaulted);
  if (!c->owner_status) return error(c);
  start = (uintptr_t)c->security; sid = (uintptr_t)c->owner_pointer;
  if (sid < start || sid % _Alignof(SID) ||
      sid - start > c->security_bytes - offsetof(SID, SubAuthority)) {
    clock_observe(c); return 0;
  }
  bytes = offsetof(SID, SubAuthority) +
            ((const SID *)c->owner_pointer)->SubAuthorityCount * sizeof(DWORD);
  if (bytes > c->security_bytes - (sid - start)) { clock_observe(c); return 0; }
  c->sid_status = IsValidSid(c->owner_pointer);
  if (!c->sid_status) { clock_observe(c); return 0; }
  c->owner_sid_bytes = GetLengthSid(c->owner_pointer);
  if (c->owner_sid_bytes != bytes) { clock_observe(c); return 0; }
  if (!bytes || bytes > sizeof(c->owner_sid) || bytes > c->security_bytes - (sid - start)) {
    clock_observe(c); return 0;
  }
  for (i = 0; i < bytes; ++i) c->owner_sid[i] = ((const unsigned char *)c->owner_pointer)[i];
  c->owner_sid_bytes = bytes;
  if (!clock_observe(c) || (!!c->standard.Directory != !!directory) || c->standard.DeletePending ||
      c->standard.EndOfFile.QuadPart < 0 ||
      (!!(c->attributes.FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != !!directory) ||
      (c->attributes.FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
    return 0;
  c->completed = 1;
  return 1;
}
int slcli_windows_file_observe(HANDLE original,struct slcli_windows_file_capture *c) {
  return file_observe(original,c,0,0);
}
int slcli_windows_directory_observe(HANDLE original,struct slcli_windows_file_capture *c) {
  return file_observe(original,c,0,1);
}
int slcli_windows_directory_source_observe(HANDLE original,struct slcli_windows_file_capture *c) {
  if(!file_observe(original,c,1,1))return 0;
  /* Directory-specific deletion of children is a mutable alias right too.
   * DACL/ancestor/creator and independent designation are separate inputs. */
  return !(c->access_raw.GrantedAccess&FILE_DELETE_CHILD);
}
int slcli_windows_source_observe(HANDLE original,struct slcli_windows_file_capture *c) {
  return file_observe(original,c,1,0);
}
static int captured_clock(const struct slcli_windows_file_capture *c){
 uint64_t seconds,remainder,whole,fraction;
 if(!c->counter_status||!c->frequency_status||c->counter_raw.QuadPart<=0||c->frequency_raw.QuadPart<=0)return 0;
 seconds=(uint64_t)(c->counter_raw.QuadPart/c->frequency_raw.QuadPart);
 remainder=(uint64_t)(c->counter_raw.QuadPart%c->frequency_raw.QuadPart);
 if(seconds>UINT64_MAX/1000000000||remainder>UINT64_MAX/1000000000)return 0;
 whole=seconds*1000000000;fraction=remainder*1000000000/(uint64_t)c->frequency_raw.QuadPart;
 return fraction<=UINT64_MAX-whole&&whole+fraction&&c->tick==whole+fraction;
}
static int captured_mode(HANDLE original,const struct slcli_windows_file_capture *c){
 return c->mode_args[0]==(uintptr_t)original&&c->mode_args[1]==(uintptr_t)&c->mode_io&&
  c->mode_args[2]==(uintptr_t)&c->mode&&c->mode_args[3]==sizeof(c->mode)&&c->mode_args[4]==16&&
  !c->mode_status&&!c->mode_io.Status&&c->mode_io.Information==sizeof(c->mode)&&(c->mode&0x30);
}
int slcli_windows_source_observation_matches_original(HANDLE original,
 const struct slcli_windows_file_capture *c){
 const void *outputs[4];size_t sizes[4],i,sid_bytes;uintptr_t start,sid;
 const SECURITY_DESCRIPTOR_RELATIVE *descriptor;
 const uint64_t classes[4]={FileIdInfo,FileStandardInfo,FileAttributeTagInfo,FileBasicInfo};
 const ACCESS_MASK forbidden=0x00000002|0x00000004|0x00000010|0x00000100|
  0x00010000|0x00040000|0x00080000|0x10000000|0x40000000;
 if(!original||original==INVALID_HANDLE_VALUE||!c||c->original!=original||c->operation!=1||
  !c->observed||!c->completed||c->retained_native_io||c->error_observed||
  !captured_clock(c)||!captured_mode(original,c)||c->type_args[0]!=(uintptr_t)original||
  c->type_error_seed!=ERROR_SUCCESS||c->file_type!=FILE_TYPE_DISK||
  c->access_args[0]!=(uintptr_t)original||c->access_args[1]||
  c->access_args[2]!=(uintptr_t)&c->access_raw||c->access_args[3]!=sizeof(c->access_raw)||
  c->access_args[4]!=(uintptr_t)&c->access_bytes||c->access_status||c->access_bytes!=sizeof(c->access_raw)||
  (c->access_raw.GrantedAccess&0x00120081)!=0x00120081||(c->access_raw.GrantedAccess&forbidden)||
  c->handle_args[0]!=(uintptr_t)original||c->handle_args[1]!=(uintptr_t)&c->handle_flags||
  !c->handle_status||(c->handle_flags&HANDLE_FLAG_INHERIT)||
  !c->identity_status||!c->standard_status||!c->attributes_status||!c->basic_status||
  c->standard.Directory||c->standard.DeletePending||c->standard.EndOfFile.QuadPart<0||
  (c->attributes.FileAttributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT))||
  c->security_args[0]!=(uintptr_t)original||c->security_args[1]!=OWNER_SECURITY_INFORMATION||
  c->security_args[2]!=(uintptr_t)c->security||c->security_args[3]!=sizeof(c->security)||
  c->security_args[4]!=(uintptr_t)&c->security_bytes||!c->security_status||!c->owner_status||!c->sid_status||
  c->security_bytes>sizeof(c->security)||c->security_bytes<offsetof(SID,SubAuthority))return 0;
 outputs[0]=&c->identity;sizes[0]=sizeof(c->identity);
 outputs[1]=&c->standard;sizes[1]=sizeof(c->standard);
 outputs[2]=&c->attributes;sizes[2]=sizeof(c->attributes);
 outputs[3]=&c->basic;sizes[3]=sizeof(c->basic);
 for(i=0;i<4;i++)if(c->information_args[i][0]!=(uintptr_t)original||
  c->information_args[i][1]!=classes[i]||c->information_args[i][2]!=(uintptr_t)outputs[i]||
  c->information_args[i][3]!=sizes[i])return 0;
 start=(uintptr_t)c->security;sid=(uintptr_t)c->owner_pointer;
 if(c->security_bytes<sizeof(*descriptor)||start%_Alignof(SECURITY_DESCRIPTOR_RELATIVE)||
  sid<start||sid%_Alignof(SID)||sid-start>c->security_bytes- offsetof(SID,SubAuthority))return 0;
 descriptor=(const SECURITY_DESCRIPTOR_RELATIVE *)c->security;
 if(descriptor->Revision!=SECURITY_DESCRIPTOR_REVISION||!(descriptor->Control&SE_SELF_RELATIVE)||
  descriptor->Owner!=sid-start||!!(descriptor->Control&SE_OWNER_DEFAULTED)!=!!c->owner_defaulted)return 0;
 sid_bytes=offsetof(SID,SubAuthority)+((const SID *)c->owner_pointer)->SubAuthorityCount*sizeof(DWORD);
 if(((const SID *)c->owner_pointer)->Revision!=SID_REVISION||
  ((const SID *)c->owner_pointer)->SubAuthorityCount>SID_MAX_SUB_AUTHORITIES||
  sid_bytes>c->security_bytes-(sid-start)||sid_bytes>sizeof(c->owner_sid)||c->owner_sid_bytes!=sid_bytes)return 0;
 for(i=0;i<sid_bytes;i++)if(c->owner_sid[i]!=((const unsigned char *)c->owner_pointer)[i])return 0;
 return 1;
}
int slcli_windows_file_read(HANDLE original, unsigned char *destination,
                            DWORD bytes, uint64_t offset,
                            struct slcli_windows_file_capture *c) {
  if (!c || c->operation || c->retained_native_io || !original || original == INVALID_HANDLE_VALUE || !destination ||
      !bytes || bytes > 16384 || offset > INT64_MAX || bytes > (uint64_t)INT64_MAX - offset)
    return 0;
  if (!disjoint(c,destination,bytes)) return 0;
  zero(&c->original,sizeof(*c)-offsetof(struct slcli_windows_file_capture,original)); c->original = original; c->operation = 2;
  c->offset = offset; c->requested = bytes; c->observed = 1;
  if (!mode_observe(c)) { clock_observe(c); return 0; }
  c->position_requested.QuadPart = (LONGLONG)offset;
  c->position_args[0]=(uintptr_t)original;c->position_args[1]=offset;
  c->position_args[2]=(uintptr_t)&c->position_returned;c->position_args[3]=FILE_BEGIN;
  c->position_status = SetFilePointerEx(original, c->position_requested,
                                        &c->position_returned, FILE_BEGIN);
  if (!c->position_status) return error(c);
  if ((uint64_t)c->position_returned.QuadPart != offset) { clock_observe(c); return 0; }
  c->read_args[0] = (uintptr_t)original; c->read_args[1] = (uintptr_t)destination;
  c->read_args[2] = bytes; c->read_args[3] = (uintptr_t)&c->transferred_raw;
  c->read_args[4] = 0;
  c->retained_native_io = 1;
  c->read_status = ReadFile(original, destination, bytes, &c->transferred_raw, NULL);
  c->status = (uint64_t)(uint32_t)c->read_status;
  if (!c->read_status) { c->error = GetLastError(); c->error_observed = 1; }
  if (c->transferred_raw > bytes) { clock_observe(c); return 0; }
  if (c->read_status || (c->error_observed && c->error == ERROR_HANDLE_EOF && !c->transferred_raw)) {
    c->returned_bytes = c->transferred_raw; c->bytes_observed = 1; c->completed = 1;
    c->retained_native_io = 0;
  }
  /* Native EOF retains actual BOOL/error/count and a genuine empty capture.
   * All other no-byte failures remain unobserved, never SHA256(empty) success. */
  if (!clock_observe(c)) return 0;
  return c->completed != 0;
}
int slcli_windows_file_read_matches_original(HANDLE original,const unsigned char *output,
 DWORD requested,uint64_t offset,const struct slcli_windows_file_capture *c){
 uint64_t seconds,remainder,whole,fraction;
 if(!original||original==INVALID_HANDLE_VALUE||!output||!requested||requested>16384||
  offset>INT64_MAX||requested>(uint64_t)INT64_MAX-offset||!c||!disjoint(c,output,requested)||
  c->original!=original||c->operation!=2||!c->observed||!c->completed||
  c->retained_native_io||!c->bytes_observed||c->offset!=offset||c->requested!=requested||
  c->mode_args[0]!=(uintptr_t)original||c->mode_args[1]!=(uintptr_t)&c->mode_io||
  c->mode_args[2]!=(uintptr_t)&c->mode||c->mode_args[3]!=sizeof(c->mode)||c->mode_args[4]!=16||
  c->mode_status||c->mode_io.Status||c->mode_io.Information!=sizeof(c->mode)||!(c->mode&0x30)||
  c->position_args[0]!=(uintptr_t)original||c->position_args[1]!=offset||
  c->position_args[2]!=(uintptr_t)&c->position_returned||c->position_args[3]!=FILE_BEGIN||
  c->position_requested.QuadPart<0||(uint64_t)c->position_requested.QuadPart!=offset||
  !c->position_status||c->position_returned.QuadPart<0||(uint64_t)c->position_returned.QuadPart!=offset||
  c->read_args[0]!=(uintptr_t)original||c->read_args[1]!=(uintptr_t)output||c->read_args[2]!=requested||
  c->read_args[3]!=(uintptr_t)&c->transferred_raw||c->read_args[4]||
  c->status!=(uint64_t)(uint32_t)c->read_status||c->transferred_raw>requested||
  c->returned_bytes!=c->transferred_raw||
  (!c->read_status&&(!c->error_observed||c->error!=ERROR_HANDLE_EOF||c->transferred_raw))||
  (c->read_status&&c->error_observed)||!c->counter_status||!c->frequency_status||
  c->counter_raw.QuadPart<=0||c->frequency_raw.QuadPart<=0)return 0;
 seconds=(uint64_t)(c->counter_raw.QuadPart/c->frequency_raw.QuadPart);
 remainder=(uint64_t)(c->counter_raw.QuadPart%c->frequency_raw.QuadPart);
 if(seconds>UINT64_MAX/1000000000||remainder>UINT64_MAX/1000000000)return 0;
 whole=seconds*1000000000;fraction=remainder*1000000000/(uint64_t)c->frequency_raw.QuadPart;
 return fraction<=UINT64_MAX-whole&&whole+fraction&&c->tick==whole+fraction;
}
int slcli_windows_file_write(HANDLE original, const unsigned char *source,
                             DWORD bytes, uint64_t offset,
                             struct slcli_windows_file_capture *c) {
  if (!c || c->operation || c->retained_native_io || !original || original == INVALID_HANDLE_VALUE || !source ||
      !bytes || bytes > 16384 || offset > INT64_MAX ||
      bytes > (uint64_t)INT64_MAX - offset) return 0;
  if (!disjoint(c,source,bytes)) return 0;
  zero(&c->original,sizeof(*c)-offsetof(struct slcli_windows_file_capture,original)); c->original = original; c->operation = 3;
  c->offset = offset; c->requested = bytes; c->observed = 1;
  if (!mode_observe(c)) { clock_observe(c); return 0; }
  c->position_requested.QuadPart = (LONGLONG)offset;
  c->position_status = SetFilePointerEx(original, c->position_requested,
                                        &c->position_returned, FILE_BEGIN);
  if (!c->position_status) return error(c);
  if ((uint64_t)c->position_returned.QuadPart != offset) { clock_observe(c); return 0; }
  c->read_args[0] = (uintptr_t)original; c->read_args[1] = (uintptr_t)source;
  c->read_args[2] = bytes; c->read_args[3] = (uintptr_t)&c->transferred_raw;
  c->read_args[4] = 0;
  c->retained_native_io = 1;
  c->read_status = WriteFile(original, source, bytes, &c->transferred_raw, NULL);
  c->status = (uint64_t)(uint32_t)c->read_status;
  if (!c->read_status) { c->error = GetLastError(); c->error_observed = 1; }
  if (c->read_status && c->transferred_raw <= bytes) {
    c->returned_bytes = c->transferred_raw; c->bytes_observed = 1; c->completed = 1;
    c->retained_native_io = 0;
  }
  if (!clock_observe(c)) return 0;
  return c->completed != 0;
}
int slcli_windows_file_flush(HANDLE original, struct slcli_windows_file_capture *c) {
  BOOL result;
  if (!c || c->operation || c->retained_native_io || !original || original == INVALID_HANDLE_VALUE) return 0;
  zero(&c->original,sizeof(*c)-offsetof(struct slcli_windows_file_capture,original)); c->original = original; c->operation = 4;
  c->read_args[0] = (uintptr_t)original;
  c->observed = 1;
  c->retained_native_io = 1;
  result = FlushFileBuffers(original);
  c->status = (uint64_t)(uint32_t)result;
  if (!result) return error(c);
  c->completed = 1; c->retained_native_io = 0;
  if (!clock_observe(c)) return 0;
  return 1;
}
int slcli_windows_file_unmap_original(HANDLE original,const void *view,
 struct slcli_windows_file_capture *c){
  if(!c||c->operation||c->retained_native_io||!original||original==INVALID_HANDLE_VALUE||
    !view||!disjoint(c,view,1))return 0;
  zero(&c->original,sizeof(*c)-offsetof(struct slcli_windows_file_capture,original));c->original=original;c->operation=6;c->observed=1;
  c->view_unmap_address=(uintptr_t)view;
  c->retained_native_io=1;c->view_unmap_called=1;
  c->view_unmap_status=UnmapViewOfFile(view);
  c->view_unmap_returned=1;c->status=(uint64_t)(uint32_t)c->view_unmap_status;
  /* This synchronous return settles this call only. A failed unmap retains
   * its original address and error; it grants no alias retirement. */
  c->retained_native_io=0;
  if(!c->view_unmap_status)return error(c);
  c->completed=1;
  return clock_observe(c);
}
int slcli_windows_file_flush_mapped(HANDLE original,const void *view,SIZE_T bytes,
 struct slcli_windows_file_capture *c){
  if(!c||c->operation||c->retained_native_io||!original||original==INVALID_HANDLE_VALUE||
    !view||!bytes||!disjoint(c,view,bytes))return 0;
  zero(&c->original,sizeof(*c)-offsetof(struct slcli_windows_file_capture,original));c->original=original;c->operation=5;c->observed=1;
  c->view_flush_address=(uintptr_t)view;c->view_flush_bytes=bytes;
  c->retained_native_io=1;c->view_flush_called=1;
  c->view_flush_status=FlushViewOfFile(view,bytes);
  c->view_flush_returned=1;c->status=(uint64_t)(uint32_t)c->view_flush_status;
  if(!c->view_flush_status)return error(c);
  c->completed=1;c->retained_native_io=0;
  return clock_observe(c);
}
#endif
