#pragma once

#include "../Core.h"

#include <vector>
#include <string>

enum ShaderType : uint8_t {
    VERTEX   = BIT(0),
    GEOMETRY = BIT(1),
    PIXEL    = BIT(2),
    COMPUTE  = BIT(3)
};

inline ShaderType operator|(ShaderType left, ShaderType right) {
    return (ShaderType)((uint8_t)left | (uint8_t)right);
}

inline ShaderType operator&(ShaderType left, ShaderType right) {
    return (ShaderType)((uint8_t)left & (uint8_t)right);
}

enum ShaderItemType {
    FLOAT_SCALAR,
    FLOAT3,
    FLOAT4,
    MAT4,

    UNKNOWN
};

static size_t GetShaderItemTypeSize(ShaderItemType type) {
    switch (type) {
    case FLOAT_SCALAR:
        return sizeof(float);
    case FLOAT3:
        return sizeof(float) * 3;
    case FLOAT4:
        return sizeof(float) * 4;
    case MAT4:
        return sizeof(float) * 4 * 4;
    default:
        PG_ASSERT(false, "Unknown shader item type!");
        return 0;
    }
}

// Shader resources
struct ConstantBufferVariable {
    std::string name;
    ShaderItemType type;
    size_t offset;
};

class ConstantBufferResource {
public:
    ConstantBufferResource(const char* name, uint32_t slot, size_t size, ShaderType shaderType)
        : m_Name(name), m_Slot(slot), m_Size(size), m_Data(nullptr), shaderType(shaderType) {
        m_Data = (uint8_t*) malloc(size);
    }

    ~ConstantBufferResource() {
        free(m_Data);
    }

    void AddConstantValue(ConstantBufferVariable* constantVariable) {
        m_Constants.push_back(constantVariable);
    }

    template<typename T>
    void SetConstantValue(ConstantBufferVariable* variable, const T& data) {
        // TODO: Validate variable!!
        memcpy(m_Data + variable->offset, &data, GetShaderItemTypeSize(variable->type));
    }

    inline const std::string GetName() { return m_Name; }
    inline size_t GetSlot() { return m_Slot; }
    inline size_t GetSize() { return m_Size; }
    inline const std::vector<ConstantBufferVariable*>& GetConstantVariables() { return m_Constants; }
    inline uint8_t* GetData() { return m_Data; }

    ShaderType shaderType;

private:
    const std::string m_Name;
    size_t m_Slot;
    size_t m_Size;
    std::vector<ConstantBufferVariable*> m_Constants;
    uint8_t* m_Data;
};

typedef std::vector<ConstantBufferResource*> ConstantBufferList;
