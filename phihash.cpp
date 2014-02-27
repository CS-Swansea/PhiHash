#include "phihash.hpp"


int main() {

	char *hw = "Hello World!!!";
	char hash[SHA512_DIGEST_LENGTH];
	char hashStr[HASH_STR];

	memset(&hash, 0, SHA512_DIGEST_LENGTH);
	memset(&hashStr, 0, HASH_STR);

	getHash(hw, &hash);
	hash2Str(&hash, &hashStr);

	std::cout << hw << std::endl;
	std::cout << hash << std::endl;
	std::cout << hashStr << std::endl;

	return 0;
};

inline void genHash(char *a, char *hash) {
	SHA512((unsigned char*)a, SHA512_DIGEST_LENGTH, (unsigned char*)hash);    
};

inline bool cmpHash(char *a, char *b) {
	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
		if (a[i] > b[i]) return false;
	}
	return true;
};

inline void hash2Str(char *hash, char *a) {
	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        sprintf(&(a[i*2]), "%02x", (unsigned int)hash[i]);
 	}
};