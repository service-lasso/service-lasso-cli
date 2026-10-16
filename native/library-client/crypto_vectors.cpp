// Source fixture only. Executing/compiling this file requires the different
// whole-product SOURCE GO and the new complete inputROOT.
#include "crypto.h"
#include <cassert>
#ifdef NDEBUG
#error Native vector fixtures require active assertions.
#endif

namespace {
slcli::digest256 hex(const char* text) {
  slcli::digest256 result{};
  for (unsigned i=0;i<32;++i) {
    auto digit=[](char c)->unsigned { if(c>='0'&&c<='9')return c-'0';if(c>='a'&&c<='f')return c-'a'+10;assert(false);return 0; };
    result[i]=static_cast<std::uint8_t>((digit(text[2*i])<<4)|digit(text[2*i+1]));
  }
  assert(text[64]=='\0');return result;
}
void equal(const slcli::digest256& actual,const char* expected) {
  auto wanted=hex(expected);assert(slcli::constant_equal(actual.data(),wanted.data(),32));
}
}
int main() {
  equal(slcli::sha256(nullptr,0),"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
  equal(slcli::sha256("abc",3),"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
  const char long_message[]="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
  equal(slcli::sha256(long_message,sizeof(long_message)-1),"248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
  slcli::sha256_state split;
  for(std::size_t i=0;i<sizeof(long_message)-1;++i)split.update(long_message+i,1);
  equal(split.finish(),"248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
  bool refused=false;try{split.update("x",1);}catch(const std::runtime_error&){refused=true;}assert(refused);
  slcli::digest256 key{};
  // RFC 4231 section 4.2, https://www.rfc-editor.org/rfc/rfc4231
  // Zero padding the 20-byte key to 32 bytes preserves its HMAC key block.
  std::memset(key.data(),0x0b,20);
  slcli::hmac256_state hmac(key);
  const char message[]="Hi There";
  hmac.update(message,sizeof(message)-1);
  equal(hmac.finish(),"b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7");
  auto a=hex("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
  auto b=a;b[31]^=1;assert(!slcli::constant_equal(a.data(),b.data(),32));
  return 0;
}
