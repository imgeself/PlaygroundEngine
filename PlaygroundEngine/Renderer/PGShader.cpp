#include "PGShader.h"

PGShader::PGShader() {

}

PGShader::~PGShader() {
    for (auto& pair : m_ShaderConstantBuffers) {
        ConstantBufferResource* constantBuffer = pair.second;

        bool deletedOnVertexBuffers = false;
        if (constantBuffer->shaderType & ShaderType::VERTEX) {
            delete m_VertexHWConstantBuffers[constantBuffer->GetSlot()];
            m_VertexHWConstantBuffers[constantBuffer->GetSlot()] = nullptr;
            deletedOnVertexBuffers = true;
        }
        if (constantBuffer->shaderType & ShaderType::PIXEL) {
            if (deletedOnVertexBuffers) { 
                m_PixelHWConstantBuffers[constantBuffer->GetSlot()] = nullptr;
            } else {
                delete m_PixelHWConstantBuffers[constantBuffer->GetSlot()];
                m_PixelHWConstantBuffers[constantBuffer->GetSlot()] = nullptr;
            }
        }
    }

    for (auto& pair : m_ShaderConstantBuffers) {
        ConstantBufferResource* cBuffer = pair.second;
        delete[] cBuffer->GetConstantVariables()[0];
        delete cBuffer;
    }
    m_ShaderConstantBuffers.clear();

    delete m_HWShader;
}

static ShaderItemType GetShaderItemTypeFromD3DType(D3D11_SHADER_TYPE_DESC* d3dShaderType) {
    if (d3dShaderType->Class == D3D_SVC_SCALAR) {
        if (d3dShaderType->Type == D3D_SVT_FLOAT) {
            return ShaderItemType::FLOAT_SCALAR;
        }
    }
    else if (d3dShaderType->Class == D3D_SVC_VECTOR) {
        if (d3dShaderType->Type == D3D_SVT_FLOAT) {
            if (d3dShaderType->Columns == 3) {
                return ShaderItemType::FLOAT3;
            }
            else if (d3dShaderType->Columns == 4) {
                return ShaderItemType::FLOAT4;
            }
        }
    }
    else if (d3dShaderType->Class == D3D_SVC_MATRIX_ROWS) {
        if (d3dShaderType->Type == D3D_SVT_FLOAT) {
            if (d3dShaderType->Columns == 4 && d3dShaderType->Rows == 4) {
                return ShaderItemType::MAT4;
            }
        }
    }

    return ShaderItemType::UNKNOWN;
}

static ID3DBlob* CompileShader(ShaderFileData* source, const char* mainFunctionName, const char* version) {
    ID3DBlob* shaderBlob;
    ID3DBlob* errorBlob;
    D3D_SHADER_MACRO compileDefines[] = { "_HLSL", "1", NULL, NULL };
    HRESULT status = D3DCompile(source->fileData, source->fileSize, NULL, compileDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        mainFunctionName, version, D3DCOMPILE_DEBUG, 0, &shaderBlob, &errorBlob);
    if (errorBlob) {
        printf("Compile Errors: %s\n", (char*)errorBlob->GetBufferPointer());
        errorBlob->Release();
    }
    PG_ASSERT(status == S_OK, "Unable to compile shader from source");

    return shaderBlob;
}

static ID3DBlob* CompileShader(const char* filename, const char* mainFunctionName, const char* version) {
    ID3DBlob* shaderBlob;
    ID3DBlob* errorBlob;

    const size_t filenameSize = strlen(filename) + 1;
    wchar_t* wcFilename = (wchar_t*) alloca(sizeof(wchar_t) * filenameSize);

    size_t numberOfConverted;
    mbstowcs_s(&numberOfConverted, wcFilename, filenameSize, filename, filenameSize - 1);

    D3D_SHADER_MACRO compileDefines[] = { "_HLSL", "1", NULL, NULL };
    HRESULT status = D3DCompileFromFile(wcFilename, compileDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        mainFunctionName, version, D3DCOMPILE_DEBUG, 0, &shaderBlob, &errorBlob);
    if (errorBlob) {
        printf("Compile Errors: %s\n", (char*)errorBlob->GetBufferPointer());
        errorBlob->Release();
    }
    PG_ASSERT(status == S_OK, "Unable to compile shader from source");

    return shaderBlob;
}

