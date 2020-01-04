#include "PGShaderLib.h"
#include "../PGLog.h"

PGShaderLib::PGShaderLib(HWRendererAPI* rendererAPI)
    : m_RendererAPI(rendererAPI) {
}

PGShaderLib::~PGShaderLib() {
    for (auto pair : m_Shaders) {
        delete (pair.second);
    }
}

ShaderRef PGShaderLib::LoadShaderFromDisk(const std::filesystem::path& shaderFilePath) {
    const std::string name = shaderFilePath.stem().string();

    auto shaderSearch = m_Shaders.find(name);
    if (shaderSearch != m_Shaders.end()) {
        return shaderSearch->second;
    }

    std::filesystem::file_time_type fileTime = std::filesystem::last_write_time(shaderFilePath);
    m_ShaderFileLastWriteTimeMap[shaderFilePath.string()] = fileTime;

    PGShader* shader = new PGShader();
    shader->LoadFromFilename(m_RendererAPI, shaderFilePath.string().c_str());
    m_Shaders[name] = shader;
    return shader;
}

void PGShaderLib::ReloadShadersIfNeeded() {
    for (const auto& pair : m_ShaderFileLastWriteTimeMap) {
        std::filesystem::path filepath(pair.first);
        std::error_code errorCode;
        std::filesystem::file_time_type newFileTime = std::filesystem::last_write_time(filepath, errorCode);
        
        if (!errorCode.value()) {
            std::filesystem::file_time_type oldFileTime = pair.second;
            if (newFileTime.time_since_epoch() - oldFileTime.time_since_epoch() > std::chrono::seconds(0)) {
                auto shaderSearch = m_Shaders.find(filepath.stem().string());
                if (shaderSearch != m_Shaders.end()) {
                    shaderSearch->second->Reload(m_RendererAPI, filepath.string().c_str());
                    m_ShaderFileLastWriteTimeMap[filepath.string()] = newFileTime;
                }
                else {
                    PG_ASSERT(false, "Shader file cannot found!");
                }
            }
        } else {
            PG_LOG_WARNING("Get file last write time error! Code value: %d, message: %s", errorCode.value(), errorCode.message().c_str());
        }
    }
}

void PGShaderLib::LoadDefaultShaders() {
    std::string shaderDirectory = "../PlaygroundEngine/Assets/Shaders";
    for (const auto& entry : std::filesystem::directory_iterator(shaderDirectory)) {
        if (!entry.path().extension().compare(".hlsl")) {
            LoadShaderFromDisk(entry.path());
        }
    }
}

ShaderRef PGShaderLib::GetDefaultShader(const std::string& name) {
    return m_Shaders[name];
}

