#pragma once

#include "../Core.h"
#include "HWRendererAPI.h"
#include "PGShader.h"

#include <unordered_map>
#include <memory>
#include <filesystem>

typedef PGShader* ShaderRef;

class PG_API PGShaderLib {
public:
    PGShaderLib(HWRendererAPI* rendererAPI);
    ~PGShaderLib();

    ShaderRef LoadShaderFromDisk(const std::filesystem::path& shaderFileName);
    void ReloadShadersIfNeeded();
    void LoadDefaultShaders();
    ShaderRef GetDefaultShader(const std::string& name);

private:
    HWRendererAPI* m_RendererAPI;
    std::unordered_map<std::string, ShaderRef> m_Shaders;
    std::unordered_map<std::string, std::filesystem::file_time_type> m_ShaderFileLastWriteTimeMap;
};

