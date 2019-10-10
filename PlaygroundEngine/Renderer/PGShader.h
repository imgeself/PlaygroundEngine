#pragma once

#include "../Core.h"
#include "../Math/math_util.h"
#include "HWRendererAPI.h"
#include "PGShaderResources.h"
#include "../Assets/Shaders/ShaderDefinitions.h"

#include <d3dcompiler.h>

#include <string>
#include <unordered_map>

#define MAX_CONSTANT_BUFFER_PER_SHADER 8
typedef std::unordered_map<std::string, ConstantBufferResource*> ConstantBufferMap;

class PGShader {
public:
    PGShader();
    ~PGShader();

    void LoadFromFilename(HWRendererAPI* rendererAPI, const char* filename);
    void LoadFromFileData(HWRendererAPI* rendererAPI, ShaderFileData* fileData);
    void Reload(HWRendererAPI* rendererAPI, const char* filename);
    void SetHWConstantBufers(HWRendererAPI* rendererAPI);
    void SetSystemConstantBuffer(HWConstantBuffer* buffer, size_t index);

    template<typename T>
    void SetConstantValue(const char* name, const T& data) {
        ConstantBufferVariable* variable = nullptr;
        ConstantBufferResource* buffer = nullptr;
        GetConstantBufferAndVariable(name, &variable, &buffer);
        PG_ASSERT(variable, "Can't find the value in any constant buffer");

        buffer->SetConstantValue(variable, data);
    }

    inline HWShaderProgram* GetHWShader() { return m_HWShader; }
    inline const ConstantBufferMap& GetShaderConstantBuffers() { return m_ShaderConstantBuffers; }

protected:
    void ReflectShader(ID3DBlob* shaderBlob, ShaderType type);


private:
    HWShaderProgram* m_HWShader;
    ConstantBufferMap m_ShaderConstantBuffers;
    HWConstantBuffer* m_VertexHWConstantBuffers[MAX_CONSTANT_BUFFER_PER_SHADER];
    HWConstantBuffer* m_PixelHWConstantBuffers[MAX_CONSTANT_BUFFER_PER_SHADER];

    void GetConstantBufferAndVariable(const char* name, ConstantBufferVariable** outVariable, ConstantBufferResource** outBuffer) {
        for (auto& constantBufferPair : m_ShaderConstantBuffers) {
            for (ConstantBufferVariable* value : constantBufferPair.second->GetConstantVariables()) {
                if (!strcmp(value->name.c_str(), name)) {
                    *outVariable = value;
                    *outBuffer = constantBufferPair.second;
                }
            }
        }
    }
};

