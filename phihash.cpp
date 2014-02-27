#include "phihash.hpp"


int main() {

	char *hw = "Hello World!!!";
	unsigned char * hash = new unsigned char[HASH_LEN];
	char * hashStr = new char[HASH_STR];

	memset(hash, 0, HASH_LEN);
	memset(hashStr, 0, HASH_STR);

	genHash(hw, hash);
	hash2Str(hash, hashStr);

	std::cout << hw << std::endl;
	std::cout << hashStr << std::endl;

	delete [] hash;
	delete [] hashStr;

	system("PAUSE");
	return 0;
};

inline void genHash(char *a, unsigned char *hash) {
	sha512((const unsigned char *) a, strlen(a), (unsigned char *) hash, 0);
};

inline bool cmpHash(unsigned char *a, unsigned char *b) {
	for (int i = 0; i < HASH_LEN; i++) {
		if (a[i] > b[i]) return false;
	}
	return true;
};

inline void hash2Str(unsigned char *hash, char *a) {
	for (int i = 0; i < HASH_LEN; i++) {
		sprintf(&(a[i * 2]), "%02X", (unsigned char)hash[i]);
	}
};