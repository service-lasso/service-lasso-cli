#ifndef SERVICE_LASSO_LIBRARY_ARENA_H
#define SERVICE_LASSO_LIBRARY_ARENA_H
#include <node_api.h>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace slcli {
// These bounds are obtained from the independently admitted package allocator
// contract, never a JavaScript argument. Actual native process observations are
// separately required; this reservation is not a process-total measurement.
struct allocator_contract {
  std::uint64_t binding_base_bytes;
  std::uint64_t array_base_bytes;
  std::uint64_t array_entry_bytes;
  std::uint64_t object_bytes;
  std::uint64_t property_bytes;
  std::uint64_t string_base_bytes;
  std::uint64_t string_byte_factor;
  std::uint64_t external_buffer_bytes;
  std::uint64_t promise_bytes;
  std::uint64_t event_bytes;
};
struct held_file {
  const char* path;
  std::uint32_t path_bytes;
  const std::uint8_t* bytes;
  std::uint32_t byte_count;
  std::uint32_t mode;
  std::uint8_t original_sha256[32];
};
struct copy_event {
  std::uint32_t sequence;
  std::uint32_t file_count;
  std::uint64_t reserved_bytes;
  std::uint64_t cumulative_bytes;
  std::uint64_t first_storage_offset;
  std::uint64_t final_storage_offset;
  std::uint64_t first_record_offset;
  std::uint64_t final_record_offset;
};
// Original service-created mapping and ledger are private native objects. The
// authenticated native channel receives/classifies them before returning any
// JavaScript object. The observer holds its own original read-only duplicate.
// No API exports a handle, writable ledger view, capability or allocator pointer.
struct client_storage {
  std::uint8_t* arena;
  std::size_t arena_bytes;
  copy_event* events;
  std::atomic<std::uint32_t>* published_sets;
  std::size_t event_capacity;
  std::uint8_t* record_arena;
  std::size_t record_capacity;
  std::uint64_t original_parent_birth;
};

class client_arena {
 public:
  static constexpr std::uint64_t quota = 8388608;
  static constexpr std::uint32_t maximum_sets = 128;
  client_arena(const client_arena&) = delete;
  client_arena& operator=(const client_arena&) = delete;

  client_arena(client_storage storage, allocator_contract measured)
      : storage_(storage), measured_(measured) {
    if (!storage_.arena || !storage_.events || !storage_.published_sets || !storage_.record_arena ||
        !storage_.original_parent_birth || storage_.event_capacity != maximum_sets ||
        !measured_.binding_base_bytes || !measured_.external_buffer_bytes ||
        !measured_.object_bytes || !measured_.string_byte_factor ||
        storage_.arena_bytes > quota || storage_.record_capacity > quota)
      throw std::runtime_error("template_session_unavailable");
    const std::uint64_t mapped = add(storage_.arena_bytes, add(storage_.record_capacity,
        add(sizeof(copy_event) * maximum_sets, sizeof(std::atomic<std::uint32_t>))));
    if (measured_.binding_base_bytes < mapped || measured_.binding_base_bytes > quota ||
        storage_.published_sets->load(std::memory_order_acquire) != 0) deny();
    reserve(measured_.binding_base_bytes);
  }

  // Snapshot installation is called only by the original authenticated native
  // inspection receiver after END and source/catalog/owner validation. It is not
  // a N-API export accepting caller Buffer objects or caller identity assertions.
  void install_snapshot(const held_file* files, std::uint32_t count) {
    if (snapshot_ || failed_ || !files || !count || count > 128)
      deny();
    snapshot_ = files;
    snapshot_count_ = count;
  }

