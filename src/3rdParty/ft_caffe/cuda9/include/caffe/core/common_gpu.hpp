#ifndef __FACETHINK_CAFFE_CORE_COMMON_GPU_HPP__
#define __FACETHINK_CAFFE_CORE_COMMON_GPU_HPP__

#include <cudnn.h>
#include <cublas_v2.h>
#include <curand.h>

#include <cuda.h>
#include <cuda_runtime.h>
#include <driver_types.h>  // cuda driver types

#define BOOST_PYTHON_STATIC_LIB

#ifdef WIN32
#ifdef DLL_EXPORTS
#define EXPORT_CLASS   __declspec(dllexport)
#define EXPORT_API  extern "C" __declspec(dllexport)
#else
#define EXPORT_CLASS   __declspec(dllimport )
#define EXPORT_API  extern "C" __declspec(dllimport )
#endif
#else
#define EXPORT_CLASS
#define EXPORT_API
#endif

namespace facethink {

	// Disable the copy and assignment operator for a class.
#define DISABLE_COPY_AND_ASSIGN(classname)	\
  private:					\
  classname(const classname&);			\
  classname& operator=(const classname&)

#define FatalError(s) do {						\
    std::stringstream _where, _message;					\
    _where << __FILE__ << ':' << __LINE__;				\
    _message << std::string(s) + "\n" << __FILE__ << ':' << __LINE__;	\
    std::cerr << _message.str() << "\nAborting...\n";			\
    cudaDeviceReset();							\
    exit(1);								\
  } while(0)


#define CHECK_CUDNN(status) do {					\
    std::stringstream _error;						\
    if (status != CUDNN_STATUS_SUCCESS) {				\
      _error << "CUDNN failure: " << cudnnGetErrorString(status);	\
      FatalError(_error.str());						\
    }									\
  } while(0)

#define CHECK_CUDA(status) do {			\
    std::stringstream _error;			\
    if (status != 0) {				\
      _error << "CUDA failure: " << status;	\
      FatalError(_error.str());			\
    }						\
  } while(0)

#define CHECK_CUBLAS(status) do {		\
    std::stringstream _error;			\
    if (status != CUBLAS_STATUS_SUCCESS){	\
      _error << "CUBLAS failure: " << status;	\
      FatalError(_error.str());			\
    }						\
  } while (0)


	// CUDA: various checks for different function calls.
#define CUDA_CHECK(condition) \
  /* Code block avoids redefinition of cudaError_t error */ \
  do { \
     std::stringstream _error;	 \
    cudaError_t error = condition; \
    /* CHECK_EQ(error, cudaSuccess) << " " << cudaGetErrorString(error); \ */ \
   if (error != cudaSuccess) {  \
		_error << " " << cudaGetErrorString(error);	\
		FatalError(_error.str());						\
     }                                             \
  } while (0)

#define CUBLAS_CHECK(condition) \
  do { \
     std::stringstream _error;	 \
    cublasStatus_t status = condition; \
   /* CHECK_EQ(status, CUBLAS_STATUS_SUCCESS) << " " \
	      << caffe::cublasGetErrorString(status); \ */  \
   if (status != CUBLAS_STATUS_SUCCESS) {  \
		_error << " " << facethink::cublasGetErrorString(status);	\
		FatalError(_error.str());						\
     }                                        \
  } while (0)

#define CURAND_CHECK(condition) \
  do { \
     std::stringstream _error;	 \
    curandStatus_t status = condition; \
   /*  CHECK_EQ(status, CURAND_STATUS_SUCCESS) << " " \
	      << caffe::curandGetErrorString(status); \  */  \
   if (status != CURAND_STATUS_SUCCESS) {  \
		_error << " " << facethink::curandGetErrorString(status);	\
		FatalError(_error.str());						\
     }                                        \
  } while (0)

	// CUDA: grid stride looping
#define CUDA_KERNEL_LOOP(i, n) \
  for (int i = blockIdx.x * blockDim.x + threadIdx.x; \
       i < (n); \
       i += blockDim.x * gridDim.x)

	// CUDA: check for error after kernel execution and exit loudly if there is one.
#define CUDA_POST_KERNEL_CHECK CUDA_CHECK(cudaPeekAtLastError())

  
const cudnnDataType_t cudnnDataType = CUDNN_DATA_FLOAT;
const cudnnTensorFormat_t cudnnTensorFormat = CUDNN_TENSOR_NCHW;
// Specify workspace limit for kernels directly until we have a
// planning strategy and a rewrite of Caffe's GPU memory mangagement
const size_t cudnn_workspace_limit_bytes = 4*1024*1024;

