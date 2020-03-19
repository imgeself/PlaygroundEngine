#pragma once

#include "PGShaderLib.h"
#include "PGTexture.h"
#include "../Assets/Shaders/ShaderDefinitions.h"

#include <memory>

enum AlphaMode {
    AlphaMode_ALWAYS_PASS,
    AlphaMode_ALPHA_TEST
};

struct Material : DrawMaterial {
    PGTexture* albedoTexture;
    PGTexture* roughnessTexture;
    PGTexture* metallicTexture;
    PGTexture* aoTexture;
    PGTexture* metallicRoughnessTexture;
    PGTexture* normalTexture;

    PGTexture* radianceMap;
    PGTexture* irradianceMap;
    PGTexture* envBrdf;

    // Pipeline variables
    ShaderRef shader; // TODO: Store technique instead of shader itself?
    AlphaMode alphaMode;
    bool doubleSided;
    bool normalMappingEnabled;

    uint16_t GetMaterialHash() {
        size_t pipelineOffset = offsetof(Material, shader);
        uint16_t materialHash = Hash(((const uint8_t*) this), pipelineOffset) & 0xFFFF;

        return materialHash;
    }

    uint16_t GetPipelineHash() {
        union {
            struct {
                uint16_t shader        : 12;
                uint16_t normalMapping : 1;
                uint16_t doubleSided   : 1;
                uint16_t alphaMode     : 2;
            };

            uint16_t hash;
        } pipelineKey;

        pipelineKey.normalMapping = normalMappingEnabled;
        pipelineKey.shader = ((size_t) shader) & 0xFFFF;
        pipelineKey.doubleSided = doubleSided;
        pipelineKey.alphaMode = (uint16_t) alphaMode;

        return pipelineKey.hash;
    }

    uint32_t GetMaterialPipelineCombinedHash() {
        uint16_t materialHash = GetMaterialHash();
        uint16_t pipelineHash = GetPipelineHash();

        return materialHash | (pipelineHash << 16);
    }

};

