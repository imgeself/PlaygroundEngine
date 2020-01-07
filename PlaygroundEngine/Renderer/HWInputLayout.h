#pragma once

#include "../Core.h"

#include <string>

enum VertexDataFormat {
    VertexDataFormat_FLOAT,
    VertexDataFormat_FLOAT2,
    VertexDataFormat_FLOAT3,
    VertexDataFormat_FLOAT4,
    VertexDataFormat_INT,
    VertexDataFormat_INT2,
    VertexDataFormat_INT3,
    VertexDataFormat_INT4,
    VertexDataFormat_UINT,
    VertexDataFormat_UINT2,
    VertexDataFormat_UINT3,
    VertexDataFormat_UINT4,
};

struct VertexInputElement {
    std::string name;
    VertexDataFormat format;
    uint32_t inputSlot;
    uint32_t offset;

    VertexInputElement(const std::string& name, VertexDataFormat format, uint32_t slot, uint32_t offset) 
        : name(name), format(format), inputSlot(slot), offset(offset) {}
};

class HWVertexInputLayout {
public:
    virtual ~HWVertexInputLayout() = default;
};

