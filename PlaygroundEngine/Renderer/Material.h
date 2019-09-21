#pragma once

#include "../Core.h"
#include "PGShader.h"

#include <memory>

struct Material {
    IShaderProgram* shader;
};

