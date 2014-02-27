#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "sha512.hpp"

#define HASH_LEN 64
#define HASH_STR ((HASH_LEN * 2) + 1)

inline void genHash(char *a, unsigned char *hash);

inline bool cmpHash(unsigned char *a, unsigned char *b);

inline void hash2Str(unsigned char *hash, char *a);