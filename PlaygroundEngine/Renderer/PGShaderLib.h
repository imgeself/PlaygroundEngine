#pragma once

#include "../Core.h"
#include "PGRendererAPI.h"

#include <unordered_map>
#include <memory>

typedef std::shared_ptr<IShaderProgram> ShaderRef;

class PG_API PGShaderLib {
public:
    PGShaderLib(IRendererAPI* rendererAPI);
    ~PGShaderLib() = default;

    ShaderRef LoadShaderFromDisk(const std::string& shaderFileName);

    void LoadDefaultShaders();
    ShaderRef GetDefaultShader(const std::string& name);

private:
    IRendererAPI* m_RendererAPI;
    std::unordered_map<std::string, ShaderRef> m_Shaders;
};

