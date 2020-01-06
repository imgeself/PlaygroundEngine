#pragma once

#include "../Core.h"
#include "HWResource.h"

// TODO: We use DXGI_FORMAT for texture format but we need cross platform enum for supporting different APIs.
#include <d3d11.h>
typedef DXGI_FORMAT TextureDataFormat;

enum TextureResourceFlags {
    // Bind flags
    BIND_SHADER_RESOURCE   = BIT(1),
    BIND_RENDER_TARGET     = BIT(2),
    BIND_DEPTH_STENCIL     = BIT(3),

    // Usage flags
    CPU_ACCESS_READ        = BIT(4),
    CPU_ACCESS_WRITE       = BIT(5),

    USAGE_DYNAMIC          = BIT(6),
    USAGE_STAGING          = BIT(7),
    USAGE_IMMUTABLE        = BIT(8),


    MISC_TEXTURE_CUBE      = BIT(9),
    MISC_GENERATE_MIPS     = BIT(10)
};

struct TextureSubresourceData {
    const void* data = nullptr;
    uint32_t memPitch = 0;
    uint32_t memSlicePitch = 0;
};

struct Texture2DDesc {
    size_t width;
    size_t height;
    TextureDataFormat format;
    uint32_t flags;
    size_t sampleCount;
    size_t arraySize;
    size_t mipCount;

    TextureSubresourceData* subresources = nullptr;
};

class HWTexture2D : public HWResource {
public:
    virtual ~HWTexture2D() = default;

    virtual Texture2DDesc GetDesc() = 0;
};

