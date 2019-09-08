#pragma once

#include "../PGShader.h"

#include <d3d11.h>

struct ShaderFile {
    char* fileData;
    size_t fileSize;
};

class DX11ShaderProgram : public IShaderProgram {
public:
    DX11ShaderProgram(ID3D11Device* device, const char* vertexShaderFilename, const char* pixelShaderFileName);
    ~DX11ShaderProgram() override;

    ShaderFile GetVertexShaderFile() { return m_VertexShaderFile; }
    ID3D11VertexShader* GetDXVertexShader() { return m_VertexShader; }
    ID3D11PixelShader* GetDXPixelShader() { return m_PixelShader; }

private:
    //NOTE: For now, we will keep vertex shader file source in here for input layout creation.
    ShaderFile m_VertexShaderFile;
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
};

