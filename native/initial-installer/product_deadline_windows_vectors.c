#include "product_deadline_windows.h"
#if defined(_WIN32)
/* Synthetic codec geometry ONLY. No native constructor/source/profile/root/
 * object/principal/clock/funding/family is created or authenticated here. UNRUN. */
int slcli_product_windows_deadline_codec_vectors(void) {
 struct slcli_product_windows_deadline_values in={1000,900,10000000,2,7,{11,13}},out={0};
 unsigned char raw[SLCLI_PRODUCT_DEADLINE_BYTES],saved[SLCLI_PRODUCT_DEADLINE_BYTES];
 struct slcli_product_windows_deadline_values sentinel={17,19,23,29,31,{37,41}};
 size_t i;
 if(!slcli_product_windows_deadline_encode(raw,sizeof(raw),&in)||
 !slcli_product_windows_deadline_decode(&out,raw,sizeof(raw))||
 out.enclosing_expires!=1000||out.active_phase_expires!=900||out.qpc_frequency!=10000000||
 out.active_phase!=2||out.parent_pid!=7||out.parent_creation[0]!=11||out.parent_creation[1]!=13)return 0;
 for(i=0;i<sizeof(raw);i++)saved[i]=raw[i];
 /* Every tag/version/size/reserved byte belongs to a closed grammar. */
 for(i=0;i<sizeof(raw);i++)if(i<16||(i>=36&&i<40)||(i>=52&&i<56)) {
 out=sentinel;raw[i]^=0xff;
 if(slcli_product_windows_deadline_decode(&out,raw,sizeof(raw))||
 out.enclosing_expires!=sentinel.enclosing_expires||out.active_phase_expires!=sentinel.active_phase_expires||
 out.qpc_frequency!=sentinel.qpc_frequency||out.active_phase!=sentinel.active_phase||
 out.parent_pid!=sentinel.parent_pid||out.parent_creation[0]!=sentinel.parent_creation[0]||
 out.parent_creation[1]!=sentinel.parent_creation[1])return 0;raw[i]=saved[i];
 }
 out=sentinel;
 if(slcli_product_windows_deadline_decode(&out,raw,sizeof(raw)-1)||
 slcli_product_windows_deadline_decode(&out,raw,sizeof(raw)+1)||
 slcli_product_windows_deadline_decode((void *)raw,raw,sizeof(raw)))return 0;
 in.active_phase_expires=1001;
 if(slcli_product_windows_deadline_encode(raw,sizeof(raw),&in))return 0;
 for(i=0;i<sizeof(raw);i++)if(raw[i]!=saved[i])return 0;
 in.active_phase_expires=900;in.active_phase=5;
 if(slcli_product_windows_deadline_encode(raw,sizeof(raw),&in))return 0;
 in.active_phase=2;in.qpc_frequency=UINT64_MAX;
 if(slcli_product_windows_deadline_encode(raw,sizeof(raw),&in))return 0;
 in.qpc_frequency=10000000;in.parent_pid=0;
 if(slcli_product_windows_deadline_encode(raw,sizeof(raw),&in))return 0;
 in.parent_pid=7;in.parent_creation[0]=0;in.parent_creation[1]=0;
 if(slcli_product_windows_deadline_encode(raw,sizeof(raw),&in))return 0;
 return 1;
}
/* Synthetic integer boundary cases ONLY. No clock/native domain/source/caller
 * or funding is constructed by these pure no-later calculations. UNRUN. */
int slcli_product_windows_deadline_bounds_vectors(void) {
 struct slcli_product_windows_deadline_values v={1000,900,10000000,2,7,{11,13}};
 struct slcli_product_windows_deadline_bounds b={17,19,23};
 if(!slcli_product_windows_deadline_conservative_bounds(&b,&v)||b.guard_nanoseconds!=101||
 b.enclosing_expires!=899||b.active_phase_expires!=799||v.enclosing_expires!=1000||v.active_phase_expires!=900)return 0;
 v.qpc_frequency=3;v.enclosing_expires=1000000000;v.active_phase_expires=500000000;
 if(!slcli_product_windows_deadline_conservative_bounds(&b,&v)||b.guard_nanoseconds!=333333335||
 b.enclosing_expires!=666666665||b.active_phase_expires!=166666665)return 0;
 v.qpc_frequency=INT64_MAX;v.enclosing_expires=1000;v.active_phase_expires=900;
 if(!slcli_product_windows_deadline_conservative_bounds(&b,&v)||b.guard_nanoseconds!=2||
 b.enclosing_expires!=998||b.active_phase_expires!=898)return 0;
 v.qpc_frequency=1;v.active_phase_expires=1000000001;v.enclosing_expires=2000000000;
 b.guard_nanoseconds=17;b.enclosing_expires=19;b.active_phase_expires=23;
 if(slcli_product_windows_deadline_conservative_bounds(&b,&v)||b.guard_nanoseconds!=17||b.enclosing_expires!=19||b.active_phase_expires!=23)return 0;
 v.active_phase_expires=1000000002;
 if(!slcli_product_windows_deadline_conservative_bounds(&b,&v)||b.active_phase_expires!=1)return 0;
 v.active_phase_expires=v.enclosing_expires+1;b.guard_nanoseconds=17;b.enclosing_expires=19;b.active_phase_expires=23;
 if(slcli_product_windows_deadline_conservative_bounds(&b,&v)||b.guard_nanoseconds!=17||b.enclosing_expires!=19||b.active_phase_expires!=23)return 0;
 v.active_phase_expires=1000000002;v.qpc_frequency=0;
 if(slcli_product_windows_deadline_conservative_bounds(&b,&v)||b.guard_nanoseconds!=17)return 0;
 v.qpc_frequency=1;
 if(slcli_product_windows_deadline_conservative_bounds((void *)&v,&v))return 0;
 return 1;
}
#endif
