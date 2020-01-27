#pragma once

#include "../Core.h"

enum HWResourceFlags {
    // Bind flags
    BIND_SHADER_RESOURCE = BIT(1),
    BIND_RENDER_TARGET = BIT(2),
    BIND_DEPTH_STENCIL = BIT(3),

    // Usage flags
    CPU_ACCESS_READ = BIT(4),
    CPU_ACCESS_WRITE = BIT(5),

    USAGE_DYNAMIC = BIT(6),
    USAGE_STAGING = BIT(7),
    USAGE_IMMUTABLE = BIT(8),

    MISC_TEXTURE_CUBE = BIT(9),
    MISC_GENERATE_MIPS = BIT(10)
};

class HWResource {
public:
    virtual ~HWResource() = default;

    virtual void* GetResourceHandle() = 0;
};

