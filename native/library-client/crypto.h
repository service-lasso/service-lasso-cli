#ifndef SERVICE_LASSO_LIBRARY_CRYPTO_H
#define SERVICE_LASSO_LIBRARY_CRYPTO_H
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace slcli {
using digest256 = std::array<std::uint8_t,32>;
inline void erase_secret(void* bytes,std::size_t count){volatile std::uint8_t* target=static_cast<volatile std::uint8_t*>(bytes);while(count--)*target++=0;}
inline bool constant_equal(const std::uint8_t* a,const std::uint8_t* b,std::size_t count){std::uint8_t different=0;for(std::size_t i=0;i<count;++i)different|=a[i]^b[i];return different==0;}
class sha256_state {
 public:
  sha256_state()=default;
  sha256_state(const sha256_state&)=delete;
  sha256_state& operator=(const sha256_state&)=delete;
  ~sha256_state(){erase_secret(block_.data(),block_.size());erase_secret(state_.data(),state_.size()*sizeof(std::uint32_t));}
  void update(const void* input,std::size_t count){
    if(finished_||(!input&&count)||count>(std::numeric_limits<std::uint64_t>::max()/8)-length_)throw std::runtime_error("template_session_unavailable");
    length_+=count;const auto* bytes=static_cast<const std::uint8_t*>(input);
    while(count){std::size_t take=64-used_;if(take>count)take=count;std::memcpy(block_.data()+used_,bytes,take);used_+=take;bytes+=take;count-=take;if(used_==64){compress(block_.data());used_=0;}}
  }
  digest256 finish(){
    if(finished_)throw std::runtime_error("template_session_unavailable");const std::uint64_t bits=length_*8;block_[used_++]=0x80;
    if(used_>56){while(used_<64)block_[used_++]=0;compress(block_.data());used_=0;}while(used_<56)block_[used_++]=0;
    for(unsigned i=0;i<8;++i)block_[56+i]=static_cast<std::uint8_t>(bits>>(56-8*i));compress(block_.data());digest256 result{};
    for(unsigned i=0;i<8;++i)for(unsigned j=0;j<4;++j)result[i*4+j]=static_cast<std::uint8_t>(state_[i]>>(24-8*j));finished_=true;erase_secret(block_.data(),block_.size());erase_secret(state_.data(),state_.size()*sizeof(std::uint32_t));return result;
  }
 private:
  static std::uint32_t rotate(std::uint32_t n,unsigned bits){return(n>>bits)|(n<<(32-bits));}
  void compress(const std::uint8_t* block){
    static constexpr std::uint32_t k[64]={
      0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
      0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
      0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
      0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
      0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
      0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
      0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
      0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
    std::uint32_t w[64];for(unsigned i=0;i<16;++i)w[i]=(std::uint32_t(block[4*i])<<24)|(std::uint32_t(block[4*i+1])<<16)|(std::uint32_t(block[4*i+2])<<8)|block[4*i+3];
    for(unsigned i=16;i<64;++i){const auto a=w[i-15],b=w[i-2];w[i]=w[i-16]+(rotate(a,7)^rotate(a,18)^(a>>3))+w[i-7]+(rotate(b,17)^rotate(b,19)^(b>>10));}
    auto a=state_[0],b=state_[1],c=state_[2],d=state_[3],e=state_[4],f=state_[5],g=state_[6],h=state_[7];
    for(unsigned i=0;i<64;++i){const auto one=h+(rotate(e,6)^rotate(e,11)^rotate(e,25))+((e&f)^((~e)&g))+k[i]+w[i];const auto two=(rotate(a,2)^rotate(a,13)^rotate(a,22))+((a&b)^(a&c)^(b&c));h=g;g=f;f=e;e=d+one;d=c;c=b;b=a;a=one+two;}
    state_[0]+=a;state_[1]+=b;state_[2]+=c;state_[3]+=d;state_[4]+=e;state_[5]+=f;state_[6]+=g;state_[7]+=h;erase_secret(w,sizeof(w));
  }
  std::array<std::uint32_t,8> state_{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
  std::array<std::uint8_t,64> block_{};std::size_t used_=0;std::uint64_t length_=0;bool finished_=false;
};
inline digest256 sha256(const void* bytes,std::size_t count){sha256_state hash;hash.update(bytes,count);return hash.finish();}
class hmac256_state {
 public:
  explicit hmac256_state(const digest256& key){std::array<std::uint8_t,64> inner{};for(unsigned i=0;i<64;++i){const std::uint8_t byte=i<key.size()?key[i]:0;inner[i]=byte^0x36;outer_[i]=byte^0x5c;}inner_.update(inner.data(),inner.size());erase_secret(inner.data(),inner.size());}
  hmac256_state(const hmac256_state&)=delete;
  hmac256_state& operator=(const hmac256_state&)=delete;
  ~hmac256_state(){erase_secret(outer_.data(),outer_.size());}
  void update(const void* bytes,std::size_t count){inner_.update(bytes,count);}
  digest256 finish(){auto inside=inner_.finish();sha256_state outer;outer.update(outer_.data(),outer_.size());outer.update(inside.data(),inside.size());auto result=outer.finish();erase_secret(inside.data(),inside.size());erase_secret(outer_.data(),outer_.size());return result;}
 private:sha256_state inner_;std::array<std::uint8_t,64> outer_{};
};
}
#endif
