#pragma once

#include "../Core.h"
#include "../Math/math_util.h"
#include "PGShaderResources.h"

struct ShaderFileData {
    char* fileData;
    size_t fileSize;
};

class PG_API HWShaderProgram {
public:
    virtual ~HWShaderProgram() = default;
    virtual HWShaderProgram& operator=(const HWShaderProgram& other) = 0;

    template<typename T>
    void SetConstantValue(const char* name, const T& data) {
        ConstantBufferVariable* variable = nullptr;
        ConstantBufferResource* buffer = nullptr;
        GetConstantBufferAndVariable(name, &variable, &buffer);
        PG_ASSERT(variable, "Can't find the value in any constant buffer");

        buffer->SetConstantValue(variable, data);
    }

    inline const ConstantBufferList& GetVertexShaderConstantBuffers() { return m_VertexShaderConstantBuffers; }
    inline const ConstantBufferList& GetPixelShaderConstantBuffers() { return m_PixelShaderConstantBuffers; }

protected:
    ConstantBufferList m_VertexShaderConstantBuffers;
    ConstantBufferList m_PixelShaderConstantBuffers;

    void GetConstantBufferAndVariable(const char* name, ConstantBufferVariable** outVariable, ConstantBufferResource** outBuffer) {
        for (ConstantBufferResource* vertexConstantBuffer : m_VertexShaderConstantBuffers) {
            for (ConstantBufferVariable* value : vertexConstantBuffer->GetConstantVariables()) {
                if (!strcmp(value->name, name)) {
                    *outVariable = value;
                    *outBuffer = vertexConstantBuffer;
                }
            }
        }

        for (ConstantBufferResource* pixelConstantBuffer : m_PixelShaderConstantBuffers) {
            for (ConstantBufferVariable* value : pixelConstantBuffer->GetConstantVariables()) {
                if (!strcmp(value->name, name)) {
                    *outVariable = value;
                    *outBuffer = pixelConstantBuffer;
                }
            }
        }
    }
};

