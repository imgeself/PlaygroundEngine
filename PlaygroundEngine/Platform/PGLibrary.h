#pragma once

#ifdef PLATFORM_WINDOWS
#include "Win32/PGPlatformDefinesWin32.h"
#endif // PLATFORM_WINDOWS

class DLibrary {
public:
    DLibrary(const char* filePath);
    ~DLibrary();

    bool Load();
    bool Unload();

    template<class ProcType>
    ProcType GetFunctionAddress(const char* procName) {
        return (ProcType) _GetProcAddress(procName);
    }

private:
    const char* m_FilePath;
    DLibraryHandle m_Handle;

    void* _GetProcAddress(const char* procName);
};

