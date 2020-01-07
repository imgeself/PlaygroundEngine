#pragma once

#include "PGShaderLib.h"
#include "PGTexture.h"
#include "../Assets/Shaders/ShaderDefinitions.h"

#include <memory>

struct Material : DrawMaterial {
    ShaderRef shader;

    PGTexture* albedoTexture;
    PGTexture* roughnessTexture;
    PGTexture* metallicTexture;
    PGTexture* aoTexture;

    PGTexture* radianceMap;
    PGTexture* irradianceMap;
    PGTexture* envBrdf;
};

