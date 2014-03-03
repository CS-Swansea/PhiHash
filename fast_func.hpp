#pragma once

#include <immintrin.h>

#define I32B __m256i
#define STORE_I32B(ptrD, ptrS) _mm256_store_si256(ptrD, *ptrS)
#define I16B __m128i
#define STORE_I16B(ptrD, ptrS) _mm_store_si128(ptrD, *ptrS)

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
#define A_memcpy_64(dest,src)	{															\
									I32B *A_memcpy_64_bufferS = (I32B*) (src);				\
									I32B *A_memcpy_64_bufferD = (I32B*) (dest);				\
									STORE_I32B(A_memcpy_64_bufferD, A_memcpy_64_bufferS);   \
									A_memcpy_64_bufferS++;									\
									A_memcpy_64_bufferD++;									\
									STORE_I32B(A_memcpy_64_bufferD, A_memcpy_64_bufferS);	\
								}

void A_memcpy_n(void *dest, const unsigned char *src, int len);