#pragma once

#include "../Core.h"
#include "HWRendererAPI.h"

#include <string>
#include <unordered_map>

#include <d3dcompiler.h>

class PGShader {
public:
    PGShader();
    ~PGShader();

    HWShaderBytecode GetVertexBytecode();
    HWShaderBytecode GetPixelBytecode();

    void LoadFromFilename(const std::string& filename);
    void Reload();

    bool needsUpdate = false;

private:
    std::string m_Filepath;
    ID3DBlob* m_VertexShaderBlob = nullptr;
    ID3DBlob* m_PixelShaderBlob = nullptr;
};

