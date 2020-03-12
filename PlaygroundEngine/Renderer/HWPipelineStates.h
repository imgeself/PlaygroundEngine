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

class HWBlendState {
public:
    virtual ~HWBlendState() = default;
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

class HWRasterizerState {
public:
    virtual ~HWRasterizerState() = default;
};
