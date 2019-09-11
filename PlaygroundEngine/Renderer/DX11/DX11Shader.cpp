#include "DX11Shader.h"

static ShaderFile ReadBinaryFile(const char* filename) {
    HANDLE shaderFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    PG_ASSERT(shaderFile != INVALID_HANDLE_VALUE, "File is invalid");

    LARGE_INTEGER size;
    BOOL result = GetFileSizeEx(shaderFile, &size);
    PG_ASSERT(result, "File size error");

    char* shaderSource = (char*)malloc(size.QuadPart);
    DWORD readSize;
    result = ReadFile(shaderFile, (LPVOID)shaderSource, (DWORD) size.QuadPart, &readSize, NULL);
    PG_ASSERT(result, "Couldn't read file");
    PG_ASSERT(readSize == size.QuadPart, "Wanted size and actual read size doesn't match");
    CloseHandle(shaderFile);

    ShaderFile file = {};
    file.fileData = shaderSource;
    file.fileSize = readSize;

    return file;
}

DX11ShaderProgram::DX11ShaderProgram(ID3D11Device* device, const char* vertexShaderFileName, const char* pixelShaderFileName) {
    // Vertex Shader creation
    m_VertexShaderFile = ReadBinaryFile(vertexShaderFileName);
    HRESULT result = device->CreateVertexShader(m_VertexShaderFile.fileData, m_VertexShaderFile.fileSize, 0, &m_VertexShader);
    PG_ASSERT(SUCCEEDED(result), "Error at creating vertex shader");

    // Pixel shader creation
    ShaderFile pixelShaderFile = ReadBinaryFile(pixelShaderFileName);
    result = device->CreatePixelShader(pixelShaderFile.fileData, pixelShaderFile.fileSize, 0, &m_PixelShader);
    PG_ASSERT(SUCCEEDED(result), "Error at creating pixel shader");
    free(pixelShaderFile.fileData);
}

DX11ShaderProgram::~DX11ShaderProgram() {
    SAFE_RELEASE(m_VertexShader);
    SAFE_RELEASE(m_PixelShader);
    free(m_VertexShaderFile.fileData);
}

