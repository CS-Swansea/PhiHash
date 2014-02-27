#pragma once

#include <string.h>

#if defined(_MSC_VER) || defined(__WATCOMC__)
#define UL64(x) x##ui64
typedef unsigned __int64 uint64_t;
#else
#include <inttypes.h>
#define UL64(x) x##ULL
#endif

extern "C" {

	/**
	* \brief          SHA-512 context structure
	*/
	typedef struct
	{
		uint64_t total[2];          /*!< number of bytes processed  */
		uint64_t state[8];          /*!< intermediate digest state  */
		unsigned char buffer[128];  /*!< data block being processed */

		unsigned char ipad[128];    /*!< HMAC: inner padding        */
		unsigned char opad[128];    /*!< HMAC: outer padding        */
		int is384;                  /*!< 0 => SHA-512, else SHA-384 */
	}
	sha512_context;

	/**
	* \brief          SHA-512 context setup
	*
	* \param ctx      context to be initialized
	* \param is384    0 = use SHA512, 1 = use SHA384
	*/
	void sha512_starts(sha512_context *ctx, int is384);

	/**
	* \brief          SHA-512 process buffer
	*
	* \param ctx      SHA-512 context
	* \param input    buffer holding the  data
	* \param ilen     length of the input data
	*/
	void sha512_update(sha512_context *ctx, const unsigned char *input, size_t ilen);

	/**
	* \brief          SHA-512 final digest
	*
	* \param ctx      SHA-512 context
	* \param output   SHA-384/512 checksum result
	*/
	void sha512_finish(sha512_context *ctx, unsigned char output[64]);

}

extern "C" {

	/**
	* \brief          Output = SHA-512( input buffer )
	*
	* \param input    buffer holding the  data
	* \param ilen     length of the input data
	* \param output   SHA-384/512 checksum result
	* \param is384    0 = use SHA512, 1 = use SHA384
	*/
	void sha512(const unsigned char *input, size_t ilen,
		unsigned char output[64], int is384);

	/* Internal use */
	void sha512_process(sha512_context *ctx, const unsigned char data[128]);
}

