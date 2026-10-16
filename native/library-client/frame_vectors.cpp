// Source fixture only; no execution is authorized before whole-product SOURCE
// GO and the newly verified complete inputROOT. This fixture cannot qualify the
// authenticated production provider, owner semantics or native resource path.
#include "frame.h"
#include <cassert>
#ifdef NDEBUG
#error Native frame fixtures require active assertions.
#endif

namespace {
template<class F> void denied(F call) { bool rejected=false;try{call();}catch(const std::runtime_error&){rejected=true;}assert(rejected); }
void put32(std::uint8_t* bytes,std::uint32_t value) { for(unsigned i=0;i<4;++i)bytes[i]=static_cast<std::uint8_t>(value>>(24-8*i)); }
}
int main() {
  slcli::digest256 nonce{},key{};nonce[0]=1;key[0]=2;
  std::uint8_t body[8]{1,0,0,0,0,17,18,19},wire[44]{};
  slcli::frame_authenticator sender(slcli::inspection_edge::facade_library,0,nonce,key);
  const auto size=sender.encode(body,sizeof(body),wire,sizeof(wire));assert(size==sizeof(wire));
  slcli::frame_authenticator receiver(slcli::inspection_edge::facade_library,0,nonce,key);
  const auto accepted=receiver.accept(wire,size);
  assert(accepted.sequence==0&&accepted.bytes==sizeof(body)&&std::memcmp(accepted.body,body,sizeof(body))==0);
  denied([&]{receiver.accept(wire,size);});
  put32(body+1,1);sender.encode(body,sizeof(body),wire,sizeof(wire));
  // A failed sequence attempt consumes the original receiver; a later otherwise
  // valid frame cannot repair/restart its authenticated source edge.
  denied([&]{receiver.accept(wire,size);});
  slcli::frame_authenticator fresh_sender(slcli::inspection_edge::facade_library,0,nonce,key);
  put32(body+1,0);fresh_sender.encode(body,sizeof(body),wire,sizeof(wire));
  slcli::frame_authenticator wrong_direction(slcli::inspection_edge::facade_library,1,nonce,key);
  denied([&]{wrong_direction.accept(wire,size);});
  slcli::frame_authenticator wrong_domain(slcli::inspection_edge::primary_service,0,nonce,key);
  denied([&]{wrong_domain.accept(wire,size);});
  auto other_nonce=nonce;other_nonce[31]=1;
  slcli::frame_authenticator wrong_nonce(slcli::inspection_edge::facade_library,0,other_nonce,key);
  denied([&]{wrong_nonce.accept(wire,size);});
  wire[9]^=1;
  slcli::frame_authenticator changed_body(slcli::inspection_edge::facade_library,0,nonce,key);
  denied([&]{changed_body.accept(wire,size);});wire[9]^=1;
  wire[size-1]^=1;
  slcli::frame_authenticator changed_mac(slcli::inspection_edge::facade_library,0,nonce,key);
  denied([&]{changed_mac.accept(wire,size);});wire[size-1]^=1;
  slcli::frame_authenticator truncated(slcli::inspection_edge::facade_library,0,nonce,key);
  denied([&]{truncated.accept(wire,size-1);});
  slcli::frame_authenticator no_alias(slcli::inspection_edge::facade_library,0,nonce,key);
  denied([&]{no_alias.encode(wire+4,sizeof(body),wire,sizeof(wire));});
  return 0;
}
