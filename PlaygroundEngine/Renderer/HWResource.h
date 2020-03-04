#pragma once

#include "../Core.h"

enum HWResourceFlags {
    // Bind flags
    BIND_SHADER_RESOURCE = BIT(1),
    BIND_RENDER_TARGET = BIT(2),
    BIND_DEPTH_STENCIL = BIT(3),

    BIND_VERTEX_BUFFER = BIT(4),
    BIND_INDEX_BUFFER = BIT(5),
    BIND_CONSTANT_BUFFER = BIT(6),

    // Usage flags
    CPU_ACCESS_READ = BIT(10),
    CPU_ACCESS_WRITE = BIT(11),

    USAGE_DYNAMIC = BIT(12),
    USAGE_STAGING = BIT(13),
    USAGE_IMMUTABLE = BIT(14),

    MISC_TEXTURE_CUBE = BIT(15),
    MISC_GENERATE_MIPS = BIT(16)
};

struct SubresourceData {
    const void* data = nullptr;
    uint32_t memPitch = 0;
    uint32_t memSlicePitch = 0;
};

class HWResource {
public:
    virtual ~HWResource() = default;

    virtual void* GetResourceHandle() = 0;
};

