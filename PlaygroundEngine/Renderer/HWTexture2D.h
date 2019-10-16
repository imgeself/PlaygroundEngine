#pragma once

#include "../Core.h"

// TODO: We use DXGI_FORMAT for texture format but we need cross platform enum for supporting different APIs.
#include <d3d11.h>
typedef DXGI_FORMAT TextureDataFormat;

enum TextureResourceFlags {
    // Bind flags
    BIND_SHADER_RESOURCE   = BIT(0),
    BIND_RENDER_TARGET     = BIT(1),
    BIND_DEPTH_STENCIL     = BIT(2),

    // Usage flags
    USAGE_CPU_READ         = BIT(4),
    USAGE_CPU_WRITE        = BIT(5),
};

struct Texture2DInitParams {
    size_t width;
    size_t height;
    TextureDataFormat format;
    uint32_t flags;

    void* data;
};

class HWTexture2D {
public:
    virtual ~HWTexture2D() = default;
};

