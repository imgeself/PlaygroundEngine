#pragma once

#include "../Core.h"

struct ShaderFileData {
    char* fileData;
    size_t fileSize;
};

class HWShaderProgram {
public:
    virtual ~HWShaderProgram() = default;
};

