#include "product_route_windows.h"
#if defined(_WIN32)
/* Pure synthetic syntax vectors ONLY. These numbers are not native handles or
 * inherited authority. No constructor/native API/source/finance is exercised. */
int slcli_product_windows_route_vectors(void) {
 struct slcli_product_windows_entry_route source={(HANDLE)(uintptr_t)9,(HANDLE)(uintptr_t)17};
 struct slcli_product_windows_entry_route out={(HANDLE)(uintptr_t)21,(HANDLE)(uintptr_t)25};
 unsigned char raw[SLCLI_PRODUCT_ROUTE_BYTES],saved[SLCLI_PRODUCT_ROUTE_BYTES];size_t i;
 if(!slcli_product_windows_route_encode(raw,sizeof(raw),&source)||
 !slcli_product_windows_route_decode(&out,raw,sizeof(raw))||out.readonly_section!=source.readonly_section||out.parent_process!=source.parent_process)return 0;
 for(i=0;i<sizeof(raw);i++)saved[i]=raw[i];
 for(i=0;i<16;i++){
 raw[i]^=0xff;out.readonly_section=(HANDLE)(uintptr_t)21;out.parent_process=(HANDLE)(uintptr_t)25;
 if(slcli_product_windows_route_decode(&out,raw,sizeof(raw))||out.readonly_section!=(HANDLE)(uintptr_t)21||out.parent_process!=(HANDLE)(uintptr_t)25)return 0;
 raw[i]=saved[i];}
 if(slcli_product_windows_route_decode(&out,raw,sizeof(raw)-1)||
 slcli_product_windows_route_decode(&out,raw,sizeof(raw)+1)||
 slcli_product_windows_route_decode((void *)raw,raw,sizeof(raw)))return 0;
 source.parent_process=source.readonly_section;
 if(slcli_product_windows_route_encode(raw,sizeof(raw),&source))return 0;
 for(i=0;i<sizeof(raw);i++)if(raw[i]!=saved[i])return 0;
 source.parent_process=INVALID_HANDLE_VALUE;
 if(slcli_product_windows_route_encode(raw,sizeof(raw),&source))return 0;
 source.parent_process=NULL;if(slcli_product_windows_route_encode(raw,sizeof(raw),&source))return 0;
 return 1;
}
#endif
