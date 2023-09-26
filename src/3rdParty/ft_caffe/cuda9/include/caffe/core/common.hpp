#ifndef __FACETHINK_CAFFE_CORE_COMMON_HPP__
#define __FACETHINK_CAFFE_CORE_COMMON_HPP__

#include <boost/log/trivial.hpp>
#include <sstream>
#include <iostream>
#include <vector>

#ifndef CPU_ONLY
#include "caffe/core/common_gpu.hpp"
#else

extern "C"
{
   #include <cblas.h>
}

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

#endif

namespace facethink {

  // Disable the copy and assignment operator for a class.
#ifndef DISABLE_COPY_AND_ASSIGN
#define DISABLE_COPY_AND_ASSIGN(classname)	\
  private:					\
  classname(const classname&);			\
  classname& operator=(const classname&)
#endif

// Instantiate a class with float and double specifications.
#define INSTANTIATE_CLASS(classname) \
  template class classname<float>
//template class classname<double>

} // namespace facethink

#endif
