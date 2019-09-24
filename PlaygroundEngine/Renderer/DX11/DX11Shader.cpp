#include "DX11Shader.h"

#include <string>
#include <stdio.h>

static ID3DBlob* CompileShader(ShaderFileData source, const char* mainFunctionName, const char* version) {
    ID3DBlob* shaderBlob;
    ID3DBlob* errorBlob;
    D3D_SHADER_MACRO shaderMacros[] = { "_HLSL", "1", NULL, NULL };
    HRESULT status = D3DCompile(source.fileData, source.fileSize, NULL, shaderMacros, D3D_COMPILE_STANDARD_FILE_INCLUDE, mainFunctionName, version, D3DCOMPILE_DEBUG, 0, &shaderBlob, &errorBlob);
    if (errorBlob) {
        printf("Compile Errors: %s\n", (char*) errorBlob->GetBufferPointer());
        errorBlob->Release();
    }
    PG_ASSERT(status == S_OK, "Unable to compile shader from source");

    return shaderBlob;
}

DX11ShaderProgram::DX11ShaderProgram(ID3D11Device* device, ShaderFileData shaderFileData) {
    // Vertex Shader creation
    ID3DBlob* vertexShaderBlob = CompileShader(shaderFileData, "VSMain", "vs_4_0");
    HRESULT result = device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), 0, &m_VertexShader);
    PG_ASSERT(SUCCEEDED(result), "Error at creating vertex shader");
    m_VertexShaderBlob = vertexShaderBlob;

    // Pixel shader creation
    ID3DBlob* pixelShaderBlob = CompileShader(shaderFileData, "PSMain", "ps_4_0");
    result = device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), 0, &m_PixelShader);
    PG_ASSERT(SUCCEEDED(result), "Error at creating pixel shader");
    SAFE_RELEASE(pixelShaderBlob);
}

DX11ShaderProgram::~DX11ShaderProgram() {
    SAFE_RELEASE(m_VertexShader);
    SAFE_RELEASE(m_PixelShader);
    SAFE_RELEASE(m_VertexShaderBlob);
}

