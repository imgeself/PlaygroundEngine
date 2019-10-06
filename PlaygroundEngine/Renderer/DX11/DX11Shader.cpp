#include "DX11Shader.h"

#include <string>
#include <stdio.h>

#include <d3d11shader.h>

static ShaderItemType GetShaderItemTypeFromD3DType(D3D11_SHADER_TYPE_DESC* d3dShaderType) {
    if (d3dShaderType->Class == D3D_SVC_SCALAR) {
        if (d3dShaderType->Type == D3D_SVT_FLOAT) {
            return ShaderItemType::FLOAT_SCALAR;
        }
    } else if (d3dShaderType->Class == D3D_SVC_VECTOR) {
        if (d3dShaderType->Type == D3D_SVT_FLOAT) {
            if (d3dShaderType->Columns == 3) {
                return ShaderItemType::FLOAT3;
            } else if (d3dShaderType->Columns == 4) {
                return ShaderItemType::FLOAT4;
            }
        }
    } else if (d3dShaderType->Class == D3D_SVC_MATRIX_ROWS) {
        if (d3dShaderType->Type == D3D_SVT_FLOAT) {
            if (d3dShaderType->Columns == 4 && d3dShaderType->Rows == 4) {
                return ShaderItemType::MAT4;
            }
        }
    }

    return ShaderItemType::UNKNOWN;
}

ID3DBlob* DX11ShaderProgram::CompileShader(ShaderFileData source, const char* mainFunctionName, ShaderType type) {
    ID3DBlob* shaderBlob;
    ID3DBlob* errorBlob;
    const char* version = ""; 
    switch (type) {
        case VERTEX:
        {
            version = "vs_5_0";
        } break;
        case PIXEL:
        {
            version = "ps_5_0";
        } break;
        default:
        {
            PG_ASSERT(false, "The specified shader type is not supported yet!");
        } break;
    }

    D3D_SHADER_MACRO compileDefines[] = { "_HLSL", "1", NULL, NULL };
    HRESULT status = D3DCompile(source.fileData, source.fileSize, NULL, compileDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE, 
        mainFunctionName, version, D3DCOMPILE_DEBUG, 0, &shaderBlob, &errorBlob);
    if (errorBlob) {
        printf("Compile Errors: %s\n", (char*) errorBlob->GetBufferPointer());
        errorBlob->Release();
    }
    PG_ASSERT(status == S_OK, "Unable to compile shader from source");

    // Shader reflection
    status = D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &m_ShaderReflection);
    PG_ASSERT(status == S_OK, "Error at shader reflection creation");

    D3D11_SHADER_DESC shaderDesc;
    m_ShaderReflection->GetDesc(&shaderDesc);

    for (uint32_t i = 0; i < shaderDesc.BoundResources; ++i) {
        D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
        m_ShaderReflection->GetResourceBindingDesc(i, &resourceDesc);

        switch (resourceDesc.Type) {
            case D3D_SIT_CBUFFER:
            {
                ID3D11ShaderReflectionConstantBuffer* constantBuffer = m_ShaderReflection->GetConstantBufferByName(resourceDesc.Name);
                D3D11_SHADER_BUFFER_DESC bufferDesc;
                constantBuffer->GetDesc(&bufferDesc);

                ConstantBufferResource* shaderConstantBuffer = new ConstantBufferResource(bufferDesc.Name, i, bufferDesc.Size);
                ConstantBufferVariable* shaderConstantBufferVariable = new ConstantBufferVariable[bufferDesc.Variables];

                for (uint32_t j = 0; j < bufferDesc.Variables; ++j) {
                    ID3D11ShaderReflectionVariable* variable = constantBuffer->GetVariableByIndex(j);
                    D3D11_SHADER_VARIABLE_DESC variableDesc;
                    variable->GetDesc(&variableDesc);

                    ID3D11ShaderReflectionType* variableType = variable->GetType();
                    D3D11_SHADER_TYPE_DESC variableTypeDesc;
                    variableType->GetDesc(&variableTypeDesc);

                    shaderConstantBufferVariable->name = variableDesc.Name;
                    shaderConstantBufferVariable->offset = variableDesc.StartOffset;
                    ShaderItemType type = GetShaderItemTypeFromD3DType(&variableTypeDesc);
                    PG_ASSERT(type != ShaderItemType::UNKNOWN, "Unknown shader item type!");
                    shaderConstantBufferVariable->type = type;

                    shaderConstantBuffer->AddConstantValue(shaderConstantBufferVariable++);

                    printf("uniform name: %s \n", variableDesc.Name);
                }

                if (type == ShaderType::VERTEX) {
                    m_VertexShaderConstantBuffers.push_back(shaderConstantBuffer);
                } else if (type == ShaderType::PIXEL) {
                    m_PixelShaderConstantBuffers.push_back(shaderConstantBuffer);
                } else {
                    PG_ASSERT(false, "The specified shader type is not supported yet!");
                }

            } break;
        }
    }

    return shaderBlob;
}

