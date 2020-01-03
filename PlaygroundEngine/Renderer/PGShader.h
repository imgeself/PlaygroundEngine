#pragma once

#include "../Core.h"
#include "../Math/math_util.h"
#include "HWRendererAPI.h"
#include "../Assets/Shaders/ShaderDefinitions.h"

#include <d3dcompiler.h>

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

