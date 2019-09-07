#include "../PGLibrary.h"

DLibrary::DLibrary(const char* filePath)
    : m_FilePath(filePath)
    , m_Handle(nullptr) {
}

DLibrary::~DLibrary() {
    Unload();
}

bool DLibrary::Load() {
    m_Handle = LoadLibrary(m_FilePath);
    return m_Handle != nullptr;
}

bool DLibrary::Unload() {
    return FreeLibrary(m_Handle);
}

void* DLibrary::_GetProcAddress(const char* procName) {
    return GetProcAddress(m_Handle, procName);
}


