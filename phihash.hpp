#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <omp.h>
#include "sha512.hpp"

/**
 * Simple PAUSE macro to stop Visual Studio from 
 * closing the console when the program terminates... 
 */
#if defined(_MSC_VER)
#define PAUSE system("PAUSE");
#else
#define PAUSE 
#endif

#define ITT 10000

#define HASH_LEN 64
#define HASH_STR ((HASH_LEN * 2) + 1)

inline void genHash(char *a, unsigned char *hash);

inline bool cmpHash(char *a, char *b);

inline void hash2Str(unsigned char *hash, char *a);