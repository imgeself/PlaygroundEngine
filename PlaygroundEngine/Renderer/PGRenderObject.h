#pragma once

#include "../Core.h"
#include "../MeshUtils.h"
#include "HWRendererAPI.h"

class PGRenderObject {
public:
    PGRenderObject(Mesh* mesh, HWRendererAPI* rendererAPI);
    ~PGRenderObject();

    void UpdatePerDrawConstantBuffer(HWRendererAPI* rendererAPI);

    HWVertexBuffer* vertexBuffer = nullptr;
    HWIndexBuffer* indexBuffer = nullptr;
    HWVertexInputLayout* inputLayout = nullptr;
    PGShader* shader = nullptr;
    Mesh* mesh = nullptr;

    HWConstantBuffer* perDrawConstantBuffer = nullptr;

};

