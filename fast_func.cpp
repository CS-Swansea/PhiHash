#include "fast_func.hpp"

#if defined(__AVX__) || !defined(_MSC_VER)

void A_memcpy_n(void *dest, const unsigned char *src, int len) {

	I32B *A_memcpy_32_bufferS = (I32B*) (src);
	I32B *A_memcpy_32_bufferD = (I32B*) (dest);

	while (len >= 32) {
		STORE_I32B(A_memcpy_32_bufferD, A_memcpy_32_bufferS);
		A_memcpy_32_bufferS++;
		A_memcpy_32_bufferD++;

		_mm_prefetch((char*) (A_memcpy_32_bufferS), _MM_HINT_T2);
		len -= 32;
	}

	char *ptrS = (char*) A_memcpy_32_bufferS;
	char *ptrD = (char*) A_memcpy_32_bufferD;

	if (len >= 16) {
		STORE_I16B((I16B*) ptrD, (I16B*) ptrS);
		ptrS += 16;
		ptrD += 16;

		len -= 16;
	}

	for (int i = 0; i < len; i++) {
		ptrD[i] = ptrS[i];
	}

};

#else

/*
void A_memcpy_n(void *dest, const unsigned char *src, int len) {

	I16B *A_memcpy_16_bufferS = (I16B*) (src);
	I16B *A_memcpy_16_bufferD = (I16B*) (dest);

	while (len >= 16) {
		STORE_I16B(A_memcpy_16_bufferD, A_memcpy_16_bufferS);
		A_memcpy_16_bufferS++;
		A_memcpy_16_bufferD++;

		_mm_prefetch((char*) (A_memcpy_16_bufferS), _MM_HINT_T2);
		len -= 16;
	}

	char *ptrS = (char*) A_memcpy_16_bufferS;
	char *ptrD = (char*) A_memcpy_16_bufferD;

	for (int i = 0; i < len; i++) {
		ptrD[i] = ptrS[i];
	}

};
*/

#endif