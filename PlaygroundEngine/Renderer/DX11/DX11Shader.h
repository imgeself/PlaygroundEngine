#pragma once

#include "../HWShader.h"

#include <d3d11.h>
#include <d3dcompiler.h>

class DX11ShaderProgram : public HWShaderProgram {
public:
    DX11ShaderProgram(ID3D11Device* device, ShaderFileData shaderFileData);
    ~DX11ShaderProgram() override;
    HWShaderProgram& operator=(const HWShaderProgram& other) override;


    ID3DBlob* GetVertexShaderBlob() { return m_VertexShaderBlob; }
    ID3D11VertexShader* GetDXVertexShader() { return m_VertexShader; }
    ID3D11PixelShader* GetDXPixelShader() { return m_PixelShader; }

protected:
    ID3DBlob* CompileShader(ShaderFileData source, const char* mainFunctionName, ShaderType type);

private:
    //NOTE: For now, we will keep vertex shader file source in here for input layout creation.
    ID3DBlob* m_VertexShaderBlob;
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
    ID3D11ShaderReflection* m_ShaderReflection;
};

