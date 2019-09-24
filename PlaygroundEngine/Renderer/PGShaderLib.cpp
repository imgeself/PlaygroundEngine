#include "PGShaderLib.h"

// TODO: This file reading code should be platform independent
static ShaderFileData ReadBinaryFile(const char* filename) {
    HANDLE shaderFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    PG_ASSERT(shaderFile != INVALID_HANDLE_VALUE, "File is invalid");

    LARGE_INTEGER size;
    BOOL result = GetFileSizeEx(shaderFile, &size);
    PG_ASSERT(result, "File size error");

    // This resource will be freed in platform spesific shader implementation
    char* shaderSource = (char*)malloc(size.QuadPart);
    DWORD readSize;
    result = ReadFile(shaderFile, (LPVOID)shaderSource, (DWORD)size.QuadPart, &readSize, NULL);
    PG_ASSERT(result, "Couldn't read file");
    PG_ASSERT(readSize == size.QuadPart, "Wanted size and actual read size doesn't match");
    CloseHandle(shaderFile);

    ShaderFileData file = {};
    file.fileData = shaderSource;
    file.fileSize = readSize;

    return file;
}

PGShaderLib::PGShaderLib(IRendererAPI* rendererAPI)
    : m_RendererAPI(rendererAPI) {
}

ShaderRef PGShaderLib::LoadShaderFromDisk(const std::string& shaderName) {
    // TODO: We read shader files from shader binary directory for now.
    // But we will read shader source files in the future for parsing constant buffers, and other fancy stuff.
    const std::string binaryShaderFileDirectory = "../bin/shaders/";
    const std::string vertexShaderFileName = binaryShaderFileDirectory + shaderName + "Vertex.cso";
    const std::string pixelShaderFileName = binaryShaderFileDirectory + shaderName + "Pixel.cso";

    ShaderFileData vertexShaderData = ReadBinaryFile(vertexShaderFileName.c_str());
    ShaderFileData pixelShaderData = ReadBinaryFile(pixelShaderFileName.c_str());

    ShaderRef shaderProgram = std::shared_ptr<IShaderProgram>(m_RendererAPI->CreateShaderProgram(vertexShaderData, pixelShaderData));
    m_Shaders[shaderName] = shaderProgram;
    return shaderProgram;
}

ShaderRef PGShaderLib::LoadShaderFromDisk(const std::string& name, const std::string& vertexShaderFileName, const std::string& pixelShaderFileName) {
    ShaderFileData vertexShaderData = ReadBinaryFile(vertexShaderFileName.c_str());
    ShaderFileData pixelShaderData = ReadBinaryFile(pixelShaderFileName.c_str());

    ShaderRef shaderProgram = std::shared_ptr<IShaderProgram>(m_RendererAPI->CreateShaderProgram(vertexShaderData, pixelShaderData));
    m_Shaders[name] = shaderProgram;
    return shaderProgram;
}

void PGShaderLib::LoadDefaultShaders() {

}

ShaderRef PGShaderLib::GetDefaultShader(const std::string& name) {
    return m_Shaders[name];
}

