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

enum InputClassification {
    PER_VERTEX_DATA,
    PER_INSTANCE_DATA
};


struct VertexInputElement {
    std::string semanticName;
    uint32_t semanticIndex;
    VertexDataFormat format;
    uint32_t inputSlot;
    InputClassification classification;
    uint32_t instanceStepRate;

    VertexInputElement(const std::string& semanticName, uint32_t semanticIndex, VertexDataFormat format, uint32_t slot, InputClassification classification, uint32_t instanceStepRate)
        : semanticName(semanticName), semanticIndex(semanticIndex), format(format), inputSlot(slot), classification(classification), instanceStepRate(instanceStepRate) {}
};

class HWVertexInputLayout {
public:
    virtual ~HWVertexInputLayout() = default;
};

