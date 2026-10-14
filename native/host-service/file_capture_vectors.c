#include "input_body.h"
/* Source-only rejection vectors. No genuine file/native acceptance is inferred.
 * Actual execution requires the different entire source/tool/input admission.
 * Supplied aligned capture and snapshot storage must already be owner charged;
 * large raw Windows structures are never free test/production stack storage. */
#if defined(_WIN32) || defined(__linux__)
int slcli_file_capture_reuse_vectors(slcli_input_capture *capture,
                                      slcli_input_capture *snapshot) {
  unsigned char output=0x5a;
  const unsigned char *a,*b;
  size_t i;
  uintptr_t x=(uintptr_t)capture,y=(uintptr_t)snapshot;
  if(!capture||!snapshot||x%_Alignof(slcli_input_capture)||
     y%_Alignof(slcli_input_capture)||x>UINTPTR_MAX-sizeof(*capture)||
     y>UINTPTR_MAX-sizeof(*snapshot)||
     (x<y+sizeof(*snapshot)&&y<x+sizeof(*capture))) return 0;
  slcli_erase(capture,sizeof(*capture));
  capture->operation=2;
  capture->tick=123;
#if defined(_WIN32)
  capture->completed=1; capture->returned_bytes=7;
#else
  capture->observed=1; capture->returned_bytes=7;
  capture->returned_bytes_observed=1;
#endif
  a=(const unsigned char *)capture;
  for(i=0;i<sizeof(*capture);++i) ((unsigned char *)snapshot)[i]=a[i];
#if defined(_WIN32)
  if(slcli_windows_file_observe((HANDLE)(uintptr_t)1,capture)||
     slcli_windows_source_observe((HANDLE)(uintptr_t)1,capture)||
     slcli_windows_file_read((HANDLE)(uintptr_t)1,&output,1,0,capture)||
     slcli_windows_file_write((HANDLE)(uintptr_t)1,&output,1,0,capture)||
     slcli_windows_file_flush((HANDLE)(uintptr_t)1,capture)) return 0;
#else
  if(slcli_linux_file_observe(0,capture)||
     slcli_linux_file_read(0,&output,1,0,capture)||
     slcli_linux_file_write(0,&output,1,0,capture)||
     slcli_linux_file_flush(0,capture)) return 0;
#endif
  if(output!=0x5a) return 0;
  b=(const unsigned char *)snapshot;
  for(i=0;i<sizeof(*capture);++i) if(a[i]!=b[i]) return 0;
  return 1;
}
#endif
