#pragma once

#include "../Core.h"

enum SamplerStateTextureAddressMode {
    TextureAddressMode_WRAP,
    TextureAddressMode_MIRROR,
    TextureAddressMode_CLAMP,
    TextureAddressMode_BORDER
};

enum SamplerStateTextureFilterMode {
    TextureFilterMode_LINEAR,
    TextureFilterMode_POINT
};

struct SamplerStateInitParams {
    SamplerStateTextureFilterMode filterMode;
    SamplerStateTextureAddressMode AddressModeU;
    SamplerStateTextureAddressMode AddressModeV;
    SamplerStateTextureAddressMode AddressModeW;
    float borderColor[4];
};

class HWSamplerState {
public:
    virtual ~HWSamplerState() = default;
};

