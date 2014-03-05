#include "phihash.hpp"

/**
 * Program entry point 
 */
int main() {

	/* 
	 * Verfiy that the hash function
	 * still produces correct results... 
	 */
#ifdef __RUN_TESTS__
	TEST_sha512();
#endif

	// Dummy offload to get all cards ready...
	//#pragma offload_transfer target(mic)

	// Setup RNG
	srand((unsigned int) time(NULL));

	// Buffers to store the global minimum (string, hash) tuple
	__ALIGN__ char minStr[HASH_LEN + 1]; minStr[HASH_LEN] = '\0';
	__ALIGN__ unsigned char minHash[HASH_LEN];
	__ALIGN__ char minHashStr[HASH_STR]; minHashStr[HASH_STR - 1] = '\0';

	// Perform an initial random hash so we have something to compare the threads to initially
	randomStr((unsigned char*)minStr);
	sha512_context tmpCtx;
	genHash(&tmpCtx, minStr, minHash);

	uint64_t round = 0;
	double tA = 0;
	while (true) {

		// Start the clock
		double t1 = omp_get_wtime();

		bool newHash;

		//#pragma offload target(mic:0) out(newHash) inout(minStr[0:HASH_LEN] : ALLOC) inout(minHashStr[0:HASH_STR] : ALLOC)
		{
			newHash = false;

			#pragma omp parallel num_threads(__THREADS__)
			{
				// Setup RNG
				//RNG_STATE = seedThreadSafeRNG(omp_get_thread_num() + (__THREADS__ * (int)round));
				sha512_context ctx;

				// A thread min buffers
				__ALIGN__ char minThreadStr[HASH_LEN + 1]; minThreadStr[HASH_LEN] = '\0';
				__ALIGN__ unsigned char minThreadHash[HASH_LEN];

				// Thread compute buffers
				__ALIGN__ char str[HASH_LEN + 1]; str[HASH_LEN] = '\0';
				__ALIGN__ unsigned char hashBuffer[HASH_LEN];

				// Perform initial random hash so we have something to compare off of initially
				randomStr((unsigned char*) minThreadStr);
				genHash(&ctx, minThreadStr, minThreadHash);
				
				// Copy the random string used to the work buffer
				A_memcpy_64(str, minThreadStr);

				// Compute <WORK_SIZE> hashes then compare them to the host global
				for (int i = 0; i < WORK_SIZE; i++) {
					
					permuteStr((unsigned char*) str);
					genHash(&ctx, str, hashBuffer);
					
					// Compare the new hash to the local minima
					if (cmpHash(hashBuffer, minThreadHash)) {
						// Blit the new string and hash to the local buffers
						A_memcpy_64((char*) minThreadHash, (char*) hashBuffer);
						A_memcpy_64(minThreadStr, str);
					}
				}

				#pragma omp critical
				{
					// Compare the new hash to the local minima
					if (cmpHash(minThreadHash, minHash)) {
						// Blit the new string and hash to the local buffers
						A_memcpy_64((char*) minHash, (char*) minThreadHash);
						A_memcpy_64(minStr, minThreadStr);

						newHash = true;
					}
				}
			}

		}

		// Stop the clock
		double t2 = (double) (WORK_SIZE * __THREADS__) / (omp_get_wtime() - t1);

		tA = ((tA * (double) round) + t2) / (double)(round + 1);
		round++;

		std::cout << std::flush << "  Cores : " << __THREADS__ << " Hash/s : " << (uint64_t) t2 << " avg: " << (uint64_t) tA << '\r';

		if (newHash) {

			hash2Str(minHash, minHashStr);

			int j;
			for (j = 0; j < HASH_STR && minHashStr[j] == '0'; j++);

			std::cout << std::endl << std::endl << " String : " << minStr << std::endl << std::endl;
			std::cout << "    0's : " << j << std::endl << std::endl;
			std::cout << "Minimum : " << minHashStr << std::endl << std::endl;
		}
	}

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
OFFLOAD_DECL inline bool cmpHash(unsigned char *newHash, unsigned char *oldHash) {
	int i = 0;
	while (i < HASH_LEN && newHash[i] == oldHash[i]) i++;
	
	unsigned char a = (newHash[i] >> 4);
	unsigned char b = (oldHash[i] >> 4);
	if (a < b) {
		return true;
	}
	else if (a > b) 
	{
		return false;
	}

	a = (newHash[i] & 0x0F);
	b = (oldHash[i] & 0x0F);
	if (a < b) {
		return true;
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
OFFLOAD_DECL inline void hash2Str(unsigned char *hash, char *hashStr) {
	const char hexIndex[17] = "0123456789ABCDEF"; 

	int nNibble, i, j;
	for (i = 0; i < HASH_LEN; i++)
	{
		j = 2 * i;
		nNibble = hash[i] >> 4;
		hashStr[j] = hexIndex[nNibble];

		nNibble = hash[i] & 0x0F;
		hashStr[j + 1] = hexIndex[nNibble];
	}
};

/**
* Increments a 64 Character String buffer to the next logical
* permutation of Readable ASCII Characters
*
* @param str
*		A 64 Character buffer for the string to permutate
*/
OFFLOAD_DECL inline void permuteStr(unsigned char *str) {
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
	int index = threadSafeRNG() % 64;
	str[index] = 32 + ((char) (threadSafeRNG() % 95));
#elif __PERMUTE_SCHEDULE__ == __RND_LOOKUP_PERMUTE__
	const char rndChars[PRINTABLE_CHARS+1] = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	int index = threadSafeRNG() % 64;
	str[index] = rndChars[(threadSafeRNG() % PRINTABLE_CHARS)];
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
OFFLOAD_DECL inline void randomStr(unsigned char *str) {
	for (int i = 0; i < HASH_LEN; i++) {
		str[i] = 32 + ((unsigned char) (threadSafeRNG() % 95));
	}
};

void TEST_sha512() {
#ifdef __RUN_TESTS__
	/* 
	 * Hard coded test cases for the SHA-512 function using 64 character 
	 * input strings and 128 character expected outputs...
	 */
	const int TEST_CASES = 3;
	char TEST_STRINGS[TEST_CASES][HASH_LEN+1] = {
		"Hello World!vsvvkdsfighaswltreterj hnhyrt g t5944-4g[bfbdffd4efr",
		"abcABC0000000000fbh6u23evfnmujqcvg4   vb][befvm3120evvfbfnbfbbb0",
		"This is a Test Case...000dfbnni4393rj-tbkcldfdwf43frv0-bi-vkfrgd"
	};
	
	char TEST_RESULTS[TEST_CASES][HASH_STR] = {
		"688B2AED46FEA96CB453563837871746050BBDACD735FE5271653D06150199F6578F0E64ADA7B027BE64D17AA575E0865832F0B0DDD691BF50017AC337830FE2",
		"4AE2EA05B25C0D3CB2440E08EDA0751ED8405D059423BE521C8E3E75C837375D4F75EA3EA4167CE76F454C43C66097F70ABD57510754EA37B68D06915CFCD242",
		"FE1EF30091DD6B77A97933D5224B482DAF975E0EF0BF3A4AAE3233ECA6550D30A3C63B98E2F1A1BA7EC48A311AF6764E65C0F073622F5C7781B835D3B267CC45"
	};

	// Buffers to put test data into
	unsigned char hashBuffer[HASH_LEN];
	char TEST_HASH[HASH_STR]; TEST_HASH[HASH_STR - 1] = '\0';

	for (int i = 0; i < TEST_CASES; i++) {
		sha512_context ctx;
		genHash(&ctx, &(TEST_STRINGS[i][0]), hashBuffer);
		hash2Str(hashBuffer, TEST_HASH);

		int result = strcmp(TEST_HASH, &(TEST_RESULTS[i][0]));

		std::cout << "TEST: " << &(TEST_STRINGS[i][0]) << std::endl 
			      << "   E: " << &(TEST_RESULTS[i][0]) << std::endl 
				  << "   R: " << TEST_HASH << std::endl << std::endl;
		assert(result == 0);
	}

	std::cout << "ALL TESTS PASSED!" << std::endl << std::endl;
#endif
};