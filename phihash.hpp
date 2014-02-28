/*
 * Force the compiler to shutup about our using 'sprintf'
 * We've hard coded the buffer sizes and there's no user input
 * so nothing should be able to go wrong... (famous last words)
 */
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <omp.h>
#include <offload.h>

#include "sha512.hpp"

/*
 * Simple PAUSE macro to stop Visual Studio from 
 * closing the console when the program terminates... 
 */
#if defined(_MSC_VER)
#define PAUSE system("PAUSE");
#else
#define PAUSE 
#endif

/*
 * Definitions for the length of a hash buffer and 
 * a corresponding string buffer...
 */
#define HASH_MIC 65
#define HASH_LEN 64
#define HASH_STR 129

/*
 * How many hashes to compute perthread before 
 * combining results...
 *
 * A higher value will reduce bottlenecks, but 
 * increase the likelihood of work duplication
 * as threads overlap.
 */
#define WORK_SIZE 10000

/*
 * String permutation schedules
 */
#define __RND_PERMUTE__ 1
#define __INC_PERMUTE__ 2
#define __PERMUTE_SCHEDULE__ 1

/**
* Compute the SHA-512 Hash of a 64 Character String
*
* @param input
*		A 64 Readable Character ASCII string to be hashed
*
* @param hash
*		A 64 byte result buffer for the hash
*/
inline void genHash(char *input, unsigned char *hash);

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
inline bool cmpHash(char *newHash, char *oldHash);

/**
* Convert the SHA-512 Hash to a 128 Character Hexidecimal String
*
* @param hash
*		A 64 byte buffer for the hash
*
* @param hashStr
*		A 129 Character (128 plus a '\0' byte) result buffer for the string
*/
inline void hash2Str(unsigned char *hash, char *hashStr);

/**
 * Increments a 64 Character String buffer to the next logical 
 * permutation of Readable ASCII Characters
 *
 * @param str
 *		A 64 Character buffer for the string to permutate
 */
inline void permuteStr(unsigned char *str);

/**
* Fills a 64 Character String buffer with a random
* permutation of Readable ASCII Characters
*
* @param str
*		A 64 Character buffer for the string to randomize
*/
inline void randomStr(unsigned char *str);

/**
* Create an empty buffer
*
* @param len
*		The length of the buffer
*
* @return Returns a pointer to the
*		first element of the buffer
*/
inline void* allocEmptyBuffer(size_t len);

/**
* Seeds the <state> of our threadsafe RNG
*
* @param state
*		An integer reference which stores the state of the RNG
*/
inline int seedThreadSafeRNG(int id) {
	return 25234 + 17 * (id * time(NULL));
}

/**
 * This is a thread safe random number generator, seed it using the 
 * accompanying function
 *
 * @param state
 *		An integer reference which stores the state of the RNG
 */
inline int threadSafeRNG(int &state) {
	// & 0x7fffffff is equivalent to modulo with RNG_MOD = 2^31
	return (state = (state * 1103515245 + 12345) & 0x7fffffff);
}