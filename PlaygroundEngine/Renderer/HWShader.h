#pragma once

#include "../Core.h"

struct ShaderFileData {
    char* fileData;
    size_t fileSize;
};

class PG_API HWShaderProgram {
public:
    virtual ~HWShaderProgram() = default;
};