// CUDA: use 512 threads per block
const int CAFFE_CUDA_NUM_THREADS = 512;

// CUDA: number of blocks for threads.
inline int CAFFE_GET_BLOCKS(const int N) {
  return (N + CAFFE_CUDA_NUM_THREADS - 1) / CAFFE_CUDA_NUM_THREADS;
}

// CUDA: grid stride looping
#define CUDA_KERNEL_LOOP(i, n)			        \
  for (int i = blockIdx.x * blockDim.x + threadIdx.x;	\
       i < (n);						\
       i += blockDim.x * gridDim.x)

// CUDA: check for error after kernel execution and exit loudly if there is one.
#define CUDA_POST_KERNEL_CHECK CHECK_CUDA(cudaPeekAtLastError())

  
#define INSTANTIATE_LAYER_GPU_FORWARD(classname) \
  template void classname<float>::ForwardComputation()


// CUDA: library error reporting.
const char* cublasGetErrorString(cublasStatus_t error);
const char* curandGetErrorString(curandStatus_t error);

// A singleton class to hold common caffe stuff, such as the handler that
// caffe is going to use for cublas, curand, etc.
class EXPORT_CLASS Caffe {
public:
	~Caffe();

	// Thread local context for Caffe. Moved to common.cpp instead of
	// including boost/thread.hpp to avoid a boost/NVCC issues (#1009, #1010)
	// on OSX. Also fails on Linux with CUDA 7.0.18.
	static Caffe& Get();

	enum Brew { CPU, GPU };

	// This random number generator facade hides boost and CUDA rng
	// implementation from one another (for cross-platform compatibility).
	class EXPORT_CLASS RNG {
	public:
		RNG();
		explicit RNG(unsigned int seed);
		explicit RNG(const RNG&);
		RNG& operator=(const RNG&);
		void* generator();
	private:
		class Generator;
		std::shared_ptr<Generator> generator_;
	};

	// Getters for boost rng, curand, and cublas handles
	inline static RNG& rng_stream() {
		if (!Get().random_generator_) {
			Get().random_generator_.reset(new RNG());
		}
		return *(Get().random_generator_);
	}
#ifndef CPU_ONLY
	inline static cublasHandle_t cublas_handle() { return Get().cublas_handle_; }
	inline static curandGenerator_t curand_generator() {
		return Get().curand_generator_;
	}
#endif

	// Returns the mode: running on CPU or GPU.
	inline static Brew mode() { return Get().mode_; }
	// The setters for the variables
	// Sets the mode. It is recommended that you don't change the mode halfway
	// into the program since that may cause allocation of pinned memory being
	// freed in a non-pinned way, which may cause problems - I haven't verified
	// it personally but better to note it here in the header file.
	inline static void set_mode(Brew mode) { Get().mode_ = mode; }
	// Sets the random seed of both boost and curand
	static void set_random_seed(const unsigned int seed);
	// Sets the device. Since we have cublas and curand stuff, set device also
	// requires us to reset those values.
	static void SetDevice(const int device_id);
	// Prints the current GPU status.
	static void DeviceQuery();
	// Check if specified device is available
	static bool CheckDevice(const int device_id);
	// Search from start_id to the highest possible device ordinal,
	// return the ordinal of the first available device.
	static int FindDevice(const int start_id = 0);
	// Parallel training
	inline static int solver_count() { return Get().solver_count_; }
	inline static void set_solver_count(int val) { Get().solver_count_ = val; }
	inline static int solver_rank() { return Get().solver_rank_; }
	inline static void set_solver_rank(int val) { Get().solver_rank_ = val; }
	inline static bool multiprocess() { return Get().multiprocess_; }
	inline static void set_multiprocess(bool val) { Get().multiprocess_ = val; }
	inline static bool root_solver() { return Get().solver_rank_ == 0; }

protected:
#ifndef CPU_ONLY
	cublasHandle_t cublas_handle_;
	curandGenerator_t curand_generator_;
#endif
	std::shared_ptr<RNG> random_generator_;

	Brew mode_;

	// Parallel training
	int solver_count_;
	int solver_rank_;
	bool multiprocess_;

private:
	// The private constructor to avoid duplicate instantiation.
	Caffe();

	DISABLE_COPY_AND_ASSIGN(Caffe);
};

} // namespace facethink

#endif
