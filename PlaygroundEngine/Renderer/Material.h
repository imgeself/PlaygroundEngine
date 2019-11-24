#pragma once

#include "../Core.h"
#include "PGShaderLib.h"
#include "PGTexture.h"

#include <memory>

struct Material : DrawMaterial {
    ShaderRef shader;

    PGTexture* albedoTexture;
    PGTexture* roughnessTexture;
    PGTexture* metallicTexture;
    PGTexture* aoTexture;
};

