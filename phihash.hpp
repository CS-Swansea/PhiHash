#include <iostream>
#include <openssl/sha.h>

#define HASH_STR ((SHA512_DIGEST_LENGTH * 2) + 1)

inline void genHash(char *a, char *hash);

inline bool cmpHash(char *a, char *b);

inline void hash2Str(char *hash, char *a);
