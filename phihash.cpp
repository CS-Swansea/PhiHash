#include "phihash.hpp"

/**
 * Program entry point
 */
int main() {

	srand(time(NULL));

	char *minStr = new char[HASH_LEN + 1];
	unsigned char *minHash = new unsigned char[HASH_LEN + 1];
	char * minHashStr = new char[HASH_STR];
	memset(minHashStr, 0, HASH_STR);

	char * hashStr = new char[HASH_STR];
	memset(hashStr, 0, HASH_STR);

	char *str = new char[HASH_LEN + 1];
	unsigned char *ustr = (unsigned char *) str;
	str[HASH_LEN] = '\0';
	
	for (int i = 0; i < HASH_LEN; i++) {
		str[i] = 32 + ((unsigned char) (rand() % 95));
	}

	unsigned char * hash = new unsigned char[HASH_LEN];
	memset(hash, 0, HASH_LEN);

	memcpy(minStr, str, HASH_LEN+1);
	genHash(minStr, hash);

	hash2Str(hash, minHashStr);
	std::cout << minStr << std::endl << std::endl;
	std::cout << minHashStr << std::endl << std::endl;

	for (int i = 0; i < WORK_SIZE; i++) {
		
		int c = 63;
		while (c >= 0 && ustr[c] >= 132) c--;
		
		if (c < 0) {
			for (int i = 0; i < HASH_LEN; i++) {
				str[i] = 32 + ((unsigned char) (rand() % 95));
			}
		}
		else {
			ustr[c]++;
		}

		genHash(str, hash);
		hash2Str(hash, hashStr);

		if (cmpHash(hashStr, minHashStr)) {
			memcpy(minHashStr, hashStr, HASH_LEN);
			memcpy(minStr, str, HASH_LEN);

			std::cout << minStr << std::endl << std::endl;
			std::cout << minHashStr << std::endl;
		}
	}

	
	std::cout << minStr << std::endl << std::endl;
	std::cout << minHashStr << std::endl;

	delete [] hash;
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