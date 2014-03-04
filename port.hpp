#pragma once

#if defined(__AVX__) || !defined(_MSC_VER)
#define __ALIGN__ __declspec(align(32)) 
#else
#define __ALIGN__ __declspec(align(16)) 
#endif

#if defined(_MSC_VER)

#define __RUN_TESTS__

#define __THREADS__ 7
#define OFFLOAD_DECL

#include <malloc.h>
#define allocAligned(ptr, size) ptr = _aligned_malloc(size, 32);

#define PAUSE system("PAUSE");

#else

#include <offload.h> 

#define __THREADS__ 31
#define OFFLOAD_DECL //__declspec(target(mic))

#define allocAligned(ptr, size) posix_memalign(&ptr, 32, size);

#define PAUSE 

#endif