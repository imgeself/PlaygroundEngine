#pragma once

#include "../Core.h"
#include "HWRendererAPI.h"

#include <string>
#include <unordered_map>

#include <d3dcompiler.h>

enum PGShaderFlags : uint32_t {
    ALPHA_TEST     = BIT(1),
    NORMAL_MAPPING = BIT(2),

    ALL_FLAGS = ALPHA_TEST | NORMAL_MAPPING
};

struct PGShaderBlob {
    ID3DBlob* shaderBlob = nullptr;
    bool isDirty = true;
};

struct PGShaderPermutation {
    PGShaderBlob vertexShaderBlob;
    PGShaderBlob pixelShaderBlob;
    uint32_t shaderFlags = 0;
};

class PGShader {
public:
    PGShader();
    ~PGShader();

    HWShaderBytecode GetVertexBytecode(uint32_t shaderFlags = 0);
    HWShaderBytecode GetPixelBytecode(uint32_t shaderFlags = 0);

    void LoadFromFilename(const std::string& filename);
    void Reload();

    bool needsUpdate = false;

private:
    std::string m_Filepath;
    std::vector<PGShaderPermutation> m_Permutations;
};

