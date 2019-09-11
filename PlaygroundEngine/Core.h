#pragma once

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define PG_ASSERT(x, string) { if(!(x)) { printf("Assertion Failed: %s\n", string); __debugbreak(); } }

#define SAFE_RELEASE(p) { if(p) { p->Release(); } }

#ifdef PLATFORM_WINDOWS
    #define DLL_EXPORT __declspec(dllexport)
    #define DLL_IMPORT __declspec(dllimport)
    #ifdef _WINDLL
        #define PG_API DLL_IMPORT
    #else
        #define PG_API DLL_EXPORT
    #endif
#endif

struct SystemInitArguments {
    const char* executablePath;
    const char* gameLibraryPath;
};

