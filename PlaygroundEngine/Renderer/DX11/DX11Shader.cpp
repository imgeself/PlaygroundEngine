#include "DX11Shader.h"

#include <string>
#include <stdio.h>

#include <d3d11shader.h>

DX11ShaderProgram::DX11ShaderProgram(ID3D11Device* device, ShaderFileData* vertexShaderBinaryFileData, ShaderFileData* pixelShaderBinaryFileData) {
    if (vertexShaderBinaryFileData) {
        HRESULT result = device->CreateVertexShader(vertexShaderBinaryFileData->fileData, vertexShaderBinaryFileData->fileSize, 0, &m_VertexShader);
        PG_ASSERT(SUCCEEDED(result), "Error at creating vertex shader");
        D3DCreateBlob(vertexShaderBinaryFileData->fileSize, &m_VertexShaderBlob);
        memcpy(m_VertexShaderBlob->GetBufferPointer(), vertexShaderBinaryFileData->fileData, vertexShaderBinaryFileData->fileSize);
    }

    if (pixelShaderBinaryFileData) {
        // Pixel shader creation
        HRESULT result = device->CreatePixelShader(pixelShaderBinaryFileData->fileData, pixelShaderBinaryFileData->fileSize, 0, &m_PixelShader);
        PG_ASSERT(SUCCEEDED(result), "Error at creating pixel shader");
    }
}


DX11ShaderProgram::~DX11ShaderProgram() {
    SAFE_RELEASE(m_VertexShader);
    SAFE_RELEASE(m_PixelShader);
    SAFE_RELEASE(m_VertexShaderBlob);
}

