#pragma once

/*
* Simple PAUSE macro to stop Visual Studio from
* closing the console when the program terminates...
*/
#if defined(_MSC_VER)
#define __THREADS__ 7
#define PAUSE system("PAUSE");
#define OFFLOAD_DECL 
#else
#include <offload.h> 
#define __THREADS__ 31
#define PAUSE 
#define OFFLOAD_DECL //__declspec(target(mic))
#endif