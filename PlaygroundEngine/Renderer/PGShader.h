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

    inline HWShaderProgram* GetHWShader() { return m_HWShader; }

private:
    HWShaderProgram* m_HWShader;
};

