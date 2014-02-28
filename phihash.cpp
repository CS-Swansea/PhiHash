#include "phihash.hpp"

/* 
 * A local copy of this variable 
 * name will exist in each thread... 
 */
int RNG_STATE;

/**
 * Program entry point 
 */
int main() {

	typedef std::chrono::high_resolution_clock Clock;

	// Setup RNG
	RNG_STATE = seedThreadSafeRNG(omp_get_thread_num());

	// Buffers to store the global minimum (string, hash) tuple
	char *minStr = (char*)allocEmptyBuffer(HASH_LEN + 1); // Don't forget the +1 for the '\0' byte
	char *minHashStr = (char*) allocEmptyBuffer(HASH_STR);

	// Perform an initial random hash so we have something to compare the threads to initially
	unsigned char *tmpBuffer = (unsigned char*) allocEmptyBuffer(HASH_LEN);
	randomStr((unsigned char*)minStr);
	genHash(minStr, tmpBuffer);
	hash2Str(tmpBuffer, minHashStr);
	delete [] tmpBuffer;

	while (true) {

		// Start the clock
		auto t1 = Clock::now();

		bool newHash = false;
		#pragma omp parallel private(RNG_STATE)
		{
			// Setup RNG
			RNG_STATE = seedThreadSafeRNG(omp_get_thread_num());

			// A thread min buffers
			char *minThreadStr = (char*) allocEmptyBuffer(HASH_LEN + 1);
			char *minThreadHashStr = (char*) allocEmptyBuffer(HASH_STR);

			// Thread compute buffers
			char *str = (char*) allocEmptyBuffer(HASH_LEN + 1);
			char *hashStr = (char*) allocEmptyBuffer(HASH_STR);

			// Intermediate buffer for hashing
			unsigned char *hashBuffer = (unsigned char*) allocEmptyBuffer(HASH_LEN);

			// Perform initial random hash so we have something to compare off of initially
			randomStr((unsigned char*) minThreadStr);
			genHash(minThreadStr, hashBuffer);
			hash2Str(hashBuffer, minThreadHashStr);

			// Copy the random string used to the work buffer
			memcpy(str, minThreadStr, HASH_LEN);

			// Compute <WORK_SIZE> hashes then compare them to the host global
			for (int i = 0; i < WORK_SIZE; i++) {
				// Increment the string or randomize it if it overflows
				permuteStr((unsigned char*) str);

				genHash(str, hashBuffer);
				hash2Str(hashBuffer, hashStr);

				// Compare the new hash to the local minima
				if (cmpHash(hashStr, minThreadHashStr)) {
					// Blit the new string and hash to the local buffers
					memcpy(minThreadHashStr, hashStr, HASH_STR);
					memcpy(minThreadStr, str, HASH_LEN + 1);
				}
			}

			#pragma omp critical
			{
				// Compare the new hash to the local minima
				if (cmpHash(minThreadHashStr, minHashStr)) {
					// Blit the new string and hash to the local buffers
					memcpy(minHashStr, minThreadHashStr, HASH_STR);
					memcpy(minStr, minThreadStr, HASH_LEN + 1);

					newHash = true;
				}
			}

			// Cleanup thread buffers
			delete[] str;
			delete[] hashStr;

			delete [] minThreadStr;
			delete [] minThreadHashStr;

			delete [] hashBuffer;
		}

		// Stop the clock
		auto t2 = Clock::now() - t1;
		using std::chrono::duration_cast;
		//nanoseconds ns = duration_cast<nanoseconds>(t2);
		//std::cout << " Hash/s : " << (unsigned long long)t2 << std::endl << std::endl;

		if (newHash) {
			std::cout << " String : " << minStr << std::endl << std::endl;
			std::cout << "Minimum : " << minHashStr << std::endl << std::endl;
		}
	}

	delete [] minStr;
	delete [] minHashStr;

	PAUSE
	return 0;
};

/**
 * Compute the SHA-512 Hash of a 64 Character String
 *
 * @param input 
 *		A 64 Readable Character ASCII string to be hashed
 *
 * @param hash
 *		A 64 byte result buffer for the hash
 */
inline void genHash(char *input, unsigned char *hash) {
	sha512((unsigned char *)input, HASH_LEN, hash, 0);
};

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
inline bool cmpHash(char *newHash, char *oldHash) {
	for (int i = 0; i < HASH_LEN; i++) {
		if (newHash[i] < oldHash[i]) {
			return true;
		}
		else if (newHash[i] > oldHash[i]) {
			return false;
		}
	}
	return false;
};

/**
* Convert the SHA-512 Hash to a 128 Character Hexidecimal String
*
* @param hash
*		A 64 byte buffer for the hash
*
* @param hashStr
*		A 129 Character (128 plus a '\0' byte) result buffer for the string
*/
inline void hash2Str(unsigned char *hash, char *hashStr) {
	for (int i = 0; i < HASH_LEN; i++) {
		sprintf(&(hashStr[i * 2]), "%02X", hash[i]);
	}
};

/**
* Increments a 64 Character String buffer to the next logical
* permutation of Readable ASCII Characters
*
* @param str
*		A 64 Character buffer for the string to permutate
*/
inline void permuteStr(unsigned char *str) {
#if __PERMUTE_SCHEDULE__ == __INC_PERMUTE__
	int c = 63;
	while (c >= 0 && str[c] >= 126) c--;

	if (c > -1) {
		str[c]++;
		return;
	}

	randomStr(str);
#elif __PERMUTE_SCHEDULE__ == __RND_PERMUTE__
	str[threadSafeRNG(RNG_STATE) % 64] = 32 + ((unsigned char) (threadSafeRNG(RNG_STATE) % 95));
#else
	randomStr(str);
#endif
};

/**
* Fills a 64 Character String buffer with a random 
* permutation of Readable ASCII Characters
*
* @param str
*		A 64 Character buffer for the string to randomize
*/
inline void randomStr(unsigned char *str) {
	for (int i = 0; i < HASH_LEN; i++) {
		str[i] = 32 + ((unsigned char) (threadSafeRNG(RNG_STATE) % 95));
	}
};

/**
* Create an empty buffer
*
* @param len
*		The length of the buffer
*
* @return Returns a pointer to the
*		first element of the buffer
*/
inline void* allocEmptyBuffer(size_t len) {
	void* ptr = new char[len];
	memset(ptr, 0, len);
	return ptr;
};