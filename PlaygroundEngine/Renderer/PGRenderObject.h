#pragma once

#include "../Core.h"
#include "../MeshUtils.h"
#include "HWRendererAPI.h"

class PGRenderObject {
public:
    PGRenderObject(Mesh* mesh, HWRendererAPI* rendererAPI);
    ~PGRenderObject();

    HWConstantBuffer* UpdatePerDrawConstantBuffer(HWRendererAPI* rendererAPI);
    void Render(HWRendererAPI* rendererAPI);

    inline PGShader* GetShader() { return m_Shader; }
    inline HWConstantBuffer* GetPerDrawConstantBuffer() { return m_PerDrawConstantBuffer; }

private:
    HWVertexBuffer* m_VertexBuffer = nullptr;
    HWIndexBuffer* m_IndexBuffer = nullptr;
    HWVertexInputLayout* m_InputLayout = nullptr;
    PGShader* m_Shader = nullptr;
    Mesh* m_Mesh = nullptr;

    HWConstantBuffer* m_PerDrawConstantBuffer = nullptr;
};

