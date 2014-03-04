#pragma once

#include <immintrin.h>
#include "port.hpp"

#define I16B __m128i
#define STORE_I16B(ptrD, ptrS) _mm_store_si128(ptrD, *ptrS)

#if false //defined(__AVX__) || !defined(_MSC_VER)

#define I32B __m256i
#define STORE_I32B(ptrD, ptrS) _mm256_store_si256(ptrD, *ptrS)

/**
 * Copies the content of one 32 byte aligned 
 * 64 byte buffer into another
 *
 * @param dest
 *		A pointer to the destination buffer
 *
 * @param src
 *		A pointer to the source buffer
 */
#define A_memcpy_64(dest,src)	{																\
									I32B *A_memcpy_32_bufferS = (I32B*) (src);					\
									I32B *A_memcpy_32_bufferD = (I32B*) (dest);					\
									STORE_I32B(A_memcpy_32_bufferD, A_memcpy_32_bufferS);		\
									A_memcpy_32_bufferS++;										\
									A_memcpy_32_bufferD++;										\
									STORE_I32B(A_memcpy_32_bufferD, A_memcpy_32_bufferS);		\
								}

/**
* Copies the content of one 32 byte aligned
* byte buffer of length n into another using AVX
*
* @param dest
*		A pointer to the destination buffer
*
* @param src
*		A pointer to the source buffer
*
* @param len
*		The number of bytes to copy
*/
OFFLOAD_DECL
void A_memcpy_n(void *dest, const unsigned char *src, int len);

#else

/**
* Copies the content of one 16 byte aligned
* 64 byte buffer into another using SSE
*
* @param dest
*		A pointer to the destination buffer
*
* @param src
*		A pointer to the source buffer
*/
#define A_memcpy_64(dest,src)	{																\
									I16B *A_memcpy_16_bufferS = (I16B*) (src);					\
									I16B *A_memcpy_16_bufferD = (I16B*) (dest);					\
									STORE_I16B(A_memcpy_16_bufferD, A_memcpy_16_bufferS);		\
									A_memcpy_16_bufferS++;										\
									A_memcpy_16_bufferD++;										\
									STORE_I16B(A_memcpy_16_bufferD, A_memcpy_16_bufferS);		\
									A_memcpy_16_bufferS++;										\
									A_memcpy_16_bufferD++;										\
									STORE_I16B(A_memcpy_16_bufferD, A_memcpy_16_bufferS);		\
									A_memcpy_16_bufferS++;										\
									A_memcpy_16_bufferD++;										\
									STORE_I16B(A_memcpy_16_bufferD, A_memcpy_16_bufferS);		\
								}

/**
* Fallback for systems not equipt with AVX instructions
*
* @param dest
*		A pointer to the destination buffer
*
* @param src
*		A pointer to the source buffer
*
* @param len
*		The number of bytes to copy
*/
#define A_memcpy_n(dest,src,n) memcpy(dest,src,n)

#endif