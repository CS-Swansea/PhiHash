/**
 * \file sha512.hpp
 *
 * \brief SHA-384 and SHA-512 cryptographic hash function
 *
 *  Original Copyright (C) 2006-2013, Brainspark B.V.
 *
 *  This file is modified from a part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Joss Whittle <637342 @ swansea.ac.uk>
 *
 *	CHANGES FROM ORIGINAL: This file and it's accompanying .cpp have been modified 
 *  to remove any external dependencies. They are now a stand-alone implementation
 *  of the SHA-512 & SHA-384 hash functions. They can be included and compiled on
 *  at least the following C++ compilers: G++, MSVC, and ICPC (the intel compiler)  
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/** Original Licence Header
 * \file sha512.h
 *
 * \brief SHA-384 and SHA-512 cryptographic hash function
 *
 *  Copyright (C) 2006-2013, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#pragma once

#include <cstdio>
#include <cstring>

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
	__declspec(target(mic))
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
	__declspec(target(mic))
	void sha512_starts(sha512_context *ctx, int is384);

	/**
	* \brief          SHA-512 process buffer
	*
	* \param ctx      SHA-512 context
	* \param input    buffer holding the  data
	* \param ilen     length of the input data
	*/
	__declspec(target(mic))
	void sha512_update(sha512_context *ctx, const unsigned char *input, size_t ilen);

	/**
	* \brief          SHA-512 final digest
	*
	* \param ctx      SHA-512 context
	* \param output   SHA-384/512 checksum result
	*/
	__declspec(target(mic))
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
	__declspec(target(mic))
	void sha512(const unsigned char *input, size_t ilen,
		unsigned char output[64], int is384);

	/* Internal use */
	__declspec(target(mic))
	void sha512_process(sha512_context *ctx, const unsigned char data[128]);
}

