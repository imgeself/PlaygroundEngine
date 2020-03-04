#pragma once

#include "../Core.h"
#include "HWResource.h"

// TODO: We use DXGI_FORMAT for texture format but we need cross platform enum for supporting different APIs.
#include <d3d11.h>
typedef DXGI_FORMAT TextureDataFormat;

struct Texture2DDesc {
    size_t width;
    size_t height;
    TextureDataFormat format;
    uint32_t flags;
    size_t sampleCount;
    size_t arraySize;
    size_t mipCount;
};

class HWTexture2D : public HWResource {
public:
    virtual ~HWTexture2D() = default;

    virtual Texture2DDesc GetDesc() = 0;
};

