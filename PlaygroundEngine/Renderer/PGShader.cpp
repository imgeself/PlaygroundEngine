#include "PGShader.h"
#include "../PGLog.h"

PGShader::PGShader() {

}

PGShader::~PGShader() {
    SAFE_RELEASE(m_VertexShaderBlob);
    SAFE_RELEASE(m_PixelShaderBlob);
}

static bool CompileShader(const char* filename, const char* mainFunctionName, const char* version, ID3DBlob** shaderBlob) {
    ID3DBlob* errorBlob = nullptr;

    const size_t filenameSize = strlen(filename) + 1;
    wchar_t* wcFilename = (wchar_t*) alloca(sizeof(wchar_t) * filenameSize);

    size_t numberOfConverted;
    mbstowcs_s(&numberOfConverted, wcFilename, filenameSize, filename, filenameSize - 1);

    D3D_SHADER_MACRO compileDefines[] = { "_HLSL", "1", NULL, NULL };
    HRESULT status = D3DCompileFromFile(wcFilename, compileDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        mainFunctionName, version, D3DCOMPILE_DEBUG, 0, shaderBlob, &errorBlob);
    if (errorBlob) {
        PG_LOG_ERROR("Shader compilation errors: %s", (char*)errorBlob->GetBufferPointer());
        errorBlob->Release();
    }

    return SUCCEEDED(status);
}

HWShaderBytecode PGShader::GetVertexBytecode() {
    if (!m_VertexShaderBlob) {
        PG_LOG_DEBUG("Compiling vertex shader: %s", m_Filepath.c_str());
        bool vertexCompilationSuccess = CompileShader(m_Filepath.c_str(), "VSMain", "vs_5_0", &m_VertexShaderBlob);
        PG_ASSERT(vertexCompilationSuccess, "Shader compilation failed!")
    }

    HWShaderBytecode bytecode = { m_VertexShaderBlob->GetBufferPointer(), m_VertexShaderBlob->GetBufferSize() };
    return bytecode;
}

HWShaderBytecode PGShader::GetPixelBytecode() {
    if (!m_PixelShaderBlob) {
        PG_LOG_DEBUG("Compiling pixel shader: %s", m_Filepath.c_str());
        ID3DBlob* pixelShaderBlob = nullptr;
        bool pixelCompilationSuccess = CompileShader(m_Filepath.c_str(), "PSMain", "ps_5_0", &m_PixelShaderBlob);
        PG_ASSERT(pixelCompilationSuccess, "Shader compilation failed!")
    }

    HWShaderBytecode bytecode = { m_PixelShaderBlob->GetBufferPointer(), m_PixelShaderBlob->GetBufferSize() };
    return bytecode;
}

void PGShader::LoadFromFilename(const std::string& filename) {
    m_Filepath = filename;
}

void PGShader::Reload() {
    if (needsUpdate) {
        needsUpdate = false;
        SAFE_RELEASE(m_VertexShaderBlob);
        SAFE_RELEASE(m_PixelShaderBlob);
    }
}