inline void PGShader::ReflectShader(ID3DBlob* shaderBlob, ShaderType type) {
    // Shader reflection
    ID3D11ShaderReflection* shaderReflection;
    HRESULT status = D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &shaderReflection);
    PG_ASSERT(status == S_OK, "Error at shader reflection creation");

    D3D11_SHADER_DESC shaderDesc;
    shaderReflection->GetDesc(&shaderDesc);

    for (uint32_t i = 0; i < shaderDesc.BoundResources; ++i) {
        D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
        shaderReflection->GetResourceBindingDesc(i, &resourceDesc);

        switch (resourceDesc.Type) {
            case D3D_SIT_CBUFFER:
            {
                ID3D11ShaderReflectionConstantBuffer* constantBuffer = shaderReflection->GetConstantBufferByName(resourceDesc.Name);
                D3D11_SHADER_BUFFER_DESC bufferDesc;
                constantBuffer->GetDesc(&bufferDesc);

                const std::string bufferName(bufferDesc.Name);
                auto searchResult = m_ShaderConstantBuffers.find(bufferName);
                if (searchResult != m_ShaderConstantBuffers.end()) {
                    // Same constant buffer, different shader stages
                    ConstantBufferResource* existingBuffer = searchResult->second;
                    existingBuffer->shaderType = existingBuffer->shaderType | type;
                    break;
                }
                if (resourceDesc.BindPoint < SystemConstantBufferSlot_Count) {
                    // System buffers will be created in the renderer. 
                    // Shader reflection constant buffer only for application defined constant buffer. 
                    break;
                }

                ConstantBufferResource* shaderConstantBuffer = new ConstantBufferResource(bufferDesc.Name, resourceDesc.BindPoint, bufferDesc.Size, type);
                ConstantBufferVariable* shaderConstantBufferVariable = new ConstantBufferVariable[bufferDesc.Variables];

                for (uint32_t j = 0; j < bufferDesc.Variables; ++j) {
                    ID3D11ShaderReflectionVariable* variable = constantBuffer->GetVariableByIndex(j);
                    D3D11_SHADER_VARIABLE_DESC variableDesc;
                    variable->GetDesc(&variableDesc);

                    ID3D11ShaderReflectionType* variableType = variable->GetType();
                    D3D11_SHADER_TYPE_DESC variableTypeDesc;
                    variableType->GetDesc(&variableTypeDesc);

                    shaderConstantBufferVariable->name = std::string(variableDesc.Name);
                    shaderConstantBufferVariable->offset = variableDesc.StartOffset;
                    ShaderItemType type = GetShaderItemTypeFromD3DType(&variableTypeDesc);
                    PG_ASSERT(type != ShaderItemType::UNKNOWN, "Unknown shader item type!");
                    shaderConstantBufferVariable->type = type;

                    shaderConstantBuffer->AddConstantValue(shaderConstantBufferVariable++);

                    printf("uniform name: %s \n", variableDesc.Name);
                }

                m_ShaderConstantBuffers[bufferName] = shaderConstantBuffer;

            } break;
        }
    }

    shaderReflection->Release();
}

