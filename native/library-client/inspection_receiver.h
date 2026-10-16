#ifndef SERVICE_LASSO_LIBRARY_INSPECTION_RECEIVER_H
#define SERVICE_LASSO_LIBRARY_INSPECTION_RECEIVER_H
#include "frame.h"
#include "inspection.h"

namespace slcli {
// The native owner reserves scratch from the original client mapping before
// BEGIN. It supplies the independently admitted catalog bounds and identities;
// no N-API caller can construct or alter these private receiver arguments.
class inspection_receiver {
 public:
  inspection_receiver(std::uint8_t* scratch, std::size_t capacity,
      const digest256& run, const digest256& nonce, const digest256& receipt,
      const digest256& catalog, std::uint32_t expected_files,
      std::uint32_t maximum_total)
      : scratch_(scratch), capacity_(capacity), run_(run), nonce_(nonce),
        receipt_(receipt), catalog_(catalog), expected_files_(expected_files),
        maximum_total_(maximum_total) {
    if (!scratch || capacity < 133 || capacity > inspection_record::maximum_bytes ||
        !expected_files || expected_files > 128 || maximum_total > inspection_record::maximum_bytes) deny();
  }
  inspection_receiver(const inspection_receiver&) = delete;
  inspection_receiver& operator=(const inspection_receiver&) = delete;
  bool accept(const authenticated_frame& frame) {
    try { return accept_original(frame); }
    catch (...) { failed_=true; throw; }
  }
  // Called only by the owning native read operation after its actual zero-byte
  // completion. END alone cannot expose the retained record. A duplicate EOF
  // or post-EOF frame consumes the association.
  void eof() { if(failed_ || !finished_ || eof_observed_)deny();eof_observed_=true; }
  const std::uint8_t* record() { if(failed_ || !finished_ || !eof_observed_)deny();return scratch_; }
  std::size_t record_bytes() { if(failed_ || !finished_ || !eof_observed_)deny();return length_; }
 private:
  bool accept_original(const authenticated_frame& frame) {
    if (failed_ || finished_ || !frame.body || frame.bytes < 5 || frame.bytes > 16384 ||
        frame.sequence != sequence_ || sequence_ > 1025) deny();
    cursor c{frame.body,frame.bytes};
    const auto operation=c.u8(); if(c.u32()!=sequence_)deny();
    if (operation==1) {
      if(sequence_!=0 || begun_)deny();
      c.identity(run_);c.identity(nonce_);c.identity(receipt_);c.identity(catalog_);
      std::memcpy(digest_.data(),c.take(32),32);
      length_=c.u32();count_=c.u32();total_=c.u32();c.end();
      if(length_<133 || length_>capacity_ || count_!=expected_files_ || total_>maximum_total_)deny();
      begun_=true;
    } else if(operation==2) {
      if(!begun_ || sequence_>1024)deny();
      c.identity(run_);c.identity(digest_);
      const auto offset=c.u32(),length=c.u32();
      if(offset!=offset_ || !length || length>16307 || length>length_-offset_)deny();
      const auto* chunk=c.take(length);c.end();
      const auto source=reinterpret_cast<std::uintptr_t>(chunk),target=reinterpret_cast<std::uintptr_t>(scratch_);
      if(length>std::numeric_limits<std::uintptr_t>::max()-source ||
          capacity_>std::numeric_limits<std::uintptr_t>::max()-target ||
          (source<target+capacity_ && target<source+length))deny();
      std::memcpy(scratch_+offset_,chunk,length);offset_+=length;
    } else if(operation==3) {
      if(!begun_ || offset_!=length_)deny();
      c.identity(run_);c.identity(digest_);
      if(c.u32()!=length_ || c.u32()!=count_ || c.u32()!=total_)deny();c.end();
      // Validate the complete original record before reporting END. Header and
      // announced count/total must agree; raw digest equality alone is inadequate.
      inspection_record record(scratch_,length_,run_,nonce_,receipt_,catalog_,digest_);
      if(record.file_count()!=count_ || record.total_bytes()!=total_)deny();
      finished_=true;
    } else deny();
    ++sequence_;return finished_;
  }
  [[noreturn]] void deny(){failed_=true;throw std::runtime_error("template_session_unavailable");}
  struct cursor {
    const std::uint8_t* bytes;std::size_t count,offset=0;
    [[noreturn]] static void deny(){throw std::runtime_error("template_session_unavailable");}
    const std::uint8_t* take(std::size_t size){if(size>count-offset)deny();const auto* result=bytes+offset;offset+=size;return result;}
    std::uint8_t u8(){return *take(1);}
    std::uint32_t u32(){const auto* b=take(4);return(std::uint32_t(b[0])<<24)|(std::uint32_t(b[1])<<16)|(std::uint32_t(b[2])<<8)|b[3];}
    void identity(const digest256& expected){if(!constant_equal(take(32),expected.data(),32))deny();}
    void end(){if(offset!=count)deny();}
  };
  std::uint8_t* scratch_;std::size_t capacity_;digest256 run_,nonce_,receipt_,catalog_,digest_{};
  std::uint32_t expected_files_,maximum_total_,sequence_=0,length_=0,count_=0,total_=0,offset_=0;
  bool begun_=false,finished_=false,eof_observed_=false,failed_=false;
};
}
#endif
