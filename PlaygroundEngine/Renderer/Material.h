#pragma once

#include "../Core.h"
#include "PGShaderLib.h"

#include <memory>

struct Material : DrawMaterial {
    ShaderRef shader;
};

