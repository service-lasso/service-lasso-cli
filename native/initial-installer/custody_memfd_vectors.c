#if defined(__linux__)
#include "custody_memfd_linux.h"
/* UNRUN regression fixture. The qualified test caller supplies a genuine live
 * launcher with a zero cache-cost field in its first mapped row, disjoint fresh
 * c/ten call rows and one retained ledger byte. Its private stack is separately
 * acquired/charged. This is not a synthetic original or an admission test.
 * Native test execution remains behind the whole-bundle/ROOT/command gates. */
/* Result: -1 invalid/unexercised input, 0 regression, 1 observed denial. The
 * eventual fixed test caller must compare exactly with 1, never truthiness. */
static int apart(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&
 (x+n<=y||y+m<=x);
}
static int fresh(const void *p,size_t n) {
 const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;
}
static int prerequisite_away(const void *p,size_t n,
 const struct slcli_initial_launcher_linux *l,const unsigned char *ledger) {
 return apart(p,n,l,sizeof(*l)) && apart(p,n,ledger,1) &&
 apart(p,n,(const void *)(uintptr_t)l->preclaim.result_original,l->preclaim.metadata_bytes) &&
 apart(p,n,l->preclaim.requests_original,l->preclaim.count*sizeof(*l->preclaim.requests_original)) &&
 apart(p,n,l->identity.origin_original,sizeof(*l->identity.origin_original)) &&
 apart(p,n,l->identity.prior_original,sizeof(*l->identity.prior_original));
}
static void copy_bytes(void *out,const void *in,size_t n) {
 unsigned char *a=out;const unsigned char *b=in;size_t i;
 for(i=0;i<n;i++)a[i]=b[i];
}
static int same_bytes(const void *a,const void *b,size_t n) {
 const unsigned char *x=a,*y=b;size_t i;
 for(i=0;i<n;i++)if(x[i]!=y[i])return 0;return 1;
}
int slcli_initial_custody_live_metadata_denial_vector(
 const struct slcli_initial_launcher_linux *l,const unsigned char *ledger_byte,
 struct slcli_initial_custody_memfd_linux *c,
 struct slcli_initial_custody_call_linux *ten_fresh_rows) {
 struct slcli_initial_preclaim_row *mapped;
 struct slcli_initial_preclaim_row before_row;
 struct slcli_initial_custody_memfd_linux before_c;
 struct slcli_initial_custody_call_linux before_calls[10];
 unsigned char *zero_subobject;
 if(!l || !c || !ten_fresh_rows || !ledger_byte ||
 !slcli_initial_launcher_linux_prebirth_matches_original(l) ||
 l->preclaim.metadata_bytes<sizeof(*mapped))return -1;
 if(!apart(c,sizeof(*c),ten_fresh_rows,sizeof(before_calls)) ||
 !apart(ledger_byte,1,(const void *)(uintptr_t)l->preclaim.result_original,l->preclaim.metadata_bytes) ||
 !prerequisite_away(c,sizeof(*c),l,ledger_byte) ||
 !prerequisite_away(ten_fresh_rows,sizeof(before_calls),l,ledger_byte) ||
 !fresh(c,sizeof(*c)) || !fresh(ten_fresh_rows,sizeof(before_calls)))return -1;
 mapped=(struct slcli_initial_preclaim_row *)(uintptr_t)l->preclaim.result_original;
 if(mapped[0].request.effects.cache_ram_bytes!=0)return -1;
 zero_subobject=(unsigned char *)&mapped[0].request.effects.cache_ram_bytes;
 copy_bytes(&before_row,mapped,sizeof(before_row));
 copy_bytes(&before_c,c,sizeof(before_c));
 copy_bytes(before_calls,ten_fresh_rows,sizeof(before_calls));
 if(slcli_initial_custody_memfd_linux_capture_original(c,l,ledger_byte,1,
 zero_subobject,ten_fresh_rows,10))return 0;
 return same_bytes(&before_row,mapped,sizeof(before_row)) &&
 same_bytes(&before_c,c,sizeof(before_c)) &&
 same_bytes(before_calls,ten_fresh_rows,sizeof(before_calls));
}
#endif
