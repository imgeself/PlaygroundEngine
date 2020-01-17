#pragma once

#include "../Core.h"
#include "HWRendererAPI.h"

#include <string>
#include <unordered_map>

class PGShader {
public:
    PGShader();
    ~PGShader();

    void LoadFromFilename(HWRendererAPI* rendererAPI, const char* filename);
    void LoadFromFileData(HWRendererAPI* rendererAPI, ShaderFileData* fileData);
    void Reload(HWRendererAPI* rendererAPI, const char* filename);

    inline HWVertexShader* GetHWVertexShader() { return m_HWVertexShader; }
    inline HWPixelShader* GetHWPixelShader() { return m_HWPixelShader; }

private:
    HWVertexShader* m_HWVertexShader;
    HWPixelShader* m_HWPixelShader;
};

