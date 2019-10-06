#include "PGShaderLib.h"

// TODO: This file reading code should be platform independent
static ShaderFileData ReadFile(const char* filepath, FILETIME* o_fileTime) {
    HANDLE shaderFile = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    PG_ASSERT(shaderFile != INVALID_HANDLE_VALUE, "File is invalid");

    LARGE_INTEGER size;
    BOOL result = GetFileSizeEx(shaderFile, &size);
    PG_ASSERT(result, "File size error");

    FILETIME lastWriteFileTime = {};
    result = GetFileTime(shaderFile, NULL, NULL, &lastWriteFileTime);
    PG_ASSERT(result, "Couldn't read file last write time");
    *o_fileTime = lastWriteFileTime;

    char* shaderSource = (char*)malloc(size.QuadPart + 1);
    DWORD readSize;
    result = ReadFile(shaderFile, (LPVOID)shaderSource, (DWORD)size.QuadPart, &readSize, NULL);
    PG_ASSERT(result, "Couldn't read file");
    PG_ASSERT(readSize == size.QuadPart, "Wanted size and actual read size doesn't match");
    shaderSource[size.QuadPart] = '\0';
    CloseHandle(shaderFile);

    ShaderFileData file = {};
    file.fileData = shaderSource;
    file.fileSize = readSize;

    return file;
}

// TODO: This code should be platform independent
static bool GetFileLastWriteTime(const char* filepath, FILETIME* o_FileTime) {
    HANDLE shaderFile = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    DWORD dLastError = GetLastError();
    if (dLastError == ERROR_SHARING_VIOLATION) {
        // File is currently using by another process
        return false;
    }
    LPCTSTR strErrorMessage = NULL;
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS | 
        FORMAT_MESSAGE_ARGUMENT_ARRAY | 
        FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL,
        dLastError,
        0,
        (LPSTR)& strErrorMessage,
        0,
        NULL);

    PG_ASSERT(shaderFile != INVALID_HANDLE_VALUE, strErrorMessage);

    FILETIME lastWriteFileTime = {};
    BOOL result = GetFileTime(shaderFile, NULL, NULL, &lastWriteFileTime);
    PG_ASSERT(result, "Couldn't read file last write time");
    CloseHandle(shaderFile);

    *o_FileTime = lastWriteFileTime;
    return true;
}

PGShaderLib::PGShaderLib(HWRendererAPI* rendererAPI)
    : m_RendererAPI(rendererAPI) {
}

PGShaderLib::~PGShaderLib() {
    for (auto pair : m_Shaders) {
        delete (pair.second);
    }
}


ShaderRef PGShaderLib::LoadShaderFromDisk(const std::string& shaderFilePath, bool createIfExists) {
    size_t fileNameEndIndex = shaderFilePath.rfind('.');
    size_t fileNameStartIndex = shaderFilePath.rfind('/') + 1;
    const std::string name = shaderFilePath.substr(fileNameStartIndex, fileNameEndIndex - fileNameStartIndex);

    auto shaderSearch = m_Shaders.find(name);
    if (shaderSearch != m_Shaders.end() && !createIfExists) {
        return shaderSearch->second;
    }
    FILETIME shaderFileTime = {};
    ShaderFileData shaderData = ReadFile(shaderFilePath.c_str(), &shaderFileTime);
    HWShaderProgram* shaderProgramPointer = m_RendererAPI->CreateShaderProgram(shaderData);
    m_ShaderFileLastWriteTimeMap[shaderFilePath] = shaderFileTime;
    free(shaderData.fileData);

    if (shaderSearch != m_Shaders.end()) {
        *(shaderSearch->second) = *shaderProgramPointer;
        return shaderSearch->second;
    } else {
        m_Shaders[name] = shaderProgramPointer;
        return shaderProgramPointer;
    }
}

void PGShaderLib::ReloadShadersIfNeeded() {
    for (const auto& pair : m_ShaderFileLastWriteTimeMap) {
        std::string filepath = pair.first;
        FILETIME newFileTime = {};
        bool isOk = GetFileLastWriteTime(filepath.c_str(), &newFileTime);
        if (isOk) {
            FILETIME oldFileTime = pair.second;
            LONG result = CompareFileTime(&newFileTime, &oldFileTime);
            if (result == 1) {
                LoadShaderFromDisk(filepath, true);
            }
        }
    }
}

void PGShaderLib::LoadDefaultShaders() {

}

ShaderRef PGShaderLib::GetDefaultShader(const std::string& name) {
    return nullptr;
}

