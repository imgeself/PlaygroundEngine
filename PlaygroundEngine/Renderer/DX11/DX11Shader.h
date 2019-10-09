#pragma once

#include "../HWShader.h"

#include <d3d11.h>
#include <d3dcompiler.h>

class DX11ShaderProgram : public HWShaderProgram {
public:
    DX11ShaderProgram(ID3D11Device* device, ShaderFileData* shaderFileData);
    DX11ShaderProgram(ID3D11Device* device, ShaderFileData* vertexShaderBinaryFileData, ShaderFileData* pixelShaderBinaryFileData);
    ~DX11ShaderProgram() override;


    ID3DBlob* GetVertexShaderBlob() { return m_VertexShaderBlob; }
    ID3D11VertexShader* GetDXVertexShader() { return m_VertexShader; }
    ID3D11PixelShader* GetDXPixelShader() { return m_PixelShader; }

private:
    //NOTE: For now, we will keep vertex shader file source in here for input layout creation.
    ID3DBlob* m_VertexShaderBlob = nullptr;
    ID3D11VertexShader* m_VertexShader = nullptr;
    ID3D11PixelShader* m_PixelShader = nullptr;
};

