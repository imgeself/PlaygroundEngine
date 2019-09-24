#pragma once

#include "../Core.h"

struct ShaderFileData {
    char* fileData;
    size_t fileSize;
};

class PG_API IShaderProgram {
public:
    virtual ~IShaderProgram() = default;
};

