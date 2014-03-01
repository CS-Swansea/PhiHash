#include "phihash.hpp"

/* 
 * A local copy of this variable 
 * name will exist in each thread... 
 */
OFFLOAD_DECL
int RNG_STATE;

/**
 * Program entry point 
 */
int main() {

	// Setup RNG
	RNG_STATE = seedThreadSafeRNG(omp_get_thread_num());

	// Buffers to store the global minimum (string, hash) tuple
	char *minStr = (char*)allocEmptyBuffer(HASH_LEN + 1); // Don't forget the +1 for the '\0' byte
	char *minHashStr = (char*) allocEmptyBuffer(HASH_STR);

	// Perform an initial random hash so we have something to compare the threads to initially
	unsigned char *tmpBuffer = (unsigned char*) allocEmptyBuffer(HASH_LEN);
	randomStr((unsigned char*)minStr);
	sha512_context tmpCtx;
	genHash(&tmpCtx, minStr, tmpBuffer);
	hash2Str(tmpBuffer, minHashStr);
	delete [] tmpBuffer;

	while (true) {

		// Start the clock
		double t1 = omp_get_wtime();

		bool newHash;

		//#pragma offload target(mic:0) out(newHash) inout(minStr : length(HASH_MIC)) inout(minHashStr : length(HASH_STR))
		{
			newHash = false;

			#pragma omp parallel __THREADS__ private(RNG_STATE)
			{
				// Setup RNG
				RNG_STATE = seedThreadSafeRNG(omp_get_thread_num());
				sha512_context ctx;

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
				genHash(&ctx, minThreadStr, hashBuffer);
				hash2Str(hashBuffer, minThreadHashStr);

				// Copy the random string used to the work buffer
				memcpy(str, minThreadStr, HASH_LEN);

				// Compute <WORK_SIZE> hashes then compare them to the host global
				for (int i = 0; i < WORK_SIZE; i++) {
					// Increment the string or randomize it if it overflows
					permuteStr((unsigned char*) str);

					genHash(&ctx, str, hashBuffer);
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
				delete [] str;
				delete [] hashStr;

				delete [] minThreadStr;
				delete [] minThreadHashStr;

				delete [] hashBuffer;
			}

		}

		// Stop the clock
		double t2 = (double) (WORK_SIZE * __THREADS__) / (omp_get_wtime() - t1);
		std::cout << std::flush << "  Cores : " << __THREADS__ << " Hash/s : " << t2 << '\r';

		if (newHash) {

			int j;
			for (j = 0; j < HASH_STR && minHashStr[j] == '0'; j++);

			std::cout << std::endl << std::endl << " String : " << minStr << std::endl << std::endl;
			std::cout << "    0's : " << j << std::endl << std::endl;
			std::cout << "Minimum : " << minHashStr << std::endl << std::endl;
		}
	}

	delete [] minStr;
	delete [] minHashStr;

	PAUSE
	return 0;
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
	while (c >= 0 && str[c] >= 126) {
		str[c] = 32;
		c--;
	}

	if (c > -1) {
		str[c]++;
		return;
	}

	randomStr(str);
#elif __PERMUTE_SCHEDULE__ == __RND_PERMUTE__
	int index = threadSafeRNG(RNG_STATE) % 64;
	str[index] = 32 + ((char) (threadSafeRNG(RNG_STATE) % 95));
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
	char* ptr = new char[len];
	//memset(ptr, 0, len);
	ptr[len - 1] = '\0';
	return ptr;
};