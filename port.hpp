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


#define __USE_OFFLOAD__ 1
#include <offload.h> 

#define __THREADS__ 120

#define ALLOC alloc_if(1) free_if(0)
#define REUSE alloc_if(0) free_if(0)
#define FREE  alloc_if(0) free_if(1)

#define OFFLOAD_DECL __attribute__ ((target(mic:0)))

#define allocAligned(ptr, size) posix_memalign(&ptr, 32, size);

#define PAUSE 

#endif