#pragma once

#include "../HWInputLayout.h"
#include "DX11Shader.h"

#include <d3d11.h>
#include <vector>

class DX11VertexInputLayout : public HWVertexInputLayout {
public:
    DX11VertexInputLayout(ID3D11Device* device, std::vector<VertexInputElement> inputElements, DX11ShaderProgram* shaderProgram, const char* debugName = 0);
    ~DX11VertexInputLayout() override;

    ID3D11InputLayout* GetDXInputLayout() { return m_InputLayout; }

private:
    ID3D11InputLayout* m_InputLayout;
};
