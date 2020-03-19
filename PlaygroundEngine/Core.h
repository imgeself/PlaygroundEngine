#pragma once

#pragma warning(disable:4251) // std libraries dll interface warning. We use same compiler and same configuration for the dll and application.

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <functional>

#define PG_ASSERT(x, string) { if(!(x)) { printf("Assertion Failed: %s\n", string); __debugbreak(); } }

#define SAFE_DELETE(p) { if(p) { delete p; p = nullptr; } }
#define SAFE_RELEASE(p) { if(p) { p->Release(); p = nullptr; } }

#define BIT(x)    (1u << (x))

#ifdef PLATFORM_WINDOWS
    #define DLL_EXPORT __declspec(dllexport)
    #define DLL_IMPORT __declspec(dllimport)
    #ifdef _WINDLL
        #define PG_API DLL_IMPORT
    #else
        #define PG_API DLL_EXPORT
    #endif
#endif

#define PG_DEBUG_GPU_DEVICE 1

// Simple stupid hash function
static size_t Hash(const uint8_t* data, size_t size) {
    std::hash<uint8_t> hasher;
    size_t hash = 0;
    for (size_t i = 0; i < size; ++i) {
        hash += hasher(*(data + i)) | *(data + i);
    }

    return hash;
}

struct SystemInitArguments {
    const char* executablePath;
    const char* gameLibraryPath;
};

