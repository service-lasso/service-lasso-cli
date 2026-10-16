/* Finite byte-extent arithmetic only, no fabricated native reads/EOF/owners. */
#include "streams_windows.h"
#if defined(_WIN32)
int slcli_build_streams_windows_vectors(void) {
 size_t n=0;
 if(!slcli_build_stream_windows_read_extent_original(0,32768,50000,&n)||n!=16384)return 0;
 if(!slcli_build_stream_windows_read_extent_original(32760,32768,8,&n)||n!=8)return 0;
 if(!slcli_build_stream_windows_read_extent_original(SIZE_MAX-8,SIZE_MAX,8,&n)||n!=8)return 0;
 n=77;if(slcli_build_stream_windows_read_extent_original(32760,32768,9,&n)||n!=77)return 0;
 if(slcli_build_stream_windows_read_extent_original(32768,32768,1,&n)||n!=77)return 0;
 if(slcli_build_stream_windows_read_extent_original(SIZE_MAX,0,1,&n)||n!=77)return 0;
 if(slcli_build_stream_windows_read_extent_original(0,32768,0,&n)||n!=77)return 0;
 if(slcli_build_stream_windows_read_extent_original(0,32768,1,NULL))return 0;
 return 1;
}
#endif