void PGShader::LoadFromFilename(HWRendererAPI* rendererAPI, const char* filename) {
    // For now, we use the Direct3D reflection system for shader reflection system.
    // D3DReflect function expects compiled binary data for input. So, we have to compile hlsl shaders here.
    // Maybe in future, we will make our cross platform shading language.
    // Then we will get rid of this D3D stuff and write our shader parser.
    ID3DBlob* vertexShaderBlob = CompileShader(filename, "VSMain", "vs_5_0");
    ID3DBlob* pixelShaderBlob = CompileShader(filename, "PSMain", "ps_5_0");
    ShaderFileData vertexShaderData = { (char*)vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
    ShaderFileData pixelShaderData = { (char*)pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };

    m_HWShader = rendererAPI->CreateShaderProgramFromBinarySource(&vertexShaderData, &pixelShaderData);

    ReflectShader(vertexShaderBlob, ShaderType::VERTEX);
    ReflectShader(pixelShaderBlob, ShaderType::PIXEL);

    // Create hardware constant buffers
    for (auto& pair : m_ShaderConstantBuffers) {
        ConstantBufferResource* constantBuffer = pair.second;
        HWConstantBuffer* hwConstantBuffer = rendererAPI->CreateConstantBuffer(constantBuffer->GetData(), constantBuffer->GetSize());

        if (constantBuffer->shaderType & ShaderType::VERTEX) {
            m_VertexHWConstantBuffers[constantBuffer->GetSlot()] = hwConstantBuffer;
        }
        if (constantBuffer->shaderType & ShaderType::PIXEL) {
            m_PixelHWConstantBuffers[constantBuffer->GetSlot()] = hwConstantBuffer;
        }
    }

    SAFE_RELEASE(vertexShaderBlob);
    SAFE_RELEASE(pixelShaderBlob);
}


void PGShader::LoadFromFileData(HWRendererAPI* rendererAPI, ShaderFileData* fileData) {
    // For now, we use the Direct3D reflection system for shader reflection system.
    // D3DReflect function expects compiled binary data for input. So, we have to compile hlsl shaders here.
    // Maybe in future, we will make our cross platform shading language.
    // Then we will get rid of this D3D stuff and write our shader parser.
    ID3DBlob* vertexShaderBlob = CompileShader(fileData, "VSMain", "vs_5_0");
    ID3DBlob* pixelShaderBlob = CompileShader(fileData, "PSMain", "ps_5_0");
    ShaderFileData vertexShaderData = { (char*) vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
    ShaderFileData pixelShaderData = { (char*) pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };

    m_HWShader = rendererAPI->CreateShaderProgramFromBinarySource(&vertexShaderData, &pixelShaderData);

    ReflectShader(vertexShaderBlob, ShaderType::VERTEX);
    ReflectShader(pixelShaderBlob, ShaderType::PIXEL);

    // Create hardware constant buffers
    for (auto& pair : m_ShaderConstantBuffers) {
        ConstantBufferResource* constantBuffer = pair.second;
        HWConstantBuffer* hwConstantBuffer = rendererAPI->CreateConstantBuffer(constantBuffer->GetData(), constantBuffer->GetSize());

        if (constantBuffer->shaderType & ShaderType::VERTEX) {
            m_VertexHWConstantBuffers[constantBuffer->GetSlot()] = hwConstantBuffer;
        }
        if (constantBuffer->shaderType & ShaderType::PIXEL) {
            m_PixelHWConstantBuffers[constantBuffer->GetSlot()] = hwConstantBuffer;
        }
    }

    SAFE_RELEASE(vertexShaderBlob);
    SAFE_RELEASE(pixelShaderBlob);
}

void PGShader::Reload(HWRendererAPI* rendererAPI, const char* filename) {
    for (auto& pair : m_ShaderConstantBuffers) {
        ConstantBufferResource* constantBuffer = pair.second;

        bool deletedOnVertexBuffers = false;
        if (constantBuffer->shaderType & ShaderType::VERTEX) {
            delete m_VertexHWConstantBuffers[constantBuffer->GetSlot()];
            m_VertexHWConstantBuffers[constantBuffer->GetSlot()] = nullptr;
            deletedOnVertexBuffers = true;
        }
        if (constantBuffer->shaderType & ShaderType::PIXEL) {
            if (deletedOnVertexBuffers) {
                m_PixelHWConstantBuffers[constantBuffer->GetSlot()] = nullptr;
            } else {
                delete m_PixelHWConstantBuffers[constantBuffer->GetSlot()];
                m_PixelHWConstantBuffers[constantBuffer->GetSlot()] = nullptr;
            }
        }
    }

    for (auto& pair : m_ShaderConstantBuffers) {
        ConstantBufferResource* cBuffer = pair.second;
        delete[] cBuffer->GetConstantVariables()[0];
        delete cBuffer;
    }
    m_ShaderConstantBuffers.clear();

    delete m_HWShader;

    LoadFromFilename(rendererAPI, filename);
}

void PGShader::SetHWConstantBufers(HWRendererAPI* rendererAPI) {
    for (auto& pair : m_ShaderConstantBuffers) {
        ConstantBufferResource* constantBuffer = pair.second;

        if (constantBuffer->shaderType & ShaderType::VERTEX) {
            HWConstantBuffer* hwConstantBuffer = m_VertexHWConstantBuffers[constantBuffer->GetSlot()];
            PG_ASSERT(hwConstantBuffer, "Hardware constant buffer is null!");

            void* data = rendererAPI->Map(hwConstantBuffer);
            memcpy(data, constantBuffer->GetData(), constantBuffer->GetSize());
            rendererAPI->Unmap(hwConstantBuffer);
            continue;
        }

        if (constantBuffer->shaderType & ShaderType::PIXEL) {
            HWConstantBuffer* hwConstantBuffer = m_PixelHWConstantBuffers[constantBuffer->GetSlot()];
            PG_ASSERT(hwConstantBuffer, "Hardware constant buffer is null!");

            void* data = rendererAPI->Map(hwConstantBuffer);
            memcpy(data, constantBuffer->GetData(), constantBuffer->GetSize());
            rendererAPI->Unmap(hwConstantBuffer);
        }
    }

    rendererAPI->SetConstanBuffersVS(m_VertexHWConstantBuffers, MAX_CONSTANT_BUFFER_PER_SHADER);
    rendererAPI->SetConstanBuffersPS(m_PixelHWConstantBuffers, MAX_CONSTANT_BUFFER_PER_SHADER);
}

void PGShader::SetSystemConstantBuffer(HWConstantBuffer* buffer, size_t index) {
    // NOTE: For now, we bind system constant buffers to all shader stages!!
    m_VertexHWConstantBuffers[index] = buffer;
    m_PixelHWConstantBuffers[index] = buffer;
}

