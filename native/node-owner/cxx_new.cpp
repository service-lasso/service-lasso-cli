#include "early_allocator.h"
#include <new>
#include <cstddef>
/* Compiled into the source-built full Node image, ahead of C++ static
 * constructors. The actual preentry owner must bind before the first call.
 * There is no CRT/system allocator fallback or late builtin initialization. */
static void *allocate(std::size_t bytes,std::size_t alignment) {
  for(;;) {
    if(void *p=slcli_node_aligned_alloc(alignment,bytes))return p;
    std::new_handler handler=std::get_new_handler();
    std::size_t ordinal;
    if(!handler||!slcli_node_allocator_handler_before_original(handler,&ordinal))throw std::bad_alloc();
    try { handler(); }
    catch(...) {
      slcli_node_allocator_handler_after_original(handler,ordinal,1);
      throw;
    }
    if(!slcli_node_allocator_handler_after_original(handler,ordinal,0))throw std::bad_alloc();
  }
}
void *operator new(std::size_t n){return allocate(n,alignof(std::max_align_t));}
void *operator new[](std::size_t n){return allocate(n,alignof(std::max_align_t));}
void *operator new(std::size_t n,const std::nothrow_t &) noexcept {try{return ::operator new(n);}catch(...){return nullptr;}}
void *operator new[](std::size_t n,const std::nothrow_t &) noexcept {try{return ::operator new[](n);}catch(...){return nullptr;}}
void operator delete(void *p) noexcept {slcli_node_free(p);}
void operator delete[](void *p) noexcept {slcli_node_free(p);}
void operator delete(void *p,std::size_t) noexcept {slcli_node_free(p);}
void operator delete[](void *p,std::size_t) noexcept {slcli_node_free(p);}
void operator delete(void *p,const std::nothrow_t &) noexcept {slcli_node_free(p);}
void operator delete[](void *p,const std::nothrow_t &) noexcept {slcli_node_free(p);}
void *operator new(std::size_t n,std::align_val_t a){return allocate(n,static_cast<std::size_t>(a));}
void *operator new[](std::size_t n,std::align_val_t a){return allocate(n,static_cast<std::size_t>(a));}
void *operator new(std::size_t n,std::align_val_t a,const std::nothrow_t &) noexcept {try{return ::operator new(n,a);}catch(...){return nullptr;}}
void *operator new[](std::size_t n,std::align_val_t a,const std::nothrow_t &) noexcept {try{return ::operator new[](n,a);}catch(...){return nullptr;}}
void operator delete(void *p,std::align_val_t) noexcept {slcli_node_free(p);}
void operator delete[](void *p,std::align_val_t) noexcept {slcli_node_free(p);}
void operator delete(void *p,std::size_t,std::align_val_t) noexcept {slcli_node_free(p);}
void operator delete[](void *p,std::size_t,std::align_val_t) noexcept {slcli_node_free(p);}
void operator delete(void *p,std::align_val_t,const std::nothrow_t &) noexcept {slcli_node_free(p);}
void operator delete[](void *p,std::align_val_t,const std::nothrow_t &) noexcept {slcli_node_free(p);}
