#ifndef SERVICE_LASSO_LIBRARY_FRAME_H
#define SERVICE_LASSO_LIBRARY_FRAME_H
#include "crypto.h"

namespace slcli {
enum class inspection_edge { sea_primary, primary_service, facade_library };
struct authenticated_frame { const std::uint8_t* body; std::uint32_t bytes; std::uint32_t sequence; };
// The private original-edge owner supplies the source-created capability/nonce
// once. Transport READ ownership/deadline and original native receipts belong to
// that owner; this class never opens, retries, reconnects or allocates a channel.
class frame_authenticator {
 public:
  frame_authenticator(inspection_edge edge, std::uint8_t direction,
      const digest256& nonce, const digest256& capability)
      : edge_(edge), direction_(direction), nonce_(nonce), capability_(capability) {
    if (direction > 1 || zero(nonce) || zero(capability)) deny();
  }
  frame_authenticator(const frame_authenticator&) = delete;
  frame_authenticator& operator=(const frame_authenticator&) = delete;
  ~frame_authenticator() { erase_secret(capability_.data(), capability_.size()); }
  authenticated_frame accept(const std::uint8_t* raw, std::size_t size) {
    if (failed_ || !raw || size < 41 || size > 16384 + 36 || sequence_ > 1025) deny();
    const auto length = get32(raw);
    if (length < 5 || length > 16384 || size != std::size_t(length) + 36 ||
        get32(raw + 5) != sequence_) deny();
    const auto actual = mac(raw + 4, length, sequence_);
    if (!constant_equal(actual.data(), raw + 4 + length, actual.size())) deny();
    authenticated_frame result{raw + 4, length, sequence_}; ++sequence_; return result;
  }
  std::size_t encode(const std::uint8_t* body, std::uint32_t length,
                     std::uint8_t* destination, std::size_t capacity) {
    if (failed_ || !body || !destination || length < 5 || length > 16384 ||
        sequence_ > 1025 || get32(body + 1) != sequence_ || capacity < std::size_t(length) + 36) deny();
    // Encoding storage is an original reserved transport buffer, never aliases
    // a received body. memmove cannot make overlap safe before the length write.
    const auto source = reinterpret_cast<std::uintptr_t>(body);
    const auto target = reinterpret_cast<std::uintptr_t>(destination);
    if (length > std::numeric_limits<std::uintptr_t>::max() - source ||
        std::size_t(length) + 36 > std::numeric_limits<std::uintptr_t>::max() - target ||
        (source < target + length + 36 && target < source + length)) deny();
    const auto digest = mac(body, length, sequence_);
    put32(destination, length); std::memcpy(destination + 4, body, length);
    std::memcpy(destination + 4 + length, digest.data(), digest.size());
    ++sequence_; return std::size_t(length) + 36;
  }
  void fail() { failed_ = true; }
 private:
  [[noreturn]] void deny() { failed_ = true; throw std::runtime_error("template_session_unavailable"); }
  static bool zero(const digest256& value) { std::uint8_t combined=0; for (auto byte:value) combined|=byte; return !combined; }
  static std::uint32_t get32(const std::uint8_t* bytes) {
    return (std::uint32_t(bytes[0]) << 24) | (std::uint32_t(bytes[1]) << 16) |
        (std::uint32_t(bytes[2]) << 8) | bytes[3];
  }
  static void put32(std::uint8_t* out, std::uint32_t value) {
    for (unsigned i=0; i<4; ++i) out[i]=static_cast<std::uint8_t>(value >> (24-8*i));
  }
  digest256 mac(const std::uint8_t* body, std::uint32_t length, std::uint32_t sequence) {
    static constexpr char sea[]="SLCLI-INSPECTION-SEA-1";
    static constexpr char primary[]="SLCLI-INSPECTION-PRIMARY-1";
    static constexpr char library[]="SLCLI-INSPECTION-LIBRARY-1";
    hmac256_state hmac(capability_);
    switch (edge_) {
      case inspection_edge::sea_primary: hmac.update(sea,sizeof(sea)); break;
      case inspection_edge::primary_service: hmac.update(primary,sizeof(primary)); break;
      case inspection_edge::facade_library: hmac.update(library,sizeof(library)); break;
      default: deny();
    }
    hmac.update(&direction_,1); hmac.update(nonce_.data(),nonce_.size());
    std::uint8_t encoded[8]; put32(encoded,sequence); put32(encoded+4,length);
    hmac.update(encoded,sizeof(encoded)); hmac.update(body,length); return hmac.finish();
  }
  inspection_edge edge_; std::uint8_t direction_; digest256 nonce_,capability_;
  std::uint32_t sequence_=0; bool failed_=false;
};
}
#endif
