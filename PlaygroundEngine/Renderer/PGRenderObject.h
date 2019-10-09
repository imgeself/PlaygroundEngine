#pragma once

#include "../Core.h"
#include "../MeshUtils.h"
#include "HWRendererAPI.h"

class PGRenderObject {
public:
    PGRenderObject(Mesh* mesh, HWRendererAPI* rendererAPI);
    ~PGRenderObject();

    void Render(HWRendererAPI* rendererAPI);

private:
    HWVertexBuffer* m_VertexBuffer = nullptr;
    HWIndexBuffer* m_IndexBuffer = nullptr;
    HWVertexInputLayout* m_InputLayout = nullptr;
    PGShader* m_Shader = nullptr;
};

