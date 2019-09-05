#pragma once

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define PG_ASSERT(x, string) { if(!(x)) { printf("Assertion Failed: %s\n", string); __debugbreak(); } }

#define SAFE_RELEASE(p) { if(p) { p->Release(); } }

#ifdef PLATFORM_WINDOWS
    #ifdef _WINDLL
        #define PG_API __declspec(dllexport)
    #else
        #define PG_API __declspec(dllimport)
    #endif
#endif
