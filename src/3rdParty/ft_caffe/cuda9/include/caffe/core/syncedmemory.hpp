#ifndef __FACETHINK_CAFFE_CORE_SYNCEDMEMORY_HPP__
#define __FACETHINK_CAFFE_CORE_SYNCEDMEMORY_HPP__

#include "caffe/core/common.hpp"

namespace facethink {
  
#ifdef CPU_ONLY
#define NO_GPU BOOST_LOG_TRIVIAL(fatal) << "Cannot use GPU in CPU-only Caffe: check mode."
#endif
  
  inline void CaffeMallocHost(void** ptr, size_t size, bool* use_cuda) {  
#ifndef CPU_ONLY
    CHECK_CUDA(cudaHostAlloc(ptr, size, cudaHostAllocDefault));
    *use_cuda = true;
#else
    *ptr = malloc(size);
    *use_cuda = false;
#endif
  }

  inline void CaffeFreeHost(void* ptr, bool use_cuda){
#ifndef CPU_ONLY
    if (use_cuda){
      CHECK_CUDA(cudaFreeHost(ptr));
      return;
    }
#endif
    free(ptr);
  }


  /*
    @brief Manages memory allocation and synchronization between the host (CPU)
    and device (GPU).
  */
  class SyncedMemory {
  public:
    SyncedMemory();
    explicit SyncedMemory(size_t size);
    ~SyncedMemory();
    
    const void* cpu_data();
    void* mutable_cpu_data();
    
#ifndef CPU_ONLY
    const void* gpu_data();
    void* mutable_gpu_data();
#endif
    
    enum SyncedHead { UNINITIALIZED, HEAD_AT_CPU, HEAD_AT_GPU, SYNCED };
    SyncedHead head() { return head_; }
    
    size_t size() { return size_; }


  private:
    void to_cpu();

#ifndef CPU_ONLY
    void to_gpu();
#endif
    
    void* cpu_ptr_;
    void* gpu_ptr_;
    size_t size_;
    SyncedHead head_;
    bool own_cpu_data_;
    bool cpu_malloc_use_cuda_;
    bool own_gpu_data_;

    DISABLE_COPY_AND_ASSIGN(SyncedMemory);
  };  // class SyncedMemory

} // namespace facethink


#endif 
