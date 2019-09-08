#pragma once

#include "../PGInputLayout.h"
#include "DX11Shader.h"

#include <d3d11.h>
#include <vector>

class DX11VertexInputLayout : public IVertexInputLayout {
public:
    DX11VertexInputLayout(ID3D11Device* device, std::vector<VertexInputElement> inputElements, DX11ShaderProgram* shaderProgram);
    ~DX11VertexInputLayout() override;

    ID3D11InputLayout* GetDXInputLayout() { return m_InputLayout; }

private:
    ID3D11InputLayout* m_InputLayout;
};
