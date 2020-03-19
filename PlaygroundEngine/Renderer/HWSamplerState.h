#pragma once

#include "../Core.h"

enum SamplerStateTextureAddressMode {
    TextureAddressMode_NONE,
    TextureAddressMode_WRAP,
    TextureAddressMode_MIRROR,
    TextureAddressMode_CLAMP,
    TextureAddressMode_BORDER
};

enum SamplerStateTextureFilterMode {
    TextureFilterMode_MIN_MAG_MIP_LINEAR,
    TextureFilterMode_MIN_MAG_MIP_POINT,
    TextureFilterMode_MIN_MAG_LINEAR_MIP_POINT,
    TextureFilterMode_ANISOTROPIC
};

enum ComparisonFunction {
    NONE,
    NEVER,
    LESS,
    EQUAL,
    LESS_EQUAL,
    GREATER,
    NOT_EQUAL,
    GREATER_EQUAL,
    ALWAYS
};

struct SamplerStateInitParams {
    SamplerStateTextureFilterMode filterMode;
    SamplerStateTextureAddressMode addressModeU;
    SamplerStateTextureAddressMode addressModeV;
    SamplerStateTextureAddressMode addressModeW;
    ComparisonFunction comparisonFunction;
    uint32_t maxAnisotropy;
    float borderColor[4];
};

class HWSamplerState {
public:
    virtual ~HWSamplerState() = default;
};

