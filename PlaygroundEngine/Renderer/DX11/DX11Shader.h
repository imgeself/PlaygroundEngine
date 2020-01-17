#pragma once

#include "../HWShader.h"

#include <d3d11.h>
#include <d3dcompiler.h>

class DX11VertexShader : public HWVertexShader {
public:
    DX11VertexShader(ID3D11Device* device, ShaderFileData* vertexShaderBinaryFileData);
    ~DX11VertexShader() override;

    ID3DBlob* GetVertexShaderBlob() { return m_VertexShaderBlob; }
    ID3D11VertexShader* GetDXVertexShader() { return m_VertexShader; }
private:
    //NOTE: For now, we will keep vertex shader file source in here for input layout creation.
    ID3DBlob* m_VertexShaderBlob = nullptr;
    ID3D11VertexShader* m_VertexShader = nullptr;
};

class DX11PixelShader : public HWPixelShader {
public:
    DX11PixelShader(ID3D11Device* device, ShaderFileData* pixelShaderBinaryFileData);
    ~DX11PixelShader() override;

    ID3D11PixelShader* GetDXPixelShader() { return m_PixelShader; }
private:
    ID3D11PixelShader* m_PixelShader = nullptr;
};

