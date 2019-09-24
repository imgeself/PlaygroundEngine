#include "DX11Shader.h"

DX11ShaderProgram::DX11ShaderProgram(ID3D11Device* device, ShaderFileData vertexShaderFileData, ShaderFileData pixelShaderFileData) {
    // Vertex Shader creation
    m_VertexShaderFile = vertexShaderFileData;
    HRESULT result = device->CreateVertexShader(m_VertexShaderFile.fileData, m_VertexShaderFile.fileSize, 0, &m_VertexShader);
    PG_ASSERT(SUCCEEDED(result), "Error at creating vertex shader");

    // Pixel shader creation
    result = device->CreatePixelShader(pixelShaderFileData.fileData, pixelShaderFileData.fileSize, 0, &m_PixelShader);
    PG_ASSERT(SUCCEEDED(result), "Error at creating pixel shader");
    free(pixelShaderFileData.fileData);
}

DX11ShaderProgram::~DX11ShaderProgram() {
    SAFE_RELEASE(m_VertexShader);
    SAFE_RELEASE(m_PixelShader);
    free(m_VertexShaderFile.fileData);
}