DX11ShaderProgram::DX11ShaderProgram(ID3D11Device* device, ShaderFileData shaderFileData) {
    // Vertex Shader creation
    ID3DBlob* vertexShaderBlob = CompileShader(shaderFileData, "VSMain", ShaderType::VERTEX);
    HRESULT result = device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), 0, &m_VertexShader);
    PG_ASSERT(SUCCEEDED(result), "Error at creating vertex shader");
    m_VertexShaderBlob = vertexShaderBlob;

    // Pixel shader creation
    ID3DBlob* pixelShaderBlob = CompileShader(shaderFileData, "PSMain", ShaderType::PIXEL);
    result = device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), 0, &m_PixelShader);
    PG_ASSERT(SUCCEEDED(result), "Error at creating pixel shader");
    SAFE_RELEASE(pixelShaderBlob);
}

DX11ShaderProgram::~DX11ShaderProgram() {
    SAFE_RELEASE(m_VertexShader);
    SAFE_RELEASE(m_PixelShader);
    SAFE_RELEASE(m_VertexShaderBlob);
    SAFE_RELEASE(m_ShaderReflection);

    for (ConstantBufferResource* cBuffer : m_VertexShaderConstantBuffers) {
        delete[] cBuffer->GetConstantVariables()[0];
        delete cBuffer;
    }

    for (ConstantBufferResource* cBuffer : m_PixelShaderConstantBuffers) {
        delete[] cBuffer->GetConstantVariables()[0];
        delete cBuffer;
    }
}

HWShaderProgram& DX11ShaderProgram::operator=(const HWShaderProgram& other) {
    DX11ShaderProgram& uOther = (DX11ShaderProgram&) other;

    SAFE_RELEASE(m_VertexShader);
    SAFE_RELEASE(m_PixelShader);
    SAFE_RELEASE(m_VertexShaderBlob);
    SAFE_RELEASE(m_ShaderReflection);

    for (ConstantBufferResource* cBuffer : m_VertexShaderConstantBuffers) {
        delete[] cBuffer->GetConstantVariables()[0];
        delete cBuffer;
    }

    for (ConstantBufferResource* cBuffer : m_PixelShaderConstantBuffers) {
        delete[] cBuffer->GetConstantVariables()[0];
        delete cBuffer;
    }

    m_VertexShader = uOther.m_VertexShader;
    m_PixelShader = uOther.m_PixelShader;
    m_VertexShaderBlob = uOther.m_VertexShaderBlob;
    m_ShaderReflection = uOther.m_ShaderReflection;
    m_VertexShaderConstantBuffers = uOther.m_VertexShaderConstantBuffers;
    m_PixelShaderConstantBuffers = uOther.m_PixelShaderConstantBuffers;
    return *this;
}


