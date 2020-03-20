#include "PGShader.h"
#include "../PGLog.h"

PGShader::PGShader() {

}

PGShader::~PGShader() {
    for (PGShaderPermutation& permutation : m_Permutations) {
        SAFE_RELEASE(permutation.vertexShaderBlob.shaderBlob);
        SAFE_RELEASE(permutation.pixelShaderBlob.shaderBlob);
    }
}

static void ApplyShaderFlags(D3D_SHADER_MACRO* compileDefines, uint32_t shaderFlags) {
    compileDefines[0].Name = "_HLSL";
    compileDefines[0].Definition = "1";

    size_t compileDefineIndex = 1;
    if (shaderFlags & PGShaderFlags::ALPHA_TEST) {
        compileDefines[compileDefineIndex].Name = "ALPHA_TEST";
        compileDefines[compileDefineIndex].Definition = "1";
        ++compileDefineIndex;
    }
    if (shaderFlags & PGShaderFlags::NORMAL_MAPPING) {
        compileDefines[compileDefineIndex].Name = "NORMAL_MAPPING";
        compileDefines[compileDefineIndex].Definition = "1";
        ++compileDefineIndex;
    }
}

static bool CompileShader(uint32_t shaderFlags, const char* filename, const char* mainFunctionName, const char* version, ID3DBlob** shaderBlob) {
    ID3DBlob* errorBlob = nullptr;

    const size_t filenameSize = strlen(filename) + 1;
    wchar_t* wcFilename = (wchar_t*) alloca(sizeof(wchar_t) * filenameSize);

    size_t numberOfConverted;
    mbstowcs_s(&numberOfConverted, wcFilename, filenameSize, filename, filenameSize - 1);

    D3D_SHADER_MACRO compileDefines[16] = {0};
    ApplyShaderFlags(compileDefines, shaderFlags);
    HRESULT status = D3DCompileFromFile(wcFilename, compileDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        mainFunctionName, version, D3DCOMPILE_DEBUG, 0, shaderBlob, &errorBlob);
    if (errorBlob) {
        PG_LOG_ERROR("Shader compilation errors: %s", (char*)errorBlob->GetBufferPointer());
        errorBlob->Release();
    }

    return SUCCEEDED(status);
}

HWShaderBytecode PGShader::GetVertexBytecode(uint32_t shaderFlags) {
    auto searchIterator = std::find_if(m_Permutations.begin(), m_Permutations.end(), [shaderFlags](PGShaderPermutation& permutation) {
        return permutation.shaderFlags == shaderFlags;
    });

    ID3DBlob* shaderBlob = nullptr;
    if (searchIterator != m_Permutations.end()) {
        if (searchIterator->vertexShaderBlob.isDirty) {
            PG_LOG_DEBUG("Compiling vertex shader: %s with flags: %d", m_Filepath.c_str(), shaderFlags);
            bool vertexCompilationSuccess = CompileShader(shaderFlags, m_Filepath.c_str(), "VSMain", "vs_5_0", &shaderBlob);
            PG_ASSERT(vertexCompilationSuccess, "Shader compilation failed!");
            searchIterator->vertexShaderBlob.shaderBlob = shaderBlob;
            searchIterator->vertexShaderBlob.isDirty = false;
        } else {
            shaderBlob = searchIterator->vertexShaderBlob.shaderBlob;
        }
    } else {
        PG_LOG_DEBUG("Compiling vertex shader: %s with flags: %d", m_Filepath.c_str(), shaderFlags);
        bool vertexCompilationSuccess = CompileShader(shaderFlags, m_Filepath.c_str(), "VSMain", "vs_5_0", &shaderBlob);
        PG_ASSERT(vertexCompilationSuccess, "Shader compilation failed!");

        PGShaderPermutation shaderPermutation = {};
        shaderPermutation.vertexShaderBlob.shaderBlob = shaderBlob;
        shaderPermutation.vertexShaderBlob.isDirty = false;
        shaderPermutation.shaderFlags = shaderFlags;

        m_Permutations.push_back(shaderPermutation);
    }

    

    HWShaderBytecode bytecode = { shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize() };
    return bytecode;
}

HWShaderBytecode PGShader::GetPixelBytecode(uint32_t shaderFlags) {
    auto searchIterator = std::find_if(m_Permutations.begin(), m_Permutations.end(), [shaderFlags](PGShaderPermutation& permutation) {
        return permutation.shaderFlags == shaderFlags;
    });

    ID3DBlob* shaderBlob = nullptr;
    if (searchIterator != m_Permutations.end()) {
        if (searchIterator->pixelShaderBlob.isDirty) {
            PG_LOG_DEBUG("Compiling pixel shader: %s with flags: %d", m_Filepath.c_str(), shaderFlags);
            bool pixelCompilationSuccess = CompileShader(shaderFlags, m_Filepath.c_str(), "PSMain", "ps_5_0", &shaderBlob);
            PG_ASSERT(pixelCompilationSuccess, "Shader compilation failed!");
            searchIterator->pixelShaderBlob.shaderBlob = shaderBlob;
            searchIterator->pixelShaderBlob.isDirty = false;
        } else {
            shaderBlob = searchIterator->pixelShaderBlob.shaderBlob;
        }
    }
    else {
        PG_LOG_DEBUG("Compiling pixel shader: %s with flags: %d", m_Filepath.c_str(), shaderFlags);
        bool pixelCompilationSuccess = CompileShader(shaderFlags, m_Filepath.c_str(), "PSMain", "ps_5_0", &shaderBlob);
        PG_ASSERT(pixelCompilationSuccess, "Shader compilation failed!");

        PGShaderPermutation shaderPermutation = {};
        shaderPermutation.pixelShaderBlob.shaderBlob = shaderBlob;
        shaderPermutation.pixelShaderBlob.isDirty = false;
        shaderPermutation.shaderFlags = shaderFlags;

        m_Permutations.push_back(shaderPermutation);
    }



    HWShaderBytecode bytecode = { shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize() };
    return bytecode;
}

void PGShader::LoadFromFilename(const std::string& filename) {
    m_Filepath = filename;
}

void PGShader::Reload() {
    if (needsUpdate) {
        needsUpdate = false;

        PG_LOG_DEBUG("Reloading shader: %s", m_Filepath.c_str());
        ID3DBlob* vertexShaderBlob = nullptr;
        bool vertexCompilationSuccess = CompileShader(PGShaderFlags::ALL_FLAGS, m_Filepath.c_str(), "VSMain", "vs_5_0", &vertexShaderBlob);
        ID3DBlob* pixelShaderBlob = nullptr;
        bool pixelCompilationSuccess = CompileShader(PGShaderFlags::ALL_FLAGS, m_Filepath.c_str(), "PSMain", "ps_5_0", &pixelShaderBlob);

        if (!vertexCompilationSuccess || !pixelCompilationSuccess) {
            PG_LOG_ERROR("Shader reload failed!");
            return;
        }

        SAFE_RELEASE(vertexShaderBlob);
        SAFE_RELEASE(pixelShaderBlob);

        for (PGShaderPermutation& permutation : m_Permutations) {
            permutation.vertexShaderBlob.isDirty = true;
            permutation.pixelShaderBlob.isDirty = true;
        }

        PG_LOG_DEBUG("Shader reloaded sucessfully");
    }
}
