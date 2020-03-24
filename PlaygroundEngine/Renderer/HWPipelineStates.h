#pragma once

#include "../Core.h"

// Blend state
enum HWBlend {
    BLEND_ZERO,
    BLEND_ONE,
    BLEND_SRC_COLOR,
    BLEND_INV_SRC_COLOR,
    BLEND_SRC_ALPHA,
    BLEND_INV_SRC_ALPHA,
    BLEND_DEST_ALPHA,
    BLEND_INV_DEST_ALPHA,
    BLEND_DEST_COLOR,
    BLEND_INV_DEST_COLOR,
    BLEND_SRC_ALPHA_SAT,
    BLEND_BLEND_FACTOR,
    BLEND_INV_BLEND_FACTOR,
};

enum HWBlendOP {
    BLEND_OP_ADD,
    BLEND_OP_SUBTRACT,
    BLEND_OP_REV_SUBTRACT,
    BLEND_OP_MIN,
    BLEND_OP_MAX
};

enum HWColorWriteEnable {
    COLOR_WRITE_ENABLE_RED,
    COLOR_WRITE_ENABLE_GREEN ,
    COLOR_WRITE_ENABLE_BLUE,
    COLOR_WRITE_ENABLE_ALPHA,
    COLOR_WRITE_ENABLE_ALL
};

struct HWRenderTargetBlendDesc {
    bool blendEnable = false;
    HWBlend srcBlend = BLEND_ONE;
    HWBlend destBlend = BLEND_ZERO;
    HWBlendOP blendOp = BLEND_OP_ADD;
    HWBlend srcBlendAlpha = BLEND_ONE;
    HWBlend destBlendAlpha = BLEND_ZERO;
    HWBlendOP blendOpAlpha = BLEND_OP_ADD;
    HWColorWriteEnable renderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
};

struct HWBlendDesc {
    bool alphaToCoverageEnable = false;
    bool independentBlendEnable = false;
    HWRenderTargetBlendDesc renderTarget[8];
};

// Rasterization state
enum HWFillMode {
    FILL_SOLID,
    FILL_WIREFRAME,
};

enum HWCullMode {
    CULL_NONE,
    CULL_FRONT,
    CULL_BACK
};

struct HWRasterizerDesc {
    HWFillMode fillMode = FILL_SOLID;
    HWCullMode cullMode = CULL_BACK;
    bool frontCounterClockwise = false;
    int32_t depthBias = 0;
    float depthBiasClamp = 0.0f;
    float slopeScaledDepthBias = 0.0f;
    bool depthClipEnable = true;
    bool scissorEnable = false;
    bool multisampleEnable = false;
    bool antialiasedLineEnable = false;
};

// Depth-stencil state
enum HWComparionsFunc {
    COMPARISON_NEVER,
    COMPARISON_LESS,
    COMPARISON_EQUAL,
    COMPARISON_LESS_EQUAL,
    COMPARISON_GREATER,
    COMPARISON_NOT_EQUAL,
    COMPARISON_GREATER_EQUAL,
    COMPARISON_ALWAYS,
};

enum HWDepthWriteMask {
    DEPTH_WRITE_MASK_ZERO,
    DEPTH_WRITE_MASK_ALL
};

enum HWStencilOP {
    STENCIL_OP_KEEP,
    STENCIL_OP_ZERO,
    STENCIL_OP_REPLACE,
    STENCIL_OP_INCR_SAT,
    STENCIL_OP_DECR_SAT,
    STENCIL_OP_INVERT,
    STENCIL_OP_INCR,
    STENCIL_OP_DECR
};

struct HWDepthStencilOPDesc {
    HWStencilOP stencilFailOp = STENCIL_OP_KEEP;
    HWStencilOP stencilDepthFailOp = STENCIL_OP_KEEP;
    HWStencilOP stencilPassOp = STENCIL_OP_KEEP;
    HWComparionsFunc stencilFunc = COMPARISON_ALWAYS;
};

struct HWDepthStencilDesc {
    bool depthEnable = true;
    HWDepthWriteMask depthWriteMask = DEPTH_WRITE_MASK_ALL;
    HWComparionsFunc depthFunc = COMPARISON_LESS;
    bool stencilEnable = false;
    uint8_t stencilReadMask = 0xFF;
    uint8_t stencilWriteMask = 0xFF;
    HWDepthStencilOPDesc frontFace;
    HWDepthStencilOPDesc backFace;
};

enum HWVertexDataFormat {
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

enum HWInputClassification {
    PER_VERTEX_DATA,
    PER_INSTANCE_DATA
};


struct HWVertexInputElement {
    const char* semanticName;
    uint32_t semanticIndex;
    HWVertexDataFormat format;
    uint32_t inputSlot;
    HWInputClassification classification;
    uint32_t instanceStepRate;

    HWVertexInputElement(const char* semanticName, uint32_t semanticIndex, HWVertexDataFormat format, uint32_t slot, HWInputClassification classification, uint32_t instanceStepRate)
        : semanticName(semanticName), semanticIndex(semanticIndex), format(format), inputSlot(slot), classification(classification), instanceStepRate(instanceStepRate) {}
};

struct HWInputLayoutDesc {
    const HWVertexInputElement* elements = nullptr;
    uint32_t elementCount = 0;
};

enum HWPrimitiveTopology {
  PRIMITIVE_TOPOLOGY_UNDEFINED,
  PRIMITIVE_TOPOLOGY_POINTLIST,
  PRIMITIVE_TOPOLOGY_LINELIST,
  PRIMITIVE_TOPOLOGY_LINESTRIP,
  PRIMITIVE_TOPOLOGY_TRIANGLELIST,
  PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
};

struct HWShaderBytecode {
    const void* shaderBytecode = nullptr;
    size_t bytecodeLength;
};

struct HWGraphicsPipelineStateDesc {
    HWShaderBytecode vertexShader;
    HWShaderBytecode pixelShader;

    HWBlendDesc blendDesc;
    uint32_t sampleMask = 0xFFFFFFFF;
    HWRasterizerDesc rasterizerDesc;
    HWInputLayoutDesc inputLayoutDesc;
    HWDepthStencilDesc depthStencilDesc;

    HWPrimitiveTopology primitiveTopology;
};

class HWGraphicsPipelineState {
public:
    virtual ~HWGraphicsPipelineState() = default;
};

struct HWComputePipelineStateDesc {
    HWShaderBytecode computeShader;
};

class HWComputePipelineState {
public:
    virtual ~HWComputePipelineState() = default;
};

