#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <ctime>
#include <cmath>
#include <omp.h>

#include "port.hpp"

#include "fast_func.hpp"

#include "sha512.hpp"

/*
 * Definitions for the length of a hash buffer and 
 * a corresponding string buffer...
 */
#define HASH_LEN 64
#define HASH_STR 129

/*
 * How many hashes to compute per thread before 
 * combining results...
 *
 * A higher value will reduce bottlenecks, but 
 * increase the likelihood of work duplication
 * as threads overlap.
 */
#define WORK_SIZE 1000000

/*
 * String permutation schedules
 */
#define __INC_PERMUTE__			1 /* FASTEST */
#define __RND_PERMUTE__			2 /* SECOND FASTEST */
#define __RND_LOOKUP_PERMUTE__	3 /* SLIGHTLY SLOWER THAN __RND_PERMUTE__ */
								  /* PURE RANDOM IS SLOWEST */

#define __PERMUTE_SCHEDULE__	1

/*
* Valid character for string permutation
*/
#define PRINTABLE_CHARS 95

/**
* Compute the SHA-512 Hash of a 64 Character String
*
* @param ctx
*		A pointer to the SHA-512 generator context
*
* @param input
*		A 64 Readable Character ASCII string to be hashed
*
* @param hash
*		A 64 byte result buffer for the hash
*/
#define genHash(ctx, input, hash) sha512(ctx, (unsigned char *)input, HASH_LEN, hash)

/**
* Compare two SHA-512 Hashes
*
* @param newHash
*		A 64 byte buffer for the new hash to compare
*
* @param oldHash
*		A 64 byte buffer for the old hash
*
* @return Returns true if the new hash has a lower
*		hexidecimal value than the old one
*/
OFFLOAD_DECL inline bool cmpHash(unsigned char *newHash, unsigned char *oldHash);

/**
* Convert the SHA-512 Hash to a 128 Character Hexidecimal String
*
* @param hash
*		A 64 byte buffer for the hash
*
* @param hashStr
*		A 129 Character (128 plus a '\0' byte) result buffer for the string
*/
OFFLOAD_DECL inline void hash2Str(unsigned char *hash, char *hashStr);

/**
 * Increments a 64 Character String buffer to the next logical 
 * permutation of Readable ASCII Characters
 *
 * @param str
 *		A 64 Character buffer for the string to permutate
 */
OFFLOAD_DECL inline void permuteStr(unsigned char *str);

/**
* Fills a 64 Character String buffer with a random
* permutation of Readable ASCII Characters
*
* @param str
*		A 64 Character buffer for the string to randomize
*/
OFFLOAD_DECL inline void randomStr(unsigned char *str);

/**
* Seeds the <state> of our threadsafe RNG
*
* @param state
*		An integer reference which stores the state of the RNG
*/
OFFLOAD_DECL inline int seedThreadSafeRNG(int id) {
	return 25234 + 17 * (id * (int)time(NULL));
}

/**
 * This is a thread safe random number generator, seed it using the 
 * accompanying function
 *
 * @param state
 *		An integer reference which stores the state of the RNG
 */
OFFLOAD_DECL inline int threadSafeRNG(int &state) {
	// & 0x7fffffff is equivalent to modulo with RNG_MOD = 2^31
	return (state = (state * 1103515245 + 12345) & 0x7fffffff);
}

OFFLOAD_DECL inline int threadSafeRNG() {
	int rnd;
	#pragma omp critical
	{
		rnd = rand();
	}
	return rnd;
}

/**
* Run a series of checks on the SHA-512 function to make sure it 
* still produces correct results
*/
void TEST_sha512();