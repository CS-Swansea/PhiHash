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
#include "port.hpp"

#if defined(_MSC_VER) || defined(__WATCOMC__)
#define UL64(x) x##ui64
typedef unsigned __int64 uint64_t;
#else
#include <inttypes.h>
#define UL64(x) x##ULL
#endif

/**
* \brief          SHA-512 context structure
*/
OFFLOAD_DECL
typedef struct
{
	uint64_t total[2];          /*!< number of bytes processed  */
	uint64_t state[8];          /*!< intermediate digest state  */
	unsigned char buffer[128];  /*!< data block being processed */
}
sha512_context;

/* Internal use */
OFFLOAD_DECL
inline void sha512_process(sha512_context *ctx, const unsigned char data[128]);

/**
* \brief          SHA-512 process buffer
*
* \param ctx      SHA-512 context
* \param input    buffer holding the  data
* \param ilen     length of the input data
*/
OFFLOAD_DECL
inline void sha512_update(sha512_context *ctx, const unsigned char *input, size_t ilen);

/*
* output = SHA-512( input buffer )
*/
OFFLOAD_DECL
void sha512(sha512_context *ctx, const unsigned char *input, size_t ilen, unsigned char output[64]);