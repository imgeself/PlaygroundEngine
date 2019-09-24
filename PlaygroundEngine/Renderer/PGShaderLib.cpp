#include "PGShaderLib.h"

// TODO: This file reading code should be platform independent
static ShaderFileData ReadFile(const char* filename) {
    HANDLE shaderFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    PG_ASSERT(shaderFile != INVALID_HANDLE_VALUE, "File is invalid");

    LARGE_INTEGER size;
    BOOL result = GetFileSizeEx(shaderFile, &size);
    PG_ASSERT(result, "File size error");

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

PGShaderLib::PGShaderLib(IRendererAPI* rendererAPI)
    : m_RendererAPI(rendererAPI) {
}

ShaderRef PGShaderLib::LoadShaderFromDisk(const std::string& shaderFilePath) {
    ShaderFileData shaderData = ReadFile(shaderFilePath.c_str());
    size_t fileNameEndIndex = shaderFilePath.rfind('.');
    size_t fileNameStartIndex = shaderFilePath.rfind('/') + 1;
    const std::string name = shaderFilePath.substr(fileNameStartIndex, fileNameEndIndex - fileNameStartIndex);
    ShaderRef shaderProgram = std::shared_ptr<IShaderProgram>(m_RendererAPI->CreateShaderProgram(shaderData));
    m_Shaders[name] = shaderProgram;
    free(shaderData.fileData);
    return shaderProgram;
}

void PGShaderLib::LoadDefaultShaders() {

}

ShaderRef PGShaderLib::GetDefaultShader(const std::string& name) {
    return m_Shaders[name];
}

