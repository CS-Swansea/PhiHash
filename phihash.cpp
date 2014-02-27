#include "phihash.hpp"


int main() {

	srand(time(NULL));

	char *minStr = new char[HASH_LEN + 1];
	unsigned char *minHash = new unsigned char[HASH_LEN + 1];
	char * minHashStr = new char[HASH_STR];
	memset(minHashStr, 0, HASH_STR);

	char *str = new char[HASH_LEN + 1];
	unsigned char *ustr = (unsigned char *) str;
	str[HASH_LEN] = '\0';
	
	for (int i = 0; i < HASH_LEN; i++) {
		str[i] = 32 + ((unsigned char) (rand() % 95));
	}

	unsigned char * hash = new unsigned char[HASH_LEN];
	memset(hash, 0, HASH_LEN);

	memcpy(minStr, str, HASH_LEN+1);
	genHash(minStr, minHash);

	hash2Str(minHash, minHashStr);
	std::cout << minStr << std::endl << std::endl;
	std::cout << minHashStr << std::endl;

	for (int i = 0; i < ITT; i++) {
		
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

		if (cmpHash(hash, minHash)) {
			memcpy(minHash, hash, HASH_LEN);
			memcpy(minStr, str, HASH_LEN);
		}
	}

	hash2Str(minHash, minHashStr);
	std::cout << minStr << std::endl << std::endl;
	std::cout << minHashStr << std::endl;

	delete [] hash;
	delete [] minHashStr;

	PAUSE
	return 0;
};

inline void genHash(char *a, unsigned char *hash) {
	sha512((unsigned char *)a, HASH_LEN, hash, 0);
};

inline bool cmpHash(unsigned char *a, unsigned char *b) {
	for (int i = 0; i < HASH_LEN; i++) {
		if ((int) a[i] < (int) b[i]) return true;
	}
	return false;
};

inline void hash2Str(unsigned char *hash, char *a) {
	for (int i = 0; i < HASH_LEN; i++) {
		sprintf(&(a[i * 2]), "%02X", hash[i]);
	}
};