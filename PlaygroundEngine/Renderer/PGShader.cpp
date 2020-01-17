#include "PGShader.h"
#include "../PGLog.h"

#include <d3dcompiler.h>

PGShader::PGShader() {

}

PGShader::~PGShader() {
    delete m_HWVertexShader;
    delete m_HWPixelShader;
}

static ID3DBlob* CompileShader(ShaderFileData* source, const char* mainFunctionName, const char* version) {
    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    D3D_SHADER_MACRO compileDefines[] = { "_HLSL", "1", NULL, NULL };
    HRESULT status = D3DCompile(source->fileData, source->fileSize, NULL, compileDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        mainFunctionName, version, D3DCOMPILE_DEBUG, 0, &shaderBlob, &errorBlob);
    if (errorBlob) {
        PG_LOG_ERROR("Shader compilation errors: %s", (char*)errorBlob->GetBufferPointer());
        errorBlob->Release();
    }

    return shaderBlob;
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

void PGShader::LoadFromFilename(HWRendererAPI* rendererAPI, const char* filename) {
    PG_LOG_DEBUG("Compiling shader: %s", filename);
    ID3DBlob* vertexShaderBlob = nullptr;
    bool vertexCompilationSuccess = CompileShader(filename, "VSMain", "vs_5_0", &vertexShaderBlob);
    ID3DBlob* pixelShaderBlob = nullptr;
    bool pixelCompilationSuccess = CompileShader(filename, "PSMain", "ps_5_0", &pixelShaderBlob);
    PG_ASSERT(vertexCompilationSuccess && pixelCompilationSuccess, "Shader compilation failed!")

    ShaderFileData vertexShaderData = { (char*)vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
    ShaderFileData pixelShaderData = { (char*)pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };

    m_HWVertexShader = rendererAPI->CreateVertexShaderFromBinarySource(&vertexShaderData);
    m_HWPixelShader = rendererAPI->CreatePixelShaderFromBinarySource(&pixelShaderData);

    SAFE_RELEASE(vertexShaderBlob);
    SAFE_RELEASE(pixelShaderBlob);
}


void PGShader::LoadFromFileData(HWRendererAPI* rendererAPI, ShaderFileData* fileData) {
    ID3DBlob* vertexShaderBlob = CompileShader(fileData, "VSMain", "vs_5_0");
    ID3DBlob* pixelShaderBlob = CompileShader(fileData, "PSMain", "ps_5_0");
    ShaderFileData vertexShaderData = { (char*) vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
    ShaderFileData pixelShaderData = { (char*) pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };

    m_HWVertexShader = rendererAPI->CreateVertexShaderFromBinarySource(&vertexShaderData);
    m_HWPixelShader = rendererAPI->CreatePixelShaderFromBinarySource(&pixelShaderData);

    SAFE_RELEASE(vertexShaderBlob);
    SAFE_RELEASE(pixelShaderBlob);
}

void PGShader::Reload(HWRendererAPI* rendererAPI, const char* filename) {
    PG_LOG_DEBUG("Reloading shader: %s", filename);
    ID3DBlob* vertexShaderBlob = nullptr;
    bool vertexCompilationSuccess = CompileShader(filename, "VSMain", "vs_5_0", &vertexShaderBlob);
    ID3DBlob* pixelShaderBlob = nullptr;
    bool pixelCompilationSuccess = CompileShader(filename, "PSMain", "ps_5_0", &pixelShaderBlob);

    if (!vertexCompilationSuccess || !pixelCompilationSuccess) {
        PG_LOG_ERROR("Shader reload failed!");
        return;
    }

    ShaderFileData vertexShaderData = { (char*)vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
    ShaderFileData pixelShaderData = { (char*)pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };

    delete m_HWVertexShader;
    delete m_HWPixelShader;
    m_HWVertexShader = rendererAPI->CreateVertexShaderFromBinarySource(&vertexShaderData);
    m_HWPixelShader = rendererAPI->CreatePixelShaderFromBinarySource(&pixelShaderData);

    SAFE_RELEASE(vertexShaderBlob);
    SAFE_RELEASE(pixelShaderBlob);
    PG_LOG_DEBUG("Shader reloaded sucessfully");
}
