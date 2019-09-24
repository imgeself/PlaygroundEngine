#pragma once

#include "../PGShader.h"

#include <d3d11.h>

class DX11ShaderProgram : public IShaderProgram {
public:
    DX11ShaderProgram(ID3D11Device* device, ShaderFileData vertexShaderFileData, ShaderFileData pixelShaderFileData);
    ~DX11ShaderProgram() override;

    ShaderFileData GetVertexShaderFile() { return m_VertexShaderFile; }
    ID3D11VertexShader* GetDXVertexShader() { return m_VertexShader; }
    ID3D11PixelShader* GetDXPixelShader() { return m_PixelShader; }

private:
    //NOTE: For now, we will keep vertex shader file source in here for input layout creation.
    ShaderFileData m_VertexShaderFile;
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
};

