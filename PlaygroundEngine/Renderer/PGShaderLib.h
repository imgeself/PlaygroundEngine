#pragma once

#include "../Core.h"
#include "HWRendererAPI.h"
#include "PGShader.h"

#include <unordered_map>
#include <memory>

typedef PGShader* ShaderRef;

class PG_API PGShaderLib {
public:
    PGShaderLib(HWRendererAPI* rendererAPI);
    ~PGShaderLib();

    ShaderRef LoadShaderFromDisk(const std::string& shaderFileName, bool createIfExists = false);
    void ReloadShadersIfNeeded();
    void LoadDefaultShaders();
    ShaderRef GetDefaultShader(const std::string& name);

private:
    HWRendererAPI* m_RendererAPI;
    std::unordered_map<std::string, ShaderRef> m_Shaders;
    std::unordered_map<std::string, FILETIME> m_ShaderFileLastWriteTimeMap;
};

