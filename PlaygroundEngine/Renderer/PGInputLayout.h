#pragma once

#include "../Core.h"

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
    char* name;
    VertexDataFormat format;
    uint32_t inputSlot;
    uint32_t offset;
};

class PG_API IVertexInputLayout {
public:
    virtual ~IVertexInputLayout() = default;
};