  // The retained snapshot remains private; returned mutable external Buffers use
  // disjoint arena storage. One complete reservation precedes every native and
  // JavaScript allocation. Any construction failure consumes the association,
  // leaves all original charge retained and returns no partial copy set.
  napi_value copies(napi_env env) {
    if (failed_ || !snapshot_ || sets_ >= maximum_sets) deny();
    std::uint64_t storage_bytes = 0;
    std::uint64_t records = sizeof(copy_event);
    std::uint64_t required = add(measured_.promise_bytes, measured_.array_base_bytes);
    for (std::uint32_t i = 0; i < snapshot_count_; ++i) {
      const held_file& file = snapshot_[i];
      if (!file.path || file.path_bytes == 0 || file.path_bytes > 240 ||
          !file.bytes || (file.mode != 0644 && file.mode != 0755)) deny();
      storage_bytes = add(storage_bytes, file.byte_count);
      records = add(records, add(52, file.path_bytes));
      required = add(required, add(file.byte_count, add(measured_.array_entry_bytes,
          add(measured_.object_bytes, add(3 * measured_.property_bytes,
          add(measured_.external_buffer_bytes, add(measured_.string_base_bytes,
          multiply(file.path_bytes, measured_.string_byte_factor))))))));
    }
    required = add(required, add(records, measured_.event_bytes));
    if (storage_bytes > storage_.arena_bytes - used_ ||
        records > storage_.record_capacity - record_used_) deny();
    reserve(required);
    const std::size_t first = used_;
    const std::size_t first_record = record_used_;
    napi_value result;
    check(napi_create_array_with_length(env, snapshot_count_, &result));
    for (std::uint32_t i = 0; i < snapshot_count_; ++i) {
      const held_file& file = snapshot_[i];
      std::uint8_t* target = storage_.arena + used_;
      std::memcpy(target, file.bytes, file.byte_count);
      used_ += file.byte_count;
      // A raw record is emitted by this actual allocator, from its private held
      // snapshot, not from caller supplied counts or digests. Big-endian fields:
      // U32 pathBytes,path,U32 mode,U32 byteCount,B32 originalHash,U64 offset.
      put32(file.path_bytes);
      put(file.path, file.path_bytes);
      put32(file.mode);
      put32(file.byte_count);
      put(file.original_sha256, 32);
      put64(static_cast<std::uint64_t>(target - storage_.arena));
      napi_value row, path, bytes, mode;
      check(napi_create_object(env, &row));
      check(napi_create_string_utf8(env, file.path, file.path_bytes, &path));
      check(napi_create_external_buffer(env, file.byte_count,
          reinterpret_cast<char*>(target), retain_until_parent_exit, nullptr, &bytes));
      check(napi_create_uint32(env, file.mode, &mode));
      check(napi_set_named_property(env, row, "path", path));
      check(napi_set_named_property(env, row, "bytes", bytes));
      check(napi_set_named_property(env, row, "mode", mode));
      check(napi_set_element(env, result, i, row));
    }
    // Publication occurs only after the entire set exists. The original native
    // service/observer reads the sequence and complete raw record before ACTION
    // can be forwarded; their held-object synchronization supplies the barrier.
    copy_event& event = storage_.events[sets_];
    event = {sets_ + 1, snapshot_count_, required, cumulative_, first, used_, first_record, record_used_};
    event_record_offsets_[sets_] = first_record;
    ++sets_;
    return result;
  }

  void reserve(std::uint64_t bytes) {
    if (failed_ || bytes > quota - cumulative_) deny();
    cumulative_ += bytes;
  }
  std::uint64_t charged_bytes() const { return cumulative_; }
  std::uint32_t issued_sets() const { return sets_; }
  bool available() const { return !failed_; }
  void consume() { failed_ = true; }

 private:
  // A GC finalizer is deliberately never a release or close observation.
  static void retain_until_parent_exit(napi_env, void*, void*) {}
  [[noreturn]] void deny() {
    failed_ = true;
    throw std::runtime_error("template_session_unavailable");
  }
  void check(napi_status status) { if (status != napi_ok) deny(); }
  std::uint64_t add(std::uint64_t a, std::uint64_t b) {
    if (b > std::numeric_limits<std::uint64_t>::max() - a) deny();
    return a + b;
  }
  std::uint64_t multiply(std::uint64_t a, std::uint64_t b) {
    if (a && b > std::numeric_limits<std::uint64_t>::max() / a) deny();
    return a * b;
  }
  void put(const void* data, std::size_t size) {
    if (size > storage_.record_capacity - record_used_) deny();
    std::memcpy(storage_.record_arena + record_used_, data, size);
    record_used_ += size;
  }
  void put32(std::uint32_t value) {
    std::uint8_t out[4] = {static_cast<std::uint8_t>(value >> 24),
        static_cast<std::uint8_t>(value >> 16), static_cast<std::uint8_t>(value >> 8),
        static_cast<std::uint8_t>(value)};
    put(out, sizeof(out));
  }
  void put64(std::uint64_t value) {
    std::uint8_t out[8];
    for (int i = 7; i >= 0; --i) { out[i] = static_cast<std::uint8_t>(value); value >>= 8; }
    put(out, sizeof(out));
  }
  client_storage storage_;
  allocator_contract measured_;
  const held_file* snapshot_ = nullptr;
  std::uint32_t snapshot_count_ = 0;
  std::uint32_t sets_ = 0;
  std::uint64_t cumulative_ = 0;
  std::size_t used_ = 0;
  std::size_t record_used_ = 0;
  std::size_t event_record_offsets_[maximum_sets] = {};
  bool failed_ = false;
};
}  // namespace slcli
#endif
