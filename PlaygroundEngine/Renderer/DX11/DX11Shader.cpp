#include "DX11Shader.h"

#include <string>
#include <stdio.h>

#include <d3d11shader.h>

DX11VertexShader::DX11VertexShader(ID3D11Device* device, ShaderFileData* vertexShaderBinaryFileData) {
    if (vertexShaderBinaryFileData) {
        HRESULT result = device->CreateVertexShader(vertexShaderBinaryFileData->fileData, vertexShaderBinaryFileData->fileSize, 0, &m_VertexShader);
        PG_ASSERT(SUCCEEDED(result), "Error at creating vertex shader");
        D3DCreateBlob(vertexShaderBinaryFileData->fileSize, &m_VertexShaderBlob);
        memcpy(m_VertexShaderBlob->GetBufferPointer(), vertexShaderBinaryFileData->fileData, vertexShaderBinaryFileData->fileSize);
    }
}

DX11VertexShader::~DX11VertexShader() {
    SAFE_RELEASE(m_VertexShaderBlob);
    SAFE_RELEASE(m_VertexShader);
}


DX11PixelShader::DX11PixelShader(ID3D11Device* device, ShaderFileData* pixelShaderBinaryFileData) {
    if (pixelShaderBinaryFileData) {
        // Pixel shader creation
        HRESULT result = device->CreatePixelShader(pixelShaderBinaryFileData->fileData, pixelShaderBinaryFileData->fileSize, 0, &m_PixelShader);
        PG_ASSERT(SUCCEEDED(result), "Error at creating pixel shader");
    }
}


DX11PixelShader::~DX11PixelShader() {
    SAFE_RELEASE(m_PixelShader);
}

