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
static int source_rights_observe(struct slcli_windows_file_capture *c) {
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
  if((access&0x00120081)!=0x00120081||(access&forbidden)) return 0;
  c->handle_args[0]=(uintptr_t)c->original;
  c->handle_args[1]=(uintptr_t)&c->handle_flags;
  c->handle_status=GetHandleInformation(c->original,&c->handle_flags);
  if(!c->handle_status) {c->error=GetLastError();c->error_observed=1;return 0;}
  return !(c->handle_flags&HANDLE_FLAG_INHERIT);
}
static int file_observe(HANDLE original,
                         struct slcli_windows_file_capture *c,int source_read_only) {
  uintptr_t start, sid;
  DWORD bytes, i;
  if (!c || c->retained_native_io || !original || original == INVALID_HANDLE_VALUE) return 0;
  zero(c, sizeof(*c)); c->original = original; c->operation = 1;
  c->observed = 1;
  if(source_read_only&&!source_rights_observe(c)) { clock_observe(c); return 0; }
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
  if (!clock_observe(c) || c->standard.Directory || c->standard.DeletePending ||
      c->standard.EndOfFile.QuadPart < 0 ||
      c->attributes.FileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT))
    return 0;
  c->completed = 1;
  return 1;
}
int slcli_windows_file_observe(HANDLE original,struct slcli_windows_file_capture *c) {
  return file_observe(original,c,0);
}
int slcli_windows_source_observe(HANDLE original,struct slcli_windows_file_capture *c) {
  return file_observe(original,c,1);
}
int slcli_windows_file_read(HANDLE original, unsigned char *destination,
                            DWORD bytes, uint64_t offset,
                            struct slcli_windows_file_capture *c) {
  if (!c || c->retained_native_io || !original || original == INVALID_HANDLE_VALUE || !destination ||
      !bytes || bytes > 16384 || offset > INT64_MAX || bytes > (uint64_t)INT64_MAX - offset)
    return 0;
  if (!disjoint(c,destination,bytes)) return 0;
  zero(c, sizeof(*c)); c->original = original; c->operation = 2;
  c->offset = offset; c->requested = bytes; c->observed = 1;
  if (!mode_observe(c)) { clock_observe(c); return 0; }
  c->position_requested.QuadPart = (LONGLONG)offset;
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
int slcli_windows_file_write(HANDLE original, const unsigned char *source,
                             DWORD bytes, uint64_t offset,
                             struct slcli_windows_file_capture *c) {
  if (!c || c->retained_native_io || !original || original == INVALID_HANDLE_VALUE || !source ||
      !bytes || bytes > 16384 || offset > INT64_MAX ||
      bytes > (uint64_t)INT64_MAX - offset) return 0;
  if (!disjoint(c,source,bytes)) return 0;
  zero(c, sizeof(*c)); c->original = original; c->operation = 3;
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
  if (!c || c->retained_native_io || !original || original == INVALID_HANDLE_VALUE) return 0;
  zero(c, sizeof(*c)); c->original = original; c->operation = 4;
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
#endif
