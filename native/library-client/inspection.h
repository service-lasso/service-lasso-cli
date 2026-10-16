#ifndef SERVICE_LASSO_LIBRARY_INSPECTION_H
#define SERVICE_LASSO_LIBRARY_INSPECTION_H
#include "arena.h"

namespace slcli {
// This reader only borrows the authenticated original record. Its fixed result
// table and record storage are charged by the native receiver before any READ.
// Policy/catalog admission is performed against these private borrowed bytes
// before client_arena installs its own immutable snapshot.
class inspection_record {
 public:
  static constexpr std::size_t maximum_bytes = 768000;
  inspection_record(const std::uint8_t* record, std::size_t count,
      const digest256& run, const digest256& launch_nonce,
      const digest256& receipt, const digest256& catalog,
      const digest256& expected_digest)
      : raw_(record), count_(count) {
    if (!record || count < 133 || count > maximum_bytes) deny();
    static constexpr char domain[] = "SLCLI-INSPECTION-RECORD-1";
    sha256_state digest;
    digest.update(domain, sizeof(domain));
    digest.update(record, count);
    const auto actual = digest.finish();
    if (!constant_equal(actual.data(), expected_digest.data(), actual.size()) || take8() != 1) deny();
    identity(run); identity(launch_nonce); identity(receipt); identity(catalog);
    files_count_ = take32();
    if (!files_count_ || files_count_ > 128) deny();
    for (std::uint32_t i = 0; i < files_count_; ++i) {
      auto& file = files_[i];
      file.path_bytes = take32();
      if (!file.path_bytes || file.path_bytes > 240) deny();
      file.path = reinterpret_cast<const char*>(take(file.path_bytes));
      path(file.path, file.path_bytes);
      for (std::uint32_t prior = 0; prior < i; ++prior)
        if (!compatible(files_[prior], file)) deny();
      if (i && compare(files_[i-1].path, files_[i-1].path_bytes,
                       file.path, file.path_bytes) >= 0) deny();
      const auto mode = take8();
      if (mode > 1) deny();
      file.mode = mode ? 0755 : 0644;
      file.byte_count = take32();
      if (file.byte_count > maximum_bytes - total_) deny();
      total_ += file.byte_count;
      file.bytes = take(file.byte_count);
      const auto digest = sha256(file.bytes, file.byte_count);
      std::memcpy(file.original_sha256, digest.data(), digest.size());
    }
    if (offset_ != count_) deny();
  }
  const held_file* files() const { return files_; }
  std::uint32_t file_count() const { return files_count_; }
  std::uint32_t total_bytes() const { return total_; }
 private:
  [[noreturn]] static void deny() { throw std::runtime_error("template_session_unavailable"); }
  const std::uint8_t* take(std::size_t count) {
    if (count > count_ - offset_) deny();
    const auto* result = raw_ + offset_; offset_ += count; return result;
  }
  std::uint8_t take8() { return *take(1); }
  std::uint32_t take32() {
    const auto* bytes = take(4);
    return (std::uint32_t(bytes[0]) << 24) | (std::uint32_t(bytes[1]) << 16) |
        (std::uint32_t(bytes[2]) << 8) | bytes[3];
  }
  void identity(const digest256& expected) {
    const auto* bytes = take(expected.size());
    if (!constant_equal(bytes, expected.data(), expected.size())) deny();
  }
  static int compare(const char* a, std::size_t a_count, const char* b, std::size_t b_count) {
    const auto length = a_count < b_count ? a_count : b_count;
    const int order = std::memcmp(a, b, length);
    if (order) return order;
    return a_count < b_count ? -1 : a_count > b_count ? 1 : 0;
  }
  static unsigned char fold(unsigned char value) {
    return value >= 'A' && value <= 'Z' ? value + ('a'-'A') : value;
  }
  static bool compatible(const held_file& a, const held_file& b) {
    std::size_t ai = 0, bi = 0;
    while (ai < a.path_bytes && bi < b.path_bytes) {
      const auto as = ai, bs = bi;
      while (ai < a.path_bytes && a.path[ai] != '/') ++ai;
      while (bi < b.path_bytes && b.path[bi] != '/') ++bi;
      const auto an = ai-as, bn = bi-bs;
      if (an != bn) return true;
      bool exact = true, folded = true;
      for (std::size_t i = 0; i < an; ++i) {
        exact = exact && a.path[as+i] == b.path[bs+i];
        folded = folded && fold(static_cast<unsigned char>(a.path[as+i])) ==
            fold(static_cast<unsigned char>(b.path[bs+i]));
      }
      if (!folded) return true;
      if (!exact) return false;
      // A file cannot also be a parent directory in the same held inventory.
      if (ai == a.path_bytes || bi == b.path_bytes) return false;
      ++ai; ++bi;
    }
    return false;
  }
  static void path(const char* bytes, std::size_t count) {
    // Paths are canonical relative ASCII slash paths, with no drive, alternate
    // separator, empty/dot segment or whitespace/control ambiguity.
    std::size_t start = 0, depth = 0;
    for (std::size_t i = 0; i <= count; ++i) {
      if (i != count) {
        const auto byte = static_cast<unsigned char>(bytes[i]);
        if (!((byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z') ||
              (byte >= '0' && byte <= '9') || byte == '_' || byte == '.' ||
              byte == '@' || byte == '/' || byte == '-')) deny();
        if (byte != '/') continue;
      }
      const auto length = i - start;
      if (!length || (length == 1 && bytes[start] == '.') ||
          (length == 2 && bytes[start] == '.' && bytes[start+1] == '.') ||
          bytes[i-1] == '.' || ++depth > 12) deny();
      std::size_t base = 0;
      while (base < length && bytes[start+base] != '.') ++base;
      char prefix[4]{};
      if (base <= sizeof(prefix)) {
        for (std::size_t j = 0; j < base; ++j) {
          const char byte = bytes[start+j];
          prefix[j] = byte >= 'A' && byte <= 'Z' ? byte + ('a'-'A') : byte;
        }
        if ((base == 3 && (!std::memcmp(prefix,"con",3) || !std::memcmp(prefix,"prn",3) ||
                          !std::memcmp(prefix,"aux",3) || !std::memcmp(prefix,"nul",3))) ||
            (base == 4 && (!std::memcmp(prefix,"com",3) || !std::memcmp(prefix,"lpt",3)) &&
                         prefix[3] >= '1' && prefix[3] <= '9')) deny();
      }
      start = i + 1;
    }
  }
  const std::uint8_t* raw_;
  std::size_t count_, offset_ = 0;
  held_file files_[128]{};
  std::uint32_t files_count_ = 0, total_ = 0;
};
}
#endif
