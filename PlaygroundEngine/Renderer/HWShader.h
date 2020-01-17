#pragma once

#include "../Core.h"

struct ShaderFileData {
    char* fileData;
    size_t fileSize;
};

class HWVertexShader {
public:
    virtual ~HWVertexShader() = default;
};

class HWPixelShader {
public:
    virtual ~HWPixelShader() = default;
};

