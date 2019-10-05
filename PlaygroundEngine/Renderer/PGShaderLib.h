#pragma once

#include "../Core.h"
#include "PGRendererAPI.h"

#include <unordered_map>
#include <memory>

typedef IShaderProgram* ShaderRef;

class PG_API PGShaderLib {
public:
    PGShaderLib(IRendererAPI* rendererAPI);
    ~PGShaderLib();

    ShaderRef LoadShaderFromDisk(const std::string& shaderFileName, bool createIfExists = false);
    void ReloadShadersIfNeeded();
    void LoadDefaultShaders();
    ShaderRef GetDefaultShader(const std::string& name);

private:
    IRendererAPI* m_RendererAPI;
    std::unordered_map<std::string, ShaderRef> m_Shaders;
    std::unordered_map<std::string, FILETIME> m_ShaderFileLastWriteTimeMap;
};

