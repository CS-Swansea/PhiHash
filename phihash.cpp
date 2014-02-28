#include "phihash.hpp"

/**
 * Program entry point 
 */
int main() {

	srand(time(NULL));

	for (int i = 32; i <= 126; i++) {
		std::cout << (char) i;
	}

	std::cout << std::endl;

	char *minStr = allocEmptyBuffer(HASH_LEN+1);

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
inline void incrementStr(unsigned char *str) {
	int c = 63;
	while (c >= 0 && str[c] >= 126) c--;

	if (c > -1) {
		str[c]++;
		return;
	}

	randomStr(str);
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
		str[i] = 32 + ((unsigned char) (rand() % 95));
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
template<typename T>
inline T* allocEmptyBuffer(size_t len) {
	T* ptr = new T[len];
	memset(ptr, 0, len * sizeof(T));
	return ptr;
};